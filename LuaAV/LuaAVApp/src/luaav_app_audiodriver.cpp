#include "luaav_app_audiodriver.h"
#include "luaav_utility.hpp"
#include "lua_utility.h"
#include "portaudio.h"
#include "string.h"

#include "allocore/system/al_Thread.hpp"

al::Thread nrtThread;
bool nrtActive = false;

enum AudioDriverState {
	DriverUninitialized = 0,
	DriverInitialized,
	DriverConfigured,
	DriverStarted
};
static AudioDriverState gState = DriverUninitialized;
static PaStream* gPaStream = 0;

// returns true on error
static bool tracePaError(PaError err) {
	if (err != paNoError) {
		printf("PortAudio error %d: %s\n", err, Pa_GetErrorText(err));
		return true;
	}
	return false;
}

std::string luaPaDescription(luaav_audio_config cfg, std::string inname, std::string outname) {
	char msg[255];
	sprintf(msg, "%s -> %s @%4.1fkHz (%ix%i channels, blocksize %i)",
			inname.c_str(), outname.c_str(), cfg.samplerate * 0.001, cfg.inchannels, cfg.outchannels, (int)cfg.blocksize);
	return msg;
}

static int luaPaError(lua_State * L, PaError err, const char * msg) {
	if (err != paNoError) {
		luaL_error(L, "Audio Driver error (%s): %s", msg, Pa_GetErrorText(err));
	}
	return 0;
}

static void printDriverState() {
	switch(gState) {
		case DriverUninitialized: printf("DriverUninitialized\n"); break;
		case DriverInitialized: printf("DriverInitialized\n"); break;
		case DriverConfigured: printf("DriverConfigured\n"); break;
		case DriverStarted: printf("DriverStarted\n"); break;
		default: printf("Driver in bad state\n");
	}
}

static PaDeviceIndex find(const char * name) {
	for (PaDeviceIndex dev = 0; dev < Pa_GetDeviceCount(); dev++) {
		const PaDeviceInfo * info = Pa_GetDeviceInfo(dev);
		if (strcmp(info->name, name) == 0) {
			return dev;
		}
	}
	return -1;
}

static int lastTime;

/* audio thread entry point */
int callback(const void *input, void *output, unsigned long frameCount, const PaStreamCallbackTimeInfo* timeInfo, PaStreamCallbackFlags statusFlags, void *userData ) 
{
	samplestamp offset;
	unsigned long ch, i;
	const float * inputs = (const float *)input;
	float * outputs = (float *)output;
	
	const luaav_audio_config cfg = luaav_audio_config_current();
	
	if (((samplestamp)frameCount) % cfg.blocksize != 0) {
		printf("error: blocksize %d; expected a multiple of %d\n", (int)frameCount, (int)cfg.blocksize);
		return -1;
	}
	
	/* 
		Zero outputs
	*/
	samplestamp passes = ((samplestamp)frameCount) / cfg.blocksize;
	//printf("passes %d\n", passes);
	for (samplestamp pass = 0; pass < passes; pass++) {
		/* 
			de-interleave audio inbufs and place into inbusses
		*/
		offset = pass * cfg.blocksize * cfg.inchannels;
		for (ch=0; ch<cfg.inchannels; ch++) {
			
			// this is the writer for our global inputs:
			sample * to = luaav_audio_input(ch);
			luaav_audiobuffer_zero(to);
			for (i=0; i<cfg.blocksize; i++) {
				to[i] = inputs[offset + i*cfg.inchannels + ch];
			}	
		}
		
		/*
			Clear output buffers:
		*/
		for (ch=0; ch<cfg.outchannels; ch++) {
			luaav_audiobuffer_zero(luaav_audio_output(ch));
		}
		
		/*	
			Resume any scheduled events in the audio thread priority queue,
			+ Execute any processes in the proclist:
		*/
		luaav_audio_tick();
		
		//luaav_audio_output(0)[0] = 1.f;
		
		/*
			read delta outbusses into audio outbufs as interleaved
		*/
		offset = pass * cfg.blocksize * cfg.outchannels;
		for (ch=0; ch<cfg.outchannels; ch++) {
		
			/* just grabs the front buffer of the outputs */
			const sample * from = luaav_audio_output(ch);
			//if (*from == 0.) printf("zero\n");
			//printf("out %p\n", from);
			for (i=0; i<cfg.blocksize; i++) {
				sample v = from[i];
				// nan & clip:
				v = al::fixnan(v);
				v = v > 1. ? 1. : v < -1. ? -1. : v;
				outputs[offset + i*cfg.outchannels + ch] = v;
			}
		}
	}

	return 0;
}

/* alternative entry point for failed audio driver: */
void * nrtThreadFunction(void * userData) {
	while (nrtActive) {
		unsigned long ch;
		
		const luaav_audio_config cfg = luaav_audio_config_current();
	
		/*
			Clear output buffers:
		*/
		for (ch=0; ch<cfg.outchannels; ch++) {
			luaav_audiobuffer_zero(luaav_audio_output(ch));
		}
		
		/*	
			Resume any scheduled events in the audio thread priority queue,
			+ Execute any processes in the proclist:
		*/
		luaav_audio_tick();
		
		// rough estimate:
		al_sleep(cfg.blocksize / cfg.samplerate);
	}

	return 0;
}

