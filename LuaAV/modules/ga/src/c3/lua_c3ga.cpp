#include "lua_c3ga.h"

const char * name_from_tag(C3Tag tag) {
	switch(tag) {
		case MV_TAG:		return "mv";
		case NO_TAG:		return "no";
		case E1_TAG:		return "e1";
		case E2_TAG:		return "e2";
		case E3_TAG:		return "e3";
		case NI_TAG:		return "ni";
		case NONI_TAG:		return "noni";
		case I3_TAG:		return "I3";
		case I5_TAG:		return "I5";
		case I5i_TAG:		return "I5i";
		case VECTORE3GA_TAG:	return "e3vector";
		case BIVECTORE3GA_TAG:	return "e3bivector";
		case ROTORE3GA_TAG:		return "e3rotor";
		case NORMALIZEDPOINT_TAG:	return "normalizedPoint";
		case DUALSPHERE_TAG:	return "dualSphere";
		case DUALPLANE_TAG:		return "dualPlane";
		case FREEVECTOR_TAG:	return "freeVector";
		case FREEBIVECTOR_TAG:	return "freeBivector";
		case FLATPOINT_TAG:		return "flatPoint";
		case POINTPAIR_TAG:		return "pointPair";
		case LINE_TAG:			return "line";
		case CIRCLE_TAG:		return "circle";
		case SPHERE_TAG:		return "sphere";
		case PLANE_TAG:			return "plane";
		case PSEUDOSCALAR_TAG:	return "pseudoscalar";
		case NORMALIZEDTRANSLATOR_TAG:	return "normalizedTranslator";
		case TRANSLATOR_TAG:	return "translator";
		case RBM_TAG:			return "RBM";
		case EVENVERSOR_TAG:	return "evenVersor";
		case ODDVERSOR_TAG:		return "oddVersor";
		default:
			return "<invalid tag>";
	}
}

int c3elem_scalar(lua_State *L) {
	C3Elem *elem = Glue<C3Elem>::checkto(L, 1);
	switch(elem->tag) {
		case MV_TAG:		lua::push<double>(L, ((mv *)(elem->elem))->get_scalar()); break;
		case ROTORE3GA_TAG:	lua::push<double>(L, ((rotorE3GA *)(elem->elem))->get_scalar()); break;
		case NORMALIZEDTRANSLATOR_TAG:	lua::push<double>(L, ((normalizedTranslator *)(elem->elem))->get_scalar()); break;
		case TRANSLATOR_TAG:	lua::push<double>(L, ((translator *)(elem->elem))->get_scalar()); break;
		case RBM_TAG:	lua::push<double>(L, ((RBM *)(elem->elem))->get_scalar()); break;
		case EVENVERSOR_TAG:	lua::push<double>(L, ((evenVersor *)(elem->elem))->get_scalar()); break;
		
		default:
			lua::push<double>(L, 0.);
			
	}
	return 1;
}

int c3elem_e1(lua_State *L) {
	C3Elem *elem = Glue<C3Elem>::checkto(L, 1);
	switch(elem->tag) {
		case MV_TAG:			lua::push<double>(L, ((mv *)(elem->elem))->get_e1()); break;
		case E1_TAG:			lua::push<double>(L, ((e1_t *)(elem->elem))->get_e1()); break;
		case VECTORE3GA_TAG:	lua::push<double>(L, ((vectorE3GA *)(elem->elem))->get_e1()); break;
		case NORMALIZEDPOINT_TAG:	lua::push<double>(L, ((normalizedPoint *)(elem->elem))->get_e1()); break;
		case DUALSPHERE_TAG:	lua::push<double>(L, ((dualSphere *)(elem->elem))->get_e1()); break;
		case DUALPLANE_TAG:		lua::push<double>(L, ((dualPlane *)(elem->elem))->get_e1()); break;
		case ODDVERSOR_TAG:		lua::push<double>(L, ((oddVersor *)(elem->elem))->get_e1()); break;
		
		default:
			lua::push<double>(L, 0.);
			
	}
	return 1;
}

