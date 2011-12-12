#include "luaav.h"
#include "al_mainloop.h"

#include "lua_utility.h"

#include "unistd.h"
#include "stdio.h"

#include "portaudio.h"

//#include "al_pq.hpp"
#include "allocore/types/al_MsgQueue.hpp"

int testmsg(al_sec t, int a1, char * a2, float a3) {
	
	printf("testmsg %i %s %f\n", a1, a2, a3);
	return 0;
}

PaError err;

/* audio thread entry point */
int callback(const void *input, void *output, unsigned long frameCount, const PaStreamCallbackTimeInfo* timeInfo, PaStreamCallbackFlags statusFlags, void *userData ) 
{
	samplestamp offset;
	unsigned long ch, i;
	const float * inputs = (const float *)input;
	float * outputs = (float *)output;
	
	const luaav_audio_config cfg = luaav_audio_config_current();
	
	if (((samplestamp)frameCount) % cfg.blocksize != 0) {
		printf("error: blocksize %d; expected a multiple of %d\n", frameCount, cfg.blocksize);
		return -1;
	}
	
	for (offset = 0; offset < frameCount; offset += cfg.blocksize) {
		/* 
			de-interleave audio inbufs and place into delta inbusses
		*/
		for (ch=0; ch<cfg.inchannels; ch++) {
			sample * to = luaav_audio_input(ch);
			for (i=0; i<cfg.blocksize; i++) {
				to[i] = inputs[offset + i*cfg.inchannels];
			}
		}
		
		/*	
			Resume any scheduled events in the audio thread priority queue,
			+ Execute any processes in the proclist:
		*/
		luaav_audio_tick();
		
		
		/*
			read delta outbusses into audio outbufs as interleaved
		*/
		for (ch=0; ch<cfg.outchannels; ch++) {
			sample * from = luaav_audio_output(ch);
			for (i=0; i<cfg.blocksize; i++) {
				outputs[offset + i*cfg.outchannels] = from[i];
			}
		}
	}

	return 0;
}

/* main thread entry point */
void tick(al_nsec ns, void * u) {
	LuaAVState * S = (LuaAVState *)u;
	al_sec t = al_time_ns2s * ns;
	//printf("@%06.3f\n", t);
	
	luaav_main_tick();
	
	if (t >= 10.) {
		printf("test run complete\n");
		al_main_exit();
	}
}

/* triggered by al_main_exit() */
void quit(void * u) {}

void script_addcwdtosearchpath(lua_State * L) {
	char path[256];
	getcwd(path, 255);
	char code [1024];
	sprintf(code, "do \
			local path = '%s/' \
			package.path = path .. '?.lua;' .. package.path \
			package.path = path .. '?/init.lua;' .. package.path \
			package.cpath = path .. '?.so;' .. package.cpath \
		end", path);
	if (luaL_dostring(L, code)) {
		printf("%s\n", lua_tostring(L, -1));
		return;
	}
}

int main (int ac, char ** av) {

	luaav_init(ac, av);
	LuaAVState * S = luaav_state_create("main", NULL);
	
	lua_State * L = luaav_getstate(S);
	script_addcwdtosearchpath(L);
	
	char path[256];
	getcwd(path, 255);
	luaav_state_gofile(S, "test.lua", path);
	
	PaStream* stream;
	err = Pa_Initialize();
	if (err != paNoError) goto pa_out;
	err = Pa_OpenDefaultStream( &stream,
                              2, //delta_inchannels(D),
                              2, //delta_outchannels(D),
                              paFloat32,
                              44100., //delta_samplerate(D),
                              64, //delta_blocksize(D),
                              callback,
                              NULL); //D );
	if (err != paNoError) goto pa_out;
	err = Pa_StartStream(stream);
	if (err != paNoError) goto pa_out;

	
	
	al_main_enter(0.01, tick, S, quit);	
	
	
	
	err = Pa_StopStream(stream);
	if (err != paNoError) goto pa_out;
	err = Pa_CloseStream(stream);
	if (err != paNoError) goto pa_out;
	err = Pa_Terminate();
	if (err != paNoError) goto pa_out;
	
	luaav_state_close(&S);
	luaav_quit();
	
	return 0;
pa_out:
	printf("PaError %s\n", Pa_GetErrorText(err));
	return -1;
}