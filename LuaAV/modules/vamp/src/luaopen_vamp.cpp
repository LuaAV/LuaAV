#include "luaopen_vamp.h"

/*
	Based on the vamp-simple-host.cpp example in the Vamp SDK
*/


#include <iostream>
#include <fstream>

using namespace std;

using Vamp::Plugin;
using Vamp::PluginHostAdapter;
using Vamp::RealTime;
using Vamp::HostExt::PluginLoader;
using Vamp::HostExt::PluginWrapper;
using Vamp::HostExt::PluginInputDomainAdapter;

enum Verbosity {
    PluginIds,
    PluginOutputIds,
    PluginInformation,
    PluginInformationDetailed
};

class luaav_audio_Vamp {
public:
	luaav3_Synth synth;
	luaav3_InPort input;
	Plugin * plugin;
	int inchannels;
	
	// TODO: some data structure to cache features
};

void luaav_audio_Vamp_features_msg(al_sec t, luaav_audio_Vamp * self) {
	// cache features into self
}

void luaav_audio_Vamp_perform(luaav3_Node * node, int from, int to) {
	luaav_audio_Vamp * self = (luaav_audio_Vamp *)node;	
	luaav_audio_config cfg = luaav_audio_config_current();
	Plugin * plugin = self->plugin;
	luaav_sample const * const amp = luaav3_Fader_data(&self->synth.amp, from, to);	
	int inchannels = self->inchannels;
	int blocksize = cfg.blocksize;
	
	// convert double to float, scale by amplitude...
	float const * inputs[inchannels];
	float * buf[inchannels * blocksize];
	for (int c=0; c<inchannels; c++) {
		luaav_sample const * in = luaav3_InPort_data(&self->input, c); 
		float * out = buf[c*blocksize];
		inputs[c] = out;
		for (int i=0; i<blocksize; i++) {
			*out++ = *in++ * amp[i];
		}
	}
	
	Plugin::FeatureSet fs = plugin->process(inputs, RealTime::frame2RealTime(node->t, cfg.samplerate));
	
	// TODO: featureset needs to be passed back to main thread via tube outbox; 
	// somehow cached there for later polling in Lua.
	// node->graph->outbox.send(luaav_audio_Vamp_features_msg, self, fs);
	// question is how to serialize the features...
	// the C interface (vamp.h) might give some ideas
	
	// FeatureSet is a std::map<int, FeatureList>, where key is output no.
	// FeatureList is a std::vector<Feature>
	// Feature stores a vector of values, maybe label, timestamp, etc.
	
	luaav3_InPort_clear(&self->input);
}

void luaav_audio_Vamp_destroy(al_sec t, luaav3_Node * node) {
	luaav_audio_Vamp * self = (luaav_audio_Vamp *)node;	
	luaav3_OutPort_release(&self->synth.output);
	luaav3_Fader_release(&self->synth.amp);
	luaav3_InPort_release(&self->input);
	// todo: call self->plugin->getRemainingFeatures() to pick up the last featureset
	delete self->plugin;
	delete self;
}

int luaav_audio_Vamp_setinput(lua_State * L) {
	luaav_audio_Vamp * self = (luaav_audio_Vamp *)luaav3_Synth_checkto(L, 1);
	luaav3_InPort_set(L, 2, (luaav3_Node *)self, &self->input, 0);
	return 0;
}

int loadPlugin(lua_State * L) {
	luaav3_Graph * G = luaav3_graph_fromstate(L);
	luaav_audio_config cfg = luaav_audio_config_current();
	
	std::string key = luaL_checkstring(L, 1);	// pluginKey
	
//	enum AdapterFlags {
//
//        ADAPT_INPUT_DOMAIN  = 0x01,
//        ADAPT_CHANNEL_COUNT = 0x02,
//        ADAPT_BUFFER_SIZE   = 0x04,
//
//        ADAPT_ALL_SAFE      = 0x03,
//
//        ADAPT_ALL           = 0xff
//    };
	PluginLoader *loader = PluginLoader::getInstance();
	PluginLoader::AdapterFlags flags = PluginLoader::ADAPT_ALL;
	Plugin * plugin = loader->loadPlugin(key, cfg.samplerate, flags);
	if (plugin == NULL)
		luaL_error(L, "counldn't find plugin %s", key.data());
		
	int inchannels = 1;
	int blocksize = cfg.blocksize;
	if (!plugin->initialise(inchannels, blocksize, blocksize))
		luaL_error(L, "couldn't initialize %s with %d channels and %d blocksize", inchannels, blocksize);
	
	// initialize struct
	luaav_audio_Vamp * self = new luaav_audio_Vamp;
	self->plugin = plugin;
	self->inchannels = inchannels;
	
	luaav3_Node_init((luaav3_Node *)self, G, luaav_audio_Vamp_perform, luaav_audio_Vamp_destroy);
	luaav3_OutPort_init(&self->synth.output, (luaav3_Node *)self);
	luaav3_Fader_init(&self->synth.amp, 1, 1, 0);
	luaav3_InPort_init(&self->input);


	if (lua_istable(L, 2)) {
		lua_getfield(L, 2, "amp");		
		luaav3_Fader_set(L, -1, (luaav3_Node *)self, &self->synth.amp, 1);
		lua_pop(L, 1);
		lua_getfield(L, 2, "input");		
		luaav3_InPort_set(L, -1, (luaav3_Node *)self, &self->input, 0);
		lua_pop(L, 1);
		
		
	} else {
		lua_newtable(L); lua_replace(L, 2);
	}	

	int env = lua_gettop(L);
	
	// send to system
	luaav3_Node_send((luaav3_Node *)self);
	// initialize userdata
	luaav3_Synth_push(L, &self->synth, env,  "luaav_audio_Vamp_meta");
	return 1;
}