int c3elem_e2(lua_State *L) {
	C3Elem *elem = Glue<C3Elem>::checkto(L, 1);
	switch(elem->tag) {
		case MV_TAG:			lua::push<double>(L, ((mv *)(elem->elem))->get_e2()); break;
		case E2_TAG:			lua::push<double>(L, ((e2_t *)(elem->elem))->get_e2()); break;
		case VECTORE3GA_TAG:	lua::push<double>(L, ((vectorE3GA *)(elem->elem))->get_e2()); break;
		case NORMALIZEDPOINT_TAG:	lua::push<double>(L, ((normalizedPoint *)(elem->elem))->get_e2()); break;
		case DUALSPHERE_TAG:	lua::push<double>(L, ((dualSphere *)(elem->elem))->get_e2()); break;
		case DUALPLANE_TAG:		lua::push<double>(L, ((dualPlane *)(elem->elem))->get_e2()); break;
		case ODDVERSOR_TAG:		lua::push<double>(L, ((oddVersor *)(elem->elem))->get_e2()); break;
		
		default:
			lua::push<double>(L, 0.);
			
	}
	return 1;
}

int c3elem_e3(lua_State *L) {
	C3Elem *elem = Glue<C3Elem>::checkto(L, 1);
	switch(elem->tag) {
		case MV_TAG:			lua::push<double>(L, ((mv *)(elem->elem))->get_e3()); break;
		case E3_TAG:			lua::push<double>(L, ((e3_t *)(elem->elem))->get_e3()); break;
		case VECTORE3GA_TAG:	lua::push<double>(L, ((vectorE3GA *)(elem->elem))->get_e3()); break;
		case NORMALIZEDPOINT_TAG:	lua::push<double>(L, ((normalizedPoint *)(elem->elem))->get_e3()); break;
		case DUALSPHERE_TAG:	lua::push<double>(L, ((dualSphere *)(elem->elem))->get_e3()); break;
		case DUALPLANE_TAG:		lua::push<double>(L, ((dualPlane *)(elem->elem))->get_e3()); break;
		case ODDVERSOR_TAG:		lua::push<double>(L, ((oddVersor *)(elem->elem))->get_e3()); break;
		
		default:
			lua::push<double>(L, 0.);
			
	}
	return 1;
}

int c3elem_no_e1(lua_State *L) {
	C3Elem *elem = Glue<C3Elem>::checkto(L, 1);
	switch(elem->tag) {
		case MV_TAG:			lua::push<double>(L, ((mv *)(elem->elem))->get_no_e1()); break;
		case POINTPAIR_TAG:		lua::push<double>(L, ((pointPair *)(elem->elem))->get_no_e1()); break;
		case EVENVERSOR_TAG:	lua::push<double>(L, ((evenVersor *)(elem->elem))->get_no_e1()); break;
		
		default:
			lua::push<double>(L, 0.);
			
	}
	return 1;
}

int c3elem_no_e2(lua_State *L) {
	C3Elem *elem = Glue<C3Elem>::checkto(L, 1);
	switch(elem->tag) {
		case MV_TAG:			lua::push<double>(L, ((mv *)(elem->elem))->get_no_e2()); break;
		case POINTPAIR_TAG:		lua::push<double>(L, ((pointPair *)(elem->elem))->get_no_e2()); break;
		case EVENVERSOR_TAG:	lua::push<double>(L, ((evenVersor *)(elem->elem))->get_no_e2()); break;
		
		default:
			lua::push<double>(L, 0.);
			
	}
	return 1;
}

int c3elem_no_e3(lua_State *L) {
	C3Elem *elem = Glue<C3Elem>::checkto(L, 1);
	switch(elem->tag) {
		case MV_TAG:			lua::push<double>(L, ((mv *)(elem->elem))->get_no_e3()); break;
		case POINTPAIR_TAG:		lua::push<double>(L, ((pointPair *)(elem->elem))->get_no_e3()); break;
		case EVENVERSOR_TAG:	lua::push<double>(L, ((evenVersor *)(elem->elem))->get_no_e3()); break;
		
		default:
			lua::push<double>(L, 0.);
			
	}
	return 1;
}

