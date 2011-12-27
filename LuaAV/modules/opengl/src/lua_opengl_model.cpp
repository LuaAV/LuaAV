#include "lua_opengl_glue.h"
#include "graphics/al_Model.hpp"
using al::gfx::GraphicsBackendOpenGL;

using std::string;
using al::gfx::Mesh;
using al::gfx::OBJReader;
using al::gfx::GraphicsBackend;
using al::gfx::GraphicsBackendOpenGL;


int opengl_model_read(lua_State * L) {
	string ctx;
	
//	if(
//		Glue<Mesh>::usr_attr_is_prototype(L) && 
//		lua::checkfield<const char *>(L, 1, "ctx")
//	) {
//		ctx.assign(lua::tofield<const char *>(L, 1, "ctx"));
//	}
//	else 
	if(lua::is<const char *>(L, 1)) {
		ctx.assign(lua::to<const char *>(L, 1));
	}
	else {
		luaL_error(L, "Mesh(): missing context name");
	}
	
	const char * path = luaL_checkstring(L, 2);
	
	GraphicsBackend * backend = lua_opengl_get_backend(ctx.c_str());
	
	OBJReader objreader(path);
	OBJReader::GroupIterator iter = objreader.groupsBegin();
	
	// handle each group:
	int groupID = 1;
	lua_newtable(L);
	int groups = lua_gettop(L);
	while (iter != objreader.groupsEnd()) {
		
		OBJReader::Group& g = iter->second;
		//printf("group %s with %d indices\n", (iter->first).data(), g.indices.size());

		if (g.indices.size() > 0) {
			Mesh * mesh = objreader.createMesh(iter, backend);
			//printf("%p %s\n", mesh, g.material.data());
			if (mesh && objreader.materialsFind(g.material) != objreader.materialsEnd()) {
				// push mesh and material into Lua
				lua_newtable(L);
				// first mesh
				Glue<Mesh>::push(L, mesh);
				lua_setfield(L, -2, "mesh");
				
				// then material
				//Material * m = objreader.createMaterial(objreader.materialsFind(g.material));
				lua_pushstring(L, g.material.c_str());
				lua_setfield(L, -2, "material");
				
				lua_rawseti(L, groups, groupID);
			}
			else {
				//printf("could not find graphics data\n");
				lua_newtable(L);
				Glue<Mesh>::push(L, mesh);
				lua_setfield(L, -2, "mesh");
				lua_rawseti(L, groups, groupID);
			}
		}
		iter++;
		groupID++;
	}
	
	// now handle materials:
	lua_newtable(L);
	int mtls = lua_gettop(L);
	OBJReader::MtlIterator miter = objreader.materialsBegin();
	while (miter != objreader.materialsEnd()) {
		
		OBJReader::Mtl& mtl = miter->second;
		lua_newtable(L);
		
		lua::push_vec_t(L, 4, mtl.diffuse.components); lua_setfield(L, -2, "diffuse");
		lua::push_vec_t(L, 4, mtl.ambient.components); lua_setfield(L, -2, "ambient");
		lua::push_vec_t(L, 4, mtl.specular.components); lua_setfield(L, -2, "specular");
		
		lua_pushnumber(L, mtl.shininess); lua_setfield(L, -2, "shininess");
		lua_pushnumber(L, mtl.optical_density); lua_setfield(L, -2, "optical_density");
		
		lua_pushstring(L, mtl.diffuseMap.c_str()); lua_setfield(L, -2, "diffuseMap");
		lua_pushstring(L, mtl.ambientMap.c_str()); lua_setfield(L, -2, "ambientMap");
		lua_pushstring(L, mtl.specularMap.c_str()); lua_setfield(L, -2, "specularMap");
		lua_pushstring(L, mtl.bumpMap.c_str()); lua_setfield(L, -2, "bumpMap");
		
		lua_setfield(L, mtls, miter->first.c_str());
		miter++;
	}
	return 2;
};



#ifdef __cplusplus
extern "C" {
#endif

extern int luaopen_opengl_Mesh(lua_State *L);

int luaopen_opengl_model(lua_State *L) {
	const char *libname = lua_tostring(L, -1);
	
	#ifndef TABLE_ENTRY
	#define TABLE_ENTRY(PREFIX, NAME)		{ #NAME, PREFIX##_##NAME }
	#endif

	static luaL_reg const lib[] = {
		TABLE_ENTRY(opengl_model, read),
		{NULL, NULL}
	};
	luaL_register(L, libname, lib);
	
	lua::call<const char *>(L, luaopen_opengl_Mesh, "opengl.Mesh");
	lua_pop(L, 1);

	return 1;
}

#ifdef __cplusplus
}
#endif
