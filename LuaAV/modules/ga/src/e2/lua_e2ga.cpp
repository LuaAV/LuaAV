#include "lua_e2ga.h"

const char * name_from_tag(E2Tag tag) {
	switch(tag) {
		case MV_TAG:		return "mv";
		case E1_TAG:		return "e1";
		case E2_TAG:		return "e2";
		case I2_TAG:		return "I2";
		case I2i_TAG:		return "I2i";
		case VECTOR_TAG:	return "vector";
		case BIVECTOR_TAG:	return "bivector";
		case ROTOR_TAG:		return "rotor";
		default:
			return "<invalid tag>";
	}
}

int e2elem_scalar(lua_State *L) {
	E2Elem *elem = Glue<E2Elem>::checkto(L, 1);
	switch(elem->tag) {
		case MV_TAG:		lua::push<double>(L, ((mv *)(elem->elem))->get_scalar()); break;
		case E1_TAG:		lua::push<double>(L, 0.); break;
		case E2_TAG:		lua::push<double>(L, 0.); break;
		case I2_TAG:		lua::push<double>(L, 0.); break;
		case I2i_TAG:		lua::push<double>(L, 0.); break;
		case VECTOR_TAG:	lua::push<double>(L, 0.); break;
		case BIVECTOR_TAG:	lua::push<double>(L, 0.); break;
		case ROTOR_TAG:		lua::push<double>(L, ((rotor *)(elem->elem))->get_scalar()); break;
	}
	return 1;
}

int e2elem_e1(lua_State *L) {
	E2Elem *elem = Glue<E2Elem>::checkto(L, 1);
	switch(elem->tag) {
		case MV_TAG:		lua::push<double>(L, ((mv *)(elem->elem))->get_e1()); break;
		case E1_TAG:		lua::push<double>(L, ((e1_t *)(elem->elem))->get_e1()); break;
		case E2_TAG:		lua::push<double>(L, 0.); break;
		case I2_TAG:		lua::push<double>(L, 0.); break;
		case I2i_TAG:		lua::push<double>(L, 0.); break;
		case VECTOR_TAG:	lua::push<double>(L, ((vector *)(elem->elem))->get_e1()); break;
		case BIVECTOR_TAG:	lua::push<double>(L, 0.); break;
		case ROTOR_TAG:		lua::push<double>(L, 0.); break;
	}
	return 1;
}

int e2elem_e2(lua_State *L) {
	E2Elem *elem = Glue<E2Elem>::checkto(L, 1);
	switch(elem->tag) {
		case MV_TAG:		lua::push<double>(L, ((mv *)(elem->elem))->get_e2()); break;
		case E1_TAG:		lua::push<double>(L, 0.); break;
		case E2_TAG:		lua::push<double>(L, ((e2_t *)(elem->elem))->get_e2()); break;
		case I2_TAG:		lua::push<double>(L, 0.); break;
		case I2i_TAG:		lua::push<double>(L, 0.); break;
		case VECTOR_TAG:	lua::push<double>(L, ((vector *)(elem->elem))->get_e2()); break;
		case BIVECTOR_TAG:	lua::push<double>(L, 0.); break;
		case ROTOR_TAG:		lua::push<double>(L, 0.); break;
	}
	return 1;
}

int e2elem_e1_e2(lua_State *L) {
	E2Elem *elem = Glue<E2Elem>::checkto(L, 1);
	switch(elem->tag) {
		case MV_TAG:		lua::push<double>(L, ((mv *)(elem->elem))->get_e1_e2()); break;
		case E1_TAG:		lua::push<double>(L, 0.); break;
		case E2_TAG:		lua::push<double>(L, 0.); break;
		case I2_TAG:		lua::push<double>(L, ((I2_t *)(elem->elem))->get_e1_e2()); break;
		case I2i_TAG:		lua::push<double>(L, ((I2i_t *)(elem->elem))->get_e1_e2()); break;
		case VECTOR_TAG:	lua::push<double>(L, 0.); break;
		case BIVECTOR_TAG:	lua::push<double>(L, ((bivector *)(elem->elem))->get_e1_e2()); break;
		case ROTOR_TAG:		lua::push<double>(L, ((rotor *)(elem->elem))->get_e1_e2()); break;
	}
	return 1;
}


int e2elem_tag(lua_State *L) {
	E2Elem *elem = Glue<E2Elem>::checkto(L, 1);
	lua::push<int>(L, elem->tag);
	return 1;
}

int e2elem_name(lua_State *L) {
	E2Elem *elem = Glue<E2Elem>::checkto(L, 1);
	lua::push<const char *>(L, name_from_tag(elem->tag));
	return 1;
}

int e2elem_description(lua_State *L) {
	E2Elem *elem = Glue<E2Elem>::checkto(L, 1);
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
			
		case I2_TAG:
			lua::push<const char *>(
				L, 
				((I2_t *)elem->elem)->c_str_f()
			);
			break;
			
		case I2i_TAG:
			lua::push<const char *>(
				L, 
				((I2i_t *)elem->elem)->c_str_f()
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
			
		case ROTOR_TAG:
			lua::push<const char *>(
				L, 
				((rotor *)elem->elem)->c_str_f()
			);
			break;
	}
	return 1;
}


template<> const char * Glue<E2Elem>::usr_name() { return "E2Elem"; }
template<> bool Glue<E2Elem>::usr_has_index() { return true; }

template<> void Glue<E2Elem>::usr_push(lua_State * L, E2Elem * u) {
	Glue<E2Elem>::usr_attr_push(L);
	Glue<E2Elem>::usr_attr_prototype(L);
}