int loadUpPlugin(lua_State * L) {
	lua_pushvalue(L, lua_upvalueindex(1));
	lua_insert(L, 1);
	return loadPlugin(L);
}

int
pluginPaths(lua_State * L)
{
    vector<string> path = PluginHostAdapter::getPluginPath();
	lua_createtable(L, path.size(), 0);
    for (size_t i = 0; i < path.size(); ++i) {
		lua_pushstring(L, path[i].data());
		lua_rawseti(L, -2, i+1);
    }
	return 1;
}

int
enumeratePlugins(lua_State * L)
{
    PluginLoader *loader = PluginLoader::getInstance();
	
	vector<PluginLoader::PluginKey> plugins = loader->listPlugins();
    lua_createtable(L, plugins.size(), 0);
	int plugins_table = lua_gettop(L);
	
    string prevPath = "";
    int index = 0;
	
	for (size_t i = 0; i < plugins.size(); ++i) {
		lua_newtable(L);
		int plugin_table = lua_gettop(L);
		
		PluginLoader::PluginKey key = plugins[i];
		lua_pushstring(L, key.data());	
			lua_pushvalue(L, -1);
			lua_pushcclosure(L, loadUpPlugin, 1);	
			lua_setfield(L, plugin_table, "load");
		lua_setfield(L, plugin_table, "key");
		
        string path = loader->getLibraryPathForPlugin(plugins[i]);
		lua_pushstring(L, path.data());	lua_setfield(L, plugin_table, "path");
		
		if (path != prevPath) {
            prevPath = path;
            index = 0;
        }
		lua_pushinteger(L, index);
		lua_setfield(L, plugin_table, "index");
		
        Plugin * plugin = loader->loadPlugin(key, 48000);
        if (plugin) {
		
			char c = char('A' + index);
            if (c > 'Z') c = char('a' + (index - 26));

            PluginLoader::PluginCategoryHierarchy category =
                loader->getPluginCategory(key);
            string catstr;
            if (!category.empty()) {
                for (size_t ci = 0; ci < category.size(); ++ci) {
                    if (ci > 0) catstr += " > ";
                        catstr += category[ci];
                }
            }
		
			lua_pushstring(L, plugin->getName().data());	
			lua_setfield(L, plugin_table, "name");
			lua_pushinteger(L, plugin->getPluginVersion());	
			lua_setfield(L, plugin_table, "plugin_version");
			lua_pushinteger(L, plugin->getVampApiVersion());	
			lua_setfield(L, plugin_table, "API_version");

			lua_pushstring(L, plugin->getDescription().data());	
			lua_setfield(L, plugin_table, "description");
			lua_pushstring(L, plugin->getCopyright().data());	
			lua_setfield(L, plugin_table, "copyright");
			lua_pushstring(L, plugin->getMaker().data());	
			lua_setfield(L, plugin_table, "maker");

			lua_pushstring(L, (plugin->getInputDomain() == Vamp::Plugin::TimeDomain ? "Time" : "Frequency"));	
			lua_setfield(L, plugin_table, "input_domain");
			lua_pushinteger(L, plugin->getPreferredStepSize());	
			lua_setfield(L, plugin_table, "step_size");
			lua_pushinteger(L, plugin->getPreferredBlockSize());	
			lua_setfield(L, plugin_table, "block_size");

			lua_pushinteger(L, plugin->getMinChannelCount());	
			lua_setfield(L, plugin_table, "min_channels");
			lua_pushinteger(L, plugin->getMaxChannelCount());	
			lua_setfield(L, plugin_table, "max_channels");
			
			Plugin::OutputList outputs = plugin->getOutputDescriptors();

			Plugin::ParameterList params = plugin->getParameterDescriptors();
			lua_createtable(L, params.size(), 0);
			int params_table = lua_gettop(L);
			
			for (size_t j = 0; j < params.size(); ++j) {
				Plugin::ParameterDescriptor &pd(params[j]);
				
				lua_newtable(L);
				int param_table = lua_gettop(L);
				
				lua_pushstring(L, pd.name.data());
				lua_setfield(L, param_table, "name");
				lua_pushstring(L, pd.identifier.data());
				lua_setfield(L, param_table, "identifier");
				lua_pushstring(L, pd.description.data());
				lua_setfield(L, param_table, "description");
				lua_pushstring(L, pd.unit.data());
				lua_setfield(L, param_table, "unit");
				lua_pushnumber(L, pd.minValue);
				lua_setfield(L, param_table, "minValue");
				lua_pushnumber(L, pd.maxValue);
				lua_setfield(L, param_table, "maxValue");				
				lua_pushnumber(L, pd.defaultValue);
				lua_setfield(L, param_table, "defaultValue");
				lua_pushboolean(L, pd.isQuantized);
				lua_setfield(L, param_table, "isQuantized");
				lua_pushnumber(L, pd.quantizeStep);
				lua_setfield(L, param_table, "quantizeStep");
				for (size_t k = 0; k < pd.valueNames.size(); ++k) {
					lua_pushstring(L, pd.valueNames[k].data());
					lua_rawseti(L, param_table, k+1);
				}
				
				lua_rawseti(L, params_table, j+1);
			}
			lua_setfield(L, plugin_table, "params");

			lua_createtable(L, outputs.size(), 0);
			int outputs_table = lua_gettop(L);
			
			for (size_t j = 0; j < outputs.size(); ++j) {				
				lua_newtable(L);
				int output_table = lua_gettop(L);
				
				Plugin::OutputDescriptor &od(outputs[j]);
				
				lua_pushstring(L, od.name.data());
				lua_setfield(L, output_table, "name");
				lua_pushstring(L, od.identifier.data());
				lua_setfield(L, output_table, "identifier");
				lua_pushstring(L, od.description.data());
				lua_setfield(L, output_table, "description");
				lua_pushstring(L, od.unit.data());
				lua_setfield(L, output_table, "unit");
				lua_pushboolean(L, od.hasFixedBinCount);
				lua_setfield(L, output_table, "hasFixedBinCount");
				lua_pushinteger(L, od.binCount);
				lua_setfield(L, output_table, "binCount");
				lua_pushboolean(L, od.hasKnownExtents);
				lua_setfield(L, output_table, "hasKnownExtents");
				lua_pushnumber(L, od.minValue);
				lua_setfield(L, output_table, "minValue");
				lua_pushnumber(L, od.maxValue);
				lua_setfield(L, output_table, "maxValue");
				lua_pushboolean(L, od.isQuantized);
				lua_setfield(L, output_table, "isQuantized");
				lua_pushnumber(L, od.quantizeStep);
				lua_setfield(L, output_table, "quantizeStep");
				lua_pushboolean(L, od.hasDuration);
				lua_setfield(L, output_table, "hasDuration");
				lua_pushstring(L, (od.sampleType ==
						 Plugin::OutputDescriptor::OneSamplePerStep ?
						 "OneSamplePerStep" :
						 od.sampleType ==
						 Plugin::OutputDescriptor::FixedSampleRate ?
						 "FixedSampleRate" :
						 "VariableSampleRate"));
				lua_setfield(L, output_table, "sampleType");
				lua_pushnumber(L, od.sampleRate);
				lua_setfield(L, output_table, "sampleRate");
				
				for (size_t k = 0; k < od.binNames.size(); ++k) {
					lua_pushstring(L, od.binNames[k].data());
					lua_rawseti(L, output_table, k+1);
				}
					 
				lua_rawseti(L, outputs_table, j+1);
			}
			lua_setfield(L, plugin_table, "outputs");

            ++index;
            delete plugin;
		}
		
		lua_rawseti(L, plugins_table, i+1);
    }
	return 1;
}

