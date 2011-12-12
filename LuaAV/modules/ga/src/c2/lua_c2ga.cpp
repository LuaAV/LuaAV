#include "lua_c2ga.h"

const char * name_from_tag(C2Tag tag) {
	switch(tag) {
		case MV_TAG:		return "mv";
		case NO_TAG:		return "no";
		case E1_TAG:		return "e1";
		case E2_TAG:		return "e2";
		case NI_TAG:		return "ni";
		case NONI_TAG:		return "noni";
		case I2_TAG:		return "I2";
		case I4_TAG:		return "I4";
		case VECTORE2GA_TAG:	return "e2vector";
		case BIVECTORE2GA_TAG:	return "e2bivector";
		case ROTORE2GA_TAG:	return "e2rotor";
		case NORMALIZEDPOINT_TAG:	return "normalizedPoint";
		case DUALCIRCLE_TAG:	return "dualCircle";
		case FREEVECTOR_TAG:	return "freeVector";
		case FREEBIVECTOR_TAG:	return "freeBivector";
		case CIRCLE_TAG:		return "circle";
		case LINE_TAG:			return "line";
		case PSEUDOSCALAR_TAG:	return "pseudoscalar";
		case NORMALIZEDTRANSLATOR_TAG:	return "normalizedTranslator";
		case TRANSLATOR_TAG:	return "translator";
		case EVENVERSOR_TAG:	return "evenVersor";
		case ODDVERSOR_TAG:		return "oddVersor";
		default:
			return "<invalid tag>";
	}
}

int c2elem_scalar(lua_State *L) {
	C2Elem *elem = Glue<C2Elem>::checkto(L, 1);
	switch(elem->tag) {
		case MV_TAG:			lua::push<double>(L, ((mv *)(elem->elem))->get_scalar()); break;
		case ROTORE2GA_TAG:	lua::push<double>(L, ((rotorE2GA *)(elem->elem))->get_scalar()); break;
		case EVENVERSOR_TAG:	lua::push<double>(L, ((evenVersor *)(elem->elem))->get_scalar()); break;
		default:
			lua::push<double>(L, 0.);
			break;
	}
	return 1;
}

int c2elem_no(lua_State *L) {
	C2Elem *elem = Glue<C2Elem>::checkto(L, 1);
	switch(elem->tag) {
		case MV_TAG:			lua::push<double>(L, ((mv *)(elem->elem))->get_no()); break;
		case NO_TAG:			lua::push<double>(L, ((no_t *)(elem->elem))->get_no()); break;
		case ODDVERSOR_TAG:	lua::push<double>(L, ((oddVersor *)(elem->elem))->get_no()); break;
		default:
			lua::push<double>(L, 0.);
			break;
	}
	return 1;
}

int c2elem_e1(lua_State *L) {
	C2Elem *elem = Glue<C2Elem>::checkto(L, 1);
	switch(elem->tag) {
		case MV_TAG:			lua::push<double>(L, ((mv *)(elem->elem))->get_e1()); break;
		case E1_TAG:			lua::push<double>(L, ((e1_t *)(elem->elem))->get_e1()); break;
		case VECTORE2GA_TAG:	lua::push<double>(L, ((vectorE2GA *)(elem->elem))->get_e1()); break;
		case ODDVERSOR_TAG:	lua::push<double>(L, ((oddVersor *)(elem->elem))->get_e1()); break;
		default:
			lua::push<double>(L, 0.);
			break;
	}
	return 1;
}

int c2elem_e2(lua_State *L) {
	C2Elem *elem = Glue<C2Elem>::checkto(L, 1);
	switch(elem->tag) {
		case MV_TAG:			lua::push<double>(L, ((mv *)(elem->elem))->get_e2()); break;
		case E2_TAG:			lua::push<double>(L, ((e2_t *)(elem->elem))->get_e2()); break;
		case VECTORE2GA_TAG:	lua::push<double>(L, ((vectorE2GA *)(elem->elem))->get_e2()); break;
		case ODDVERSOR_TAG:	lua::push<double>(L, ((oddVersor *)(elem->elem))->get_e2()); break;
		default:
			lua::push<double>(L, 0.);
			break;
	}
	return 1;
}