int luaav_app_audio_init() {
	PaError err;
	if (gPaStream != 0) return 0;
	
	err = Pa_Initialize();
	if (err != paNoError) goto pa_out;
	
	luaav_add_opt_module("audio.driver", luaopen_audio_driver);
	
	{
		const luaav_audio_config cfg = luaav_audio_config_current();
	
		err = Pa_OpenDefaultStream( &gPaStream,
		                          cfg.inchannels,
		                          cfg.outchannels,
		                          paFloat32,
		                          cfg.samplerate,
		                          cfg.blocksize,
		                          callback,
		                          NULL);
	}

	if (err != paNoError) goto pa_out;
	gState = DriverConfigured;

	err = Pa_StartStream(gPaStream);
	if (err != paNoError) goto pa_out;
	gState = DriverStarted;

	return 0;
pa_out:
	printf("PortAudio Error %s\n", Pa_GetErrorText(err));
	
	/* since this didn't work, try launching a thread instead */
	nrtActive = true;
	nrtThread.start(nrtThreadFunction, NULL);
	
	return -1;
}

int luaav_app_audio_quit() {
	PaError err;
	if (nrtActive) {
		nrtActive = false;
		nrtThread.wait();
		return 0;
		
	} else {

		printf("closing Audio Driver\n");
		// time to close PortAudio... cascade the states back down to zero:
		if (gState == DriverStarted && Pa_IsStreamActive(gPaStream)) {
			err = Pa_StopStream(gPaStream);
			printf("stopping stream %i %s", err, Pa_GetErrorText(err));
			gState = DriverConfigured;
		}
		

		err = Pa_CloseStream(gPaStream);
		if (err != paNoError) goto pa_out;
		err = Pa_Terminate();
		if (err != paNoError) goto pa_out;
		
		gPaStream = 0;
		return 0;
	}
pa_out:
	printf("PaError %s\n", Pa_GetErrorText(err));
	return -1;
}


int lua_audio_driver_configure(lua_State * L) {
	if (nrtActive)
		luaL_error(L, "LuaAV audio failed in application launch; please try restarting LuaAV.");

	PaError err;
	
	if (!lua_istable(L, 1)) 
		luaL_error(L, "audio driver configuration requires a table argument");

	if (gState == DriverUninitialized) {
		luaL_error(L, "Audio Driver error: attempt to configure uninitialized driver");
	}
	
	// ensure that gState == DriverInitialized:
	if (gState == DriverConfigured || gState == DriverStarted) {
		err = Pa_CloseStream(gPaStream);
		luaPaError(L, err, "closing stream");
		gPaStream = 0;
		gState = DriverInitialized;
	}

	/* retrieve user options into local configure & vars */
	luaav_audio_config newcfg;
	const luaav_audio_config oldcfg = luaav_audio_config_current();
	newcfg.blocksize = lua::optfield(L, 1, oldcfg.blocksize, "blocksize");
	newcfg.samplerate = lua::optfield(L, 1, oldcfg.samplerate, "samplerate");
	newcfg.inchannels = lua::optfield(L, 1, oldcfg.inchannels, "inputs");
	newcfg.outchannels = lua::optfield(L, 1, oldcfg.outchannels, "outputs");
	
	/* pass config through luaav_audio_configure() to validate it */
	luaav_audio_configure(newcfg);
	
	/* retrieve current configuration */
	const luaav_audio_config cfg = luaav_audio_config_current();
	
	/* apply to PortAudio */
	PaStreamParameters inParams, outParams;
	PaDeviceInfo * inInfo, * outInfo;
	lua_getfield(L, 1, "input");
	PaDeviceIndex indev = (lua_isstring(L, -1)) ? find(lua_tostring(L, -1)) : Pa_GetDefaultInputDevice();
	lua_pop(L, 1);
	lua_getfield(L, 1, "output");
	PaDeviceIndex outdev = (lua_isstring(L, -1)) ? find(lua_tostring(L, -1)) : Pa_GetDefaultOutputDevice();
	lua_pop(L, 1);
	if(indev < 0 || outdev < 0) {
        //fprintf(stderr, "Audio input/output not supported");
        lua_pushboolean(L, 0);
        lua_pushstring(L, "Audio input/output not supported");
        return 2;
	}

	inParams.device = indev;
	inInfo = (PaDeviceInfo *)Pa_GetDeviceInfo(indev);
	inParams.channelCount = cfg.inchannels;
	inParams.sampleFormat = paFloat32; // | paNonInterleaved; //paHostFramesPerBufferUnspecified
	inParams.suggestedLatency = inInfo->defaultLowInputLatency;
	inParams.hostApiSpecificStreamInfo = NULL;

	outParams.device = outdev;
	outInfo = (PaDeviceInfo *)Pa_GetDeviceInfo(outdev);
	outParams.channelCount = cfg.outchannels;
	outParams.sampleFormat = paFloat32; // | paNonInterleaved;
	outParams.suggestedLatency = outInfo->defaultLowOutputLatency;
	outParams.hostApiSpecificStreamInfo = NULL;

	err = Pa_IsFormatSupported( &inParams, &outParams, cfg.samplerate );
	luaPaError(L, err, "configuration not supported");

	// go ahead and open the stream:
	err = Pa_OpenStream(
		&gPaStream, //PaStream** stream,
		&inParams,
		&outParams,
		cfg.samplerate, //double sampleRate,
		cfg.blocksize, //unsigned long framesPerBuffer,
		paClipOff, //paNoFlag, //PaStreamFlags streamFlags,
		callback, //PaStreamCallback *streamCallback,
		NULL // udata
	);
	luaPaError(L, err, luaPaDescription(cfg, inInfo->name, outInfo->name).c_str());

	gState = DriverConfigured;

	err = Pa_StartStream(gPaStream);
	luaPaError(L, err, "starting stream");

	gState = DriverStarted;

	lua_pushboolean(L, true);
	lua_pushstring(L, luaPaDescription(cfg, inInfo->name, outInfo->name).c_str());
	return 2;
}

