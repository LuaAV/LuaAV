#include "lua_e3ga.h"

const char * name_from_tag(E3Tag tag) {
	switch(tag) {
		case MV_TAG:		return "mv";
		case E1_TAG:		return "e1";
		case E2_TAG:		return "e2";
		case E3_TAG:		return "e3";
		case I3_TAG:		return "I3";
		case VECTOR_TAG:	return "vector";
		case BIVECTOR_TAG:	return "bivector";
		case TRIVECTOR_TAG:	return "trivector";
		case ROTOR_TAG:		return "rotor";
		case ODDVERSOR_TAG:	return "oddVersor";
		default:
			return "<invalid tag>";
	}
}

int e3elem_scalar(lua_State *L) {
	E3Elem *elem = Glue<E3Elem>::checkto(L, 1);
	switch(elem->tag) {
		case MV_TAG:		lua::push<double>(L, ((mv *)(elem->elem))->get_scalar()); break;
		case E1_TAG:		lua::push<double>(L, 0.); break;
		case E2_TAG:		lua::push<double>(L, 0.); break;
		case E3_TAG:		lua::push<double>(L, 0.); break;
		case I3_TAG:		lua::push<double>(L, 0.); break;
		case VECTOR_TAG:	lua::push<double>(L, 0.); break;
		case BIVECTOR_TAG:	lua::push<double>(L, 0.); break;
		case TRIVECTOR_TAG:	lua::push<double>(L, 0.); break;
		case ROTOR_TAG:		lua::push<double>(L, ((rotor *)(elem->elem))->get_scalar()); break;
		case ODDVERSOR_TAG:	lua::push<double>(L, 0.); break;
	}
	return 1;
}

int e3elem_e1(lua_State *L) {
	E3Elem *elem = Glue<E3Elem>::checkto(L, 1);
	switch(elem->tag) {
		case MV_TAG:		lua::push<double>(L, ((mv *)(elem->elem))->get_e1()); break;
		case E1_TAG:		lua::push<double>(L, ((e1_t *)(elem->elem))->get_e1()); break;
		case E2_TAG:		lua::push<double>(L, 0.); break;
		case E3_TAG:		lua::push<double>(L, 0.); break;
		case I3_TAG:		lua::push<double>(L, 0.); break;
		case VECTOR_TAG:	lua::push<double>(L, ((vector *)(elem->elem))->get_e1()); break;
		case BIVECTOR_TAG:	lua::push<double>(L, 0.); break;
		case TRIVECTOR_TAG:	lua::push<double>(L, 0.); break;
		case ROTOR_TAG:		lua::push<double>(L, 0.); break;
		case ODDVERSOR_TAG:	lua::push<double>(L, ((oddVersor *)(elem->elem))->get_e1()); break;
	}
	return 1;
}

int e3elem_e2(lua_State *L) {
	E3Elem *elem = Glue<E3Elem>::checkto(L, 1);
	switch(elem->tag) {
		case MV_TAG:		lua::push<double>(L, ((mv *)(elem->elem))->get_e2()); break;
		case E1_TAG:		lua::push<double>(L, 0.); break;
		case E2_TAG:		lua::push<double>(L, ((e2_t *)(elem->elem))->get_e2()); break;
		case E3_TAG:		lua::push<double>(L, 0.); break;
		case I3_TAG:		lua::push<double>(L, 0.); break;
		case VECTOR_TAG:	lua::push<double>(L, ((vector *)(elem->elem))->get_e2()); break;
		case BIVECTOR_TAG:	lua::push<double>(L, 0.); break;
		case TRIVECTOR_TAG:	lua::push<double>(L, 0.); break;
		case ROTOR_TAG:		lua::push<double>(L, 0.); break;
		case ODDVERSOR_TAG:	lua::push<double>(L, ((oddVersor *)(elem->elem))->get_e2()); break;
	}
	return 1;
}

int e3elem_e3(lua_State *L) {
	E3Elem *elem = Glue<E3Elem>::checkto(L, 1);
	switch(elem->tag) {
		case MV_TAG:		lua::push<double>(L, ((mv *)(elem->elem))->get_e3()); break;
		case E1_TAG:		lua::push<double>(L, 0.); break;
		case E2_TAG:		lua::push<double>(L, 0.); break;
		case E3_TAG:		lua::push<double>(L, ((e3_t *)(elem->elem))->get_e3()); break;
		case I3_TAG:		lua::push<double>(L, 0.); break;
		case VECTOR_TAG:	lua::push<double>(L, ((vector *)(elem->elem))->get_e3()); break;
		case BIVECTOR_TAG:	lua::push<double>(L, 0.); break;
		case TRIVECTOR_TAG:	lua::push<double>(L, 0.); break;
		case ROTOR_TAG:		lua::push<double>(L, 0.); break;
		case ODDVERSOR_TAG:	lua::push<double>(L, ((oddVersor *)(elem->elem))->get_e3()); break;
	}
	return 1;
}

