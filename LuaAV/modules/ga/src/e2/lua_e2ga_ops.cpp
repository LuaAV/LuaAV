#include "lua_e2ga.h"

#ifdef __cplusplus
extern "C" {
#endif


int e2space_vinv(lua_State *L) {
	E2Elem *elem = Glue<E2Elem>::checkto(L, 1);
	switch(elem->tag) {
		case MV_TAG:
			Glue<E2Elem>::push(
				L, 
				new E2Elem(
					MV_TAG,
					new mv(
						versorInverse(*((mv *)elem->elem))
					)
				)
			);
			break;
		
		case E1_TAG:
			Glue<E2Elem>::push(
				L, 
				new E2Elem(
					E1_TAG,
					new e1_t(
						versorInverse(*((e1_t *)elem->elem))
					)
				)
			);
			break;
			
		case I2_TAG:
			Glue<E2Elem>::push(
				L, 
				new E2Elem(
					BIVECTOR_TAG,
					new bivector(
						versorInverse(*((I2_t *)elem->elem))
					)
				)
			);
			break;
		
		case I2i_TAG:
			Glue<E2Elem>::push(
				L, 
				new E2Elem(
					I2_TAG,
					new I2_t(
						versorInverse(*((I2i_t *)elem->elem))
					)
				)
			);
			break;
		
		case VECTOR_TAG:
			Glue<E2Elem>::push(
				L, 
				new E2Elem(
					VECTOR_TAG,
					new vector(
						versorInverse(*((vector *)elem->elem))
					)
				)
			);
			break;
		
		case BIVECTOR_TAG:
			Glue<E2Elem>::push(
				L, 
				new E2Elem(
					BIVECTOR_TAG,
					new bivector(
						versorInverse(*((bivector *)elem->elem))
					)
				)
			);
			break;
			
		case ROTOR_TAG:
			Glue<E2Elem>::push(
				L, 
				new E2Elem(
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
				"e2.vinv: operation not defined for %s arguments", 
				name_from_tag(elem->tag)
			);
			break;
	}
	return 1;
}

int e2space_reverse(lua_State *L) {
	E2Elem *elem = Glue<E2Elem>::checkto(L, 1);
	switch(elem->tag) {
		case MV_TAG:
			Glue<E2Elem>::push(
				L, 
				new E2Elem(
					MV_TAG,
					new mv(
						reverse(*((mv *)elem->elem))
					)
				)
			);
			break;
			
		case E2_TAG:
			Glue<E2Elem>::push(
				L, 
				new E2Elem(
					E2_TAG,
					new e2_t(
						reverse(*((e2_t *)elem->elem))
					)
				)
			);
			break;
		
		case I2i_TAG:
			Glue<E2Elem>::push(
				L, 
				new E2Elem(
					I2_TAG,
					new I2_t(
						reverse(*((I2i_t *)elem->elem))
					)
				)
			);
			break;
		
		case VECTOR_TAG:
			Glue<E2Elem>::push(
				L, 
				new E2Elem(
					VECTOR_TAG,
					new vector(
						reverse(*((vector *)elem->elem))
					)
				)
			);
			break;
		
		case BIVECTOR_TAG:
			Glue<E2Elem>::push(
				L, 
				new E2Elem(
					BIVECTOR_TAG,
					new bivector(
						reverse(*((bivector *)elem->elem))
					)
				)
			);
			break;
			
		case ROTOR_TAG:
			Glue<E2Elem>::push(
				L, 
				new E2Elem(
					ROTOR_TAG,
					new rotor(
						reverse(*((rotor *)elem->elem))
					)
				)
			);
			break;
			
		default:
			luaL_error(
				L, 
				"e2.reverse: operation not defined for %s arguments", 
				name_from_tag(elem->tag)
			);
			break;
	}
	return 1;
}

int e2space_unit(lua_State *L) {
	E2Elem *elem = Glue<E2Elem>::checkto(L, 1);
	switch(elem->tag) {
		case MV_TAG:
			Glue<E2Elem>::push(
				L, 
				new E2Elem(
					MV_TAG,
					new mv(
						unit(*((mv *)elem->elem))
					)
				)
			);
			break;
			
		case E1_TAG:
			Glue<E2Elem>::push(
				L, 
				new E2Elem(
					E1_TAG,
					new e1_t(
						unit(*((e1_t *)elem->elem))
					)
				)
			);
			break;
		
		case VECTOR_TAG:
			Glue<E2Elem>::push(
				L, 
				new E2Elem(
					VECTOR_TAG,
					new vector(
						unit(*((vector *)elem->elem))
					)
				)
			);
			break;
		
		case BIVECTOR_TAG:
			Glue<E2Elem>::push(
				L, 
				new E2Elem(
					BIVECTOR_TAG,
					new bivector(
						unit(*((bivector *)elem->elem))
					)
				)
			);
			break;
			
		case ROTOR_TAG:
			Glue<E2Elem>::push(
				L, 
				new E2Elem(
					ROTOR_TAG,
					new rotor(
						unit(*((rotor *)elem->elem))
					)
				)
			);
			break;
			
		default:
			luaL_error(
				L, 
				"e2.unit: operation not defined for %s arguments", 
				name_from_tag(elem->tag)
			);
			break;
	}
	return 1;
}

int e2space_exp(lua_State *L) {
	E2Elem *elem = Glue<E2Elem>::checkto(L, 1);
	switch(elem->tag) {
		case MV_TAG:
			Glue<E2Elem>::push(
				L, 
				new E2Elem(
					MV_TAG,
					new mv(
						exp(*((mv *)elem->elem))
					)
				)
			);
			break;
		
		case BIVECTOR_TAG:
			Glue<E2Elem>::push(
				L, 
				new E2Elem(
					ROTOR_TAG,
					new rotor(
						exp(*((bivector *)elem->elem))
					)
				)
			);
			break;
			
		case I2_TAG:
			Glue<E2Elem>::push(
				L, 
				new E2Elem(
					ROTOR_TAG,
					new rotor(
						exp(*((I2_t *)elem->elem))
					)
				)
			);
			break;
			
		default:
			luaL_error(
				L, 
				"e2.exp: operation not defined for %s arguments", 
				name_from_tag(elem->tag)
			);
			break;
	}
	return 1;
}

#ifdef __cplusplus
}
#endif