int c2elem_ni(lua_State *L) {
	C2Elem *elem = Glue<C2Elem>::checkto(L, 1);
	switch(elem->tag) {
		case MV_TAG:			lua::push<double>(L, ((mv *)(elem->elem))->get_ni()); break;
		case NI_TAG:			lua::push<double>(L, ((ni_t *)(elem->elem))->get_ni()); break;
		case ODDVERSOR_TAG:	lua::push<double>(L, ((oddVersor *)(elem->elem))->get_ni()); break;
		default:
			lua::push<double>(L, 0.);
			break;
	}
	return 1;
}


int c2elem_tag(lua_State *L) {
	C2Elem *elem = Glue<C2Elem>::checkto(L, 1);
	lua::push<int>(L, elem->tag);
	return 1;
}

int c2elem_name(lua_State *L) {
	C2Elem *elem = Glue<C2Elem>::checkto(L, 1);
	lua::push<const char *>(L, name_from_tag(elem->tag));
	return 1;
}

int c2elem_description(lua_State *L) {
	C2Elem *elem = Glue<C2Elem>::checkto(L, 1);
	switch(elem->tag) {
		case MV_TAG:
			lua::push<const char *>(
				L, 
				((mv *)elem->elem)->c_str_f()
			);
			break;
			
		case NO_TAG:
			lua::push<const char *>(
				L, 
				((no_t *)elem->elem)->c_str_f()
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
			
		case NI_TAG:
			lua::push<const char *>(
				L, 
				((ni_t *)elem->elem)->c_str_f()
			);
			break;
			
		case NONI_TAG:
			lua::push<const char *>(
				L, 
				((noni_t *)elem->elem)->c_str_f()
			);
			break;
		
		case I2_TAG:
			lua::push<const char *>(
				L, 
				((I2_t *)elem->elem)->c_str_f()
			);
			break;
			
		case VECTORE2GA_TAG:
			lua::push<const char *>(
				L, 
				((vectorE2GA *)elem->elem)->c_str_f()
			);
			break;
			
		case BIVECTORE2GA_TAG:
			lua::push<const char *>(
				L, 
				((bivectorE2GA *)elem->elem)->c_str_f()
			);
			break;
			
		case ROTORE2GA_TAG:
			lua::push<const char *>(
				L, 
				((rotorE2GA *)elem->elem)->c_str_f()
			);
			break;
			
		case NORMALIZEDPOINT_TAG:
			lua::push<const char *>(
				L, 
				((rotorE2GA *)elem->elem)->c_str_f()
			);
			break;
			
		case DUALCIRCLE_TAG:
			lua::push<const char *>(
				L, 
				((dualCircle *)elem->elem)->c_str_f()
			);
			break;
			
		case FREEVECTOR_TAG:
			lua::push<const char *>(
				L, 
				((freeVector *)elem->elem)->c_str_f()
			);
			break;
			
		case FREEBIVECTOR_TAG:
			lua::push<const char *>(
				L, 
				((freeBivector *)elem->elem)->c_str_f()
			);
			break;
			
		case CIRCLE_TAG:
			lua::push<const char *>(
				L, 
				((circle *)elem->elem)->c_str_f()
			);
			break;
			
		case LINE_TAG:
			lua::push<const char *>(
				L, 
				((line *)elem->elem)->c_str_f()
			);
			break;
			
		case PSEUDOSCALAR_TAG:
			lua::push<const char *>(
				L, 
				((pseudoscalar *)elem->elem)->c_str_f()
			);
			break;
			
		case NORMALIZEDTRANSLATOR_TAG:
			lua::push<const char *>(
				L, 
				((normalizedTranslator *)elem->elem)->c_str_f()
			);
			break;
			
		case TRANSLATOR_TAG:
			lua::push<const char *>(
				L, 
				((translator *)elem->elem)->c_str_f()
			);
			break;
			
		case EVENVERSOR_TAG:
			lua::push<const char *>(
				L, 
				((evenVersor *)elem->elem)->c_str_f()
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


template<> const char * Glue<C2Elem>::usr_name() { return "C2Elem"; }
template<> bool Glue<C2Elem>::usr_has_index() { return true; }

template<> void Glue<C2Elem>::usr_push(lua_State * L, C2Elem * u) {
	Glue<C2Elem>::usr_attr_push(L);
	Glue<C2Elem>::usr_attr_prototype(L);
}

template<> void Glue<C2Elem>::usr_index(lua_State * L, C2Elem * u) {
	Glue<C2Elem>::usr_attr_index(L);
}

template<> void Glue<C2Elem>::usr_newindex(lua_State * L, C2Elem * u) {
	Glue<C2Elem>::usr_attr_newindex(L);
}

template<> int Glue<C2Elem>::usr_tostring(lua_State * L, C2Elem * u) {
	lua_pushfstring(L, "%s: %p", Glue<C2Elem>::usr_name(), u); 
	return 1;
}

template<> void Glue<C2Elem>::usr_gc(lua_State * L, C2Elem * u) { 
	delete u;
}

template<> void Glue<C2Elem>::usr_mt(lua_State * L) {
	static luaL_reg methods[] = {
		{ NULL, NULL}
	};
	static luaL_reg getters[] = {
		{ "scalar", c2elem_scalar },
		{ "no", c2elem_no },
		{ "e1", c2elem_e1 },
		{ "e2", c2elem_e2 },
		{ "ni", c2elem_ni },
		{ "tag", c2elem_tag },
		{ "name", c2elem_name },
		{ "description", c2elem_description },
		{ NULL, NULL}
	};
	static luaL_reg setters[] = {
		{ NULL, NULL}
	};
	Glue<C2Elem>::usr_attr_mt(L, methods, getters, setters);
}


int c2space_no(lua_State *L) {
	C2Elem *elem = new C2Elem(
		NO_TAG,
		new no_t()
	);
	Glue<C2Elem>::push(L, elem);
	return 1;
}

int c2space_e1(lua_State *L) {
	C2Elem *elem = new C2Elem(
		E1_TAG,
		new e1_t()
	);
	Glue<C2Elem>::push(L, elem);
	return 1;
}

int c2space_e2(lua_State *L) {
	C2Elem *elem = new C2Elem(
		E2_TAG,
		new e2_t()
	);
	Glue<C2Elem>::push(L, elem);
	return 1;
}

int c2space_ni(lua_State *L) {
	C2Elem *elem = new C2Elem(
		NI_TAG,
		new ni_t()
	);
	Glue<C2Elem>::push(L, elem);
	return 1;
}

int e2space_e2vector(lua_State *L) {
	double vals[2] = {0, 0};
	lua::to_vec<double>(L, 1, 2, vals);

	C2Elem *elem = new C2Elem(
		VECTORE2GA_TAG,
		new vectorE2GA(
			vectorE2GA::coord_e1_e2, 
			vals
		)
	);
	Glue<C2Elem>::push(L, elem);
	return 1;
}



#ifdef __cplusplus
extern "C" {
#endif
/*
// ops
extern int e2space_vinv(lua_State *L);

// ops_gen
extern int e2space_add(lua_State *L);
extern int e2space_subtract(lua_State *L);
extern int e2space_hip(lua_State *L);
extern int e2space_lc(lua_State *L);
extern int e2space_op(lua_State *L);
*/
extern int c2space_gp(lua_State *L);

int luaopen_ga_c2(lua_State *L) {
	const char *libname = lua_tostring(L, -1);
	static const luaL_reg ga_e2_lib[] = {
		{ "no", c2space_no },
		{ "e1", c2space_e1 },
		{ "e2", c2space_e2 },
		{ "ni", c2space_ni },
		{ "e2vector", e2space_e2vector },
		
		/*
		{ "vinv", e2space_vinv },
		
		{ "add", e2space_add },
		{ "sub", e2space_subtract },
		{ "hip", e2space_hip },
		{ "lc", e2space_lc },
		{ "op", e2space_op },
		*/
		{ "gp", c2space_gp },
		{NULL, NULL}
	};
	luaL_register(L, libname, ga_e2_lib);
	
	lua::pushfield<int>(L, -1, "dim", c2ga_spaceDim);
	lua::pushfield<int>(L, -1, "ngroups", c2ga_nbGroups);
	
	lua::push_vec_t<const char *>(L, c2ga_spaceDim, c2ga_basisVectorNames);
	lua_setfield(L, -2, "basis");
	
	Glue<C2Elem>::define(L);
	
	return 1;
}

#ifdef __cplusplus
}
#endif