int luaopen_vamp(lua_State * L) {

		// define Pan2 metatable:
	struct luaL_reg luaav_audio_Vamp_meta[] = {
		{ "output", luaav3_Synth_setoutput },
		{ "setoutput", luaav3_Synth_setoutput },
		{ "amp", luaav3_Synth_setamp },
		{ "setamp", luaav3_Synth_setamp },
		{ "fadeout", luaav3_Synth_fadeout },
		{ "input", luaav_audio_Vamp_setinput },
		{ "setinput", luaav_audio_Vamp_setinput },
		{ NULL, NULL },
	};
	luaL_newmetatable(L, "luaav_audio_Vamp_meta");
		lua_pushvalue(L, -1);	lua_setfield(L, -2, "__index");
		lua_pushstring(L, "VampPlugin");	lua_setfield(L, -2, "name");
		luaL_register(L, NULL, luaav_audio_Vamp_meta);
	lua_pop(L, 1);

	luaL_Reg lib[] = {
		{ NULL, NULL }
	};
	luaL_register(L, "vamp", lib);
	
	// get list of plugins:
	enumeratePlugins(L);
	lua_setfield(L, -2, "plugins");
	
	pluginPaths(L);
	lua_setfield(L, -2, "paths");
	
	#define REGISTER_ENUM(N)	lua_pushinteger(L, N); lua_setfield(L, -2, #N);
	REGISTER_ENUM(PluginIds)
	REGISTER_ENUM(PluginOutputIds)
	REGISTER_ENUM(PluginInformation)
	REGISTER_ENUM(PluginInformationDetailed)
	
	REGISTER_ENUM(VAMP_API_VERSION)
	lua_pushstring(L, VAMP_SDK_VERSION); lua_setfield(L, -2, "VAMP_SDK_VERSION");
	
	return 1;
}



