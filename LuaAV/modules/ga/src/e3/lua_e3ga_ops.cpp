#include "lua_e3ga.h"

#ifdef __cplusplus
extern "C" {
#endif


int e3space_vinv(lua_State *L) {
	E3Elem *elem = Glue<E3Elem>::checkto(L, 1);
	switch(elem->tag) {
		case MV_TAG:
			Glue<E3Elem>::push(
				L, 
				new E3Elem(
					MV_TAG,
					new mv(
						versorInverse(*((mv *)elem->elem))
					)
				)
			);
			break;
		
		case E1_TAG:
			Glue<E3Elem>::push(
				L, 
				new E3Elem(
					E1_TAG,
					new e1_t(
						versorInverse(*((e1_t *)elem->elem))
					)
				)
			);
			break;
		
		case E3_TAG:
			Glue<E3Elem>::push(
				L, 
				new E3Elem(
					E3_TAG,
					new e3_t(
						versorInverse(*((e3_t *)elem->elem))
					)
				)
			);
			break;
			
		case I3_TAG:
			Glue<E3Elem>::push(
				L, 
				new E3Elem(
					TRIVECTOR_TAG,
					new trivector(
						versorInverse(*((I3_t *)elem->elem))
					)
				)
			);
			break;
		
		case VECTOR_TAG:
			Glue<E3Elem>::push(
				L, 
				new E3Elem(
					VECTOR_TAG,
					new vector(
						versorInverse(*((vector *)elem->elem))
					)
				)
			);
			break;
		
		case BIVECTOR_TAG:
			Glue<E3Elem>::push(
				L, 
				new E3Elem(
					BIVECTOR_TAG,
					new bivector(
						versorInverse(*((bivector *)elem->elem))
					)
				)
			);
			break;
			
		case TRIVECTOR_TAG:
			Glue<E3Elem>::push(
				L, 
				new E3Elem(
					TRIVECTOR_TAG,
					new trivector(
						versorInverse(*((trivector *)elem->elem))
					)
				)
			);
			break;
			
		case ROTOR_TAG:
			Glue<E3Elem>::push(
				L, 
				new E3Elem(
					ROTOR_TAG,
					new rotor(
						versorInverse(*((rotor *)elem->elem))
					)
				)
			);
			break;
			
		default:
			luaL_error(
				L, 
				"e3.vinv: operation not defined for %s arguments", 
				name_from_tag(elem->tag)
			);
			break;
	}
	return 1;
}

int e3space_dual(lua_State *L) {
	E3Elem *elem = Glue<E3Elem>::checkto(L, 1);
	switch(elem->tag) {
		case MV_TAG:
			Glue<E3Elem>::push(
				L, 
				new E3Elem(
					MV_TAG,
					new mv(
						dual(*((mv *)elem->elem))
					)
				)
			);
			break;
		
		case E1_TAG:
			Glue<E3Elem>::push(
				L, 
				new E3Elem(
					BIVECTOR_TAG,
					new bivector(
						dual(*((e1_t *)elem->elem))
					)
				)
			);
			break;
			
		case E2_TAG:
			Glue<E3Elem>::push(
				L, 
				new E3Elem(
					MV_TAG,
					new mv(
						dual(
							mv(*((e2_t *)elem->elem))
						)
					)
				)
			);
			break;
			
		case E3_TAG:
			Glue<E3Elem>::push(
				L, 
				new E3Elem(
					MV_TAG,
					new mv(
						dual(
							mv(*((e3_t *)elem->elem))
						)
					)
				)
			);
			break;
			
		case I3_TAG:
			lua::push<double>(
				L, 
				dual(*((I3_t *)elem->elem))
			);
			break;
		
		case VECTOR_TAG:
			Glue<E3Elem>::push(
				L, 
				new E3Elem(
					BIVECTOR_TAG,
					new bivector(
						dual(*((vector *)elem->elem))
					)
				)
			);
			break;
		
		case BIVECTOR_TAG:
			Glue<E3Elem>::push(
				L, 
				new E3Elem(
					VECTOR_TAG,
					new vector(
						dual(*((bivector *)elem->elem))
					)
				)
			);
			break;
			
		case TRIVECTOR_TAG:
			lua::push<double>(
				L, 
				dual(*((trivector *)elem->elem))
			);
			break;
			
		case ROTOR_TAG:
			Glue<E3Elem>::push(
				L, 
				new E3Elem(
					ODDVERSOR_TAG,
					new oddVersor(
						dual(*((rotor *)elem->elem))
					)
				)
			);
			break;
			
		case ODDVERSOR_TAG:
			Glue<E3Elem>::push(
				L, 
				new E3Elem(
					ROTOR_TAG,
					new rotor(
						dual(*((oddVersor *)elem->elem))
					)
				)
			);
			break;
			
		default:
			luaL_error(
				L, 
				"e3.dual: operation not defined for %s arguments", 
				name_from_tag(elem->tag)
			);
			break;
	}
	return 1;
}

int e3space_reverse(lua_State *L) {
	double d;
	E3Tag tag;
	E3Elem *elem = Glue<E3Elem>::to(L, 1);
	
	if(!elem) {
		if(lua::is<double>(L, 1)) {
			d = lua::to<double>(L, 1);
			elem = (E3Elem *)(&d);
			tag = DOUBLE_TAG;
		}
		else {
			luaL_error(L, "e3.add: invalid arguments");
		}
	}
	else {
		tag = elem->tag;
	}
	
	
	switch(tag) {
		case MV_TAG:
			Glue<E3Elem>::push(
				L, 
				new E3Elem(
					MV_TAG,
					new mv(
						reverse(*((mv *)elem->elem))
					)
				)
			);
			break;
			
		case I3_TAG:
			Glue<E3Elem>::push(
				L, 
				new E3Elem(
					TRIVECTOR_TAG,
					new trivector(
						reverse(*((I3_t *)elem->elem))
					)
				)
			);
			break;
		
		case VECTOR_TAG:
			Glue<E3Elem>::push(
				L, 
				new E3Elem(
					VECTOR_TAG,
					new vector(
						reverse(*((vector *)elem->elem))
					)
				)
			);
			break;
		
		case BIVECTOR_TAG:
			Glue<E3Elem>::push(
				L, 
				new E3Elem(
					BIVECTOR_TAG,
					new bivector(
						reverse(*((bivector *)elem->elem))
					)
				)
			);
			break;
			
		case TRIVECTOR_TAG:
			Glue<E3Elem>::push(
				L, 
				new E3Elem(
					TRIVECTOR_TAG,
					new trivector(
						reverse(*((trivector *)elem->elem))
					)
				)
			);
			break;
			
		case ROTOR_TAG:
			Glue<E3Elem>::push(
				L, 
				new E3Elem(
					ROTOR_TAG,
					new rotor(
						reverse(*((rotor *)elem->elem))
					)
				)
			);
			break;
			
		case DOUBLE_TAG:
			lua::push<double>(
				L, 
				reverse(*(double *)elem)
			);
			break;
			
		default:
			luaL_error(
				L, 
				"e3.reverse: operation not defined for %s arguments", 
				name_from_tag(elem->tag)
			);
			break;
	}
	return 1;
}


#ifdef __cplusplus
}
#endif
