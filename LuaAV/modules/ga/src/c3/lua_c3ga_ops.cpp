#include "lua_c3ga.h"

#ifdef __cplusplus
extern "C" {
#endif

int c3space_norm(lua_State *L) {
	C3Elem *elem = Glue<C3Elem>::checkto(L, 1);
	switch(elem->tag) {
		case MV_TAG:
			lua::push<double>(
				L,
				norm(*((mv *)elem->elem))
			);
			break;
	}
	
	return 1;
}

int c3space_unit(lua_State *L) {
	C3Elem *elem = Glue<C3Elem>::checkto(L, 1);
	switch(elem->tag) {
		case MV_TAG:
			Glue<C3Elem>::push(
				L, 
				new C3Elem(
					MV_TAG,
					new mv(
						unit(*((mv *)elem->elem))
					)
				)
			);
			break;
			
		case VECTORE3GA_TAG:
			Glue<C3Elem>::push(
				L, 
				new C3Elem(
					VECTORE3GA_TAG,
					new vectorE3GA(
						unit(*((vectorE3GA *)elem->elem))
					)
				)
			);
			break;
		
		case BIVECTORE3GA_TAG:
			Glue<C3Elem>::push(
				L, 
				new C3Elem(
					BIVECTORE3GA_TAG,
					new bivectorE3GA(
						unit(*((bivectorE3GA *)elem->elem))
					)
				)
			);
			break;
		
		case LINE_TAG:
			Glue<C3Elem>::push(
				L, 
				new C3Elem(
					LINE_TAG,
					new line(
						unit(*((line *)elem->elem))
					)
				)
			);
			break;
		
		case SPHERE_TAG:
			Glue<C3Elem>::push(
				L, 
				new C3Elem(
					SPHERE_TAG,
					new sphere(
						unit(*((sphere *)elem->elem))
					)
				)
			);
			break;
			
		case ROTORE3GA_TAG:
			Glue<C3Elem>::push(
				L, 
				new C3Elem(
					ROTORE3GA_TAG,
					new rotorE3GA(
						unit(*((rotorE3GA *)elem->elem))
					)
				)
			);
			break;
			
		case ODDVERSOR_TAG:
			Glue<C3Elem>::push(
				L, 
				new C3Elem(
					ODDVERSOR_TAG,
					new oddVersor(
						unit(*((oddVersor *)elem->elem))
					)
				)
			);
			break;
			
		case EVENVERSOR_TAG:
			Glue<C3Elem>::push(
				L, 
				new C3Elem(
					EVENVERSOR_TAG,
					new evenVersor(
						unit(*((evenVersor *)elem->elem))
					)
				)
			);
			break;
			
		case NORMALIZEDPOINT_TAG:
			Glue<C3Elem>::push(
				L, 
				new C3Elem(
					DUALSPHERE_TAG,
					new dualSphere(
						unit(*((normalizedPoint *)elem->elem))
					)
				)
			);
			break;
			
		case DUALSPHERE_TAG:
			Glue<C3Elem>::push(
				L, 
				new C3Elem(
					DUALSPHERE_TAG,
					new dualSphere(
						unit(*((dualSphere *)elem->elem))
					)
				)
			);
			break;
			
		case PSEUDOSCALAR_TAG:
			Glue<C3Elem>::push(
				L, 
				new C3Elem(
					PSEUDOSCALAR_TAG,
					new pseudoscalar(
						unit(*((pseudoscalar *)elem->elem))
					)
				)
			);
			break;
			
		case POINTPAIR_TAG:
			Glue<C3Elem>::push(
				L, 
				new C3Elem(
					POINTPAIR_TAG,
					new pointPair(
						unit(*((pointPair *)elem->elem))
					)
				)
			);
			break;
			
		case I5_TAG:
			Glue<C3Elem>::push(
				L, 
				new C3Elem(
					I5_TAG,
					new I5_t(
						unit(*((I5_t *)elem->elem))
					)
				)
			);
			break;
			
		default:
			luaL_error(
				L, 
				"c3.unit: operation not defined for %s arguments", 
				name_from_tag(elem->tag)
			);
			break;
	}
	return 1;
}

int c3space_exp(lua_State *L) {
	C3Elem *elem = Glue<C3Elem>::checkto(L, 1);
	int precision = 12;
	switch(elem->tag) {
		case MV_TAG:
			Glue<C3Elem>::push(
				L, 
				new C3Elem(
					MV_TAG,
					new mv(
						exp(*((mv *)elem->elem), precision)
					)
				)
			);
			break;
			
		case POINTPAIR_TAG:
			Glue<C3Elem>::push(
				L, 
				new C3Elem(
					EVENVERSOR_TAG,
					new evenVersor(
						exp(*((pointPair *)elem->elem), precision)
					)
				)
			);
			break;
		
		case FREEVECTOR_TAG:
			Glue<C3Elem>::push(
				L, 
				new C3Elem(
					NORMALIZEDTRANSLATOR_TAG,
					new normalizedTranslator(
						exp(*((freeVector *)elem->elem))
					)
				)
			);
			break;
			
		case NONI_TAG:
			Glue<C3Elem>::push(
				L, 
				new C3Elem(
					EVENVERSOR_TAG,
					new evenVersor(
						exp(*((noni_t *)elem->elem))
					)
				)
			);
			break;
			
		default:
			luaL_error(
				L, 
				"c3.unit: operation not defined for %s arguments", 
				name_from_tag(elem->tag)
			);
			break;
	}
	return 1;
}

int c3space_vinv(lua_State *L) {
	C3Elem *elem = Glue<C3Elem>::checkto(L, 1);
	switch(elem->tag) {
		case MV_TAG:
			Glue<C3Elem>::push(
				L, 
				new C3Elem(
					MV_TAG,
					new mv(
						versorInverse(*((mv *)elem->elem))
					)
				)
			);
			break;
			
		default:
			luaL_error(
				L, 
				"c3.unit: operation not defined for %s arguments", 
				name_from_tag(elem->tag)
			);
			break;
	}
	return 1;
}

int c3space_sp(lua_State *L) {
	C3Elem *elem1 = Glue<C3Elem>::checkto(L, 1);
	C3Elem *elem2 = Glue<C3Elem>::checkto(L, 2);

	switch(elem1->tag) {
	case MV_TAG:
	
		switch(elem2->tag) {
		case MV_TAG:
			lua::push<double>(
				L,
				sp(
					*((mv *)elem1->elem),
					*((mv *)elem2->elem)
				)
			);
			break;
		
		default:
			luaL_error(
				L, 
				"c3.sp: operation not defined for %s arguments", 
				name_from_tag(elem2->tag)
			);
			break;	
		}
		break;


	default:
		luaL_error(
			L, 
			"c3.sp: operation not defined for %s arguments", 
			name_from_tag(elem1->tag)
		);
		break;	
	}
	
	return 1;
}

int c3space_dual(lua_State *L) {
	C3Elem *elem = Glue<C3Elem>::checkto(L, 1);
	
	switch(elem->tag) {
	case MV_TAG:
		Glue<C3Elem>::push(
			L, 
			new C3Elem(
				MV_TAG,
				new mv(
					dual(*((mv *)elem->elem))
				)
			)
		);
		break;

	default:
		luaL_error(
			L, 
			"c3.dual: operation not defined for %s arguments", 
			name_from_tag(elem->tag)
		);
		break;	
	}
	
	return 1;
}

int c3space_gradeInvolution(lua_State *L) {
	C3Elem *elem = Glue<C3Elem>::checkto(L, 1);
	
	switch(elem->tag) {
	case MV_TAG:
		Glue<C3Elem>::push(
			L, 
			new C3Elem(
				MV_TAG,
				new mv(
					gradeInvolution(*((mv *)elem->elem))
				)
			)
		);
		break;

	default:
		luaL_error(
			L, 
			"c3.gradeInvolution: operation not defined for %s arguments", 
			name_from_tag(elem->tag)
		);
		break;	
	}
	
	return 1;
}

int c3space_gradeBitmap(lua_State *L) {
	C3Elem *elem = Glue<C3Elem>::checkto(L, 1);
	double eps = lua::opt<double>(L, 2, 0.);
	
	switch(elem->tag) {
	case MV_TAG:
		lua::push<int>(L, gradeBitmap(*((mv *)elem->elem), eps));
		break;

	default:
		luaL_error(
			L, 
			"c3.gradeBitmap: operation not defined for %s arguments", 
			name_from_tag(elem->tag)
		);
		break;	
	}
	
	return 1;
}

int c3space_extractGrade(lua_State *L) {
	C3Elem *elem = Glue<C3Elem>::checkto(L, 1);
	if(lua::is<int>(L, 2)) {
		int bmp = lua::to<int>(L, 2);
		
		switch(elem->tag) {
		case MV_TAG:
			Glue<C3Elem>::push(
				L, 
				new C3Elem(
					MV_TAG,
					new mv(
						extractGrade(*((mv *)elem->elem), bmp)
					)
				)
			);
			break;

		default:
			luaL_error(
				L, 
				"c3.extractGrade: operation not defined for %s arguments", 
				name_from_tag(elem->tag)
			);
			break;	
		}
	}
	else {
		luaL_error(L, "c3.extractGrade: invalid arguments");
	}
	
	return 1;
}

int c3space_thresh(lua_State *L) {
	C3Elem *elem = Glue<C3Elem>::checkto(L, 1);
	if(lua::is<int>(L, 2)) {
		double eps = lua::to<double>(L, 2);
		
		switch(elem->tag) {
		case MV_TAG: {
				mv *m = new mv(*((mv *)elem->elem));
				m->compress(eps);
				for(int i=0; i < 32; i++) {
					if(fabs(m->m_c[i]) < eps) {
						m->m_c[i] = 0;
					}
				}
				
				Glue<C3Elem>::push(
					L, 
					new C3Elem(
						MV_TAG,
						m
					)
				);
			}
			break;

		default:
			luaL_error(
				L, 
				"c3.thresh: operation not defined for %s arguments", 
				name_from_tag(elem->tag)
			);
			break;	
		}
	}
	else {
		luaL_error(L, "c3.thresh: invalid arguments");
	}
	
	return 1;
}


#ifdef __cplusplus
}
#endif