int c3elem_e1_e2(lua_State *L) {
	C3Elem *elem = Glue<C3Elem>::checkto(L, 1);
	switch(elem->tag) {
		case MV_TAG:			lua::push<double>(L, ((mv *)(elem->elem))->get_e1_e2()); break;
		case BIVECTORE3GA_TAG:	lua::push<double>(L, ((bivectorE3GA *)(elem->elem))->get_e1_e2()); break;
		case ROTORE3GA_TAG:		lua::push<double>(L, ((rotorE3GA *)(elem->elem))->get_e1_e2()); break;
		case POINTPAIR_TAG:		lua::push<double>(L, ((pointPair *)(elem->elem))->get_e1_e2()); break;
		case RBM_TAG:			lua::push<double>(L, ((RBM *)(elem->elem))->get_e1_e2()); break;
		case EVENVERSOR_TAG:	lua::push<double>(L, ((evenVersor *)(elem->elem))->get_e1_e2()); break;
		
		default:
			lua::push<double>(L, 0.);
			
	}
	return 1;
}

int c3elem_e2_e3(lua_State *L) {
	C3Elem *elem = Glue<C3Elem>::checkto(L, 1);
	switch(elem->tag) {
		case MV_TAG:			lua::push<double>(L, ((mv *)(elem->elem))->get_e2_e3()); break;
		case BIVECTORE3GA_TAG:	lua::push<double>(L, ((bivectorE3GA *)(elem->elem))->get_e2_e3()); break;
		case ROTORE3GA_TAG:		lua::push<double>(L, ((rotorE3GA *)(elem->elem))->get_e2_e3()); break;
		case POINTPAIR_TAG:		lua::push<double>(L, ((pointPair *)(elem->elem))->get_e2_e3()); break;
		case RBM_TAG:			lua::push<double>(L, ((RBM *)(elem->elem))->get_e2_e3()); break;
		case EVENVERSOR_TAG:	lua::push<double>(L, ((evenVersor *)(elem->elem))->get_e2_e3()); break;
		
		default:
			lua::push<double>(L, 0.);
			
	}
	return 1;
}

int c3elem_e3_e1(lua_State *L) {
	C3Elem *elem = Glue<C3Elem>::checkto(L, 1);
	switch(elem->tag) {
		case MV_TAG:			lua::push<double>(L, -((mv *)(elem->elem))->get_e1_e3()); break;
		case BIVECTORE3GA_TAG:	lua::push<double>(L, ((bivectorE3GA *)(elem->elem))->get_e3_e1()); break;
		case ROTORE3GA_TAG:		lua::push<double>(L, ((rotorE3GA *)(elem->elem))->get_e3_e1()); break;
		case POINTPAIR_TAG:		lua::push<double>(L, ((pointPair *)(elem->elem))->get_e3_e1()); break;
		case RBM_TAG:			lua::push<double>(L, ((RBM *)(elem->elem))->get_e3_e1()); break;
		case EVENVERSOR_TAG:	lua::push<double>(L, ((evenVersor *)(elem->elem))->get_e3_e1()); break;
		
		default:
			lua::push<double>(L, 0.);
			
	}
	return 1;
}

int c3elem_e1_ni(lua_State *L) {
	C3Elem *elem = Glue<C3Elem>::checkto(L, 1);
	switch(elem->tag) {
		case MV_TAG:			lua::push<double>(L, ((mv *)(elem->elem))->get_e1_ni()); break;
		case FREEVECTOR_TAG:	lua::push<double>(L, ((freeVector *)(elem->elem))->get_e1_ni()); break;
		case FLATPOINT_TAG:		lua::push<double>(L, ((flatPoint *)(elem->elem))->get_e1_ni()); break;
		case POINTPAIR_TAG:		lua::push<double>(L, ((pointPair *)(elem->elem))->get_e1_ni()); break;
		case NORMALIZEDTRANSLATOR_TAG: lua::push<double>(L, ((normalizedTranslator *)(elem->elem))->get_e1_ni()); break;
		case TRANSLATOR_TAG:	lua::push<double>(L, ((translator *)(elem->elem))->get_e1_ni()); break;
		case RBM_TAG:			lua::push<double>(L, ((RBM *)(elem->elem))->get_e1_ni()); break;
		case EVENVERSOR_TAG:	lua::push<double>(L, ((evenVersor *)(elem->elem))->get_e1_ni()); break;
		
		default:
			lua::push<double>(L, 0.);
			
	}
	return 1;
}