template<> void Glue<E2Elem>::usr_index(lua_State * L, E2Elem * u) {
	Glue<E2Elem>::usr_attr_index(L);
}

template<> void Glue<E2Elem>::usr_newindex(lua_State * L, E2Elem * u) {
	Glue<E2Elem>::usr_attr_newindex(L);
}

template<> int Glue<E2Elem>::usr_tostring(lua_State * L, E2Elem * u) {
	lua_pushfstring(L, "%s: %p", Glue<E2Elem>::usr_name(), u); 
	return 1;
}

template<> void Glue<E2Elem>::usr_gc(lua_State * L, E2Elem * u) { 
	delete u;
}

template<> void Glue<E2Elem>::usr_mt(lua_State * L) {
	static luaL_reg methods[] = {
		{ NULL, NULL}
	};
	static luaL_reg getters[] = {
		{ "scalar", e2elem_scalar },
		{ "e1", e2elem_e1 },
		{ "e2", e2elem_e2 },
		{ "e1_e2", e2elem_e1_e2 },
		{ "tag", e2elem_tag },
		{ "name", e2elem_name },
		{ "description", e2elem_description },
		{ NULL, NULL}
	};
	static luaL_reg setters[] = {
		{ NULL, NULL}
	};
	Glue<E2Elem>::usr_attr_mt(L, methods, getters, setters);
}



int e2space_e1(lua_State *L) {
	E2Elem *elem = new E2Elem(
		E1_TAG,
		new e1_t()
	);
	Glue<E2Elem>::push(L, elem);
	return 1;
}

int e2space_e2(lua_State *L) {
	E2Elem *elem = new E2Elem(
		E2_TAG,
		new e2_t()
	);
	Glue<E2Elem>::push(L, elem);
	return 1;
}

int e2space_vector(lua_State *L) {
	double vals[2] = {0, 0};
	lua::to_vec<double>(L, 1, 2, vals);

	E2Elem *elem = new E2Elem(
		VECTOR_TAG,
		new vector(
			vector::coord_e1_e2, 
			vals
		)
	);
	Glue<E2Elem>::push(L, elem);
	return 1;
}

int e2space_I2(lua_State *L) {
	E2Elem *elem = new E2Elem(
		I2_TAG,
		new I2_t()
	);
	Glue<E2Elem>::push(L, elem);
	return 1;
}


int e2space_mv(lua_State *L) {
	E2Elem *e = Glue<E2Elem>::checkto(L, 1);
	E2Elem *elem = 0;
	
	switch(e->tag) {
		case MV_TAG:	
			elem = new E2Elem(
				MV_TAG,
				new mv(*(mv *)(e->elem))
			);
			break;
			
		case E1_TAG:	
			elem = new E2Elem(
				MV_TAG,
				new mv(*(e1_t *)(e->elem))
			);
			break;
			
		case E2_TAG:	
			elem = new E2Elem(
				MV_TAG,
				new mv(*(e2_t *)(e->elem))
			);
			break;
			
		case I2_TAG:	
			elem = new E2Elem(
				MV_TAG,
				new mv(*(I2_t *)(e->elem))
			);
			break;
			
		case I2i_TAG:	
			elem = new E2Elem(
				MV_TAG,
				new mv(*(I2i_t *)(e->elem))
			);
			break;
			
		case VECTOR_TAG:	
			elem = new E2Elem(
				MV_TAG,
				new mv(*(vector *)(e->elem))
			);
			break;
			
		case BIVECTOR_TAG:	
			elem = new E2Elem(
				MV_TAG,
				new mv(*(bivector *)(e->elem))
			);
			break;
			
		case ROTOR_TAG:	
			elem = new E2Elem(
				MV_TAG,
				new mv(*(rotor *)(e->elem))
			);
			break;
	}

	Glue<E2Elem>::push(L, elem);
	return 1;
}



#ifdef __cplusplus
extern "C" {
#endif

// ops
extern int e2space_vinv(lua_State *L);
extern int e2space_reverse(lua_State *L);
extern int e2space_unit(lua_State *L);
extern int e2space_exp(lua_State *L);

// ops_gen
extern int e2space_add(lua_State *L);
extern int e2space_subtract(lua_State *L);
extern int e2space_hip(lua_State *L);
extern int e2space_lc(lua_State *L);
extern int e2space_op(lua_State *L);
extern int e2space_gp(lua_State *L);


int luaopen_ga_e2(lua_State *L) {
	const char *libname = lua_tostring(L, -1);
	static const luaL_reg ga_e2_lib[] = {
		{ "e1", e2space_e1 },
		{ "e2", e2space_e2 },
		{ "vector", e2space_vector },
		{ "I2", e2space_I2 },
		{ "mv", e2space_mv },
		
		{ "vinv", e2space_vinv },
		{ "reverse", e2space_reverse },
		{ "unit", e2space_unit },
		{ "exp", e2space_exp },
		
		{ "add", e2space_add },
		{ "sub", e2space_subtract },
		{ "hip", e2space_hip },
		{ "lc", e2space_lc },
		{ "op", e2space_op },
		{ "gp", e2space_gp },
		{NULL, NULL}
	};
	luaL_register(L, libname, ga_e2_lib);
	
	lua::pushfield<int>(L, -1, "dim", e2ga_spaceDim);
	lua::pushfield<int>(L, -1, "ngroups", e2ga_nbGroups);
	
	lua::push_vec_t<const char *>(L, e2ga_spaceDim, e2ga_basisVectorNames);
	lua_setfield(L, -2, "basis");
	
	Glue<E2Elem>::define(L);
	
	return 1;
}

#ifdef __cplusplus
}
#endif