int e3elem_vector(lua_State *L) {
	E3Elem *elem = Glue<E3Elem>::checkto(L, 1);
	double vec[] = {0, 0, 0};
	
	switch(elem->tag) {
		case MV_TAG: {
			mv &v = *((mv *)(elem->elem));
			vec[0] = v.get_e1();
			vec[1] = v.get_e2();
			vec[2] = v.get_e3();
		} break;
		
		case E1_TAG: {
			e1_t &v = *((e1_t *)(elem->elem));
			vec[0] = v.get_e1();
			vec[1] = 0.;
			vec[2] = 0.;
		} break;
		
		case E2_TAG: {
			e2_t &v = *((e2_t *)(elem->elem));
			vec[0] = 0.;
			vec[1] = v.get_e2();
			vec[2] = 0.;
		} break;
		
		case E3_TAG: {
			e3_t &v = *((e3_t *)(elem->elem));
			vec[0] = 0.;
			vec[1] = 0.;
			vec[2] = v.get_e3();
		} break;
		
		case VECTOR_TAG: {
			vector &v = *((vector *)(elem->elem));
			vec[0] = v.get_e1();
			vec[1] = v.get_e2();
			vec[2] = v.get_e3();
		} break;
		
		case ODDVERSOR_TAG: {
			oddVersor &v = *((oddVersor *)(elem->elem));
			vec[0] = v.get_e1();
			vec[1] = v.get_e2();
			vec[2] = v.get_e3();
		} break;
	}
	
	lua::push_vec_t<double>(L, 3, vec);
	return 1;
}

/*
	i = e32
	j = e13
	k = e21
*/
int e3elem_bivector(lua_State *L) {
	E3Elem *elem = Glue<E3Elem>::checkto(L, 1);
	double bivec[] = {0, 0, 0};
	
	switch(elem->tag) {
		case MV_TAG: {
			mv &v = *((mv *)(elem->elem));
			bivec[0] = v.get_e1_e2();
			bivec[1] = v.get_e2_e3();
			bivec[2] = -v.get_e1_e3();
		} break;
		
		case BIVECTOR_TAG: {
			bivector &v = *((bivector *)(elem->elem));
			bivec[0] = v.get_e1_e2();
			bivec[1] = v.get_e2_e3();
			bivec[2] = v.get_e3_e1();
		} break;
		
		case ROTOR_TAG: {
			rotor &v = *((rotor *)(elem->elem));
			bivec[0] = v.get_e1_e2();
			bivec[1] = v.get_e2_e3();
			bivec[2] = v.get_e3_e1();
		} break;
	}
	
	lua::push_vec_t<double>(L, 3, bivec);
	return 1;
}

int e3elem_trivector(lua_State *L) {
	E3Elem *elem = Glue<E3Elem>::checkto(L, 1);
	
	switch(elem->tag) {
		case MV_TAG: {
			mv &v = *((mv *)(elem->elem));
			lua::push<double>(L, v.get_e1_e2_e3());
		} break;
		
		case TRIVECTOR_TAG: {
			trivector &v = *((trivector *)(elem->elem));
			lua::push<double>(L, v.get_e1_e2_e3());
		} break;
		
		case ODDVERSOR_TAG: {
			oddVersor &v = *((oddVersor *)(elem->elem));
			lua::push<double>(L, v.get_e1_e2_e3());
		} break;
		
		default:
			lua::push<double>(L, 0.);
			break;
	}
	
	return 1;
}


int e3elem_tag(lua_State *L) {
	E3Elem *elem = Glue<E3Elem>::checkto(L, 1);
	lua::push<int>(L, elem->tag);
	return 1;
}

int e3elem_name(lua_State *L) {
	E3Elem *elem = Glue<E3Elem>::checkto(L, 1);
	lua::push<const char *>(L, name_from_tag(elem->tag));
	return 1;
}

int e3elem_description(lua_State *L) {
	E3Elem *elem = Glue<E3Elem>::checkto(L, 1);
	switch(elem->tag) {
		case MV_TAG:
			lua::push<const char *>(
				L, 
				((mv *)elem->elem)->c_str_f()
			);
			break;
			
		case E1_TAG:
			lua::push<const char *>(
				L, 
				((e1_t *)elem->elem)->c_str_f()
			);
			break;
			
		case E2_TAG:
			lua::push<const char *>(
				L, 
				((e2_t *)elem->elem)->c_str_f()
			);
			break;
			
		case E3_TAG:
			lua::push<const char *>(
				L, 
				((e3_t *)elem->elem)->c_str_f()
			);
			break;
			
		case I3_TAG:
			lua::push<const char *>(
				L, 
				((I3_t *)elem->elem)->c_str_f()
			);
			break;
			
		case VECTOR_TAG:
			lua::push<const char *>(
				L, 
				((vector *)elem->elem)->c_str_f()
			);
			break;
			
		case BIVECTOR_TAG:
			lua::push<const char *>(
				L, 
				((bivector *)elem->elem)->c_str_f()
			);
			break;
			
		case TRIVECTOR_TAG:
			lua::push<const char *>(
				L, 
				((trivector *)elem->elem)->c_str_f()
			);
			break;
			
		case ROTOR_TAG:
			lua::push<const char *>(
				L, 
				((rotor *)elem->elem)->c_str_f()
			);
			break;
			
		case ODDVERSOR_TAG:
			lua::push<const char *>(
				L, 
				((oddVersor *)elem->elem)->c_str_f()
			);
			break;
	}
	return 1;
}