int c3elem_e2_ni(lua_State *L) {
	C3Elem *elem = Glue<C3Elem>::checkto(L, 1);
	switch(elem->tag) {
		case MV_TAG:			lua::push<double>(L, ((mv *)(elem->elem))->get_e2_ni()); break;
		case FREEVECTOR_TAG:	lua::push<double>(L, ((freeVector *)(elem->elem))->get_e2_ni()); break;
		case FLATPOINT_TAG:		lua::push<double>(L, ((flatPoint *)(elem->elem))->get_e2_ni()); break;
		case POINTPAIR_TAG:		lua::push<double>(L, ((pointPair *)(elem->elem))->get_e2_ni()); break;
		case NORMALIZEDTRANSLATOR_TAG: lua::push<double>(L, ((normalizedTranslator *)(elem->elem))->get_e2_ni()); break;
		case TRANSLATOR_TAG:	lua::push<double>(L, ((translator *)(elem->elem))->get_e2_ni()); break;
		case RBM_TAG:			lua::push<double>(L, ((RBM *)(elem->elem))->get_e2_ni()); break;
		case EVENVERSOR_TAG:	lua::push<double>(L, ((evenVersor *)(elem->elem))->get_e2_ni()); break;
		
		default:
			lua::push<double>(L, 0.);
			
	}
	return 1;
}

int c3elem_e3_ni(lua_State *L) {
	C3Elem *elem = Glue<C3Elem>::checkto(L, 1);
	switch(elem->tag) {
		case MV_TAG:			lua::push<double>(L, ((mv *)(elem->elem))->get_e3_ni()); break;
		case FREEVECTOR_TAG:	lua::push<double>(L, ((freeVector *)(elem->elem))->get_e3_ni()); break;
		case FLATPOINT_TAG:		lua::push<double>(L, ((flatPoint *)(elem->elem))->get_e3_ni()); break;
		case POINTPAIR_TAG:		lua::push<double>(L, ((pointPair *)(elem->elem))->get_e3_ni()); break;
		case NORMALIZEDTRANSLATOR_TAG: lua::push<double>(L, ((normalizedTranslator *)(elem->elem))->get_e3_ni()); break;
		case TRANSLATOR_TAG:	lua::push<double>(L, ((translator *)(elem->elem))->get_e3_ni()); break;
		case RBM_TAG:			lua::push<double>(L, ((RBM *)(elem->elem))->get_e3_ni()); break;
		case EVENVERSOR_TAG:	lua::push<double>(L, ((evenVersor *)(elem->elem))->get_e3_ni()); break;
		
		default:
			lua::push<double>(L, 0.);
			
	}
	return 1;
}

int c3elem_no_ni(lua_State *L) {
	C3Elem *elem = Glue<C3Elem>::checkto(L, 1);
	switch(elem->tag) {
		case MV_TAG:			lua::push<double>(L, ((mv *)(elem->elem))->get_no_ni()); break;
		case NONI_TAG:			lua::push<double>(L, ((noni_t *)(elem->elem))->get_no_ni()); break;
		case FLATPOINT_TAG:		lua::push<double>(L, ((flatPoint *)(elem->elem))->get_e3_ni()); break;
		case POINTPAIR_TAG:		lua::push<double>(L, ((pointPair *)(elem->elem))->get_e3_ni()); break;
		case EVENVERSOR_TAG:	lua::push<double>(L, ((evenVersor *)(elem->elem))->get_e3_ni()); break;
		
		default:
			lua::push<double>(L, 0.);
			
	}
	return 1;
}

int c3elem_tag(lua_State *L) {
	C3Elem *elem = Glue<C3Elem>::checkto(L, 1);
	lua::push<int>(L, elem->tag);
	return 1;
}

int c3elem_name(lua_State *L) {
	C3Elem *elem = Glue<C3Elem>::checkto(L, 1);
	lua::push<const char *>(L, name_from_tag(elem->tag));
	return 1;
}