static int lua_audio_driver_status(lua_State * L) {
	if (nrtActive)
		luaL_error(L, "LuaAV audio failed in application launch; please try restarting LuaAV.");
	lua_pushfstring(L, "cpu: %f", 100.0 * Pa_GetStreamCpuLoad(gPaStream));
	return 1;
}

int lua_audio_driver_refresh(lua_State * L) {
	if (nrtActive)
		luaL_error(L, "LuaAV audio failed in application launch; please try restarting LuaAV.");
	lua_pushvalue(L, lua_upvalueindex(1));
	lua::clear_table(L, -1);
	
	for (PaDeviceIndex dev = 0; dev < Pa_GetDeviceCount(); dev++) {
		const PaDeviceInfo * info = Pa_GetDeviceInfo(dev);
		if (info->maxInputChannels > 0) {
			lua_newtable(L);
				lua_pushinteger(L, info->maxInputChannels);
				lua_setfield(L, -2, "maxInputChannels");
				lua_pushnumber(L, info->defaultLowInputLatency);
				lua_setfield(L, -2, "defaultLowInputLatency");
				lua_pushnumber(L, info->defaultHighInputLatency);
				lua_setfield(L, -2, "defaultHighInputLatency");
				lua_pushnumber(L, info->defaultSampleRate);
				lua_setfield(L, -2, "defaultSampleRate");
				lua_pushboolean(L, dev == Pa_GetDefaultInputDevice());
				lua_setfield(L, -2, "defaultInputDevice");
			lua_setfield(L, -2, info->name);
		}
	}
	lua_pop(L, 1);
	
	lua_pushvalue(L, lua_upvalueindex(2));
	lua::clear_table(L, -1);
	
	for (PaDeviceIndex dev = 0; dev < Pa_GetDeviceCount(); dev++) {
		const PaDeviceInfo * info = Pa_GetDeviceInfo(dev);
		if (info->maxOutputChannels > 0) {
			lua_newtable(L);
				lua_pushinteger(L, info->maxOutputChannels);
				lua_setfield(L, -2, "maxOutputChannels");
				lua_pushnumber(L, info->defaultLowOutputLatency);
				lua_setfield(L, -2, "defaultLowOutputLatency");
				lua_pushnumber(L, info->defaultHighOutputLatency);
				lua_setfield(L, -2, "defaultHighOutputLatency");
				lua_pushnumber(L, info->defaultSampleRate);
				lua_setfield(L, -2, "defaultSampleRate");
				lua_pushboolean(L, dev == Pa_GetDefaultOutputDevice());
				lua_setfield(L, -2, "defaultOutputDevice");
			lua_setfield(L, -2, info->name);
		}
	}
	lua_pop(L, 1);
	return 0;
}

int luaopen_audio_driver(lua_State * L) {
	
	const char * libname = lua_tostring(L, 1);
	struct luaL_reg lib[] = {
		{"configure", lua_audio_driver_configure},
		{"status", lua_audio_driver_status},
		{NULL, NULL},
	};
	luaL_register(L, libname, lib);
	const int driver = lua_gettop(L);
	
	// add driver name:
	lua_pushstring(L, Pa_GetVersionText());
	lua_setfield(L, driver, "name");
	
	// add lists of available input/output devices:
	lua_newtable(L);
	lua_newtable(L);
	lua_pushvalue(L, -2);
	lua_pushvalue(L, -2);
	lua_pushcclosure(L, lua_audio_driver_refresh, 2);
	lua_pushvalue(L, -1);
	lua_call(L, 0, 0);
	lua_setfield(L, driver, "refresh");
	lua_setfield(L, driver, "outputs");
	lua_setfield(L, driver, "inputs");
	
	return 1;
}