template<> const char * Glue<E3Elem>::usr_name() { return "E3Elem"; }
template<> bool Glue<E3Elem>::usr_has_index() { return true; }

template<> void Glue<E3Elem>::usr_push(lua_State * L, E3Elem * u) {
	Glue<E3Elem>::usr_attr_push(L);
	Glue<E3Elem>::usr_attr_prototype(L);
}

template<> void Glue<E3Elem>::usr_index(lua_State * L, E3Elem * u) {
	Glue<E3Elem>::usr_attr_index(L);
}

template<> void Glue<E3Elem>::usr_newindex(lua_State * L, E3Elem * u) {
	Glue<E3Elem>::usr_attr_newindex(L);
}

template<> int Glue<E3Elem>::usr_tostring(lua_State * L, E3Elem * u) {
	lua_pushfstring(L, "%s: %p", Glue<E3Elem>::usr_name(), u); 
	return 1;
}

template<> void Glue<E3Elem>::usr_gc(lua_State * L, E3Elem * u) { 
	delete u;
}

template<> void Glue<E3Elem>::usr_mt(lua_State * L) {
	static luaL_reg methods[] = {
		{ NULL, NULL}
	};
	static luaL_reg getters[] = {
		{ "scalar", e3elem_scalar },
		{ "e1", e3elem_e1 },
		{ "e2", e3elem_e2 },
		{ "e3", e3elem_e3 },
		{ "vector", e3elem_vector },
		{ "bivector", e3elem_bivector },
		{ "trivector", e3elem_trivector },
		{ "tag", e3elem_tag },
		{ "name", e3elem_name },
		{ "description", e3elem_description },
		{ NULL, NULL}
	};
	static luaL_reg setters[] = {
		{ NULL, NULL}
	};
	Glue<E3Elem>::usr_attr_mt(L, methods, getters, setters);
}



int e3space_e1(lua_State *L) {
	E3Elem *elem = new E3Elem(
		E1_TAG,
		new e1_t()
	);
	Glue<E3Elem>::push(L, elem);
	return 1;
}

int e3space_e2(lua_State *L) {
	E3Elem *elem = new E3Elem(
		E2_TAG,
		new e2_t()
	);
	Glue<E3Elem>::push(L, elem);
	return 1;
}

int e3space_e3(lua_State *L) {
	E3Elem *elem = new E3Elem(
		E3_TAG,
		new e3_t()
	);
	Glue<E3Elem>::push(L, elem);
	return 1;
}

int e3space_vector(lua_State *L) {
	double vals[3] = {0, 0, 0};
	lua::to_vec<double>(L, 1, 3, vals);

	E3Elem *elem = new E3Elem(
		VECTOR_TAG,
		new vector(
			vector::coord_e1_e2_e3, 
			vals
		)
	);
	Glue<E3Elem>::push(L, elem);
	return 1;
}

int e3space_I3(lua_State *L) {
	E3Elem *elem = new E3Elem(
		I3_TAG,
		new I3_t()
	);
	Glue<E3Elem>::push(L, elem);
	return 1;
}



#ifdef __cplusplus
extern "C" {
#endif

// ops
extern int e3space_vinv(lua_State *L);
extern int e3space_dual(lua_State *L);
extern int e3space_reverse(lua_State *L);

// ops_gen
extern int e3space_add(lua_State *L);
extern int e3space_subtract(lua_State *L);
extern int e3space_hip(lua_State *L);
extern int e3space_lc(lua_State *L);
extern int e3space_op(lua_State *L);
extern int e3space_gp(lua_State *L);


int luaopen_ga_e3(lua_State *L) {
	const char *libname = lua_tostring(L, -1);
	static const luaL_reg ga_e3_lib[] = {
		{ "e1", e3space_e1 },
		{ "e2", e3space_e2 },
		{ "e3", e3space_e3 },
		{ "vector", e3space_vector },
		{ "I3", e3space_I3 },
		
		{ "vinv", e3space_vinv },
		{ "dual", e3space_dual },
		{ "reverse", e3space_reverse },
		
		{ "add", e3space_add },
		{ "sub", e3space_subtract },
		{ "hip", e3space_hip },
		{ "lc", e3space_lc },
		{ "op", e3space_op },
		{ "gp", e3space_gp },
		{NULL, NULL}
	};
	luaL_register(L, libname, ga_e3_lib);
	
	lua::pushfield<int>(L, -1, "dim", e3ga_spaceDim);
	lua::pushfield<int>(L, -1, "ngroups", e3ga_nbGroups);
	
	lua::push_vec_t<const char *>(L, e3ga_spaceDim, e3ga_basisVectorNames);
	lua_setfield(L, -2, "basis");
	
	Glue<E3Elem>::define(L);
	
	return 1;
}

#ifdef __cplusplus
}
#endif