int c3elem_description(lua_State *L) {
	C3Elem *elem = Glue<C3Elem>::checkto(L, 1);
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
		
		case E3_TAG:
			lua::push<const char *>(
				L, 
				((e3_t *)elem->elem)->c_str_f()
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
		
		case I3_TAG:
			lua::push<const char *>(
				L, 
				((I3_t *)elem->elem)->c_str_f()
			);
			break;
		
		case I5_TAG:
			lua::push<const char *>(
				L, 
				((I5_t *)elem->elem)->c_str_f()
			);
			break;
			
		case I5i_TAG:
			lua::push<const char *>(
				L, 
				((I5i_t *)elem->elem)->c_str_f()
			);
			break;
			
		case VECTORE3GA_TAG:
			lua::push<const char *>(
				L, 
				((vectorE3GA *)elem->elem)->c_str_f()
			);
			break;
			
		case BIVECTORE3GA_TAG:
			lua::push<const char *>(
				L, 
				((bivectorE3GA *)elem->elem)->c_str_f()
			);
			break;
			
		case ROTORE3GA_TAG:
			lua::push<const char *>(
				L, 
				((rotorE3GA *)elem->elem)->c_str_f()
			);
			break;
			
		case NORMALIZEDPOINT_TAG:
			lua::push<const char *>(
				L, 
				((normalizedPoint *)elem->elem)->c_str_f()
			);
			break;
			
		case DUALSPHERE_TAG:
			lua::push<const char *>(
				L, 
				((dualSphere *)elem->elem)->c_str_f()
			);
			break;
			
		case DUALPLANE_TAG:
			lua::push<const char *>(
				L, 
				((dualPlane *)elem->elem)->c_str_f()
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
			
		case FLATPOINT_TAG:
			lua::push<const char *>(
				L, 
				((flatPoint *)elem->elem)->c_str_f()
			);
			break;
			
		case POINTPAIR_TAG:
			lua::push<const char *>(
				L, 
				((pointPair *)elem->elem)->c_str_f()
			);
			break;
			
		case LINE_TAG:
			lua::push<const char *>(
				L, 
				((line *)elem->elem)->c_str_f()
			);
			break;
		
		case CIRCLE_TAG:
			lua::push<const char *>(
				L, 
				((circle *)elem->elem)->c_str_f()
			);
			break;
			
		case SPHERE_TAG:
			lua::push<const char *>(
				L, 
				((sphere *)elem->elem)->c_str_f()
			);
			break;
		
		case PLANE_TAG:
			lua::push<const char *>(
				L, 
				((plane *)elem->elem)->c_str_f()
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
			
		case RBM_TAG:
			lua::push<const char *>(
				L, 
				((RBM *)elem->elem)->c_str_f()
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

template<> const char * Glue<C3Elem>::usr_name() { return "C3Elem"; }
template<> bool Glue<C3Elem>::usr_has_index() { return true; }

template<> void Glue<C3Elem>::usr_push(lua_State * L, C3Elem * u) {
	Glue<C3Elem>::usr_attr_push(L);
	Glue<C3Elem>::usr_attr_prototype(L);
}

template<> void Glue<C3Elem>::usr_index(lua_State * L, C3Elem * u) {
	Glue<C3Elem>::usr_attr_index(L);
}

template<> void Glue<C3Elem>::usr_newindex(lua_State * L, C3Elem * u) {
	Glue<C3Elem>::usr_attr_newindex(L);
}

template<> int Glue<C3Elem>::usr_tostring(lua_State * L, C3Elem * u) {
	lua_pushfstring(L, "%s: %p", Glue<C3Elem>::usr_name(), u); 
	return 1;
}

template<> void Glue<C3Elem>::usr_gc(lua_State * L, C3Elem * u) { 
	delete u;
}

template<> void Glue<C3Elem>::usr_mt(lua_State * L) {
	static luaL_reg methods[] = {
		{ NULL, NULL}
	};
	static luaL_reg getters[] = {
		{ "scalar", c3elem_scalar },
		
		{ "e1", c3elem_e1 },
		{ "e2", c3elem_e2 },
		{ "e3", c3elem_e3 },
		
		{ "no_e1", c3elem_no_e1 },
		{ "no_e2", c3elem_no_e2 },
		{ "no_e3", c3elem_no_e3 },
		{ "e1_e2", c3elem_e1_e2 },
		{ "e2_e3", c3elem_e2_e3 },
		{ "e3_e1", c3elem_e3_e1 },
		{ "e1_ni", c3elem_e1_ni },
		{ "e2_ni", c3elem_e2_ni },
		{ "e3_ni", c3elem_e3_ni },
		{ "no_ni", c3elem_no_ni },
		
		{ "tag", c3elem_tag },
		{ "name", c3elem_name },
		{ "description", c3elem_description },
		{ NULL, NULL}
	};
	static luaL_reg setters[] = {
		{ NULL, NULL}
	};
	Glue<C3Elem>::usr_attr_mt(L, methods, getters, setters);
}

int c3space_no(lua_State *L) {
	C3Elem *elem = new C3Elem(
		NO_TAG,
		new no_t()
	);
	Glue<C3Elem>::push(L, elem);
	return 1;
}

int c3space_e1(lua_State *L) {
	C3Elem *elem = new C3Elem(
		E1_TAG,
		new e1_t()
	);
	Glue<C3Elem>::push(L, elem);
	return 1;
}

int c3space_e2(lua_State *L) {
	C3Elem *elem = new C3Elem(
		E2_TAG,
		new e2_t()
	);
	Glue<C3Elem>::push(L, elem);
	return 1;
}

int c3space_e3(lua_State *L) {
	C3Elem *elem = new C3Elem(
		E3_TAG,
		new e3_t()
	);
	Glue<C3Elem>::push(L, elem);
	return 1;
}

int c3space_ni(lua_State *L) {
	C3Elem *elem = new C3Elem(
		NI_TAG,
		new ni_t()
	);
	Glue<C3Elem>::push(L, elem);
	return 1;
}

int c3space_I3(lua_State *L) {
	C3Elem *elem = new C3Elem(
		I3_TAG,
		new I3_t()
	);
	Glue<C3Elem>::push(L, elem);
	return 1;
}

int c3space_e3vector(lua_State *L) {
	double vals[3] = {0, 0, 0};
	lua::to_vec<double>(L, 1, 3, vals);

	C3Elem *elem = new C3Elem(
		VECTORE3GA_TAG,
		new vectorE3GA(
			vectorE3GA::coord_e1_e2_e3, 
			vals
		)
	);
	Glue<C3Elem>::push(L, elem);
	return 1;
}

int c3space_e3bivector(lua_State *L) {
	double vals[3] = {0, 0, 0};
	lua::to_vec<double>(L, 1, 3, vals);

	C3Elem *elem = new C3Elem(
		BIVECTORE3GA_TAG,
		new bivectorE3GA(
			bivectorE3GA::coord_e1e2_e2e3_e3e1, 
			vals
		)
	);
	Glue<C3Elem>::push(L, elem);
	return 1;
}

int c3space_normalizedPoint(lua_State *L) {
	double vals[4] = {0, 0, 0, 0.};
	lua::to_vec<double>(L, 1, 4, vals);

	C3Elem *elem = new C3Elem(
		NORMALIZEDPOINT_TAG,
		new normalizedPoint(
			normalizedPoint::coord_e1_e2_e3_ni, 
			vals
		)
	);
	Glue<C3Elem>::push(L, elem);
	return 1;
}

int c3space_dualSphere(lua_State *L) {
	double vals[5] = {0, 0, 0, 0., 0.};
	lua::to_vec<double>(L, 1, 5, vals);

	C3Elem *elem = new C3Elem(
		DUALSPHERE_TAG,
		new dualSphere(
			dualSphere::coord_no_e1_e2_e3_ni, 
			vals
		)
	);
	Glue<C3Elem>::push(L, elem);
	return 1;
}

int c3space_freeVector(lua_State *L) {
	C3Elem *e = Glue<C3Elem>::to(L, 1);
	if(e && e->tag == MV_TAG) {
		C3Elem *elem = new C3Elem(
			FREEVECTOR_TAG,
			new freeVector(
				*((mv *)e->elem), 0
			)
		);
		Glue<C3Elem>::push(L, elem);
	}
	else {
		double vals[3] = {0, 0, 0.};
		lua::to_vec<double>(L, 1, 3, vals);

		C3Elem *elem = new C3Elem(
			FREEVECTOR_TAG,
			new freeVector(
				freeVector::coord_e1ni_e2ni_e3ni, 
				vals
			)
		);
		Glue<C3Elem>::push(L, elem);
	}
	return 1;
}

int c3space_circle(lua_State *L) {
	C3Elem *e = Glue<C3Elem>::to(L, 1);
	if(e && e->tag == MV_TAG) {
		C3Elem *elem = new C3Elem(
			CIRCLE_TAG,
			new circle(
				*((mv *)e->elem), 0
			)
		);
		Glue<C3Elem>::push(L, elem);
	}
	else {
		luaL_error(L, "c3.circle: invalid argument");
	}
	return 1;
}

int c3space_mv(lua_State *L) {
	C3Elem *e = Glue<C3Elem>::to(L, 1);
	C3Elem *elem = NULL;
	
	if(!e) {
		elem = new C3Elem(
			MV_TAG,
			new mv()
		);
	}
	else {	
		switch(e->tag) {
			case MV_TAG:
				elem = new C3Elem(
					MV_TAG,
					new mv(
						*((mv *)e->elem)
					)
				);
				break;
				
			case NO_TAG:
				elem = new C3Elem(
					MV_TAG,
					new mv(
						*((no_t *)e->elem)
					)
				);
				break;
				
			case E1_TAG:
				elem = new C3Elem(
					MV_TAG,
					new mv(
						*((e1_t *)e->elem)
					)
				);
				break;
				
			case E2_TAG:
				elem = new C3Elem(
					MV_TAG,
					new mv(
						*((e2_t *)e->elem)
					)
				);
				break;
				
			case E3_TAG:
				elem = new C3Elem(
					MV_TAG,
					new mv(
						*((e3_t *)e->elem)
					)
				);
				break;
				
			case NI_TAG:
				elem = new C3Elem(
					MV_TAG,
					new mv(
						*((ni_t *)e->elem)
					)
				);
				break;
				
			case NONI_TAG:
				elem = new C3Elem(
					MV_TAG,
					new mv(
						*((noni_t *)e->elem)
					)
				);
				break;
				
			case I3_TAG:
				elem = new C3Elem(
					MV_TAG,
					new mv(
						*((I3_t *)e->elem)
					)
				);
				break;
				
			case I5_TAG:
				elem = new C3Elem(
					MV_TAG,
					new mv(
						*((I5_t *)e->elem)
					)
				);
				break;
				
			case I5i_TAG:
				elem = new C3Elem(
					MV_TAG,
					new mv(
						*((I5i_t *)e->elem)
					)
				);
				break;
				
			case VECTORE3GA_TAG:
				elem = new C3Elem(
					MV_TAG,
					new mv(
						*((vectorE3GA *)e->elem)
					)
				);
				break;
				
			case BIVECTORE3GA_TAG:
				elem = new C3Elem(
					MV_TAG,
					new mv(
						*((bivectorE3GA *)e->elem)
					)
				);
				break;
				
			case ROTORE3GA_TAG:
				elem = new C3Elem(
					MV_TAG,
					new mv(
						*((rotorE3GA *)e->elem)
					)
				);
				break;
				
			case NORMALIZEDPOINT_TAG:
				elem = new C3Elem(
					MV_TAG,
					new mv(
						*((normalizedPoint *)e->elem)
					)
				);
				break;
				
			case DUALSPHERE_TAG:
				elem = new C3Elem(
					MV_TAG,
					new mv(
						*((dualSphere *)e->elem)
					)
				);
				break;
				
			case DUALPLANE_TAG:
				elem = new C3Elem(
					MV_TAG,
					new mv(
						*((dualPlane *)e->elem)
					)
				);
				break;
				
			case FREEVECTOR_TAG:
				elem = new C3Elem(
					MV_TAG,
					new mv(
						*((freeVector *)e->elem)
					)
				);
				break;
				
			case FREEBIVECTOR_TAG:
				elem = new C3Elem(
					MV_TAG,
					new mv(
						*((freeBivector *)e->elem)
					)
				);
				break;
				
			case FLATPOINT_TAG:
				elem = new C3Elem(
					MV_TAG,
					new mv(
						*((flatPoint *)e->elem)
					)
				);
				break;
				
			case POINTPAIR_TAG:
				elem = new C3Elem(
					MV_TAG,
					new mv(
						*((pointPair *)e->elem)
					)
				);
				break;
				
			case LINE_TAG:
				elem = new C3Elem(
					MV_TAG,
					new mv(
						*((line *)e->elem)
					)
				);
				break;
				
			case CIRCLE_TAG:
				elem = new C3Elem(
					MV_TAG,
					new mv(
						*((circle *)e->elem)
					)
				);
				break;
				
			case SPHERE_TAG:
				elem = new C3Elem(
					MV_TAG,
					new mv(
						*((sphere *)e->elem)
					)
				);
				break;
				
			case PLANE_TAG:
				elem = new C3Elem(
					MV_TAG,
					new mv(
						*((plane *)e->elem)
					)
				);
				break;
				
			case PSEUDOSCALAR_TAG:
				elem = new C3Elem(
					MV_TAG,
					new mv(
						*((pseudoscalar *)e->elem)
					)
				);
				break;
				
			case NORMALIZEDTRANSLATOR_TAG:
				elem = new C3Elem(
					MV_TAG,
					new mv(
						*((normalizedTranslator *)e->elem)
					)
				);
				break;
				
			case TRANSLATOR_TAG:
				elem = new C3Elem(
					MV_TAG,
					new mv(
						*((translator *)e->elem)
					)
				);
				break;
				
			case RBM_TAG:
				elem = new C3Elem(
					MV_TAG,
					new mv(
						*((RBM *)e->elem)
					)
				);
				break;
				
			case EVENVERSOR_TAG:
				elem = new C3Elem(
					MV_TAG,
					new mv(
						*((evenVersor *)e->elem)
					)
				);
				break;
				
			case ODDVERSOR_TAG:
				elem = new C3Elem(
					MV_TAG,
					new mv(
						*((oddVersor *)e->elem)
					)
				);
				break;
		}
	}

	Glue<C3Elem>::push(L, elem);

	return 1;
}


#ifdef __cplusplus
extern "C" {
#endif

// ops
extern int c3space_norm(lua_State *L);
extern int c3space_vinv(lua_State *L);
extern int c3space_unit(lua_State *L);
extern int c3space_exp(lua_State *L);
extern int c3space_sp(lua_State *L);
extern int c3space_dual(lua_State *L);
extern int c3space_gradeInvolution(lua_State *L);
extern int c3space_gradeBitmap(lua_State *L);
extern int c3space_extractGrade(lua_State *L);
extern int c3space_thresh(lua_State *L);

// ops_gen
extern int c3space_add(lua_State *L);
extern int c3space_subtract(lua_State *L);
extern int c3space_hip(lua_State *L);
extern int c3space_lc(lua_State *L);
extern int c3space_rc(lua_State *L);
extern int c3space_op(lua_State *L);
extern int c3space_gp(lua_State *L);
extern int c3space_applyUnitVersor(lua_State *L);
extern int c3space_applyVersor(lua_State *L);


int luaopen_ga_c3(lua_State *L) {
	const char *libname = lua_tostring(L, -1);
	static const luaL_reg ga_c3_lib[] = {
		{ "no", c3space_no },
		{ "e1", c3space_e1 },
		{ "e2", c3space_e2 },
		{ "e3", c3space_e3 },
		{ "ni", c3space_ni },
		{ "I3", c3space_I3 },
		
		{ "e3vector", c3space_e3vector },
		{ "e3bivector", c3space_e3bivector },
		{ "normalizedPoint", c3space_normalizedPoint },
		{ "dualSphere", c3space_dualSphere },
		{ "freeVector", c3space_freeVector },
		{ "circle", c3space_circle },
		{ "mv", c3space_mv },
		
		{ "norm", c3space_norm },
		{ "vinv", c3space_vinv },
		{ "unit", c3space_unit },
		{ "exp", c3space_exp },
		{ "sp", c3space_sp },
		{ "dual", c3space_dual },
		{ "gradeInvolution", c3space_gradeInvolution },
		{ "gradeBitmap", c3space_gradeBitmap },
		{ "extractGrade", c3space_extractGrade },
		{ "thresh", c3space_thresh },
		
		{ "add", c3space_add },
		{ "sub", c3space_subtract },
		{ "hip", c3space_hip },
		{ "lc", c3space_lc },
		{ "rc", c3space_rc },
		{ "op", c3space_op },
		{ "gp", c3space_gp },
		{ "applyUnitVersor", c3space_applyUnitVersor },
		{ "applyVersor", c3space_applyVersor },
		{NULL, NULL}
	};
	luaL_register(L, libname, ga_c3_lib);
	
	lua::pushfield<int>(L, -1, "dim", c3ga_spaceDim);
	lua::pushfield<int>(L, -1, "ngroups", c3ga_nbGroups);
	
	lua::push_vec_t<const char *>(L, c3ga_spaceDim, c3ga_basisVectorNames);
	lua_setfield(L, -2, "basis");
	
	Glue<C3Elem>::define(L);
	
	return 1;
}

#ifdef __cplusplus
}
#endif
