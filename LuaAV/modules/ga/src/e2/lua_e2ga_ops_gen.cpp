// THIS FILE GENERATED AUTOMATICALLY, DO NOT EDIT
#include "lua_e2ga.h"

#ifdef __cplusplus
extern "C" {
#endif




int e2space_add(lua_State *L) {
	double d1, d2;
	E2Tag tag1;
	E2Tag tag2;
	E2Elem *elem1 = Glue<E2Elem>::to(L, 1);
	E2Elem *elem2 = Glue<E2Elem>::to(L, 2);
	
	
	if(!elem1) {
		if(lua::is<double>(L, 1)) {
			d1 = lua::to<double>(L, 1);
			elem1 = (E2Elem *)(&d1);
			tag1 = DOUBLE_TAG;
		}
		else {
			luaL_error(L, "e2.add: invalid arguments");
		}
	}
	else {
		tag1 = elem1->tag;
	}
	
	if(!elem2) {
		if(lua::is<double>(L, 2)) {
			d2 = lua::to<double>(L, 2);
			elem2 = (E2Elem *)(&d2);
			tag2 = DOUBLE_TAG;
		}
		else {
			luaL_error(L, "e2.add: invalid arguments");
		}
	}
	else {
		tag2 = elem2->tag;
	}
	
switch(tag1) {
case MV_TAG:
switch(tag2) {
case MV_TAG:
	Glue<E2Elem>::push(
		L, 
		new E2Elem(
			MV_TAG,
			new mv(
				add(*((mv *)elem1->elem), *((mv *)elem2->elem))
			)
		)
	);
break;

case E1_TAG:
	Glue<E2Elem>::push(
		L, 
		new E2Elem(
			MV_TAG,
			new mv(
				add(mv(*((mv *)elem1->elem)), mv(*((e1_t *)elem2->elem)))
			)
		)
	);
break;

case E2_TAG:
	Glue<E2Elem>::push(
		L, 
		new E2Elem(
			MV_TAG,
			new mv(
				add(mv(*((mv *)elem1->elem)), mv(*((e2_t *)elem2->elem)))
			)
		)
	);
break;

case I2_TAG:
	Glue<E2Elem>::push(
		L, 
		new E2Elem(
			MV_TAG,
			new mv(
				add(mv(*((mv *)elem1->elem)), mv(*((I2_t *)elem2->elem)))
			)
		)
	);
break;

case I2i_TAG:
	Glue<E2Elem>::push(
		L, 
		new E2Elem(
			MV_TAG,
			new mv(
				add(mv(*((mv *)elem1->elem)), mv(*((I2i_t *)elem2->elem)))
			)
		)
	);
break;

case VECTOR_TAG:
	Glue<E2Elem>::push(
		L, 
		new E2Elem(
			MV_TAG,
			new mv(
				add(mv(*((mv *)elem1->elem)), mv(*((vector *)elem2->elem)))
			)
		)
	);
break;

case BIVECTOR_TAG:
	Glue<E2Elem>::push(
		L, 
		new E2Elem(
			MV_TAG,
			new mv(
				add(mv(*((mv *)elem1->elem)), mv(*((bivector *)elem2->elem)))
			)
		)
	);
break;

case ROTOR_TAG:
	Glue<E2Elem>::push(
		L, 
		new E2Elem(
			MV_TAG,
			new mv(
				add(mv(*((mv *)elem1->elem)), mv(*((rotor *)elem2->elem)))
			)
		)
	);
break;

case DOUBLE_TAG:
	Glue<E2Elem>::push(
		L, 
		new E2Elem(
			MV_TAG,
			new mv(
				add(mv(*((mv *)elem1->elem)), *((double *)elem2))
			)
		)
	);
break;
}
break;

case E1_TAG:
switch(tag2) {
case MV_TAG:
	Glue<E2Elem>::push(
		L, 
		new E2Elem(
			MV_TAG,
			new mv(
				add(mv(*((e1_t *)elem1->elem)), mv(*((mv *)elem2->elem)))
			)
		)
	);
break;

case E1_TAG:
	Glue<E2Elem>::push(
		L, 
		new E2Elem(
			MV_TAG,
			new mv(
				add(mv(*((e1_t *)elem1->elem)), mv(*((e1_t *)elem2->elem)))
			)
		)
	);
break;

case E2_TAG:
	Glue<E2Elem>::push(
		L, 
		new E2Elem(
			VECTOR_TAG,
			new vector(
				add(*((e1_t *)elem1->elem), *((e2_t *)elem2->elem))
			)
		)
	);
break;

case I2_TAG:
	Glue<E2Elem>::push(
		L, 
		new E2Elem(
			MV_TAG,
			new mv(
				add(mv(*((e1_t *)elem1->elem)), mv(*((I2_t *)elem2->elem)))
			)
		)
	);
break;

case I2i_TAG:
	Glue<E2Elem>::push(
		L, 
		new E2Elem(
			MV_TAG,
			new mv(
				add(mv(*((e1_t *)elem1->elem)), mv(*((I2i_t *)elem2->elem)))
			)
		)
	);
break;

case VECTOR_TAG:
	Glue<E2Elem>::push(
		L, 
		new E2Elem(
			MV_TAG,
			new mv(
				add(mv(*((e1_t *)elem1->elem)), mv(*((vector *)elem2->elem)))
			)
		)
	);
break;

case BIVECTOR_TAG:
	Glue<E2Elem>::push(
		L, 
		new E2Elem(
			MV_TAG,
			new mv(
				add(mv(*((e1_t *)elem1->elem)), mv(*((bivector *)elem2->elem)))
			)
		)
	);
break;

case ROTOR_TAG:
	Glue<E2Elem>::push(
		L, 
		new E2Elem(
			MV_TAG,
			new mv(
				add(mv(*((e1_t *)elem1->elem)), mv(*((rotor *)elem2->elem)))
			)
		)
	);
break;

case DOUBLE_TAG:
	Glue<E2Elem>::push(
		L, 
		new E2Elem(
			MV_TAG,
			new mv(
				add(mv(*((e1_t *)elem1->elem)), *((double *)elem2))
			)
		)
	);
break;
}
break;

case E2_TAG:
switch(tag2) {
case MV_TAG:
	Glue<E2Elem>::push(
		L, 
		new E2Elem(
			MV_TAG,
			new mv(
				add(mv(*((e2_t *)elem1->elem)), mv(*((mv *)elem2->elem)))
			)
		)
	);
break;

case E1_TAG:
	Glue<E2Elem>::push(
		L, 
		new E2Elem(
			MV_TAG,
			new mv(
				add(mv(*((e2_t *)elem1->elem)), mv(*((e1_t *)elem2->elem)))
			)
		)
	);
break;

case E2_TAG:
	Glue<E2Elem>::push(
		L, 
		new E2Elem(
			MV_TAG,
			new mv(
				add(mv(*((e2_t *)elem1->elem)), mv(*((e2_t *)elem2->elem)))
			)
		)
	);
break;

case I2_TAG:
	Glue<E2Elem>::push(
		L, 
		new E2Elem(
			MV_TAG,
			new mv(
				add(mv(*((e2_t *)elem1->elem)), mv(*((I2_t *)elem2->elem)))
			)
		)
	);
break;

case I2i_TAG:
	Glue<E2Elem>::push(
		L, 
		new E2Elem(
			MV_TAG,
			new mv(
				add(mv(*((e2_t *)elem1->elem)), mv(*((I2i_t *)elem2->elem)))
			)
		)
	);
break;

case VECTOR_TAG:
	Glue<E2Elem>::push(
		L, 
		new E2Elem(
			MV_TAG,
			new mv(
				add(mv(*((e2_t *)elem1->elem)), mv(*((vector *)elem2->elem)))
			)
		)
	);
break;

case BIVECTOR_TAG:
	Glue<E2Elem>::push(
		L, 
		new E2Elem(
			MV_TAG,
			new mv(
				add(mv(*((e2_t *)elem1->elem)), mv(*((bivector *)elem2->elem)))
			)
		)
	);
break;

case ROTOR_TAG:
	Glue<E2Elem>::push(
		L, 
		new E2Elem(
			MV_TAG,
			new mv(
				add(mv(*((e2_t *)elem1->elem)), mv(*((rotor *)elem2->elem)))
			)
		)
	);
break;

case DOUBLE_TAG:
	Glue<E2Elem>::push(
		L, 
		new E2Elem(
			MV_TAG,
			new mv(
				add(mv(*((e2_t *)elem1->elem)), *((double *)elem2))
			)
		)
	);
break;
}
break;

case I2_TAG:
switch(tag2) {
case MV_TAG:
	Glue<E2Elem>::push(
		L, 
		new E2Elem(
			MV_TAG,
			new mv(
				add(mv(*((I2_t *)elem1->elem)), mv(*((mv *)elem2->elem)))
			)
		)
	);
break;

case E1_TAG:
	Glue<E2Elem>::push(
		L, 
		new E2Elem(
			MV_TAG,
			new mv(
				add(mv(*((I2_t *)elem1->elem)), mv(*((e1_t *)elem2->elem)))
			)
		)
	);
break;

case E2_TAG:
	Glue<E2Elem>::push(
		L, 
		new E2Elem(
			MV_TAG,
			new mv(
				add(mv(*((I2_t *)elem1->elem)), mv(*((e2_t *)elem2->elem)))
			)
		)
	);
break;

case I2_TAG:
	Glue<E2Elem>::push(
		L, 
		new E2Elem(
			MV_TAG,
			new mv(
				add(mv(*((I2_t *)elem1->elem)), mv(*((I2_t *)elem2->elem)))
			)
		)
	);
break;

case I2i_TAG:
				lua::push<double>(
					L, 
					add(*((I2_t *)elem1->elem), *((I2i_t *)elem2->elem))
				);
			
break;

case VECTOR_TAG:
	Glue<E2Elem>::push(
		L, 
		new E2Elem(
			MV_TAG,
			new mv(
				add(mv(*((I2_t *)elem1->elem)), mv(*((vector *)elem2->elem)))
			)
		)
	);
break;

case BIVECTOR_TAG:
	Glue<E2Elem>::push(
		L, 
		new E2Elem(
			MV_TAG,
			new mv(
				add(mv(*((I2_t *)elem1->elem)), mv(*((bivector *)elem2->elem)))
			)
		)
	);
break;

case ROTOR_TAG:
	Glue<E2Elem>::push(
		L, 
		new E2Elem(
			MV_TAG,
			new mv(
				add(mv(*((I2_t *)elem1->elem)), mv(*((rotor *)elem2->elem)))
			)
		)
	);
break;

case DOUBLE_TAG:
	Glue<E2Elem>::push(
		L, 
		new E2Elem(
			MV_TAG,
			new mv(
				add(mv(*((I2_t *)elem1->elem)), *((double *)elem2))
			)
		)
	);
break;
}
break;

case I2i_TAG:
switch(tag2) {
case MV_TAG:
	Glue<E2Elem>::push(
		L, 
		new E2Elem(
			MV_TAG,
			new mv(
				add(mv(*((I2i_t *)elem1->elem)), mv(*((mv *)elem2->elem)))
			)
		)
	);
break;

case E1_TAG:
	Glue<E2Elem>::push(
		L, 
		new E2Elem(
			MV_TAG,
			new mv(
				add(mv(*((I2i_t *)elem1->elem)), mv(*((e1_t *)elem2->elem)))
			)
		)
	);
break;

case E2_TAG:
	Glue<E2Elem>::push(
		L, 
		new E2Elem(
			MV_TAG,
			new mv(
				add(mv(*((I2i_t *)elem1->elem)), mv(*((e2_t *)elem2->elem)))
			)
		)
	);
break;

case I2_TAG:
	Glue<E2Elem>::push(
		L, 
		new E2Elem(
			MV_TAG,
			new mv(
				add(mv(*((I2i_t *)elem1->elem)), mv(*((I2_t *)elem2->elem)))
			)
		)
	);
break;

case I2i_TAG:
	Glue<E2Elem>::push(
		L, 
		new E2Elem(
			MV_TAG,
			new mv(
				add(mv(*((I2i_t *)elem1->elem)), mv(*((I2i_t *)elem2->elem)))
			)
		)
	);
break;

case VECTOR_TAG:
	Glue<E2Elem>::push(
		L, 
		new E2Elem(
			MV_TAG,
			new mv(
				add(mv(*((I2i_t *)elem1->elem)), mv(*((vector *)elem2->elem)))
			)
		)
	);
break;

case BIVECTOR_TAG:
	Glue<E2Elem>::push(
		L, 
		new E2Elem(
			MV_TAG,
			new mv(
				add(mv(*((I2i_t *)elem1->elem)), mv(*((bivector *)elem2->elem)))
			)
		)
	);
break;

case ROTOR_TAG:
	Glue<E2Elem>::push(
		L, 
		new E2Elem(
			MV_TAG,
			new mv(
				add(mv(*((I2i_t *)elem1->elem)), mv(*((rotor *)elem2->elem)))
			)
		)
	);
break;

case DOUBLE_TAG:
	Glue<E2Elem>::push(
		L, 
		new E2Elem(
			MV_TAG,
			new mv(
				add(mv(*((I2i_t *)elem1->elem)), *((double *)elem2))
			)
		)
	);
break;
}
break;

case VECTOR_TAG:
switch(tag2) {
case MV_TAG:
	Glue<E2Elem>::push(
		L, 
		new E2Elem(
			MV_TAG,
			new mv(
				add(mv(*((vector *)elem1->elem)), mv(*((mv *)elem2->elem)))
			)
		)
	);
break;

case E1_TAG:
	Glue<E2Elem>::push(
		L, 
		new E2Elem(
			MV_TAG,
			new mv(
				add(mv(*((vector *)elem1->elem)), mv(*((e1_t *)elem2->elem)))
			)
		)
	);
break;

case E2_TAG:
	Glue<E2Elem>::push(
		L, 
		new E2Elem(
			MV_TAG,
			new mv(
				add(mv(*((vector *)elem1->elem)), mv(*((e2_t *)elem2->elem)))
			)
		)
	);
break;

case I2_TAG:
	Glue<E2Elem>::push(
		L, 
		new E2Elem(
			MV_TAG,
			new mv(
				add(mv(*((vector *)elem1->elem)), mv(*((I2_t *)elem2->elem)))
			)
		)
	);
break;

case I2i_TAG:
	Glue<E2Elem>::push(
		L, 
		new E2Elem(
			MV_TAG,
			new mv(
				add(mv(*((vector *)elem1->elem)), mv(*((I2i_t *)elem2->elem)))
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
				add(*((vector *)elem1->elem), *((vector *)elem2->elem))
			)
		)
	);
break;

case BIVECTOR_TAG:
	Glue<E2Elem>::push(
		L, 
		new E2Elem(
			MV_TAG,
			new mv(
				add(mv(*((vector *)elem1->elem)), mv(*((bivector *)elem2->elem)))
			)
		)
	);
break;

case ROTOR_TAG:
	Glue<E2Elem>::push(
		L, 
		new E2Elem(
			MV_TAG,
			new mv(
				add(mv(*((vector *)elem1->elem)), mv(*((rotor *)elem2->elem)))
			)
		)
	);
break;

case DOUBLE_TAG:
	Glue<E2Elem>::push(
		L, 
		new E2Elem(
			MV_TAG,
			new mv(
				add(mv(*((vector *)elem1->elem)), *((double *)elem2))
			)
		)
	);
break;
}
break;

case BIVECTOR_TAG:
switch(tag2) {
case MV_TAG:
	Glue<E2Elem>::push(
		L, 
		new E2Elem(
			MV_TAG,
			new mv(
				add(mv(*((bivector *)elem1->elem)), mv(*((mv *)elem2->elem)))
			)
		)
	);
break;

case E1_TAG:
	Glue<E2Elem>::push(
		L, 
		new E2Elem(
			MV_TAG,
			new mv(
				add(mv(*((bivector *)elem1->elem)), mv(*((e1_t *)elem2->elem)))
			)
		)
	);
break;

case E2_TAG:
	Glue<E2Elem>::push(
		L, 
		new E2Elem(
			MV_TAG,
			new mv(
				add(mv(*((bivector *)elem1->elem)), mv(*((e2_t *)elem2->elem)))
			)
		)
	);
break;

case I2_TAG:
	Glue<E2Elem>::push(
		L, 
		new E2Elem(
			MV_TAG,
			new mv(
				add(mv(*((bivector *)elem1->elem)), mv(*((I2_t *)elem2->elem)))
			)
		)
	);
break;

case I2i_TAG:
	Glue<E2Elem>::push(
		L, 
		new E2Elem(
			MV_TAG,
			new mv(
				add(mv(*((bivector *)elem1->elem)), mv(*((I2i_t *)elem2->elem)))
			)
		)
	);
break;

case VECTOR_TAG:
	Glue<E2Elem>::push(
		L, 
		new E2Elem(
			MV_TAG,
			new mv(
				add(mv(*((bivector *)elem1->elem)), mv(*((vector *)elem2->elem)))
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
				add(*((bivector *)elem1->elem), *((bivector *)elem2->elem))
			)
		)
	);
break;

case ROTOR_TAG:
	Glue<E2Elem>::push(
		L, 
		new E2Elem(
			MV_TAG,
			new mv(
				add(mv(*((bivector *)elem1->elem)), mv(*((rotor *)elem2->elem)))
			)
		)
	);
break;

case DOUBLE_TAG:
	Glue<E2Elem>::push(
		L, 
		new E2Elem(
			MV_TAG,
			new mv(
				add(mv(*((bivector *)elem1->elem)), *((double *)elem2))
			)
		)
	);
break;
}
break;

case ROTOR_TAG:
switch(tag2) {
case MV_TAG:
	Glue<E2Elem>::push(
		L, 
		new E2Elem(
			MV_TAG,
			new mv(
				add(mv(*((rotor *)elem1->elem)), mv(*((mv *)elem2->elem)))
			)
		)
	);
break;

case E1_TAG:
	Glue<E2Elem>::push(
		L, 
		new E2Elem(
			MV_TAG,
			new mv(
				add(mv(*((rotor *)elem1->elem)), mv(*((e1_t *)elem2->elem)))
			)
		)
	);
break;

case E2_TAG:
	Glue<E2Elem>::push(
		L, 
		new E2Elem(
			MV_TAG,
			new mv(
				add(mv(*((rotor *)elem1->elem)), mv(*((e2_t *)elem2->elem)))
			)
		)
	);
break;

case I2_TAG:
	Glue<E2Elem>::push(
		L, 
		new E2Elem(
			MV_TAG,
			new mv(
				add(mv(*((rotor *)elem1->elem)), mv(*((I2_t *)elem2->elem)))
			)
		)
	);
break;

case I2i_TAG:
	Glue<E2Elem>::push(
		L, 
		new E2Elem(
			MV_TAG,
			new mv(
				add(mv(*((rotor *)elem1->elem)), mv(*((I2i_t *)elem2->elem)))
			)
		)
	);
break;

case VECTOR_TAG:
	Glue<E2Elem>::push(
		L, 
		new E2Elem(
			MV_TAG,
			new mv(
				add(mv(*((rotor *)elem1->elem)), mv(*((vector *)elem2->elem)))
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
				add(*((rotor *)elem1->elem), *((bivector *)elem2->elem))
			)
		)
	);
break;

case ROTOR_TAG:
	Glue<E2Elem>::push(
		L, 
		new E2Elem(
			MV_TAG,
			new mv(
				add(mv(*((rotor *)elem1->elem)), mv(*((rotor *)elem2->elem)))
			)
		)
	);
break;

case DOUBLE_TAG:
	Glue<E2Elem>::push(
		L, 
		new E2Elem(
			MV_TAG,
			new mv(
				add(mv(*((rotor *)elem1->elem)), *((double *)elem2))
			)
		)
	);
break;
}
break;

case DOUBLE_TAG:
switch(tag2) {
case MV_TAG:
	Glue<E2Elem>::push(
		L, 
		new E2Elem(
			MV_TAG,
			new mv(
				add(*((double *)elem1), mv(*((mv *)elem2->elem)))
			)
		)
	);
break;

case E1_TAG:
	Glue<E2Elem>::push(
		L, 
		new E2Elem(
			MV_TAG,
			new mv(
				add(*((double *)elem1), mv(*((e1_t *)elem2->elem)))
			)
		)
	);
break;

case E2_TAG:
	Glue<E2Elem>::push(
		L, 
		new E2Elem(
			MV_TAG,
			new mv(
				add(*((double *)elem1), mv(*((e2_t *)elem2->elem)))
			)
		)
	);
break;

case I2_TAG:
	Glue<E2Elem>::push(
		L, 
		new E2Elem(
			MV_TAG,
			new mv(
				add(*((double *)elem1), mv(*((I2_t *)elem2->elem)))
			)
		)
	);
break;

case I2i_TAG:
	Glue<E2Elem>::push(
		L, 
		new E2Elem(
			MV_TAG,
			new mv(
				add(*((double *)elem1), mv(*((I2i_t *)elem2->elem)))
			)
		)
	);
break;

case VECTOR_TAG:
	Glue<E2Elem>::push(
		L, 
		new E2Elem(
			MV_TAG,
			new mv(
				add(*((double *)elem1), mv(*((vector *)elem2->elem)))
			)
		)
	);
break;

case BIVECTOR_TAG:
	Glue<E2Elem>::push(
		L, 
		new E2Elem(
			MV_TAG,
			new mv(
				add(*((double *)elem1), mv(*((bivector *)elem2->elem)))
			)
		)
	);
break;

case ROTOR_TAG:
	Glue<E2Elem>::push(
		L, 
		new E2Elem(
			MV_TAG,
			new mv(
				add(*((double *)elem1), mv(*((rotor *)elem2->elem)))
			)
		)
	);
break;

case DOUBLE_TAG:
	Glue<E2Elem>::push(
		L, 
		new E2Elem(
			MV_TAG,
			new mv(
				add(*((double *)elem1), *((double *)elem2))
			)
		)
	);
break;
}
break;
}
return 1;
}


int e2space_subtract(lua_State *L) {
	double d1, d2;
	E2Tag tag1;
	E2Tag tag2;
	E2Elem *elem1 = Glue<E2Elem>::to(L, 1);
	E2Elem *elem2 = Glue<E2Elem>::to(L, 2);
	
	
	if(!elem1) {
		if(lua::is<double>(L, 1)) {
			d1 = lua::to<double>(L, 1);
			elem1 = (E2Elem *)(&d1);
			tag1 = DOUBLE_TAG;
		}
		else {
			luaL_error(L, "e2.subtract: invalid arguments");
		}
	}
	else {
		tag1 = elem1->tag;
	}
	
	if(!elem2) {
		if(lua::is<double>(L, 2)) {
			d2 = lua::to<double>(L, 2);
			elem2 = (E2Elem *)(&d2);
			tag2 = DOUBLE_TAG;
		}
		else {
			luaL_error(L, "e2.subtract: invalid arguments");
		}
	}
	else {
		tag2 = elem2->tag;
	}
	
switch(tag1) {
case MV_TAG:
switch(tag2) {
case MV_TAG:
	Glue<E2Elem>::push(
		L, 
		new E2Elem(
			MV_TAG,
			new mv(
				subtract(*((mv *)elem1->elem), *((mv *)elem2->elem))
			)
		)
	);
break;

case E1_TAG:
	Glue<E2Elem>::push(
		L, 
		new E2Elem(
			MV_TAG,
			new mv(
				subtract(mv(*((mv *)elem1->elem)), mv(*((e1_t *)elem2->elem)))
			)
		)
	);
break;

case E2_TAG:
	Glue<E2Elem>::push(
		L, 
		new E2Elem(
			MV_TAG,
			new mv(
				subtract(mv(*((mv *)elem1->elem)), mv(*((e2_t *)elem2->elem)))
			)
		)
	);
break;

case I2_TAG:
	Glue<E2Elem>::push(
		L, 
		new E2Elem(
			MV_TAG,
			new mv(
				subtract(mv(*((mv *)elem1->elem)), mv(*((I2_t *)elem2->elem)))
			)
		)
	);
break;

case I2i_TAG:
	Glue<E2Elem>::push(
		L, 
		new E2Elem(
			MV_TAG,
			new mv(
				subtract(mv(*((mv *)elem1->elem)), mv(*((I2i_t *)elem2->elem)))
			)
		)
	);
break;

case VECTOR_TAG:
	Glue<E2Elem>::push(
		L, 
		new E2Elem(
			MV_TAG,
			new mv(
				subtract(mv(*((mv *)elem1->elem)), mv(*((vector *)elem2->elem)))
			)
		)
	);
break;

case BIVECTOR_TAG:
	Glue<E2Elem>::push(
		L, 
		new E2Elem(
			MV_TAG,
			new mv(
				subtract(mv(*((mv *)elem1->elem)), mv(*((bivector *)elem2->elem)))
			)
		)
	);
break;

case ROTOR_TAG:
	Glue<E2Elem>::push(
		L, 
		new E2Elem(
			MV_TAG,
			new mv(
				subtract(mv(*((mv *)elem1->elem)), mv(*((rotor *)elem2->elem)))
			)
		)
	);
break;

case DOUBLE_TAG:
	Glue<E2Elem>::push(
		L, 
		new E2Elem(
			MV_TAG,
			new mv(
				subtract(mv(*((mv *)elem1->elem)), *((double *)elem2))
			)
		)
	);
break;
}
break;

case E1_TAG:
switch(tag2) {
case MV_TAG:
	Glue<E2Elem>::push(
		L, 
		new E2Elem(
			MV_TAG,
			new mv(
				subtract(mv(*((e1_t *)elem1->elem)), mv(*((mv *)elem2->elem)))
			)
		)
	);
break;

case E1_TAG:
	Glue<E2Elem>::push(
		L, 
		new E2Elem(
			MV_TAG,
			new mv(
				subtract(mv(*((e1_t *)elem1->elem)), mv(*((e1_t *)elem2->elem)))
			)
		)
	);
break;

case E2_TAG:
	Glue<E2Elem>::push(
		L, 
		new E2Elem(
			VECTOR_TAG,
			new vector(
				subtract(*((e1_t *)elem1->elem), *((e2_t *)elem2->elem))
			)
		)
	);
break;

case I2_TAG:
	Glue<E2Elem>::push(
		L, 
		new E2Elem(
			MV_TAG,
			new mv(
				subtract(mv(*((e1_t *)elem1->elem)), mv(*((I2_t *)elem2->elem)))
			)
		)
	);
break;

case I2i_TAG:
	Glue<E2Elem>::push(
		L, 
		new E2Elem(
			MV_TAG,
			new mv(
				subtract(mv(*((e1_t *)elem1->elem)), mv(*((I2i_t *)elem2->elem)))
			)
		)
	);
break;

case VECTOR_TAG:
	Glue<E2Elem>::push(
		L, 
		new E2Elem(
			MV_TAG,
			new mv(
				subtract(mv(*((e1_t *)elem1->elem)), mv(*((vector *)elem2->elem)))
			)
		)
	);
break;

case BIVECTOR_TAG:
	Glue<E2Elem>::push(
		L, 
		new E2Elem(
			MV_TAG,
			new mv(
				subtract(mv(*((e1_t *)elem1->elem)), mv(*((bivector *)elem2->elem)))
			)
		)
	);
break;

case ROTOR_TAG:
	Glue<E2Elem>::push(
		L, 
		new E2Elem(
			MV_TAG,
			new mv(
				subtract(mv(*((e1_t *)elem1->elem)), mv(*((rotor *)elem2->elem)))
			)
		)
	);
break;

case DOUBLE_TAG:
	Glue<E2Elem>::push(
		L, 
		new E2Elem(
			MV_TAG,
			new mv(
				subtract(mv(*((e1_t *)elem1->elem)), *((double *)elem2))
			)
		)
	);
break;
}
break;

case E2_TAG:
switch(tag2) {
case MV_TAG:
	Glue<E2Elem>::push(
		L, 
		new E2Elem(
			MV_TAG,
			new mv(
				subtract(mv(*((e2_t *)elem1->elem)), mv(*((mv *)elem2->elem)))
			)
		)
	);
break;

case E1_TAG:
	Glue<E2Elem>::push(
		L, 
		new E2Elem(
			MV_TAG,
			new mv(
				subtract(mv(*((e2_t *)elem1->elem)), mv(*((e1_t *)elem2->elem)))
			)
		)
	);
break;

case E2_TAG:
	Glue<E2Elem>::push(
		L, 
		new E2Elem(
			MV_TAG,
			new mv(
				subtract(mv(*((e2_t *)elem1->elem)), mv(*((e2_t *)elem2->elem)))
			)
		)
	);
break;

case I2_TAG:
	Glue<E2Elem>::push(
		L, 
		new E2Elem(
			MV_TAG,
			new mv(
				subtract(mv(*((e2_t *)elem1->elem)), mv(*((I2_t *)elem2->elem)))
			)
		)
	);
break;

case I2i_TAG:
	Glue<E2Elem>::push(
		L, 
		new E2Elem(
			MV_TAG,
			new mv(
				subtract(mv(*((e2_t *)elem1->elem)), mv(*((I2i_t *)elem2->elem)))
			)
		)
	);
break;

case VECTOR_TAG:
	Glue<E2Elem>::push(
		L, 
		new E2Elem(
			MV_TAG,
			new mv(
				subtract(mv(*((e2_t *)elem1->elem)), mv(*((vector *)elem2->elem)))
			)
		)
	);
break;

case BIVECTOR_TAG:
	Glue<E2Elem>::push(
		L, 
		new E2Elem(
			MV_TAG,
			new mv(
				subtract(mv(*((e2_t *)elem1->elem)), mv(*((bivector *)elem2->elem)))
			)
		)
	);
break;

case ROTOR_TAG:
	Glue<E2Elem>::push(
		L, 
		new E2Elem(
			MV_TAG,
			new mv(
				subtract(mv(*((e2_t *)elem1->elem)), mv(*((rotor *)elem2->elem)))
			)
		)
	);
break;

case DOUBLE_TAG:
	Glue<E2Elem>::push(
		L, 
		new E2Elem(
			MV_TAG,
			new mv(
				subtract(mv(*((e2_t *)elem1->elem)), *((double *)elem2))
			)
		)
	);
break;
}
break;

case I2_TAG:
switch(tag2) {
case MV_TAG:
	Glue<E2Elem>::push(
		L, 
		new E2Elem(
			MV_TAG,
			new mv(
				subtract(mv(*((I2_t *)elem1->elem)), mv(*((mv *)elem2->elem)))
			)
		)
	);
break;

case E1_TAG:
	Glue<E2Elem>::push(
		L, 
		new E2Elem(
			MV_TAG,
			new mv(
				subtract(mv(*((I2_t *)elem1->elem)), mv(*((e1_t *)elem2->elem)))
			)
		)
	);
break;

case E2_TAG:
	Glue<E2Elem>::push(
		L, 
		new E2Elem(
			MV_TAG,
			new mv(
				subtract(mv(*((I2_t *)elem1->elem)), mv(*((e2_t *)elem2->elem)))
			)
		)
	);
break;

case I2_TAG:
	Glue<E2Elem>::push(
		L, 
		new E2Elem(
			MV_TAG,
			new mv(
				subtract(mv(*((I2_t *)elem1->elem)), mv(*((I2_t *)elem2->elem)))
			)
		)
	);
break;

case I2i_TAG:
	Glue<E2Elem>::push(
		L, 
		new E2Elem(
			BIVECTOR_TAG,
			new bivector(
				subtract(*((I2_t *)elem1->elem), *((I2i_t *)elem2->elem))
			)
		)
	);
break;

case VECTOR_TAG:
	Glue<E2Elem>::push(
		L, 
		new E2Elem(
			MV_TAG,
			new mv(
				subtract(mv(*((I2_t *)elem1->elem)), mv(*((vector *)elem2->elem)))
			)
		)
	);
break;

case BIVECTOR_TAG:
	Glue<E2Elem>::push(
		L, 
		new E2Elem(
			MV_TAG,
			new mv(
				subtract(mv(*((I2_t *)elem1->elem)), mv(*((bivector *)elem2->elem)))
			)
		)
	);
break;

case ROTOR_TAG:
	Glue<E2Elem>::push(
		L, 
		new E2Elem(
			MV_TAG,
			new mv(
				subtract(mv(*((I2_t *)elem1->elem)), mv(*((rotor *)elem2->elem)))
			)
		)
	);
break;

case DOUBLE_TAG:
	Glue<E2Elem>::push(
		L, 
		new E2Elem(
			MV_TAG,
			new mv(
				subtract(mv(*((I2_t *)elem1->elem)), *((double *)elem2))
			)
		)
	);
break;
}
break;

case I2i_TAG:
switch(tag2) {
case MV_TAG:
	Glue<E2Elem>::push(
		L, 
		new E2Elem(
			MV_TAG,
			new mv(
				subtract(mv(*((I2i_t *)elem1->elem)), mv(*((mv *)elem2->elem)))
			)
		)
	);
break;

case E1_TAG:
	Glue<E2Elem>::push(
		L, 
		new E2Elem(
			MV_TAG,
			new mv(
				subtract(mv(*((I2i_t *)elem1->elem)), mv(*((e1_t *)elem2->elem)))
			)
		)
	);
break;

case E2_TAG:
	Glue<E2Elem>::push(
		L, 
		new E2Elem(
			MV_TAG,
			new mv(
				subtract(mv(*((I2i_t *)elem1->elem)), mv(*((e2_t *)elem2->elem)))
			)
		)
	);
break;

case I2_TAG:
	Glue<E2Elem>::push(
		L, 
		new E2Elem(
			MV_TAG,
			new mv(
				subtract(mv(*((I2i_t *)elem1->elem)), mv(*((I2_t *)elem2->elem)))
			)
		)
	);
break;

case I2i_TAG:
	Glue<E2Elem>::push(
		L, 
		new E2Elem(
			MV_TAG,
			new mv(
				subtract(mv(*((I2i_t *)elem1->elem)), mv(*((I2i_t *)elem2->elem)))
			)
		)
	);
break;

case VECTOR_TAG:
	Glue<E2Elem>::push(
		L, 
		new E2Elem(
			MV_TAG,
			new mv(
				subtract(mv(*((I2i_t *)elem1->elem)), mv(*((vector *)elem2->elem)))
			)
		)
	);
break;

case BIVECTOR_TAG:
	Glue<E2Elem>::push(
		L, 
		new E2Elem(
			MV_TAG,
			new mv(
				subtract(mv(*((I2i_t *)elem1->elem)), mv(*((bivector *)elem2->elem)))
			)
		)
	);
break;

case ROTOR_TAG:
	Glue<E2Elem>::push(
		L, 
		new E2Elem(
			MV_TAG,
			new mv(
				subtract(mv(*((I2i_t *)elem1->elem)), mv(*((rotor *)elem2->elem)))
			)
		)
	);
break;

case DOUBLE_TAG:
	Glue<E2Elem>::push(
		L, 
		new E2Elem(
			MV_TAG,
			new mv(
				subtract(mv(*((I2i_t *)elem1->elem)), *((double *)elem2))
			)
		)
	);
break;
}
break;

case VECTOR_TAG:
switch(tag2) {
case MV_TAG:
	Glue<E2Elem>::push(
		L, 
		new E2Elem(
			MV_TAG,
			new mv(
				subtract(mv(*((vector *)elem1->elem)), mv(*((mv *)elem2->elem)))
			)
		)
	);
break;

case E1_TAG:
	Glue<E2Elem>::push(
		L, 
		new E2Elem(
			MV_TAG,
			new mv(
				subtract(mv(*((vector *)elem1->elem)), mv(*((e1_t *)elem2->elem)))
			)
		)
	);
break;

case E2_TAG:
	Glue<E2Elem>::push(
		L, 
		new E2Elem(
			MV_TAG,
			new mv(
				subtract(mv(*((vector *)elem1->elem)), mv(*((e2_t *)elem2->elem)))
			)
		)
	);
break;

case I2_TAG:
	Glue<E2Elem>::push(
		L, 
		new E2Elem(
			MV_TAG,
			new mv(
				subtract(mv(*((vector *)elem1->elem)), mv(*((I2_t *)elem2->elem)))
			)
		)
	);
break;

case I2i_TAG:
	Glue<E2Elem>::push(
		L, 
		new E2Elem(
			MV_TAG,
			new mv(
				subtract(mv(*((vector *)elem1->elem)), mv(*((I2i_t *)elem2->elem)))
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
				subtract(*((vector *)elem1->elem), *((vector *)elem2->elem))
			)
		)
	);
break;

case BIVECTOR_TAG:
	Glue<E2Elem>::push(
		L, 
		new E2Elem(
			MV_TAG,
			new mv(
				subtract(mv(*((vector *)elem1->elem)), mv(*((bivector *)elem2->elem)))
			)
		)
	);
break;

case ROTOR_TAG:
	Glue<E2Elem>::push(
		L, 
		new E2Elem(
			MV_TAG,
			new mv(
				subtract(mv(*((vector *)elem1->elem)), mv(*((rotor *)elem2->elem)))
			)
		)
	);
break;

case DOUBLE_TAG:
	Glue<E2Elem>::push(
		L, 
		new E2Elem(
			MV_TAG,
			new mv(
				subtract(mv(*((vector *)elem1->elem)), *((double *)elem2))
			)
		)
	);
break;
}
break;

case BIVECTOR_TAG:
switch(tag2) {
case MV_TAG:
	Glue<E2Elem>::push(
		L, 
		new E2Elem(
			MV_TAG,
			new mv(
				subtract(mv(*((bivector *)elem1->elem)), mv(*((mv *)elem2->elem)))
			)
		)
	);
break;

case E1_TAG:
	Glue<E2Elem>::push(
		L, 
		new E2Elem(
			MV_TAG,
			new mv(
				subtract(mv(*((bivector *)elem1->elem)), mv(*((e1_t *)elem2->elem)))
			)
		)
	);
break;

case E2_TAG:
	Glue<E2Elem>::push(
		L, 
		new E2Elem(
			MV_TAG,
			new mv(
				subtract(mv(*((bivector *)elem1->elem)), mv(*((e2_t *)elem2->elem)))
			)
		)
	);
break;

case I2_TAG:
	Glue<E2Elem>::push(
		L, 
		new E2Elem(
			MV_TAG,
			new mv(
				subtract(mv(*((bivector *)elem1->elem)), mv(*((I2_t *)elem2->elem)))
			)
		)
	);
break;

case I2i_TAG:
	Glue<E2Elem>::push(
		L, 
		new E2Elem(
			MV_TAG,
			new mv(
				subtract(mv(*((bivector *)elem1->elem)), mv(*((I2i_t *)elem2->elem)))
			)
		)
	);
break;

case VECTOR_TAG:
	Glue<E2Elem>::push(
		L, 
		new E2Elem(
			MV_TAG,
			new mv(
				subtract(mv(*((bivector *)elem1->elem)), mv(*((vector *)elem2->elem)))
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
				subtract(*((bivector *)elem1->elem), *((bivector *)elem2->elem))
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
				subtract(*((bivector *)elem1->elem), *((rotor *)elem2->elem))
			)
		)
	);
break;

case DOUBLE_TAG:
	Glue<E2Elem>::push(
		L, 
		new E2Elem(
			MV_TAG,
			new mv(
				subtract(mv(*((bivector *)elem1->elem)), *((double *)elem2))
			)
		)
	);
break;
}
break;

case ROTOR_TAG:
switch(tag2) {
case MV_TAG:
	Glue<E2Elem>::push(
		L, 
		new E2Elem(
			MV_TAG,
			new mv(
				subtract(mv(*((rotor *)elem1->elem)), mv(*((mv *)elem2->elem)))
			)
		)
	);
break;

case E1_TAG:
	Glue<E2Elem>::push(
		L, 
		new E2Elem(
			MV_TAG,
			new mv(
				subtract(mv(*((rotor *)elem1->elem)), mv(*((e1_t *)elem2->elem)))
			)
		)
	);
break;

case E2_TAG:
	Glue<E2Elem>::push(
		L, 
		new E2Elem(
			MV_TAG,
			new mv(
				subtract(mv(*((rotor *)elem1->elem)), mv(*((e2_t *)elem2->elem)))
			)
		)
	);
break;

case I2_TAG:
	Glue<E2Elem>::push(
		L, 
		new E2Elem(
			MV_TAG,
			new mv(
				subtract(mv(*((rotor *)elem1->elem)), mv(*((I2_t *)elem2->elem)))
			)
		)
	);
break;

case I2i_TAG:
	Glue<E2Elem>::push(
		L, 
		new E2Elem(
			MV_TAG,
			new mv(
				subtract(mv(*((rotor *)elem1->elem)), mv(*((I2i_t *)elem2->elem)))
			)
		)
	);
break;

case VECTOR_TAG:
	Glue<E2Elem>::push(
		L, 
		new E2Elem(
			MV_TAG,
			new mv(
				subtract(mv(*((rotor *)elem1->elem)), mv(*((vector *)elem2->elem)))
			)
		)
	);
break;

case BIVECTOR_TAG:
	Glue<E2Elem>::push(
		L, 
		new E2Elem(
			MV_TAG,
			new mv(
				subtract(mv(*((rotor *)elem1->elem)), mv(*((bivector *)elem2->elem)))
			)
		)
	);
break;

case ROTOR_TAG:
	Glue<E2Elem>::push(
		L, 
		new E2Elem(
			MV_TAG,
			new mv(
				subtract(mv(*((rotor *)elem1->elem)), mv(*((rotor *)elem2->elem)))
			)
		)
	);
break;

case DOUBLE_TAG:
	Glue<E2Elem>::push(
		L, 
		new E2Elem(
			MV_TAG,
			new mv(
				subtract(mv(*((rotor *)elem1->elem)), *((double *)elem2))
			)
		)
	);
break;
}
break;

case DOUBLE_TAG:
switch(tag2) {
case MV_TAG:
	Glue<E2Elem>::push(
		L, 
		new E2Elem(
			MV_TAG,
			new mv(
				subtract(*((double *)elem1), mv(*((mv *)elem2->elem)))
			)
		)
	);
break;

case E1_TAG:
	Glue<E2Elem>::push(
		L, 
		new E2Elem(
			MV_TAG,
			new mv(
				subtract(*((double *)elem1), mv(*((e1_t *)elem2->elem)))
			)
		)
	);
break;

case E2_TAG:
	Glue<E2Elem>::push(
		L, 
		new E2Elem(
			MV_TAG,
			new mv(
				subtract(*((double *)elem1), mv(*((e2_t *)elem2->elem)))
			)
		)
	);
break;

case I2_TAG:
	Glue<E2Elem>::push(
		L, 
		new E2Elem(
			MV_TAG,
			new mv(
				subtract(*((double *)elem1), mv(*((I2_t *)elem2->elem)))
			)
		)
	);
break;

case I2i_TAG:
	Glue<E2Elem>::push(
		L, 
		new E2Elem(
			MV_TAG,
			new mv(
				subtract(*((double *)elem1), mv(*((I2i_t *)elem2->elem)))
			)
		)
	);
break;

case VECTOR_TAG:
	Glue<E2Elem>::push(
		L, 
		new E2Elem(
			MV_TAG,
			new mv(
				subtract(*((double *)elem1), mv(*((vector *)elem2->elem)))
			)
		)
	);
break;

case BIVECTOR_TAG:
	Glue<E2Elem>::push(
		L, 
		new E2Elem(
			MV_TAG,
			new mv(
				subtract(*((double *)elem1), mv(*((bivector *)elem2->elem)))
			)
		)
	);
break;

case ROTOR_TAG:
	Glue<E2Elem>::push(
		L, 
		new E2Elem(
			MV_TAG,
			new mv(
				subtract(*((double *)elem1), mv(*((rotor *)elem2->elem)))
			)
		)
	);
break;

case DOUBLE_TAG:
	Glue<E2Elem>::push(
		L, 
		new E2Elem(
			MV_TAG,
			new mv(
				subtract(*((double *)elem1), *((double *)elem2))
			)
		)
	);
break;
}
break;
}
return 1;
}


int e2space_hip(lua_State *L) {
	double d1, d2;
	E2Tag tag1;
	E2Tag tag2;
	E2Elem *elem1 = Glue<E2Elem>::to(L, 1);
	E2Elem *elem2 = Glue<E2Elem>::to(L, 2);
	
	
	if(!elem1) {
		if(lua::is<double>(L, 1)) {
			d1 = lua::to<double>(L, 1);
			elem1 = (E2Elem *)(&d1);
			tag1 = DOUBLE_TAG;
		}
		else {
			luaL_error(L, "e2.hip: invalid arguments");
		}
	}
	else {
		tag1 = elem1->tag;
	}
	
	if(!elem2) {
		if(lua::is<double>(L, 2)) {
			d2 = lua::to<double>(L, 2);
			elem2 = (E2Elem *)(&d2);
			tag2 = DOUBLE_TAG;
		}
		else {
			luaL_error(L, "e2.hip: invalid arguments");
		}
	}
	else {
		tag2 = elem2->tag;
	}
	
switch(tag1) {
case MV_TAG:
switch(tag2) {
case MV_TAG:
	Glue<E2Elem>::push(
		L, 
		new E2Elem(
			MV_TAG,
			new mv(
				hip(*((mv *)elem1->elem), *((mv *)elem2->elem))
			)
		)
	);
break;

case E1_TAG:
	Glue<E2Elem>::push(
		L, 
		new E2Elem(
			MV_TAG,
			new mv(
				hip(mv(*((mv *)elem1->elem)), mv(*((e1_t *)elem2->elem)))
			)
		)
	);
break;

case E2_TAG:
	Glue<E2Elem>::push(
		L, 
		new E2Elem(
			MV_TAG,
			new mv(
				hip(mv(*((mv *)elem1->elem)), mv(*((e2_t *)elem2->elem)))
			)
		)
	);
break;

case I2_TAG:
	Glue<E2Elem>::push(
		L, 
		new E2Elem(
			MV_TAG,
			new mv(
				hip(mv(*((mv *)elem1->elem)), mv(*((I2_t *)elem2->elem)))
			)
		)
	);
break;

case I2i_TAG:
	Glue<E2Elem>::push(
		L, 
		new E2Elem(
			MV_TAG,
			new mv(
				hip(mv(*((mv *)elem1->elem)), mv(*((I2i_t *)elem2->elem)))
			)
		)
	);
break;

case VECTOR_TAG:
	Glue<E2Elem>::push(
		L, 
		new E2Elem(
			MV_TAG,
			new mv(
				hip(mv(*((mv *)elem1->elem)), mv(*((vector *)elem2->elem)))
			)
		)
	);
break;

case BIVECTOR_TAG:
	Glue<E2Elem>::push(
		L, 
		new E2Elem(
			MV_TAG,
			new mv(
				hip(mv(*((mv *)elem1->elem)), mv(*((bivector *)elem2->elem)))
			)
		)
	);
break;

case ROTOR_TAG:
	Glue<E2Elem>::push(
		L, 
		new E2Elem(
			MV_TAG,
			new mv(
				hip(mv(*((mv *)elem1->elem)), mv(*((rotor *)elem2->elem)))
			)
		)
	);
break;

case DOUBLE_TAG:
	Glue<E2Elem>::push(
		L, 
		new E2Elem(
			MV_TAG,
			new mv(
				hip(mv(*((mv *)elem1->elem)), *((double *)elem2))
			)
		)
	);
break;
}
break;

case E1_TAG:
switch(tag2) {
case MV_TAG:
	Glue<E2Elem>::push(
		L, 
		new E2Elem(
			MV_TAG,
			new mv(
				hip(mv(*((e1_t *)elem1->elem)), mv(*((mv *)elem2->elem)))
			)
		)
	);
break;

case E1_TAG:
	Glue<E2Elem>::push(
		L, 
		new E2Elem(
			MV_TAG,
			new mv(
				hip(mv(*((e1_t *)elem1->elem)), mv(*((e1_t *)elem2->elem)))
			)
		)
	);
break;

case E2_TAG:
	Glue<E2Elem>::push(
		L, 
		new E2Elem(
			MV_TAG,
			new mv(
				hip(mv(*((e1_t *)elem1->elem)), mv(*((e2_t *)elem2->elem)))
			)
		)
	);
break;

case I2_TAG:
	Glue<E2Elem>::push(
		L, 
		new E2Elem(
			MV_TAG,
			new mv(
				hip(mv(*((e1_t *)elem1->elem)), mv(*((I2_t *)elem2->elem)))
			)
		)
	);
break;

case I2i_TAG:
	Glue<E2Elem>::push(
		L, 
		new E2Elem(
			MV_TAG,
			new mv(
				hip(mv(*((e1_t *)elem1->elem)), mv(*((I2i_t *)elem2->elem)))
			)
		)
	);
break;

case VECTOR_TAG:
	Glue<E2Elem>::push(
		L, 
		new E2Elem(
			MV_TAG,
			new mv(
				hip(mv(*((e1_t *)elem1->elem)), mv(*((vector *)elem2->elem)))
			)
		)
	);
break;

case BIVECTOR_TAG:
	Glue<E2Elem>::push(
		L, 
		new E2Elem(
			VECTOR_TAG,
			new vector(
				hip(*((e1_t *)elem1->elem), *((bivector *)elem2->elem))
			)
		)
	);
break;

case ROTOR_TAG:
	Glue<E2Elem>::push(
		L, 
		new E2Elem(
			MV_TAG,
			new mv(
				hip(mv(*((e1_t *)elem1->elem)), mv(*((rotor *)elem2->elem)))
			)
		)
	);
break;

case DOUBLE_TAG:
	Glue<E2Elem>::push(
		L, 
		new E2Elem(
			MV_TAG,
			new mv(
				hip(mv(*((e1_t *)elem1->elem)), *((double *)elem2))
			)
		)
	);
break;
}
break;

case E2_TAG:
switch(tag2) {
case MV_TAG:
	Glue<E2Elem>::push(
		L, 
		new E2Elem(
			MV_TAG,
			new mv(
				hip(mv(*((e2_t *)elem1->elem)), mv(*((mv *)elem2->elem)))
			)
		)
	);
break;

case E1_TAG:
	Glue<E2Elem>::push(
		L, 
		new E2Elem(
			MV_TAG,
			new mv(
				hip(mv(*((e2_t *)elem1->elem)), mv(*((e1_t *)elem2->elem)))
			)
		)
	);
break;

case E2_TAG:
	Glue<E2Elem>::push(
		L, 
		new E2Elem(
			MV_TAG,
			new mv(
				hip(mv(*((e2_t *)elem1->elem)), mv(*((e2_t *)elem2->elem)))
			)
		)
	);
break;

case I2_TAG:
	Glue<E2Elem>::push(
		L, 
		new E2Elem(
			MV_TAG,
			new mv(
				hip(mv(*((e2_t *)elem1->elem)), mv(*((I2_t *)elem2->elem)))
			)
		)
	);
break;

case I2i_TAG:
	Glue<E2Elem>::push(
		L, 
		new E2Elem(
			MV_TAG,
			new mv(
				hip(mv(*((e2_t *)elem1->elem)), mv(*((I2i_t *)elem2->elem)))
			)
		)
	);
break;

case VECTOR_TAG:
	Glue<E2Elem>::push(
		L, 
		new E2Elem(
			MV_TAG,
			new mv(
				hip(mv(*((e2_t *)elem1->elem)), mv(*((vector *)elem2->elem)))
			)
		)
	);
break;

case BIVECTOR_TAG:
	Glue<E2Elem>::push(
		L, 
		new E2Elem(
			MV_TAG,
			new mv(
				hip(mv(*((e2_t *)elem1->elem)), mv(*((bivector *)elem2->elem)))
			)
		)
	);
break;

case ROTOR_TAG:
	Glue<E2Elem>::push(
		L, 
		new E2Elem(
			MV_TAG,
			new mv(
				hip(mv(*((e2_t *)elem1->elem)), mv(*((rotor *)elem2->elem)))
			)
		)
	);
break;

case DOUBLE_TAG:
	Glue<E2Elem>::push(
		L, 
		new E2Elem(
			MV_TAG,
			new mv(
				hip(mv(*((e2_t *)elem1->elem)), *((double *)elem2))
			)
		)
	);
break;
}
break;

case I2_TAG:
switch(tag2) {
case MV_TAG:
	Glue<E2Elem>::push(
		L, 
		new E2Elem(
			MV_TAG,
			new mv(
				hip(mv(*((I2_t *)elem1->elem)), mv(*((mv *)elem2->elem)))
			)
		)
	);
break;

case E1_TAG:
	Glue<E2Elem>::push(
		L, 
		new E2Elem(
			MV_TAG,
			new mv(
				hip(mv(*((I2_t *)elem1->elem)), mv(*((e1_t *)elem2->elem)))
			)
		)
	);
break;

case E2_TAG:
	Glue<E2Elem>::push(
		L, 
		new E2Elem(
			MV_TAG,
			new mv(
				hip(mv(*((I2_t *)elem1->elem)), mv(*((e2_t *)elem2->elem)))
			)
		)
	);
break;

case I2_TAG:
	Glue<E2Elem>::push(
		L, 
		new E2Elem(
			MV_TAG,
			new mv(
				hip(mv(*((I2_t *)elem1->elem)), mv(*((I2_t *)elem2->elem)))
			)
		)
	);
break;

case I2i_TAG:
	Glue<E2Elem>::push(
		L, 
		new E2Elem(
			MV_TAG,
			new mv(
				hip(mv(*((I2_t *)elem1->elem)), mv(*((I2i_t *)elem2->elem)))
			)
		)
	);
break;

case VECTOR_TAG:
	Glue<E2Elem>::push(
		L, 
		new E2Elem(
			MV_TAG,
			new mv(
				hip(mv(*((I2_t *)elem1->elem)), mv(*((vector *)elem2->elem)))
			)
		)
	);
break;

case BIVECTOR_TAG:
	Glue<E2Elem>::push(
		L, 
		new E2Elem(
			MV_TAG,
			new mv(
				hip(mv(*((I2_t *)elem1->elem)), mv(*((bivector *)elem2->elem)))
			)
		)
	);
break;

case ROTOR_TAG:
	Glue<E2Elem>::push(
		L, 
		new E2Elem(
			MV_TAG,
			new mv(
				hip(mv(*((I2_t *)elem1->elem)), mv(*((rotor *)elem2->elem)))
			)
		)
	);
break;

case DOUBLE_TAG:
	Glue<E2Elem>::push(
		L, 
		new E2Elem(
			MV_TAG,
			new mv(
				hip(mv(*((I2_t *)elem1->elem)), *((double *)elem2))
			)
		)
	);
break;
}
break;

case I2i_TAG:
switch(tag2) {
case MV_TAG:
	Glue<E2Elem>::push(
		L, 
		new E2Elem(
			MV_TAG,
			new mv(
				hip(mv(*((I2i_t *)elem1->elem)), mv(*((mv *)elem2->elem)))
			)
		)
	);
break;

case E1_TAG:
	Glue<E2Elem>::push(
		L, 
		new E2Elem(
			MV_TAG,
			new mv(
				hip(mv(*((I2i_t *)elem1->elem)), mv(*((e1_t *)elem2->elem)))
			)
		)
	);
break;

case E2_TAG:
	Glue<E2Elem>::push(
		L, 
		new E2Elem(
			MV_TAG,
			new mv(
				hip(mv(*((I2i_t *)elem1->elem)), mv(*((e2_t *)elem2->elem)))
			)
		)
	);
break;

case I2_TAG:
	Glue<E2Elem>::push(
		L, 
		new E2Elem(
			MV_TAG,
			new mv(
				hip(mv(*((I2i_t *)elem1->elem)), mv(*((I2_t *)elem2->elem)))
			)
		)
	);
break;

case I2i_TAG:
	Glue<E2Elem>::push(
		L, 
		new E2Elem(
			MV_TAG,
			new mv(
				hip(mv(*((I2i_t *)elem1->elem)), mv(*((I2i_t *)elem2->elem)))
			)
		)
	);
break;

case VECTOR_TAG:
	Glue<E2Elem>::push(
		L, 
		new E2Elem(
			MV_TAG,
			new mv(
				hip(mv(*((I2i_t *)elem1->elem)), mv(*((vector *)elem2->elem)))
			)
		)
	);
break;

case BIVECTOR_TAG:
	Glue<E2Elem>::push(
		L, 
		new E2Elem(
			MV_TAG,
			new mv(
				hip(mv(*((I2i_t *)elem1->elem)), mv(*((bivector *)elem2->elem)))
			)
		)
	);
break;

case ROTOR_TAG:
	Glue<E2Elem>::push(
		L, 
		new E2Elem(
			MV_TAG,
			new mv(
				hip(mv(*((I2i_t *)elem1->elem)), mv(*((rotor *)elem2->elem)))
			)
		)
	);
break;

case DOUBLE_TAG:
	Glue<E2Elem>::push(
		L, 
		new E2Elem(
			MV_TAG,
			new mv(
				hip(mv(*((I2i_t *)elem1->elem)), *((double *)elem2))
			)
		)
	);
break;
}
break;

case VECTOR_TAG:
switch(tag2) {
case MV_TAG:
	Glue<E2Elem>::push(
		L, 
		new E2Elem(
			MV_TAG,
			new mv(
				hip(mv(*((vector *)elem1->elem)), mv(*((mv *)elem2->elem)))
			)
		)
	);
break;

case E1_TAG:
	Glue<E2Elem>::push(
		L, 
		new E2Elem(
			MV_TAG,
			new mv(
				hip(mv(*((vector *)elem1->elem)), mv(*((e1_t *)elem2->elem)))
			)
		)
	);
break;

case E2_TAG:
	Glue<E2Elem>::push(
		L, 
		new E2Elem(
			MV_TAG,
			new mv(
				hip(mv(*((vector *)elem1->elem)), mv(*((e2_t *)elem2->elem)))
			)
		)
	);
break;

case I2_TAG:
	Glue<E2Elem>::push(
		L, 
		new E2Elem(
			MV_TAG,
			new mv(
				hip(mv(*((vector *)elem1->elem)), mv(*((I2_t *)elem2->elem)))
			)
		)
	);
break;

case I2i_TAG:
	Glue<E2Elem>::push(
		L, 
		new E2Elem(
			MV_TAG,
			new mv(
				hip(mv(*((vector *)elem1->elem)), mv(*((I2i_t *)elem2->elem)))
			)
		)
	);
break;

case VECTOR_TAG:
				lua::push<double>(
					L, 
					hip(*((vector *)elem1->elem), *((vector *)elem2->elem))
				);
			
break;

case BIVECTOR_TAG:
	Glue<E2Elem>::push(
		L, 
		new E2Elem(
			VECTOR_TAG,
			new vector(
				hip(*((vector *)elem1->elem), *((bivector *)elem2->elem))
			)
		)
	);
break;

case ROTOR_TAG:
	Glue<E2Elem>::push(
		L, 
		new E2Elem(
			VECTOR_TAG,
			new vector(
				hip(*((vector *)elem1->elem), *((rotor *)elem2->elem))
			)
		)
	);
break;

case DOUBLE_TAG:
	Glue<E2Elem>::push(
		L, 
		new E2Elem(
			MV_TAG,
			new mv(
				hip(mv(*((vector *)elem1->elem)), *((double *)elem2))
			)
		)
	);
break;
}
break;

case BIVECTOR_TAG:
switch(tag2) {
case MV_TAG:
	Glue<E2Elem>::push(
		L, 
		new E2Elem(
			MV_TAG,
			new mv(
				hip(mv(*((bivector *)elem1->elem)), mv(*((mv *)elem2->elem)))
			)
		)
	);
break;

case E1_TAG:
	Glue<E2Elem>::push(
		L, 
		new E2Elem(
			MV_TAG,
			new mv(
				hip(mv(*((bivector *)elem1->elem)), mv(*((e1_t *)elem2->elem)))
			)
		)
	);
break;

case E2_TAG:
	Glue<E2Elem>::push(
		L, 
		new E2Elem(
			MV_TAG,
			new mv(
				hip(mv(*((bivector *)elem1->elem)), mv(*((e2_t *)elem2->elem)))
			)
		)
	);
break;

case I2_TAG:
	Glue<E2Elem>::push(
		L, 
		new E2Elem(
			MV_TAG,
			new mv(
				hip(mv(*((bivector *)elem1->elem)), mv(*((I2_t *)elem2->elem)))
			)
		)
	);
break;

case I2i_TAG:
	Glue<E2Elem>::push(
		L, 
		new E2Elem(
			MV_TAG,
			new mv(
				hip(mv(*((bivector *)elem1->elem)), mv(*((I2i_t *)elem2->elem)))
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
				hip(*((bivector *)elem1->elem), *((vector *)elem2->elem))
			)
		)
	);
break;

case BIVECTOR_TAG:
	Glue<E2Elem>::push(
		L, 
		new E2Elem(
			MV_TAG,
			new mv(
				hip(mv(*((bivector *)elem1->elem)), mv(*((bivector *)elem2->elem)))
			)
		)
	);
break;

case ROTOR_TAG:
	Glue<E2Elem>::push(
		L, 
		new E2Elem(
			MV_TAG,
			new mv(
				hip(mv(*((bivector *)elem1->elem)), mv(*((rotor *)elem2->elem)))
			)
		)
	);
break;

case DOUBLE_TAG:
	Glue<E2Elem>::push(
		L, 
		new E2Elem(
			MV_TAG,
			new mv(
				hip(mv(*((bivector *)elem1->elem)), *((double *)elem2))
			)
		)
	);
break;
}
break;

case ROTOR_TAG:
switch(tag2) {
case MV_TAG:
	Glue<E2Elem>::push(
		L, 
		new E2Elem(
			MV_TAG,
			new mv(
				hip(mv(*((rotor *)elem1->elem)), mv(*((mv *)elem2->elem)))
			)
		)
	);
break;

case E1_TAG:
	Glue<E2Elem>::push(
		L, 
		new E2Elem(
			MV_TAG,
			new mv(
				hip(mv(*((rotor *)elem1->elem)), mv(*((e1_t *)elem2->elem)))
			)
		)
	);
break;

case E2_TAG:
	Glue<E2Elem>::push(
		L, 
		new E2Elem(
			MV_TAG,
			new mv(
				hip(mv(*((rotor *)elem1->elem)), mv(*((e2_t *)elem2->elem)))
			)
		)
	);
break;

case I2_TAG:
	Glue<E2Elem>::push(
		L, 
		new E2Elem(
			MV_TAG,
			new mv(
				hip(mv(*((rotor *)elem1->elem)), mv(*((I2_t *)elem2->elem)))
			)
		)
	);
break;

case I2i_TAG:
	Glue<E2Elem>::push(
		L, 
		new E2Elem(
			MV_TAG,
			new mv(
				hip(mv(*((rotor *)elem1->elem)), mv(*((I2i_t *)elem2->elem)))
			)
		)
	);
break;

case VECTOR_TAG:
	Glue<E2Elem>::push(
		L, 
		new E2Elem(
			MV_TAG,
			new mv(
				hip(mv(*((rotor *)elem1->elem)), mv(*((vector *)elem2->elem)))
			)
		)
	);
break;

case BIVECTOR_TAG:
				lua::push<double>(
					L, 
					hip(*((rotor *)elem1->elem), *((bivector *)elem2->elem))
				);
			
break;

case ROTOR_TAG:
	Glue<E2Elem>::push(
		L, 
		new E2Elem(
			MV_TAG,
			new mv(
				hip(mv(*((rotor *)elem1->elem)), mv(*((rotor *)elem2->elem)))
			)
		)
	);
break;

case DOUBLE_TAG:
	Glue<E2Elem>::push(
		L, 
		new E2Elem(
			MV_TAG,
			new mv(
				hip(mv(*((rotor *)elem1->elem)), *((double *)elem2))
			)
		)
	);
break;
}
break;

case DOUBLE_TAG:
switch(tag2) {
case MV_TAG:
	Glue<E2Elem>::push(
		L, 
		new E2Elem(
			MV_TAG,
			new mv(
				hip(*((double *)elem1), mv(*((mv *)elem2->elem)))
			)
		)
	);
break;

case E1_TAG:
	Glue<E2Elem>::push(
		L, 
		new E2Elem(
			MV_TAG,
			new mv(
				hip(*((double *)elem1), mv(*((e1_t *)elem2->elem)))
			)
		)
	);
break;

case E2_TAG:
	Glue<E2Elem>::push(
		L, 
		new E2Elem(
			MV_TAG,
			new mv(
				hip(*((double *)elem1), mv(*((e2_t *)elem2->elem)))
			)
		)
	);
break;

case I2_TAG:
	Glue<E2Elem>::push(
		L, 
		new E2Elem(
			MV_TAG,
			new mv(
				hip(*((double *)elem1), mv(*((I2_t *)elem2->elem)))
			)
		)
	);
break;

case I2i_TAG:
	Glue<E2Elem>::push(
		L, 
		new E2Elem(
			MV_TAG,
			new mv(
				hip(*((double *)elem1), mv(*((I2i_t *)elem2->elem)))
			)
		)
	);
break;

case VECTOR_TAG:
	Glue<E2Elem>::push(
		L, 
		new E2Elem(
			MV_TAG,
			new mv(
				hip(*((double *)elem1), mv(*((vector *)elem2->elem)))
			)
		)
	);
break;

case BIVECTOR_TAG:
	Glue<E2Elem>::push(
		L, 
		new E2Elem(
			MV_TAG,
			new mv(
				hip(*((double *)elem1), mv(*((bivector *)elem2->elem)))
			)
		)
	);
break;

case ROTOR_TAG:
	Glue<E2Elem>::push(
		L, 
		new E2Elem(
			MV_TAG,
			new mv(
				hip(*((double *)elem1), mv(*((rotor *)elem2->elem)))
			)
		)
	);
break;

case DOUBLE_TAG:
	Glue<E2Elem>::push(
		L, 
		new E2Elem(
			MV_TAG,
			new mv(
				hip(*((double *)elem1), *((double *)elem2))
			)
		)
	);
break;
}
break;
}
return 1;
}


int e2space_mhip(lua_State *L) {
	double d1, d2;
	E2Tag tag1;
	E2Tag tag2;
	E2Elem *elem1 = Glue<E2Elem>::to(L, 1);
	E2Elem *elem2 = Glue<E2Elem>::to(L, 2);
	
	
	if(!elem1) {
		if(lua::is<double>(L, 1)) {
			d1 = lua::to<double>(L, 1);
			elem1 = (E2Elem *)(&d1);
			tag1 = DOUBLE_TAG;
		}
		else {
			luaL_error(L, "e2.mhip: invalid arguments");
		}
	}
	else {
		tag1 = elem1->tag;
	}
	
	if(!elem2) {
		if(lua::is<double>(L, 2)) {
			d2 = lua::to<double>(L, 2);
			elem2 = (E2Elem *)(&d2);
			tag2 = DOUBLE_TAG;
		}
		else {
			luaL_error(L, "e2.mhip: invalid arguments");
		}
	}
	else {
		tag2 = elem2->tag;
	}
	
switch(tag1) {
case MV_TAG:
switch(tag2) {
case MV_TAG:
	Glue<E2Elem>::push(
		L, 
		new E2Elem(
			MV_TAG,
			new mv(
				mhip(*((mv *)elem1->elem), *((mv *)elem2->elem))
			)
		)
	);
break;

case E1_TAG:
	Glue<E2Elem>::push(
		L, 
		new E2Elem(
			MV_TAG,
			new mv(
				mhip(mv(*((mv *)elem1->elem)), mv(*((e1_t *)elem2->elem)))
			)
		)
	);
break;

case E2_TAG:
	Glue<E2Elem>::push(
		L, 
		new E2Elem(
			MV_TAG,
			new mv(
				mhip(mv(*((mv *)elem1->elem)), mv(*((e2_t *)elem2->elem)))
			)
		)
	);
break;

case I2_TAG:
	Glue<E2Elem>::push(
		L, 
		new E2Elem(
			MV_TAG,
			new mv(
				mhip(mv(*((mv *)elem1->elem)), mv(*((I2_t *)elem2->elem)))
			)
		)
	);
break;

case I2i_TAG:
	Glue<E2Elem>::push(
		L, 
		new E2Elem(
			MV_TAG,
			new mv(
				mhip(mv(*((mv *)elem1->elem)), mv(*((I2i_t *)elem2->elem)))
			)
		)
	);
break;

case VECTOR_TAG:
	Glue<E2Elem>::push(
		L, 
		new E2Elem(
			MV_TAG,
			new mv(
				mhip(mv(*((mv *)elem1->elem)), mv(*((vector *)elem2->elem)))
			)
		)
	);
break;

case BIVECTOR_TAG:
	Glue<E2Elem>::push(
		L, 
		new E2Elem(
			MV_TAG,
			new mv(
				mhip(mv(*((mv *)elem1->elem)), mv(*((bivector *)elem2->elem)))
			)
		)
	);
break;

case ROTOR_TAG:
	Glue<E2Elem>::push(
		L, 
		new E2Elem(
			MV_TAG,
			new mv(
				mhip(mv(*((mv *)elem1->elem)), mv(*((rotor *)elem2->elem)))
			)
		)
	);
break;

case DOUBLE_TAG:
	Glue<E2Elem>::push(
		L, 
		new E2Elem(
			MV_TAG,
			new mv(
				mhip(mv(*((mv *)elem1->elem)), *((double *)elem2))
			)
		)
	);
break;
}
break;

case E1_TAG:
switch(tag2) {
case MV_TAG:
	Glue<E2Elem>::push(
		L, 
		new E2Elem(
			MV_TAG,
			new mv(
				mhip(mv(*((e1_t *)elem1->elem)), mv(*((mv *)elem2->elem)))
			)
		)
	);
break;

case E1_TAG:
	Glue<E2Elem>::push(
		L, 
		new E2Elem(
			MV_TAG,
			new mv(
				mhip(mv(*((e1_t *)elem1->elem)), mv(*((e1_t *)elem2->elem)))
			)
		)
	);
break;

case E2_TAG:
	Glue<E2Elem>::push(
		L, 
		new E2Elem(
			MV_TAG,
			new mv(
				mhip(mv(*((e1_t *)elem1->elem)), mv(*((e2_t *)elem2->elem)))
			)
		)
	);
break;

case I2_TAG:
	Glue<E2Elem>::push(
		L, 
		new E2Elem(
			MV_TAG,
			new mv(
				mhip(mv(*((e1_t *)elem1->elem)), mv(*((I2_t *)elem2->elem)))
			)
		)
	);
break;

case I2i_TAG:
	Glue<E2Elem>::push(
		L, 
		new E2Elem(
			MV_TAG,
			new mv(
				mhip(mv(*((e1_t *)elem1->elem)), mv(*((I2i_t *)elem2->elem)))
			)
		)
	);
break;

case VECTOR_TAG:
	Glue<E2Elem>::push(
		L, 
		new E2Elem(
			MV_TAG,
			new mv(
				mhip(mv(*((e1_t *)elem1->elem)), mv(*((vector *)elem2->elem)))
			)
		)
	);
break;

case BIVECTOR_TAG:
	Glue<E2Elem>::push(
		L, 
		new E2Elem(
			MV_TAG,
			new mv(
				mhip(mv(*((e1_t *)elem1->elem)), mv(*((bivector *)elem2->elem)))
			)
		)
	);
break;

case ROTOR_TAG:
	Glue<E2Elem>::push(
		L, 
		new E2Elem(
			MV_TAG,
			new mv(
				mhip(mv(*((e1_t *)elem1->elem)), mv(*((rotor *)elem2->elem)))
			)
		)
	);
break;

case DOUBLE_TAG:
	Glue<E2Elem>::push(
		L, 
		new E2Elem(
			MV_TAG,
			new mv(
				mhip(mv(*((e1_t *)elem1->elem)), *((double *)elem2))
			)
		)
	);
break;
}
break;

case E2_TAG:
switch(tag2) {
case MV_TAG:
	Glue<E2Elem>::push(
		L, 
		new E2Elem(
			MV_TAG,
			new mv(
				mhip(mv(*((e2_t *)elem1->elem)), mv(*((mv *)elem2->elem)))
			)
		)
	);
break;

case E1_TAG:
	Glue<E2Elem>::push(
		L, 
		new E2Elem(
			MV_TAG,
			new mv(
				mhip(mv(*((e2_t *)elem1->elem)), mv(*((e1_t *)elem2->elem)))
			)
		)
	);
break;

case E2_TAG:
	Glue<E2Elem>::push(
		L, 
		new E2Elem(
			MV_TAG,
			new mv(
				mhip(mv(*((e2_t *)elem1->elem)), mv(*((e2_t *)elem2->elem)))
			)
		)
	);
break;

case I2_TAG:
	Glue<E2Elem>::push(
		L, 
		new E2Elem(
			MV_TAG,
			new mv(
				mhip(mv(*((e2_t *)elem1->elem)), mv(*((I2_t *)elem2->elem)))
			)
		)
	);
break;

case I2i_TAG:
	Glue<E2Elem>::push(
		L, 
		new E2Elem(
			MV_TAG,
			new mv(
				mhip(mv(*((e2_t *)elem1->elem)), mv(*((I2i_t *)elem2->elem)))
			)
		)
	);
break;

case VECTOR_TAG:
	Glue<E2Elem>::push(
		L, 
		new E2Elem(
			MV_TAG,
			new mv(
				mhip(mv(*((e2_t *)elem1->elem)), mv(*((vector *)elem2->elem)))
			)
		)
	);
break;

case BIVECTOR_TAG:
	Glue<E2Elem>::push(
		L, 
		new E2Elem(
			MV_TAG,
			new mv(
				mhip(mv(*((e2_t *)elem1->elem)), mv(*((bivector *)elem2->elem)))
			)
		)
	);
break;

case ROTOR_TAG:
	Glue<E2Elem>::push(
		L, 
		new E2Elem(
			MV_TAG,
			new mv(
				mhip(mv(*((e2_t *)elem1->elem)), mv(*((rotor *)elem2->elem)))
			)
		)
	);
break;

case DOUBLE_TAG:
	Glue<E2Elem>::push(
		L, 
		new E2Elem(
			MV_TAG,
			new mv(
				mhip(mv(*((e2_t *)elem1->elem)), *((double *)elem2))
			)
		)
	);
break;
}
break;

case I2_TAG:
switch(tag2) {
case MV_TAG:
	Glue<E2Elem>::push(
		L, 
		new E2Elem(
			MV_TAG,
			new mv(
				mhip(mv(*((I2_t *)elem1->elem)), mv(*((mv *)elem2->elem)))
			)
		)
	);
break;

case E1_TAG:
	Glue<E2Elem>::push(
		L, 
		new E2Elem(
			MV_TAG,
			new mv(
				mhip(mv(*((I2_t *)elem1->elem)), mv(*((e1_t *)elem2->elem)))
			)
		)
	);
break;

case E2_TAG:
	Glue<E2Elem>::push(
		L, 
		new E2Elem(
			MV_TAG,
			new mv(
				mhip(mv(*((I2_t *)elem1->elem)), mv(*((e2_t *)elem2->elem)))
			)
		)
	);
break;

case I2_TAG:
	Glue<E2Elem>::push(
		L, 
		new E2Elem(
			MV_TAG,
			new mv(
				mhip(mv(*((I2_t *)elem1->elem)), mv(*((I2_t *)elem2->elem)))
			)
		)
	);
break;

case I2i_TAG:
	Glue<E2Elem>::push(
		L, 
		new E2Elem(
			MV_TAG,
			new mv(
				mhip(mv(*((I2_t *)elem1->elem)), mv(*((I2i_t *)elem2->elem)))
			)
		)
	);
break;

case VECTOR_TAG:
	Glue<E2Elem>::push(
		L, 
		new E2Elem(
			MV_TAG,
			new mv(
				mhip(mv(*((I2_t *)elem1->elem)), mv(*((vector *)elem2->elem)))
			)
		)
	);
break;

case BIVECTOR_TAG:
	Glue<E2Elem>::push(
		L, 
		new E2Elem(
			MV_TAG,
			new mv(
				mhip(mv(*((I2_t *)elem1->elem)), mv(*((bivector *)elem2->elem)))
			)
		)
	);
break;

case ROTOR_TAG:
	Glue<E2Elem>::push(
		L, 
		new E2Elem(
			MV_TAG,
			new mv(
				mhip(mv(*((I2_t *)elem1->elem)), mv(*((rotor *)elem2->elem)))
			)
		)
	);
break;

case DOUBLE_TAG:
	Glue<E2Elem>::push(
		L, 
		new E2Elem(
			MV_TAG,
			new mv(
				mhip(mv(*((I2_t *)elem1->elem)), *((double *)elem2))
			)
		)
	);
break;
}
break;

case I2i_TAG:
switch(tag2) {
case MV_TAG:
	Glue<E2Elem>::push(
		L, 
		new E2Elem(
			MV_TAG,
			new mv(
				mhip(mv(*((I2i_t *)elem1->elem)), mv(*((mv *)elem2->elem)))
			)
		)
	);
break;

case E1_TAG:
	Glue<E2Elem>::push(
		L, 
		new E2Elem(
			MV_TAG,
			new mv(
				mhip(mv(*((I2i_t *)elem1->elem)), mv(*((e1_t *)elem2->elem)))
			)
		)
	);
break;

case E2_TAG:
	Glue<E2Elem>::push(
		L, 
		new E2Elem(
			MV_TAG,
			new mv(
				mhip(mv(*((I2i_t *)elem1->elem)), mv(*((e2_t *)elem2->elem)))
			)
		)
	);
break;

case I2_TAG:
	Glue<E2Elem>::push(
		L, 
		new E2Elem(
			MV_TAG,
			new mv(
				mhip(mv(*((I2i_t *)elem1->elem)), mv(*((I2_t *)elem2->elem)))
			)
		)
	);
break;

case I2i_TAG:
	Glue<E2Elem>::push(
		L, 
		new E2Elem(
			MV_TAG,
			new mv(
				mhip(mv(*((I2i_t *)elem1->elem)), mv(*((I2i_t *)elem2->elem)))
			)
		)
	);
break;

case VECTOR_TAG:
	Glue<E2Elem>::push(
		L, 
		new E2Elem(
			MV_TAG,
			new mv(
				mhip(mv(*((I2i_t *)elem1->elem)), mv(*((vector *)elem2->elem)))
			)
		)
	);
break;

case BIVECTOR_TAG:
	Glue<E2Elem>::push(
		L, 
		new E2Elem(
			MV_TAG,
			new mv(
				mhip(mv(*((I2i_t *)elem1->elem)), mv(*((bivector *)elem2->elem)))
			)
		)
	);
break;

case ROTOR_TAG:
	Glue<E2Elem>::push(
		L, 
		new E2Elem(
			MV_TAG,
			new mv(
				mhip(mv(*((I2i_t *)elem1->elem)), mv(*((rotor *)elem2->elem)))
			)
		)
	);
break;

case DOUBLE_TAG:
	Glue<E2Elem>::push(
		L, 
		new E2Elem(
			MV_TAG,
			new mv(
				mhip(mv(*((I2i_t *)elem1->elem)), *((double *)elem2))
			)
		)
	);
break;
}
break;

case VECTOR_TAG:
switch(tag2) {
case MV_TAG:
	Glue<E2Elem>::push(
		L, 
		new E2Elem(
			MV_TAG,
			new mv(
				mhip(mv(*((vector *)elem1->elem)), mv(*((mv *)elem2->elem)))
			)
		)
	);
break;

case E1_TAG:
	Glue<E2Elem>::push(
		L, 
		new E2Elem(
			MV_TAG,
			new mv(
				mhip(mv(*((vector *)elem1->elem)), mv(*((e1_t *)elem2->elem)))
			)
		)
	);
break;

case E2_TAG:
	Glue<E2Elem>::push(
		L, 
		new E2Elem(
			MV_TAG,
			new mv(
				mhip(mv(*((vector *)elem1->elem)), mv(*((e2_t *)elem2->elem)))
			)
		)
	);
break;

case I2_TAG:
	Glue<E2Elem>::push(
		L, 
		new E2Elem(
			MV_TAG,
			new mv(
				mhip(mv(*((vector *)elem1->elem)), mv(*((I2_t *)elem2->elem)))
			)
		)
	);
break;

case I2i_TAG:
	Glue<E2Elem>::push(
		L, 
		new E2Elem(
			MV_TAG,
			new mv(
				mhip(mv(*((vector *)elem1->elem)), mv(*((I2i_t *)elem2->elem)))
			)
		)
	);
break;

case VECTOR_TAG:
				lua::push<double>(
					L, 
					mhip(*((vector *)elem1->elem), *((vector *)elem2->elem))
				);
			
break;

case BIVECTOR_TAG:
	Glue<E2Elem>::push(
		L, 
		new E2Elem(
			VECTOR_TAG,
			new vector(
				mhip(*((vector *)elem1->elem), *((bivector *)elem2->elem))
			)
		)
	);
break;

case ROTOR_TAG:
	Glue<E2Elem>::push(
		L, 
		new E2Elem(
			VECTOR_TAG,
			new vector(
				mhip(*((vector *)elem1->elem), *((rotor *)elem2->elem))
			)
		)
	);
break;

case DOUBLE_TAG:
	Glue<E2Elem>::push(
		L, 
		new E2Elem(
			MV_TAG,
			new mv(
				mhip(mv(*((vector *)elem1->elem)), *((double *)elem2))
			)
		)
	);
break;
}
break;

case BIVECTOR_TAG:
switch(tag2) {
case MV_TAG:
	Glue<E2Elem>::push(
		L, 
		new E2Elem(
			MV_TAG,
			new mv(
				mhip(mv(*((bivector *)elem1->elem)), mv(*((mv *)elem2->elem)))
			)
		)
	);
break;

case E1_TAG:
	Glue<E2Elem>::push(
		L, 
		new E2Elem(
			VECTOR_TAG,
			new vector(
				mhip(*((bivector *)elem1->elem), *((e1_t *)elem2->elem))
			)
		)
	);
break;

case E2_TAG:
	Glue<E2Elem>::push(
		L, 
		new E2Elem(
			MV_TAG,
			new mv(
				mhip(mv(*((bivector *)elem1->elem)), mv(*((e2_t *)elem2->elem)))
			)
		)
	);
break;

case I2_TAG:
	Glue<E2Elem>::push(
		L, 
		new E2Elem(
			MV_TAG,
			new mv(
				mhip(mv(*((bivector *)elem1->elem)), mv(*((I2_t *)elem2->elem)))
			)
		)
	);
break;

case I2i_TAG:
	Glue<E2Elem>::push(
		L, 
		new E2Elem(
			MV_TAG,
			new mv(
				mhip(mv(*((bivector *)elem1->elem)), mv(*((I2i_t *)elem2->elem)))
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
				mhip(*((bivector *)elem1->elem), *((vector *)elem2->elem))
			)
		)
	);
break;

case BIVECTOR_TAG:
	Glue<E2Elem>::push(
		L, 
		new E2Elem(
			MV_TAG,
			new mv(
				mhip(mv(*((bivector *)elem1->elem)), mv(*((bivector *)elem2->elem)))
			)
		)
	);
break;

case ROTOR_TAG:
	Glue<E2Elem>::push(
		L, 
		new E2Elem(
			MV_TAG,
			new mv(
				mhip(mv(*((bivector *)elem1->elem)), mv(*((rotor *)elem2->elem)))
			)
		)
	);
break;

case DOUBLE_TAG:
	Glue<E2Elem>::push(
		L, 
		new E2Elem(
			MV_TAG,
			new mv(
				mhip(mv(*((bivector *)elem1->elem)), *((double *)elem2))
			)
		)
	);
break;
}
break;

case ROTOR_TAG:
switch(tag2) {
case MV_TAG:
	Glue<E2Elem>::push(
		L, 
		new E2Elem(
			MV_TAG,
			new mv(
				mhip(mv(*((rotor *)elem1->elem)), mv(*((mv *)elem2->elem)))
			)
		)
	);
break;

case E1_TAG:
	Glue<E2Elem>::push(
		L, 
		new E2Elem(
			MV_TAG,
			new mv(
				mhip(mv(*((rotor *)elem1->elem)), mv(*((e1_t *)elem2->elem)))
			)
		)
	);
break;

case E2_TAG:
	Glue<E2Elem>::push(
		L, 
		new E2Elem(
			MV_TAG,
			new mv(
				mhip(mv(*((rotor *)elem1->elem)), mv(*((e2_t *)elem2->elem)))
			)
		)
	);
break;

case I2_TAG:
	Glue<E2Elem>::push(
		L, 
		new E2Elem(
			MV_TAG,
			new mv(
				mhip(mv(*((rotor *)elem1->elem)), mv(*((I2_t *)elem2->elem)))
			)
		)
	);
break;

case I2i_TAG:
	Glue<E2Elem>::push(
		L, 
		new E2Elem(
			MV_TAG,
			new mv(
				mhip(mv(*((rotor *)elem1->elem)), mv(*((I2i_t *)elem2->elem)))
			)
		)
	);
break;

case VECTOR_TAG:
	Glue<E2Elem>::push(
		L, 
		new E2Elem(
			MV_TAG,
			new mv(
				mhip(mv(*((rotor *)elem1->elem)), mv(*((vector *)elem2->elem)))
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
				mhip(*((rotor *)elem1->elem), *((bivector *)elem2->elem))
			)
		)
	);
break;

case ROTOR_TAG:
	Glue<E2Elem>::push(
		L, 
		new E2Elem(
			MV_TAG,
			new mv(
				mhip(mv(*((rotor *)elem1->elem)), mv(*((rotor *)elem2->elem)))
			)
		)
	);
break;

case DOUBLE_TAG:
	Glue<E2Elem>::push(
		L, 
		new E2Elem(
			MV_TAG,
			new mv(
				mhip(mv(*((rotor *)elem1->elem)), *((double *)elem2))
			)
		)
	);
break;
}
break;

case DOUBLE_TAG:
switch(tag2) {
case MV_TAG:
	Glue<E2Elem>::push(
		L, 
		new E2Elem(
			MV_TAG,
			new mv(
				mhip(*((double *)elem1), mv(*((mv *)elem2->elem)))
			)
		)
	);
break;

case E1_TAG:
	Glue<E2Elem>::push(
		L, 
		new E2Elem(
			MV_TAG,
			new mv(
				mhip(*((double *)elem1), mv(*((e1_t *)elem2->elem)))
			)
		)
	);
break;

case E2_TAG:
	Glue<E2Elem>::push(
		L, 
		new E2Elem(
			MV_TAG,
			new mv(
				mhip(*((double *)elem1), mv(*((e2_t *)elem2->elem)))
			)
		)
	);
break;

case I2_TAG:
	Glue<E2Elem>::push(
		L, 
		new E2Elem(
			MV_TAG,
			new mv(
				mhip(*((double *)elem1), mv(*((I2_t *)elem2->elem)))
			)
		)
	);
break;

case I2i_TAG:
	Glue<E2Elem>::push(
		L, 
		new E2Elem(
			MV_TAG,
			new mv(
				mhip(*((double *)elem1), mv(*((I2i_t *)elem2->elem)))
			)
		)
	);
break;

case VECTOR_TAG:
	Glue<E2Elem>::push(
		L, 
		new E2Elem(
			MV_TAG,
			new mv(
				mhip(*((double *)elem1), mv(*((vector *)elem2->elem)))
			)
		)
	);
break;

case BIVECTOR_TAG:
	Glue<E2Elem>::push(
		L, 
		new E2Elem(
			MV_TAG,
			new mv(
				mhip(*((double *)elem1), mv(*((bivector *)elem2->elem)))
			)
		)
	);
break;

case ROTOR_TAG:
	Glue<E2Elem>::push(
		L, 
		new E2Elem(
			MV_TAG,
			new mv(
				mhip(*((double *)elem1), mv(*((rotor *)elem2->elem)))
			)
		)
	);
break;

case DOUBLE_TAG:
	Glue<E2Elem>::push(
		L, 
		new E2Elem(
			MV_TAG,
			new mv(
				mhip(*((double *)elem1), *((double *)elem2))
			)
		)
	);
break;
}
break;
}
return 1;
}


int e2space_op(lua_State *L) {
	double d1, d2;
	E2Tag tag1;
	E2Tag tag2;
	E2Elem *elem1 = Glue<E2Elem>::to(L, 1);
	E2Elem *elem2 = Glue<E2Elem>::to(L, 2);
	
	
	if(!elem1) {
		if(lua::is<double>(L, 1)) {
			d1 = lua::to<double>(L, 1);
			elem1 = (E2Elem *)(&d1);
			tag1 = DOUBLE_TAG;
		}
		else {
			luaL_error(L, "e2.op: invalid arguments");
		}
	}
	else {
		tag1 = elem1->tag;
	}
	
	if(!elem2) {
		if(lua::is<double>(L, 2)) {
			d2 = lua::to<double>(L, 2);
			elem2 = (E2Elem *)(&d2);
			tag2 = DOUBLE_TAG;
		}
		else {
			luaL_error(L, "e2.op: invalid arguments");
		}
	}
	else {
		tag2 = elem2->tag;
	}
	
switch(tag1) {
case MV_TAG:
switch(tag2) {
case MV_TAG:
	Glue<E2Elem>::push(
		L, 
		new E2Elem(
			MV_TAG,
			new mv(
				op(*((mv *)elem1->elem), *((mv *)elem2->elem))
			)
		)
	);
break;

case E1_TAG:
	Glue<E2Elem>::push(
		L, 
		new E2Elem(
			MV_TAG,
			new mv(
				op(mv(*((mv *)elem1->elem)), mv(*((e1_t *)elem2->elem)))
			)
		)
	);
break;

case E2_TAG:
	Glue<E2Elem>::push(
		L, 
		new E2Elem(
			MV_TAG,
			new mv(
				op(mv(*((mv *)elem1->elem)), mv(*((e2_t *)elem2->elem)))
			)
		)
	);
break;

case I2_TAG:
	Glue<E2Elem>::push(
		L, 
		new E2Elem(
			MV_TAG,
			new mv(
				op(mv(*((mv *)elem1->elem)), mv(*((I2_t *)elem2->elem)))
			)
		)
	);
break;

case I2i_TAG:
	Glue<E2Elem>::push(
		L, 
		new E2Elem(
			MV_TAG,
			new mv(
				op(mv(*((mv *)elem1->elem)), mv(*((I2i_t *)elem2->elem)))
			)
		)
	);
break;

case VECTOR_TAG:
	Glue<E2Elem>::push(
		L, 
		new E2Elem(
			MV_TAG,
			new mv(
				op(mv(*((mv *)elem1->elem)), mv(*((vector *)elem2->elem)))
			)
		)
	);
break;

case BIVECTOR_TAG:
	Glue<E2Elem>::push(
		L, 
		new E2Elem(
			MV_TAG,
			new mv(
				op(mv(*((mv *)elem1->elem)), mv(*((bivector *)elem2->elem)))
			)
		)
	);
break;

case ROTOR_TAG:
	Glue<E2Elem>::push(
		L, 
		new E2Elem(
			MV_TAG,
			new mv(
				op(mv(*((mv *)elem1->elem)), mv(*((rotor *)elem2->elem)))
			)
		)
	);
break;

case DOUBLE_TAG:
	Glue<E2Elem>::push(
		L, 
		new E2Elem(
			MV_TAG,
			new mv(
				op(mv(*((mv *)elem1->elem)), *((double *)elem2))
			)
		)
	);
break;
}
break;

case E1_TAG:
switch(tag2) {
case MV_TAG:
	Glue<E2Elem>::push(
		L, 
		new E2Elem(
			MV_TAG,
			new mv(
				op(mv(*((e1_t *)elem1->elem)), mv(*((mv *)elem2->elem)))
			)
		)
	);
break;

case E1_TAG:
	Glue<E2Elem>::push(
		L, 
		new E2Elem(
			MV_TAG,
			new mv(
				op(mv(*((e1_t *)elem1->elem)), mv(*((e1_t *)elem2->elem)))
			)
		)
	);
break;

case E2_TAG:
	Glue<E2Elem>::push(
		L, 
		new E2Elem(
			I2_TAG,
			new I2_t(
				op(*((e1_t *)elem1->elem), *((e2_t *)elem2->elem))
			)
		)
	);
break;

case I2_TAG:
				lua::push<double>(
					L, 
					op(*((e1_t *)elem1->elem), *((I2_t *)elem2->elem))
				);
			
break;

case I2i_TAG:
	Glue<E2Elem>::push(
		L, 
		new E2Elem(
			MV_TAG,
			new mv(
				op(mv(*((e1_t *)elem1->elem)), mv(*((I2i_t *)elem2->elem)))
			)
		)
	);
break;

case VECTOR_TAG:
	Glue<E2Elem>::push(
		L, 
		new E2Elem(
			MV_TAG,
			new mv(
				op(mv(*((e1_t *)elem1->elem)), mv(*((vector *)elem2->elem)))
			)
		)
	);
break;

case BIVECTOR_TAG:
	Glue<E2Elem>::push(
		L, 
		new E2Elem(
			MV_TAG,
			new mv(
				op(mv(*((e1_t *)elem1->elem)), mv(*((bivector *)elem2->elem)))
			)
		)
	);
break;

case ROTOR_TAG:
	Glue<E2Elem>::push(
		L, 
		new E2Elem(
			MV_TAG,
			new mv(
				op(mv(*((e1_t *)elem1->elem)), mv(*((rotor *)elem2->elem)))
			)
		)
	);
break;

case DOUBLE_TAG:
	Glue<E2Elem>::push(
		L, 
		new E2Elem(
			MV_TAG,
			new mv(
				op(mv(*((e1_t *)elem1->elem)), *((double *)elem2))
			)
		)
	);
break;
}
break;

case E2_TAG:
switch(tag2) {
case MV_TAG:
	Glue<E2Elem>::push(
		L, 
		new E2Elem(
			MV_TAG,
			new mv(
				op(mv(*((e2_t *)elem1->elem)), mv(*((mv *)elem2->elem)))
			)
		)
	);
break;

case E1_TAG:
	Glue<E2Elem>::push(
		L, 
		new E2Elem(
			MV_TAG,
			new mv(
				op(mv(*((e2_t *)elem1->elem)), mv(*((e1_t *)elem2->elem)))
			)
		)
	);
break;

case E2_TAG:
	Glue<E2Elem>::push(
		L, 
		new E2Elem(
			MV_TAG,
			new mv(
				op(mv(*((e2_t *)elem1->elem)), mv(*((e2_t *)elem2->elem)))
			)
		)
	);
break;

case I2_TAG:
	Glue<E2Elem>::push(
		L, 
		new E2Elem(
			MV_TAG,
			new mv(
				op(mv(*((e2_t *)elem1->elem)), mv(*((I2_t *)elem2->elem)))
			)
		)
	);
break;

case I2i_TAG:
	Glue<E2Elem>::push(
		L, 
		new E2Elem(
			MV_TAG,
			new mv(
				op(mv(*((e2_t *)elem1->elem)), mv(*((I2i_t *)elem2->elem)))
			)
		)
	);
break;

case VECTOR_TAG:
	Glue<E2Elem>::push(
		L, 
		new E2Elem(
			MV_TAG,
			new mv(
				op(mv(*((e2_t *)elem1->elem)), mv(*((vector *)elem2->elem)))
			)
		)
	);
break;

case BIVECTOR_TAG:
	Glue<E2Elem>::push(
		L, 
		new E2Elem(
			MV_TAG,
			new mv(
				op(mv(*((e2_t *)elem1->elem)), mv(*((bivector *)elem2->elem)))
			)
		)
	);
break;

case ROTOR_TAG:
	Glue<E2Elem>::push(
		L, 
		new E2Elem(
			MV_TAG,
			new mv(
				op(mv(*((e2_t *)elem1->elem)), mv(*((rotor *)elem2->elem)))
			)
		)
	);
break;

case DOUBLE_TAG:
	Glue<E2Elem>::push(
		L, 
		new E2Elem(
			MV_TAG,
			new mv(
				op(mv(*((e2_t *)elem1->elem)), *((double *)elem2))
			)
		)
	);
break;
}
break;

case I2_TAG:
switch(tag2) {
case MV_TAG:
	Glue<E2Elem>::push(
		L, 
		new E2Elem(
			MV_TAG,
			new mv(
				op(mv(*((I2_t *)elem1->elem)), mv(*((mv *)elem2->elem)))
			)
		)
	);
break;

case E1_TAG:
	Glue<E2Elem>::push(
		L, 
		new E2Elem(
			MV_TAG,
			new mv(
				op(mv(*((I2_t *)elem1->elem)), mv(*((e1_t *)elem2->elem)))
			)
		)
	);
break;

case E2_TAG:
	Glue<E2Elem>::push(
		L, 
		new E2Elem(
			MV_TAG,
			new mv(
				op(mv(*((I2_t *)elem1->elem)), mv(*((e2_t *)elem2->elem)))
			)
		)
	);
break;

case I2_TAG:
	Glue<E2Elem>::push(
		L, 
		new E2Elem(
			MV_TAG,
			new mv(
				op(mv(*((I2_t *)elem1->elem)), mv(*((I2_t *)elem2->elem)))
			)
		)
	);
break;

case I2i_TAG:
	Glue<E2Elem>::push(
		L, 
		new E2Elem(
			MV_TAG,
			new mv(
				op(mv(*((I2_t *)elem1->elem)), mv(*((I2i_t *)elem2->elem)))
			)
		)
	);
break;

case VECTOR_TAG:
	Glue<E2Elem>::push(
		L, 
		new E2Elem(
			MV_TAG,
			new mv(
				op(mv(*((I2_t *)elem1->elem)), mv(*((vector *)elem2->elem)))
			)
		)
	);
break;

case BIVECTOR_TAG:
	Glue<E2Elem>::push(
		L, 
		new E2Elem(
			MV_TAG,
			new mv(
				op(mv(*((I2_t *)elem1->elem)), mv(*((bivector *)elem2->elem)))
			)
		)
	);
break;

case ROTOR_TAG:
	Glue<E2Elem>::push(
		L, 
		new E2Elem(
			MV_TAG,
			new mv(
				op(mv(*((I2_t *)elem1->elem)), mv(*((rotor *)elem2->elem)))
			)
		)
	);
break;

case DOUBLE_TAG:
	Glue<E2Elem>::push(
		L, 
		new E2Elem(
			MV_TAG,
			new mv(
				op(mv(*((I2_t *)elem1->elem)), *((double *)elem2))
			)
		)
	);
break;
}
break;

case I2i_TAG:
switch(tag2) {
case MV_TAG:
	Glue<E2Elem>::push(
		L, 
		new E2Elem(
			MV_TAG,
			new mv(
				op(mv(*((I2i_t *)elem1->elem)), mv(*((mv *)elem2->elem)))
			)
		)
	);
break;

case E1_TAG:
	Glue<E2Elem>::push(
		L, 
		new E2Elem(
			MV_TAG,
			new mv(
				op(mv(*((I2i_t *)elem1->elem)), mv(*((e1_t *)elem2->elem)))
			)
		)
	);
break;

case E2_TAG:
	Glue<E2Elem>::push(
		L, 
		new E2Elem(
			MV_TAG,
			new mv(
				op(mv(*((I2i_t *)elem1->elem)), mv(*((e2_t *)elem2->elem)))
			)
		)
	);
break;

case I2_TAG:
	Glue<E2Elem>::push(
		L, 
		new E2Elem(
			MV_TAG,
			new mv(
				op(mv(*((I2i_t *)elem1->elem)), mv(*((I2_t *)elem2->elem)))
			)
		)
	);
break;

case I2i_TAG:
	Glue<E2Elem>::push(
		L, 
		new E2Elem(
			MV_TAG,
			new mv(
				op(mv(*((I2i_t *)elem1->elem)), mv(*((I2i_t *)elem2->elem)))
			)
		)
	);
break;

case VECTOR_TAG:
	Glue<E2Elem>::push(
		L, 
		new E2Elem(
			MV_TAG,
			new mv(
				op(mv(*((I2i_t *)elem1->elem)), mv(*((vector *)elem2->elem)))
			)
		)
	);
break;

case BIVECTOR_TAG:
	Glue<E2Elem>::push(
		L, 
		new E2Elem(
			MV_TAG,
			new mv(
				op(mv(*((I2i_t *)elem1->elem)), mv(*((bivector *)elem2->elem)))
			)
		)
	);
break;

case ROTOR_TAG:
	Glue<E2Elem>::push(
		L, 
		new E2Elem(
			MV_TAG,
			new mv(
				op(mv(*((I2i_t *)elem1->elem)), mv(*((rotor *)elem2->elem)))
			)
		)
	);
break;

case DOUBLE_TAG:
	Glue<E2Elem>::push(
		L, 
		new E2Elem(
			MV_TAG,
			new mv(
				op(mv(*((I2i_t *)elem1->elem)), *((double *)elem2))
			)
		)
	);
break;
}
break;

case VECTOR_TAG:
switch(tag2) {
case MV_TAG:
	Glue<E2Elem>::push(
		L, 
		new E2Elem(
			MV_TAG,
			new mv(
				op(mv(*((vector *)elem1->elem)), mv(*((mv *)elem2->elem)))
			)
		)
	);
break;

case E1_TAG:
	Glue<E2Elem>::push(
		L, 
		new E2Elem(
			MV_TAG,
			new mv(
				op(mv(*((vector *)elem1->elem)), mv(*((e1_t *)elem2->elem)))
			)
		)
	);
break;

case E2_TAG:
	Glue<E2Elem>::push(
		L, 
		new E2Elem(
			MV_TAG,
			new mv(
				op(mv(*((vector *)elem1->elem)), mv(*((e2_t *)elem2->elem)))
			)
		)
	);
break;

case I2_TAG:
	Glue<E2Elem>::push(
		L, 
		new E2Elem(
			MV_TAG,
			new mv(
				op(mv(*((vector *)elem1->elem)), mv(*((I2_t *)elem2->elem)))
			)
		)
	);
break;

case I2i_TAG:
	Glue<E2Elem>::push(
		L, 
		new E2Elem(
			MV_TAG,
			new mv(
				op(mv(*((vector *)elem1->elem)), mv(*((I2i_t *)elem2->elem)))
			)
		)
	);
break;

case VECTOR_TAG:
	Glue<E2Elem>::push(
		L, 
		new E2Elem(
			BIVECTOR_TAG,
			new bivector(
				op(*((vector *)elem1->elem), *((vector *)elem2->elem))
			)
		)
	);
break;

case BIVECTOR_TAG:
	Glue<E2Elem>::push(
		L, 
		new E2Elem(
			MV_TAG,
			new mv(
				op(mv(*((vector *)elem1->elem)), mv(*((bivector *)elem2->elem)))
			)
		)
	);
break;

case ROTOR_TAG:
	Glue<E2Elem>::push(
		L, 
		new E2Elem(
			VECTOR_TAG,
			new vector(
				op(*((vector *)elem1->elem), *((rotor *)elem2->elem))
			)
		)
	);
break;

case DOUBLE_TAG:
	Glue<E2Elem>::push(
		L, 
		new E2Elem(
			MV_TAG,
			new mv(
				op(mv(*((vector *)elem1->elem)), *((double *)elem2))
			)
		)
	);
break;
}
break;

case BIVECTOR_TAG:
switch(tag2) {
case MV_TAG:
	Glue<E2Elem>::push(
		L, 
		new E2Elem(
			MV_TAG,
			new mv(
				op(mv(*((bivector *)elem1->elem)), mv(*((mv *)elem2->elem)))
			)
		)
	);
break;

case E1_TAG:
	Glue<E2Elem>::push(
		L, 
		new E2Elem(
			MV_TAG,
			new mv(
				op(mv(*((bivector *)elem1->elem)), mv(*((e1_t *)elem2->elem)))
			)
		)
	);
break;

case E2_TAG:
	Glue<E2Elem>::push(
		L, 
		new E2Elem(
			MV_TAG,
			new mv(
				op(mv(*((bivector *)elem1->elem)), mv(*((e2_t *)elem2->elem)))
			)
		)
	);
break;

case I2_TAG:
	Glue<E2Elem>::push(
		L, 
		new E2Elem(
			MV_TAG,
			new mv(
				op(mv(*((bivector *)elem1->elem)), mv(*((I2_t *)elem2->elem)))
			)
		)
	);
break;

case I2i_TAG:
	Glue<E2Elem>::push(
		L, 
		new E2Elem(
			MV_TAG,
			new mv(
				op(mv(*((bivector *)elem1->elem)), mv(*((I2i_t *)elem2->elem)))
			)
		)
	);
break;

case VECTOR_TAG:
	Glue<E2Elem>::push(
		L, 
		new E2Elem(
			MV_TAG,
			new mv(
				op(mv(*((bivector *)elem1->elem)), mv(*((vector *)elem2->elem)))
			)
		)
	);
break;

case BIVECTOR_TAG:
				lua::push<double>(
					L, 
					op(*((bivector *)elem1->elem), *((bivector *)elem2->elem))
				);
			
break;

case ROTOR_TAG:
	Glue<E2Elem>::push(
		L, 
		new E2Elem(
			MV_TAG,
			new mv(
				op(mv(*((bivector *)elem1->elem)), mv(*((rotor *)elem2->elem)))
			)
		)
	);
break;

case DOUBLE_TAG:
	Glue<E2Elem>::push(
		L, 
		new E2Elem(
			MV_TAG,
			new mv(
				op(mv(*((bivector *)elem1->elem)), *((double *)elem2))
			)
		)
	);
break;
}
break;

case ROTOR_TAG:
switch(tag2) {
case MV_TAG:
	Glue<E2Elem>::push(
		L, 
		new E2Elem(
			MV_TAG,
			new mv(
				op(mv(*((rotor *)elem1->elem)), mv(*((mv *)elem2->elem)))
			)
		)
	);
break;

case E1_TAG:
	Glue<E2Elem>::push(
		L, 
		new E2Elem(
			MV_TAG,
			new mv(
				op(mv(*((rotor *)elem1->elem)), mv(*((e1_t *)elem2->elem)))
			)
		)
	);
break;

case E2_TAG:
	Glue<E2Elem>::push(
		L, 
		new E2Elem(
			MV_TAG,
			new mv(
				op(mv(*((rotor *)elem1->elem)), mv(*((e2_t *)elem2->elem)))
			)
		)
	);
break;

case I2_TAG:
	Glue<E2Elem>::push(
		L, 
		new E2Elem(
			MV_TAG,
			new mv(
				op(mv(*((rotor *)elem1->elem)), mv(*((I2_t *)elem2->elem)))
			)
		)
	);
break;

case I2i_TAG:
	Glue<E2Elem>::push(
		L, 
		new E2Elem(
			MV_TAG,
			new mv(
				op(mv(*((rotor *)elem1->elem)), mv(*((I2i_t *)elem2->elem)))
			)
		)
	);
break;

case VECTOR_TAG:
	Glue<E2Elem>::push(
		L, 
		new E2Elem(
			MV_TAG,
			new mv(
				op(mv(*((rotor *)elem1->elem)), mv(*((vector *)elem2->elem)))
			)
		)
	);
break;

case BIVECTOR_TAG:
	Glue<E2Elem>::push(
		L, 
		new E2Elem(
			MV_TAG,
			new mv(
				op(mv(*((rotor *)elem1->elem)), mv(*((bivector *)elem2->elem)))
			)
		)
	);
break;

case ROTOR_TAG:
	Glue<E2Elem>::push(
		L, 
		new E2Elem(
			MV_TAG,
			new mv(
				op(mv(*((rotor *)elem1->elem)), mv(*((rotor *)elem2->elem)))
			)
		)
	);
break;

case DOUBLE_TAG:
	Glue<E2Elem>::push(
		L, 
		new E2Elem(
			MV_TAG,
			new mv(
				op(mv(*((rotor *)elem1->elem)), *((double *)elem2))
			)
		)
	);
break;
}
break;

case DOUBLE_TAG:
switch(tag2) {
case MV_TAG:
	Glue<E2Elem>::push(
		L, 
		new E2Elem(
			MV_TAG,
			new mv(
				op(*((double *)elem1), mv(*((mv *)elem2->elem)))
			)
		)
	);
break;

case E1_TAG:
	Glue<E2Elem>::push(
		L, 
		new E2Elem(
			MV_TAG,
			new mv(
				op(*((double *)elem1), mv(*((e1_t *)elem2->elem)))
			)
		)
	);
break;

case E2_TAG:
	Glue<E2Elem>::push(
		L, 
		new E2Elem(
			MV_TAG,
			new mv(
				op(*((double *)elem1), mv(*((e2_t *)elem2->elem)))
			)
		)
	);
break;

case I2_TAG:
	Glue<E2Elem>::push(
		L, 
		new E2Elem(
			MV_TAG,
			new mv(
				op(*((double *)elem1), mv(*((I2_t *)elem2->elem)))
			)
		)
	);
break;

case I2i_TAG:
	Glue<E2Elem>::push(
		L, 
		new E2Elem(
			MV_TAG,
			new mv(
				op(*((double *)elem1), mv(*((I2i_t *)elem2->elem)))
			)
		)
	);
break;

case VECTOR_TAG:
	Glue<E2Elem>::push(
		L, 
		new E2Elem(
			MV_TAG,
			new mv(
				op(*((double *)elem1), mv(*((vector *)elem2->elem)))
			)
		)
	);
break;

case BIVECTOR_TAG:
	Glue<E2Elem>::push(
		L, 
		new E2Elem(
			MV_TAG,
			new mv(
				op(*((double *)elem1), mv(*((bivector *)elem2->elem)))
			)
		)
	);
break;

case ROTOR_TAG:
	Glue<E2Elem>::push(
		L, 
		new E2Elem(
			MV_TAG,
			new mv(
				op(*((double *)elem1), mv(*((rotor *)elem2->elem)))
			)
		)
	);
break;

case DOUBLE_TAG:
	Glue<E2Elem>::push(
		L, 
		new E2Elem(
			MV_TAG,
			new mv(
				op(*((double *)elem1), *((double *)elem2))
			)
		)
	);
break;
}
break;
}
return 1;
}


int e2space_gp(lua_State *L) {
	double d1, d2;
	E2Tag tag1;
	E2Tag tag2;
	E2Elem *elem1 = Glue<E2Elem>::to(L, 1);
	E2Elem *elem2 = Glue<E2Elem>::to(L, 2);
	
	
	if(!elem1) {
		if(lua::is<double>(L, 1)) {
			d1 = lua::to<double>(L, 1);
			elem1 = (E2Elem *)(&d1);
			tag1 = DOUBLE_TAG;
		}
		else {
			luaL_error(L, "e2.gp: invalid arguments");
		}
	}
	else {
		tag1 = elem1->tag;
	}
	
	if(!elem2) {
		if(lua::is<double>(L, 2)) {
			d2 = lua::to<double>(L, 2);
			elem2 = (E2Elem *)(&d2);
			tag2 = DOUBLE_TAG;
		}
		else {
			luaL_error(L, "e2.gp: invalid arguments");
		}
	}
	else {
		tag2 = elem2->tag;
	}
	
switch(tag1) {
case MV_TAG:
switch(tag2) {
case MV_TAG:
	Glue<E2Elem>::push(
		L, 
		new E2Elem(
			MV_TAG,
			new mv(
				gp(*((mv *)elem1->elem), *((mv *)elem2->elem))
			)
		)
	);
break;

case E1_TAG:
	Glue<E2Elem>::push(
		L, 
		new E2Elem(
			MV_TAG,
			new mv(
				gp(mv(*((mv *)elem1->elem)), mv(*((e1_t *)elem2->elem)))
			)
		)
	);
break;

case E2_TAG:
	Glue<E2Elem>::push(
		L, 
		new E2Elem(
			MV_TAG,
			new mv(
				gp(mv(*((mv *)elem1->elem)), mv(*((e2_t *)elem2->elem)))
			)
		)
	);
break;

case I2_TAG:
	Glue<E2Elem>::push(
		L, 
		new E2Elem(
			MV_TAG,
			new mv(
				gp(mv(*((mv *)elem1->elem)), mv(*((I2_t *)elem2->elem)))
			)
		)
	);
break;

case I2i_TAG:
	Glue<E2Elem>::push(
		L, 
		new E2Elem(
			MV_TAG,
			new mv(
				gp(mv(*((mv *)elem1->elem)), mv(*((I2i_t *)elem2->elem)))
			)
		)
	);
break;

case VECTOR_TAG:
	Glue<E2Elem>::push(
		L, 
		new E2Elem(
			MV_TAG,
			new mv(
				gp(mv(*((mv *)elem1->elem)), mv(*((vector *)elem2->elem)))
			)
		)
	);
break;

case BIVECTOR_TAG:
	Glue<E2Elem>::push(
		L, 
		new E2Elem(
			MV_TAG,
			new mv(
				gp(mv(*((mv *)elem1->elem)), mv(*((bivector *)elem2->elem)))
			)
		)
	);
break;

case ROTOR_TAG:
	Glue<E2Elem>::push(
		L, 
		new E2Elem(
			MV_TAG,
			new mv(
				gp(mv(*((mv *)elem1->elem)), mv(*((rotor *)elem2->elem)))
			)
		)
	);
break;

case DOUBLE_TAG:
	Glue<E2Elem>::push(
		L, 
		new E2Elem(
			MV_TAG,
			new mv(
				gp(*((mv *)elem1->elem), *((double *)elem2))
			)
		)
	);
break;
}
break;

case E1_TAG:
switch(tag2) {
case MV_TAG:
	Glue<E2Elem>::push(
		L, 
		new E2Elem(
			MV_TAG,
			new mv(
				gp(mv(*((e1_t *)elem1->elem)), mv(*((mv *)elem2->elem)))
			)
		)
	);
break;

case E1_TAG:
	Glue<E2Elem>::push(
		L, 
		new E2Elem(
			MV_TAG,
			new mv(
				gp(mv(*((e1_t *)elem1->elem)), mv(*((e1_t *)elem2->elem)))
			)
		)
	);
break;

case E2_TAG:
	Glue<E2Elem>::push(
		L, 
		new E2Elem(
			I2_TAG,
			new I2_t(
				gp(*((e1_t *)elem1->elem), *((e2_t *)elem2->elem))
			)
		)
	);
break;

case I2_TAG:
	Glue<E2Elem>::push(
		L, 
		new E2Elem(
			E2_TAG,
			new e2_t(
				gp(*((e1_t *)elem1->elem), *((I2_t *)elem2->elem))
			)
		)
	);
break;

case I2i_TAG:
	Glue<E2Elem>::push(
		L, 
		new E2Elem(
			MV_TAG,
			new mv(
				gp(mv(*((e1_t *)elem1->elem)), mv(*((I2i_t *)elem2->elem)))
			)
		)
	);
break;

case VECTOR_TAG:
	Glue<E2Elem>::push(
		L, 
		new E2Elem(
			MV_TAG,
			new mv(
				gp(mv(*((e1_t *)elem1->elem)), mv(*((vector *)elem2->elem)))
			)
		)
	);
break;

case BIVECTOR_TAG:
	Glue<E2Elem>::push(
		L, 
		new E2Elem(
			MV_TAG,
			new mv(
				gp(mv(*((e1_t *)elem1->elem)), mv(*((bivector *)elem2->elem)))
			)
		)
	);
break;

case ROTOR_TAG:
	Glue<E2Elem>::push(
		L, 
		new E2Elem(
			MV_TAG,
			new mv(
				gp(mv(*((e1_t *)elem1->elem)), mv(*((rotor *)elem2->elem)))
			)
		)
	);
break;

case DOUBLE_TAG:
	Glue<E2Elem>::push(
		L, 
		new E2Elem(
			MV_TAG,
			new mv(
				gp(mv(*((e1_t *)elem1->elem)), *((double *)elem2))
			)
		)
	);
break;
}
break;

case E2_TAG:
switch(tag2) {
case MV_TAG:
	Glue<E2Elem>::push(
		L, 
		new E2Elem(
			MV_TAG,
			new mv(
				gp(mv(*((e2_t *)elem1->elem)), mv(*((mv *)elem2->elem)))
			)
		)
	);
break;

case E1_TAG:
	Glue<E2Elem>::push(
		L, 
		new E2Elem(
			MV_TAG,
			new mv(
				gp(mv(*((e2_t *)elem1->elem)), mv(*((e1_t *)elem2->elem)))
			)
		)
	);
break;

case E2_TAG:
	Glue<E2Elem>::push(
		L, 
		new E2Elem(
			MV_TAG,
			new mv(
				gp(mv(*((e2_t *)elem1->elem)), mv(*((e2_t *)elem2->elem)))
			)
		)
	);
break;

case I2_TAG:
	Glue<E2Elem>::push(
		L, 
		new E2Elem(
			MV_TAG,
			new mv(
				gp(mv(*((e2_t *)elem1->elem)), mv(*((I2_t *)elem2->elem)))
			)
		)
	);
break;

case I2i_TAG:
	Glue<E2Elem>::push(
		L, 
		new E2Elem(
			MV_TAG,
			new mv(
				gp(mv(*((e2_t *)elem1->elem)), mv(*((I2i_t *)elem2->elem)))
			)
		)
	);
break;

case VECTOR_TAG:
	Glue<E2Elem>::push(
		L, 
		new E2Elem(
			MV_TAG,
			new mv(
				gp(mv(*((e2_t *)elem1->elem)), mv(*((vector *)elem2->elem)))
			)
		)
	);
break;

case BIVECTOR_TAG:
	Glue<E2Elem>::push(
		L, 
		new E2Elem(
			MV_TAG,
			new mv(
				gp(mv(*((e2_t *)elem1->elem)), mv(*((bivector *)elem2->elem)))
			)
		)
	);
break;

case ROTOR_TAG:
	Glue<E2Elem>::push(
		L, 
		new E2Elem(
			MV_TAG,
			new mv(
				gp(mv(*((e2_t *)elem1->elem)), mv(*((rotor *)elem2->elem)))
			)
		)
	);
break;

case DOUBLE_TAG:
	Glue<E2Elem>::push(
		L, 
		new E2Elem(
			MV_TAG,
			new mv(
				gp(mv(*((e2_t *)elem1->elem)), *((double *)elem2))
			)
		)
	);
break;
}
break;

case I2_TAG:
switch(tag2) {
case MV_TAG:
	Glue<E2Elem>::push(
		L, 
		new E2Elem(
			MV_TAG,
			new mv(
				gp(mv(*((I2_t *)elem1->elem)), mv(*((mv *)elem2->elem)))
			)
		)
	);
break;

case E1_TAG:
	Glue<E2Elem>::push(
		L, 
		new E2Elem(
			MV_TAG,
			new mv(
				gp(mv(*((I2_t *)elem1->elem)), mv(*((e1_t *)elem2->elem)))
			)
		)
	);
break;

case E2_TAG:
	Glue<E2Elem>::push(
		L, 
		new E2Elem(
			MV_TAG,
			new mv(
				gp(mv(*((I2_t *)elem1->elem)), mv(*((e2_t *)elem2->elem)))
			)
		)
	);
break;

case I2_TAG:
	Glue<E2Elem>::push(
		L, 
		new E2Elem(
			MV_TAG,
			new mv(
				gp(mv(*((I2_t *)elem1->elem)), mv(*((I2_t *)elem2->elem)))
			)
		)
	);
break;

case I2i_TAG:
	Glue<E2Elem>::push(
		L, 
		new E2Elem(
			MV_TAG,
			new mv(
				gp(mv(*((I2_t *)elem1->elem)), mv(*((I2i_t *)elem2->elem)))
			)
		)
	);
break;

case VECTOR_TAG:
	Glue<E2Elem>::push(
		L, 
		new E2Elem(
			MV_TAG,
			new mv(
				gp(mv(*((I2_t *)elem1->elem)), mv(*((vector *)elem2->elem)))
			)
		)
	);
break;

case BIVECTOR_TAG:
	Glue<E2Elem>::push(
		L, 
		new E2Elem(
			MV_TAG,
			new mv(
				gp(mv(*((I2_t *)elem1->elem)), mv(*((bivector *)elem2->elem)))
			)
		)
	);
break;

case ROTOR_TAG:
	Glue<E2Elem>::push(
		L, 
		new E2Elem(
			MV_TAG,
			new mv(
				gp(mv(*((I2_t *)elem1->elem)), mv(*((rotor *)elem2->elem)))
			)
		)
	);
break;

case DOUBLE_TAG:
	Glue<E2Elem>::push(
		L, 
		new E2Elem(
			MV_TAG,
			new mv(
				gp(mv(*((I2_t *)elem1->elem)), *((double *)elem2))
			)
		)
	);
break;
}
break;

case I2i_TAG:
switch(tag2) {
case MV_TAG:
	Glue<E2Elem>::push(
		L, 
		new E2Elem(
			MV_TAG,
			new mv(
				gp(mv(*((I2i_t *)elem1->elem)), mv(*((mv *)elem2->elem)))
			)
		)
	);
break;

case E1_TAG:
	Glue<E2Elem>::push(
		L, 
		new E2Elem(
			MV_TAG,
			new mv(
				gp(mv(*((I2i_t *)elem1->elem)), mv(*((e1_t *)elem2->elem)))
			)
		)
	);
break;

case E2_TAG:
	Glue<E2Elem>::push(
		L, 
		new E2Elem(
			MV_TAG,
			new mv(
				gp(mv(*((I2i_t *)elem1->elem)), mv(*((e2_t *)elem2->elem)))
			)
		)
	);
break;

case I2_TAG:
	Glue<E2Elem>::push(
		L, 
		new E2Elem(
			MV_TAG,
			new mv(
				gp(mv(*((I2i_t *)elem1->elem)), mv(*((I2_t *)elem2->elem)))
			)
		)
	);
break;

case I2i_TAG:
	Glue<E2Elem>::push(
		L, 
		new E2Elem(
			MV_TAG,
			new mv(
				gp(mv(*((I2i_t *)elem1->elem)), mv(*((I2i_t *)elem2->elem)))
			)
		)
	);
break;

case VECTOR_TAG:
	Glue<E2Elem>::push(
		L, 
		new E2Elem(
			MV_TAG,
			new mv(
				gp(mv(*((I2i_t *)elem1->elem)), mv(*((vector *)elem2->elem)))
			)
		)
	);
break;

case BIVECTOR_TAG:
	Glue<E2Elem>::push(
		L, 
		new E2Elem(
			MV_TAG,
			new mv(
				gp(mv(*((I2i_t *)elem1->elem)), mv(*((bivector *)elem2->elem)))
			)
		)
	);
break;

case ROTOR_TAG:
	Glue<E2Elem>::push(
		L, 
		new E2Elem(
			MV_TAG,
			new mv(
				gp(mv(*((I2i_t *)elem1->elem)), mv(*((rotor *)elem2->elem)))
			)
		)
	);
break;

case DOUBLE_TAG:
	Glue<E2Elem>::push(
		L, 
		new E2Elem(
			MV_TAG,
			new mv(
				gp(mv(*((I2i_t *)elem1->elem)), *((double *)elem2))
			)
		)
	);
break;
}
break;

case VECTOR_TAG:
switch(tag2) {
case MV_TAG:
	Glue<E2Elem>::push(
		L, 
		new E2Elem(
			MV_TAG,
			new mv(
				gp(mv(*((vector *)elem1->elem)), mv(*((mv *)elem2->elem)))
			)
		)
	);
break;

case E1_TAG:
	Glue<E2Elem>::push(
		L, 
		new E2Elem(
			MV_TAG,
			new mv(
				gp(mv(*((vector *)elem1->elem)), mv(*((e1_t *)elem2->elem)))
			)
		)
	);
break;

case E2_TAG:
	Glue<E2Elem>::push(
		L, 
		new E2Elem(
			MV_TAG,
			new mv(
				gp(mv(*((vector *)elem1->elem)), mv(*((e2_t *)elem2->elem)))
			)
		)
	);
break;

case I2_TAG:
	Glue<E2Elem>::push(
		L, 
		new E2Elem(
			MV_TAG,
			new mv(
				gp(mv(*((vector *)elem1->elem)), mv(*((I2_t *)elem2->elem)))
			)
		)
	);
break;

case I2i_TAG:
	Glue<E2Elem>::push(
		L, 
		new E2Elem(
			MV_TAG,
			new mv(
				gp(mv(*((vector *)elem1->elem)), mv(*((I2i_t *)elem2->elem)))
			)
		)
	);
break;

case VECTOR_TAG:
	Glue<E2Elem>::push(
		L, 
		new E2Elem(
			ROTOR_TAG,
			new rotor(
				gp(*((vector *)elem1->elem), *((vector *)elem2->elem))
			)
		)
	);
break;

case BIVECTOR_TAG:
	Glue<E2Elem>::push(
		L, 
		new E2Elem(
			MV_TAG,
			new mv(
				gp(mv(*((vector *)elem1->elem)), mv(*((bivector *)elem2->elem)))
			)
		)
	);
break;

case ROTOR_TAG:
	Glue<E2Elem>::push(
		L, 
		new E2Elem(
			VECTOR_TAG,
			new vector(
				gp(*((vector *)elem1->elem), *((rotor *)elem2->elem))
			)
		)
	);
break;

case DOUBLE_TAG:
	Glue<E2Elem>::push(
		L, 
		new E2Elem(
			MV_TAG,
			new mv(
				gp(mv(*((vector *)elem1->elem)), *((double *)elem2))
			)
		)
	);
break;
}
break;

case BIVECTOR_TAG:
switch(tag2) {
case MV_TAG:
	Glue<E2Elem>::push(
		L, 
		new E2Elem(
			MV_TAG,
			new mv(
				gp(mv(*((bivector *)elem1->elem)), mv(*((mv *)elem2->elem)))
			)
		)
	);
break;

case E1_TAG:
	Glue<E2Elem>::push(
		L, 
		new E2Elem(
			MV_TAG,
			new mv(
				gp(mv(*((bivector *)elem1->elem)), mv(*((e1_t *)elem2->elem)))
			)
		)
	);
break;

case E2_TAG:
	Glue<E2Elem>::push(
		L, 
		new E2Elem(
			MV_TAG,
			new mv(
				gp(mv(*((bivector *)elem1->elem)), mv(*((e2_t *)elem2->elem)))
			)
		)
	);
break;

case I2_TAG:
	Glue<E2Elem>::push(
		L, 
		new E2Elem(
			MV_TAG,
			new mv(
				gp(mv(*((bivector *)elem1->elem)), mv(*((I2_t *)elem2->elem)))
			)
		)
	);
break;

case I2i_TAG:
	Glue<E2Elem>::push(
		L, 
		new E2Elem(
			MV_TAG,
			new mv(
				gp(mv(*((bivector *)elem1->elem)), mv(*((I2i_t *)elem2->elem)))
			)
		)
	);
break;

case VECTOR_TAG:
	Glue<E2Elem>::push(
		L, 
		new E2Elem(
			MV_TAG,
			new mv(
				gp(mv(*((bivector *)elem1->elem)), mv(*((vector *)elem2->elem)))
			)
		)
	);
break;

case BIVECTOR_TAG:
				lua::push<double>(
					L, 
					gp(*((bivector *)elem1->elem), *((bivector *)elem2->elem))
				);
			
break;

case ROTOR_TAG:
	Glue<E2Elem>::push(
		L, 
		new E2Elem(
			MV_TAG,
			new mv(
				gp(mv(*((bivector *)elem1->elem)), mv(*((rotor *)elem2->elem)))
			)
		)
	);
break;

case DOUBLE_TAG:
	Glue<E2Elem>::push(
		L, 
		new E2Elem(
			MV_TAG,
			new mv(
				gp(mv(*((bivector *)elem1->elem)), *((double *)elem2))
			)
		)
	);
break;
}
break;

case ROTOR_TAG:
switch(tag2) {
case MV_TAG:
	Glue<E2Elem>::push(
		L, 
		new E2Elem(
			MV_TAG,
			new mv(
				gp(mv(*((rotor *)elem1->elem)), mv(*((mv *)elem2->elem)))
			)
		)
	);
break;

case E1_TAG:
	Glue<E2Elem>::push(
		L, 
		new E2Elem(
			MV_TAG,
			new mv(
				gp(mv(*((rotor *)elem1->elem)), mv(*((e1_t *)elem2->elem)))
			)
		)
	);
break;

case E2_TAG:
	Glue<E2Elem>::push(
		L, 
		new E2Elem(
			MV_TAG,
			new mv(
				gp(mv(*((rotor *)elem1->elem)), mv(*((e2_t *)elem2->elem)))
			)
		)
	);
break;

case I2_TAG:
	Glue<E2Elem>::push(
		L, 
		new E2Elem(
			MV_TAG,
			new mv(
				gp(mv(*((rotor *)elem1->elem)), mv(*((I2_t *)elem2->elem)))
			)
		)
	);
break;

case I2i_TAG:
	Glue<E2Elem>::push(
		L, 
		new E2Elem(
			MV_TAG,
			new mv(
				gp(mv(*((rotor *)elem1->elem)), mv(*((I2i_t *)elem2->elem)))
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
				gp(*((rotor *)elem1->elem), *((vector *)elem2->elem))
			)
		)
	);
break;

case BIVECTOR_TAG:
	Glue<E2Elem>::push(
		L, 
		new E2Elem(
			MV_TAG,
			new mv(
				gp(mv(*((rotor *)elem1->elem)), mv(*((bivector *)elem2->elem)))
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
				gp(*((rotor *)elem1->elem), *((rotor *)elem2->elem))
			)
		)
	);
break;

case DOUBLE_TAG:
	Glue<E2Elem>::push(
		L, 
		new E2Elem(
			MV_TAG,
			new mv(
				gp(mv(*((rotor *)elem1->elem)), *((double *)elem2))
			)
		)
	);
break;
}
break;

case DOUBLE_TAG:
switch(tag2) {
case MV_TAG:
	Glue<E2Elem>::push(
		L, 
		new E2Elem(
			MV_TAG,
			new mv(
				gp(*((double *)elem1), *((mv *)elem2->elem))
			)
		)
	);
break;

case E1_TAG:
	Glue<E2Elem>::push(
		L, 
		new E2Elem(
			MV_TAG,
			new mv(
				gp(*((double *)elem1), *((e1_t *)elem2->elem))
			)
		)
	);
break;

case E2_TAG:
	Glue<E2Elem>::push(
		L, 
		new E2Elem(
			MV_TAG,
			new mv(
				gp(*((double *)elem1), *((e2_t *)elem2->elem))
			)
		)
	);
break;

case I2_TAG:
	Glue<E2Elem>::push(
		L, 
		new E2Elem(
			MV_TAG,
			new mv(
				gp(*((double *)elem1), *((I2_t *)elem2->elem))
			)
		)
	);
break;

case I2i_TAG:
	Glue<E2Elem>::push(
		L, 
		new E2Elem(
			MV_TAG,
			new mv(
				gp(*((double *)elem1), *((I2i_t *)elem2->elem))
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
				gp(*((double *)elem1), *((vector *)elem2->elem))
			)
		)
	);
break;

case BIVECTOR_TAG:
	Glue<E2Elem>::push(
		L, 
		new E2Elem(
			MV_TAG,
			new mv(
				gp(*((double *)elem1), *((bivector *)elem2->elem))
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
				gp(*((double *)elem1), *((rotor *)elem2->elem))
			)
		)
	);
break;

case DOUBLE_TAG:
	Glue<E2Elem>::push(
		L, 
		new E2Elem(
			MV_TAG,
			new mv(
				gp(*((double *)elem1), *((double *)elem2))
			)
		)
	);
break;
}
break;
}
return 1;
}


int e2space_lc(lua_State *L) {
	double d1, d2;
	E2Tag tag1;
	E2Tag tag2;
	E2Elem *elem1 = Glue<E2Elem>::to(L, 1);
	E2Elem *elem2 = Glue<E2Elem>::to(L, 2);
	
	
	if(!elem1) {
		if(lua::is<double>(L, 1)) {
			d1 = lua::to<double>(L, 1);
			elem1 = (E2Elem *)(&d1);
			tag1 = DOUBLE_TAG;
		}
		else {
			luaL_error(L, "e2.lc: invalid arguments");
		}
	}
	else {
		tag1 = elem1->tag;
	}
	
	if(!elem2) {
		if(lua::is<double>(L, 2)) {
			d2 = lua::to<double>(L, 2);
			elem2 = (E2Elem *)(&d2);
			tag2 = DOUBLE_TAG;
		}
		else {
			luaL_error(L, "e2.lc: invalid arguments");
		}
	}
	else {
		tag2 = elem2->tag;
	}
	
switch(tag1) {
case MV_TAG:
switch(tag2) {
case MV_TAG:
	Glue<E2Elem>::push(
		L, 
		new E2Elem(
			MV_TAG,
			new mv(
				lc(*((mv *)elem1->elem), *((mv *)elem2->elem))
			)
		)
	);
break;

case E1_TAG:
	Glue<E2Elem>::push(
		L, 
		new E2Elem(
			MV_TAG,
			new mv(
				lc(mv(*((mv *)elem1->elem)), mv(*((e1_t *)elem2->elem)))
			)
		)
	);
break;

case E2_TAG:
	Glue<E2Elem>::push(
		L, 
		new E2Elem(
			MV_TAG,
			new mv(
				lc(mv(*((mv *)elem1->elem)), mv(*((e2_t *)elem2->elem)))
			)
		)
	);
break;

case I2_TAG:
	Glue<E2Elem>::push(
		L, 
		new E2Elem(
			MV_TAG,
			new mv(
				lc(mv(*((mv *)elem1->elem)), mv(*((I2_t *)elem2->elem)))
			)
		)
	);
break;

case I2i_TAG:
	Glue<E2Elem>::push(
		L, 
		new E2Elem(
			MV_TAG,
			new mv(
				lc(mv(*((mv *)elem1->elem)), mv(*((I2i_t *)elem2->elem)))
			)
		)
	);
break;

case VECTOR_TAG:
	Glue<E2Elem>::push(
		L, 
		new E2Elem(
			MV_TAG,
			new mv(
				lc(mv(*((mv *)elem1->elem)), mv(*((vector *)elem2->elem)))
			)
		)
	);
break;

case BIVECTOR_TAG:
	Glue<E2Elem>::push(
		L, 
		new E2Elem(
			MV_TAG,
			new mv(
				lc(mv(*((mv *)elem1->elem)), mv(*((bivector *)elem2->elem)))
			)
		)
	);
break;

case ROTOR_TAG:
	Glue<E2Elem>::push(
		L, 
		new E2Elem(
			MV_TAG,
			new mv(
				lc(mv(*((mv *)elem1->elem)), mv(*((rotor *)elem2->elem)))
			)
		)
	);
break;

case DOUBLE_TAG:
	Glue<E2Elem>::push(
		L, 
		new E2Elem(
			MV_TAG,
			new mv(
				lc(mv(*((mv *)elem1->elem)), *((double *)elem2))
			)
		)
	);
break;
}
break;

case E1_TAG:
switch(tag2) {
case MV_TAG:
	Glue<E2Elem>::push(
		L, 
		new E2Elem(
			MV_TAG,
			new mv(
				lc(mv(*((e1_t *)elem1->elem)), mv(*((mv *)elem2->elem)))
			)
		)
	);
break;

case E1_TAG:
	Glue<E2Elem>::push(
		L, 
		new E2Elem(
			MV_TAG,
			new mv(
				lc(mv(*((e1_t *)elem1->elem)), mv(*((e1_t *)elem2->elem)))
			)
		)
	);
break;

case E2_TAG:
	Glue<E2Elem>::push(
		L, 
		new E2Elem(
			MV_TAG,
			new mv(
				lc(mv(*((e1_t *)elem1->elem)), mv(*((e2_t *)elem2->elem)))
			)
		)
	);
break;

case I2_TAG:
	Glue<E2Elem>::push(
		L, 
		new E2Elem(
			MV_TAG,
			new mv(
				lc(mv(*((e1_t *)elem1->elem)), mv(*((I2_t *)elem2->elem)))
			)
		)
	);
break;

case I2i_TAG:
	Glue<E2Elem>::push(
		L, 
		new E2Elem(
			MV_TAG,
			new mv(
				lc(mv(*((e1_t *)elem1->elem)), mv(*((I2i_t *)elem2->elem)))
			)
		)
	);
break;

case VECTOR_TAG:
	Glue<E2Elem>::push(
		L, 
		new E2Elem(
			MV_TAG,
			new mv(
				lc(mv(*((e1_t *)elem1->elem)), mv(*((vector *)elem2->elem)))
			)
		)
	);
break;

case BIVECTOR_TAG:
	Glue<E2Elem>::push(
		L, 
		new E2Elem(
			MV_TAG,
			new mv(
				lc(mv(*((e1_t *)elem1->elem)), mv(*((bivector *)elem2->elem)))
			)
		)
	);
break;

case ROTOR_TAG:
	Glue<E2Elem>::push(
		L, 
		new E2Elem(
			MV_TAG,
			new mv(
				lc(mv(*((e1_t *)elem1->elem)), mv(*((rotor *)elem2->elem)))
			)
		)
	);
break;

case DOUBLE_TAG:
	Glue<E2Elem>::push(
		L, 
		new E2Elem(
			MV_TAG,
			new mv(
				lc(mv(*((e1_t *)elem1->elem)), *((double *)elem2))
			)
		)
	);
break;
}
break;

case E2_TAG:
switch(tag2) {
case MV_TAG:
	Glue<E2Elem>::push(
		L, 
		new E2Elem(
			MV_TAG,
			new mv(
				lc(mv(*((e2_t *)elem1->elem)), mv(*((mv *)elem2->elem)))
			)
		)
	);
break;

case E1_TAG:
	Glue<E2Elem>::push(
		L, 
		new E2Elem(
			MV_TAG,
			new mv(
				lc(mv(*((e2_t *)elem1->elem)), mv(*((e1_t *)elem2->elem)))
			)
		)
	);
break;

case E2_TAG:
	Glue<E2Elem>::push(
		L, 
		new E2Elem(
			MV_TAG,
			new mv(
				lc(mv(*((e2_t *)elem1->elem)), mv(*((e2_t *)elem2->elem)))
			)
		)
	);
break;

case I2_TAG:
	Glue<E2Elem>::push(
		L, 
		new E2Elem(
			MV_TAG,
			new mv(
				lc(mv(*((e2_t *)elem1->elem)), mv(*((I2_t *)elem2->elem)))
			)
		)
	);
break;

case I2i_TAG:
	Glue<E2Elem>::push(
		L, 
		new E2Elem(
			MV_TAG,
			new mv(
				lc(mv(*((e2_t *)elem1->elem)), mv(*((I2i_t *)elem2->elem)))
			)
		)
	);
break;

case VECTOR_TAG:
				lua::push<double>(
					L, 
					lc(*((e2_t *)elem1->elem), *((vector *)elem2->elem))
				);
			
break;

case BIVECTOR_TAG:
	Glue<E2Elem>::push(
		L, 
		new E2Elem(
			MV_TAG,
			new mv(
				lc(mv(*((e2_t *)elem1->elem)), mv(*((bivector *)elem2->elem)))
			)
		)
	);
break;

case ROTOR_TAG:
	Glue<E2Elem>::push(
		L, 
		new E2Elem(
			MV_TAG,
			new mv(
				lc(mv(*((e2_t *)elem1->elem)), mv(*((rotor *)elem2->elem)))
			)
		)
	);
break;

case DOUBLE_TAG:
	Glue<E2Elem>::push(
		L, 
		new E2Elem(
			MV_TAG,
			new mv(
				lc(mv(*((e2_t *)elem1->elem)), *((double *)elem2))
			)
		)
	);
break;
}
break;

case I2_TAG:
switch(tag2) {
case MV_TAG:
	Glue<E2Elem>::push(
		L, 
		new E2Elem(
			MV_TAG,
			new mv(
				lc(mv(*((I2_t *)elem1->elem)), mv(*((mv *)elem2->elem)))
			)
		)
	);
break;

case E1_TAG:
	Glue<E2Elem>::push(
		L, 
		new E2Elem(
			MV_TAG,
			new mv(
				lc(mv(*((I2_t *)elem1->elem)), mv(*((e1_t *)elem2->elem)))
			)
		)
	);
break;

case E2_TAG:
	Glue<E2Elem>::push(
		L, 
		new E2Elem(
			MV_TAG,
			new mv(
				lc(mv(*((I2_t *)elem1->elem)), mv(*((e2_t *)elem2->elem)))
			)
		)
	);
break;

case I2_TAG:
	Glue<E2Elem>::push(
		L, 
		new E2Elem(
			MV_TAG,
			new mv(
				lc(mv(*((I2_t *)elem1->elem)), mv(*((I2_t *)elem2->elem)))
			)
		)
	);
break;

case I2i_TAG:
	Glue<E2Elem>::push(
		L, 
		new E2Elem(
			MV_TAG,
			new mv(
				lc(mv(*((I2_t *)elem1->elem)), mv(*((I2i_t *)elem2->elem)))
			)
		)
	);
break;

case VECTOR_TAG:
	Glue<E2Elem>::push(
		L, 
		new E2Elem(
			MV_TAG,
			new mv(
				lc(mv(*((I2_t *)elem1->elem)), mv(*((vector *)elem2->elem)))
			)
		)
	);
break;

case BIVECTOR_TAG:
	Glue<E2Elem>::push(
		L, 
		new E2Elem(
			MV_TAG,
			new mv(
				lc(mv(*((I2_t *)elem1->elem)), mv(*((bivector *)elem2->elem)))
			)
		)
	);
break;

case ROTOR_TAG:
	Glue<E2Elem>::push(
		L, 
		new E2Elem(
			MV_TAG,
			new mv(
				lc(mv(*((I2_t *)elem1->elem)), mv(*((rotor *)elem2->elem)))
			)
		)
	);
break;

case DOUBLE_TAG:
	Glue<E2Elem>::push(
		L, 
		new E2Elem(
			MV_TAG,
			new mv(
				lc(mv(*((I2_t *)elem1->elem)), *((double *)elem2))
			)
		)
	);
break;
}
break;

case I2i_TAG:
switch(tag2) {
case MV_TAG:
	Glue<E2Elem>::push(
		L, 
		new E2Elem(
			MV_TAG,
			new mv(
				lc(mv(*((I2i_t *)elem1->elem)), mv(*((mv *)elem2->elem)))
			)
		)
	);
break;

case E1_TAG:
	Glue<E2Elem>::push(
		L, 
		new E2Elem(
			MV_TAG,
			new mv(
				lc(mv(*((I2i_t *)elem1->elem)), mv(*((e1_t *)elem2->elem)))
			)
		)
	);
break;

case E2_TAG:
	Glue<E2Elem>::push(
		L, 
		new E2Elem(
			MV_TAG,
			new mv(
				lc(mv(*((I2i_t *)elem1->elem)), mv(*((e2_t *)elem2->elem)))
			)
		)
	);
break;

case I2_TAG:
	Glue<E2Elem>::push(
		L, 
		new E2Elem(
			MV_TAG,
			new mv(
				lc(mv(*((I2i_t *)elem1->elem)), mv(*((I2_t *)elem2->elem)))
			)
		)
	);
break;

case I2i_TAG:
	Glue<E2Elem>::push(
		L, 
		new E2Elem(
			MV_TAG,
			new mv(
				lc(mv(*((I2i_t *)elem1->elem)), mv(*((I2i_t *)elem2->elem)))
			)
		)
	);
break;

case VECTOR_TAG:
	Glue<E2Elem>::push(
		L, 
		new E2Elem(
			MV_TAG,
			new mv(
				lc(mv(*((I2i_t *)elem1->elem)), mv(*((vector *)elem2->elem)))
			)
		)
	);
break;

case BIVECTOR_TAG:
	Glue<E2Elem>::push(
		L, 
		new E2Elem(
			MV_TAG,
			new mv(
				lc(mv(*((I2i_t *)elem1->elem)), mv(*((bivector *)elem2->elem)))
			)
		)
	);
break;

case ROTOR_TAG:
	Glue<E2Elem>::push(
		L, 
		new E2Elem(
			MV_TAG,
			new mv(
				lc(mv(*((I2i_t *)elem1->elem)), mv(*((rotor *)elem2->elem)))
			)
		)
	);
break;

case DOUBLE_TAG:
	Glue<E2Elem>::push(
		L, 
		new E2Elem(
			MV_TAG,
			new mv(
				lc(mv(*((I2i_t *)elem1->elem)), *((double *)elem2))
			)
		)
	);
break;
}
break;

case VECTOR_TAG:
switch(tag2) {
case MV_TAG:
	Glue<E2Elem>::push(
		L, 
		new E2Elem(
			MV_TAG,
			new mv(
				lc(mv(*((vector *)elem1->elem)), mv(*((mv *)elem2->elem)))
			)
		)
	);
break;

case E1_TAG:
	Glue<E2Elem>::push(
		L, 
		new E2Elem(
			MV_TAG,
			new mv(
				lc(mv(*((vector *)elem1->elem)), mv(*((e1_t *)elem2->elem)))
			)
		)
	);
break;

case E2_TAG:
	Glue<E2Elem>::push(
		L, 
		new E2Elem(
			MV_TAG,
			new mv(
				lc(mv(*((vector *)elem1->elem)), mv(*((e2_t *)elem2->elem)))
			)
		)
	);
break;

case I2_TAG:
	Glue<E2Elem>::push(
		L, 
		new E2Elem(
			MV_TAG,
			new mv(
				lc(mv(*((vector *)elem1->elem)), mv(*((I2_t *)elem2->elem)))
			)
		)
	);
break;

case I2i_TAG:
	Glue<E2Elem>::push(
		L, 
		new E2Elem(
			MV_TAG,
			new mv(
				lc(mv(*((vector *)elem1->elem)), mv(*((I2i_t *)elem2->elem)))
			)
		)
	);
break;

case VECTOR_TAG:
				lua::push<double>(
					L, 
					lc(*((vector *)elem1->elem), *((vector *)elem2->elem))
				);
			
break;

case BIVECTOR_TAG:
	Glue<E2Elem>::push(
		L, 
		new E2Elem(
			VECTOR_TAG,
			new vector(
				lc(*((vector *)elem1->elem), *((bivector *)elem2->elem))
			)
		)
	);
break;

case ROTOR_TAG:
	Glue<E2Elem>::push(
		L, 
		new E2Elem(
			VECTOR_TAG,
			new vector(
				lc(*((vector *)elem1->elem), *((rotor *)elem2->elem))
			)
		)
	);
break;

case DOUBLE_TAG:
	Glue<E2Elem>::push(
		L, 
		new E2Elem(
			MV_TAG,
			new mv(
				lc(mv(*((vector *)elem1->elem)), *((double *)elem2))
			)
		)
	);
break;
}
break;

case BIVECTOR_TAG:
switch(tag2) {
case MV_TAG:
	Glue<E2Elem>::push(
		L, 
		new E2Elem(
			MV_TAG,
			new mv(
				lc(mv(*((bivector *)elem1->elem)), mv(*((mv *)elem2->elem)))
			)
		)
	);
break;

case E1_TAG:
	Glue<E2Elem>::push(
		L, 
		new E2Elem(
			MV_TAG,
			new mv(
				lc(mv(*((bivector *)elem1->elem)), mv(*((e1_t *)elem2->elem)))
			)
		)
	);
break;

case E2_TAG:
	Glue<E2Elem>::push(
		L, 
		new E2Elem(
			MV_TAG,
			new mv(
				lc(mv(*((bivector *)elem1->elem)), mv(*((e2_t *)elem2->elem)))
			)
		)
	);
break;

case I2_TAG:
	Glue<E2Elem>::push(
		L, 
		new E2Elem(
			MV_TAG,
			new mv(
				lc(mv(*((bivector *)elem1->elem)), mv(*((I2_t *)elem2->elem)))
			)
		)
	);
break;

case I2i_TAG:
	Glue<E2Elem>::push(
		L, 
		new E2Elem(
			MV_TAG,
			new mv(
				lc(mv(*((bivector *)elem1->elem)), mv(*((I2i_t *)elem2->elem)))
			)
		)
	);
break;

case VECTOR_TAG:
				lua::push<double>(
					L, 
					lc(*((bivector *)elem1->elem), *((vector *)elem2->elem))
				);
			
break;

case BIVECTOR_TAG:
	Glue<E2Elem>::push(
		L, 
		new E2Elem(
			MV_TAG,
			new mv(
				lc(mv(*((bivector *)elem1->elem)), mv(*((bivector *)elem2->elem)))
			)
		)
	);
break;

case ROTOR_TAG:
	Glue<E2Elem>::push(
		L, 
		new E2Elem(
			MV_TAG,
			new mv(
				lc(mv(*((bivector *)elem1->elem)), mv(*((rotor *)elem2->elem)))
			)
		)
	);
break;

case DOUBLE_TAG:
	Glue<E2Elem>::push(
		L, 
		new E2Elem(
			MV_TAG,
			new mv(
				lc(mv(*((bivector *)elem1->elem)), *((double *)elem2))
			)
		)
	);
break;
}
break;

case ROTOR_TAG:
switch(tag2) {
case MV_TAG:
	Glue<E2Elem>::push(
		L, 
		new E2Elem(
			MV_TAG,
			new mv(
				lc(mv(*((rotor *)elem1->elem)), mv(*((mv *)elem2->elem)))
			)
		)
	);
break;

case E1_TAG:
	Glue<E2Elem>::push(
		L, 
		new E2Elem(
			MV_TAG,
			new mv(
				lc(mv(*((rotor *)elem1->elem)), mv(*((e1_t *)elem2->elem)))
			)
		)
	);
break;

case E2_TAG:
	Glue<E2Elem>::push(
		L, 
		new E2Elem(
			MV_TAG,
			new mv(
				lc(mv(*((rotor *)elem1->elem)), mv(*((e2_t *)elem2->elem)))
			)
		)
	);
break;

case I2_TAG:
	Glue<E2Elem>::push(
		L, 
		new E2Elem(
			MV_TAG,
			new mv(
				lc(mv(*((rotor *)elem1->elem)), mv(*((I2_t *)elem2->elem)))
			)
		)
	);
break;

case I2i_TAG:
	Glue<E2Elem>::push(
		L, 
		new E2Elem(
			MV_TAG,
			new mv(
				lc(mv(*((rotor *)elem1->elem)), mv(*((I2i_t *)elem2->elem)))
			)
		)
	);
break;

case VECTOR_TAG:
	Glue<E2Elem>::push(
		L, 
		new E2Elem(
			MV_TAG,
			new mv(
				lc(mv(*((rotor *)elem1->elem)), mv(*((vector *)elem2->elem)))
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
				lc(*((rotor *)elem1->elem), *((bivector *)elem2->elem))
			)
		)
	);
break;

case ROTOR_TAG:
	Glue<E2Elem>::push(
		L, 
		new E2Elem(
			MV_TAG,
			new mv(
				lc(mv(*((rotor *)elem1->elem)), mv(*((rotor *)elem2->elem)))
			)
		)
	);
break;

case DOUBLE_TAG:
	Glue<E2Elem>::push(
		L, 
		new E2Elem(
			MV_TAG,
			new mv(
				lc(mv(*((rotor *)elem1->elem)), *((double *)elem2))
			)
		)
	);
break;
}
break;

case DOUBLE_TAG:
switch(tag2) {
case MV_TAG:
	Glue<E2Elem>::push(
		L, 
		new E2Elem(
			MV_TAG,
			new mv(
				lc(*((double *)elem1), mv(*((mv *)elem2->elem)))
			)
		)
	);
break;

case E1_TAG:
	Glue<E2Elem>::push(
		L, 
		new E2Elem(
			MV_TAG,
			new mv(
				lc(*((double *)elem1), mv(*((e1_t *)elem2->elem)))
			)
		)
	);
break;

case E2_TAG:
	Glue<E2Elem>::push(
		L, 
		new E2Elem(
			MV_TAG,
			new mv(
				lc(*((double *)elem1), mv(*((e2_t *)elem2->elem)))
			)
		)
	);
break;

case I2_TAG:
	Glue<E2Elem>::push(
		L, 
		new E2Elem(
			MV_TAG,
			new mv(
				lc(*((double *)elem1), mv(*((I2_t *)elem2->elem)))
			)
		)
	);
break;

case I2i_TAG:
	Glue<E2Elem>::push(
		L, 
		new E2Elem(
			MV_TAG,
			new mv(
				lc(*((double *)elem1), mv(*((I2i_t *)elem2->elem)))
			)
		)
	);
break;

case VECTOR_TAG:
	Glue<E2Elem>::push(
		L, 
		new E2Elem(
			MV_TAG,
			new mv(
				lc(*((double *)elem1), mv(*((vector *)elem2->elem)))
			)
		)
	);
break;

case BIVECTOR_TAG:
	Glue<E2Elem>::push(
		L, 
		new E2Elem(
			MV_TAG,
			new mv(
				lc(*((double *)elem1), mv(*((bivector *)elem2->elem)))
			)
		)
	);
break;

case ROTOR_TAG:
	Glue<E2Elem>::push(
		L, 
		new E2Elem(
			MV_TAG,
			new mv(
				lc(*((double *)elem1), mv(*((rotor *)elem2->elem)))
			)
		)
	);
break;

case DOUBLE_TAG:
	Glue<E2Elem>::push(
		L, 
		new E2Elem(
			MV_TAG,
			new mv(
				lc(*((double *)elem1), *((double *)elem2))
			)
		)
	);
break;
}
break;
}
return 1;
}


int e2space_rc(lua_State *L) {
	double d1, d2;
	E2Tag tag1;
	E2Tag tag2;
	E2Elem *elem1 = Glue<E2Elem>::to(L, 1);
	E2Elem *elem2 = Glue<E2Elem>::to(L, 2);
	
	
	if(!elem1) {
		if(lua::is<double>(L, 1)) {
			d1 = lua::to<double>(L, 1);
			elem1 = (E2Elem *)(&d1);
			tag1 = DOUBLE_TAG;
		}
		else {
			luaL_error(L, "e2.rc: invalid arguments");
		}
	}
	else {
		tag1 = elem1->tag;
	}
	
	if(!elem2) {
		if(lua::is<double>(L, 2)) {
			d2 = lua::to<double>(L, 2);
			elem2 = (E2Elem *)(&d2);
			tag2 = DOUBLE_TAG;
		}
		else {
			luaL_error(L, "e2.rc: invalid arguments");
		}
	}
	else {
		tag2 = elem2->tag;
	}
	
switch(tag1) {
case MV_TAG:
switch(tag2) {
case MV_TAG:
	Glue<E2Elem>::push(
		L, 
		new E2Elem(
			MV_TAG,
			new mv(
				rc(*((mv *)elem1->elem), *((mv *)elem2->elem))
			)
		)
	);
break;

case E1_TAG:
	Glue<E2Elem>::push(
		L, 
		new E2Elem(
			MV_TAG,
			new mv(
				rc(mv(*((mv *)elem1->elem)), mv(*((e1_t *)elem2->elem)))
			)
		)
	);
break;

case E2_TAG:
	Glue<E2Elem>::push(
		L, 
		new E2Elem(
			MV_TAG,
			new mv(
				rc(mv(*((mv *)elem1->elem)), mv(*((e2_t *)elem2->elem)))
			)
		)
	);
break;

case I2_TAG:
	Glue<E2Elem>::push(
		L, 
		new E2Elem(
			MV_TAG,
			new mv(
				rc(mv(*((mv *)elem1->elem)), mv(*((I2_t *)elem2->elem)))
			)
		)
	);
break;

case I2i_TAG:
	Glue<E2Elem>::push(
		L, 
		new E2Elem(
			MV_TAG,
			new mv(
				rc(mv(*((mv *)elem1->elem)), mv(*((I2i_t *)elem2->elem)))
			)
		)
	);
break;

case VECTOR_TAG:
	Glue<E2Elem>::push(
		L, 
		new E2Elem(
			MV_TAG,
			new mv(
				rc(mv(*((mv *)elem1->elem)), mv(*((vector *)elem2->elem)))
			)
		)
	);
break;

case BIVECTOR_TAG:
	Glue<E2Elem>::push(
		L, 
		new E2Elem(
			MV_TAG,
			new mv(
				rc(mv(*((mv *)elem1->elem)), mv(*((bivector *)elem2->elem)))
			)
		)
	);
break;

case ROTOR_TAG:
	Glue<E2Elem>::push(
		L, 
		new E2Elem(
			MV_TAG,
			new mv(
				rc(mv(*((mv *)elem1->elem)), mv(*((rotor *)elem2->elem)))
			)
		)
	);
break;

case DOUBLE_TAG:
	Glue<E2Elem>::push(
		L, 
		new E2Elem(
			MV_TAG,
			new mv(
				rc(mv(*((mv *)elem1->elem)), *((double *)elem2))
			)
		)
	);
break;
}
break;

case E1_TAG:
switch(tag2) {
case MV_TAG:
	Glue<E2Elem>::push(
		L, 
		new E2Elem(
			MV_TAG,
			new mv(
				rc(mv(*((e1_t *)elem1->elem)), mv(*((mv *)elem2->elem)))
			)
		)
	);
break;

case E1_TAG:
	Glue<E2Elem>::push(
		L, 
		new E2Elem(
			MV_TAG,
			new mv(
				rc(mv(*((e1_t *)elem1->elem)), mv(*((e1_t *)elem2->elem)))
			)
		)
	);
break;

case E2_TAG:
	Glue<E2Elem>::push(
		L, 
		new E2Elem(
			MV_TAG,
			new mv(
				rc(mv(*((e1_t *)elem1->elem)), mv(*((e2_t *)elem2->elem)))
			)
		)
	);
break;

case I2_TAG:
	Glue<E2Elem>::push(
		L, 
		new E2Elem(
			MV_TAG,
			new mv(
				rc(mv(*((e1_t *)elem1->elem)), mv(*((I2_t *)elem2->elem)))
			)
		)
	);
break;

case I2i_TAG:
	Glue<E2Elem>::push(
		L, 
		new E2Elem(
			MV_TAG,
			new mv(
				rc(mv(*((e1_t *)elem1->elem)), mv(*((I2i_t *)elem2->elem)))
			)
		)
	);
break;

case VECTOR_TAG:
	Glue<E2Elem>::push(
		L, 
		new E2Elem(
			MV_TAG,
			new mv(
				rc(mv(*((e1_t *)elem1->elem)), mv(*((vector *)elem2->elem)))
			)
		)
	);
break;

case BIVECTOR_TAG:
	Glue<E2Elem>::push(
		L, 
		new E2Elem(
			MV_TAG,
			new mv(
				rc(mv(*((e1_t *)elem1->elem)), mv(*((bivector *)elem2->elem)))
			)
		)
	);
break;

case ROTOR_TAG:
	Glue<E2Elem>::push(
		L, 
		new E2Elem(
			MV_TAG,
			new mv(
				rc(mv(*((e1_t *)elem1->elem)), mv(*((rotor *)elem2->elem)))
			)
		)
	);
break;

case DOUBLE_TAG:
	Glue<E2Elem>::push(
		L, 
		new E2Elem(
			MV_TAG,
			new mv(
				rc(mv(*((e1_t *)elem1->elem)), *((double *)elem2))
			)
		)
	);
break;
}
break;

case E2_TAG:
switch(tag2) {
case MV_TAG:
	Glue<E2Elem>::push(
		L, 
		new E2Elem(
			MV_TAG,
			new mv(
				rc(mv(*((e2_t *)elem1->elem)), mv(*((mv *)elem2->elem)))
			)
		)
	);
break;

case E1_TAG:
	Glue<E2Elem>::push(
		L, 
		new E2Elem(
			MV_TAG,
			new mv(
				rc(mv(*((e2_t *)elem1->elem)), mv(*((e1_t *)elem2->elem)))
			)
		)
	);
break;

case E2_TAG:
	Glue<E2Elem>::push(
		L, 
		new E2Elem(
			MV_TAG,
			new mv(
				rc(mv(*((e2_t *)elem1->elem)), mv(*((e2_t *)elem2->elem)))
			)
		)
	);
break;

case I2_TAG:
	Glue<E2Elem>::push(
		L, 
		new E2Elem(
			MV_TAG,
			new mv(
				rc(mv(*((e2_t *)elem1->elem)), mv(*((I2_t *)elem2->elem)))
			)
		)
	);
break;

case I2i_TAG:
	Glue<E2Elem>::push(
		L, 
		new E2Elem(
			MV_TAG,
			new mv(
				rc(mv(*((e2_t *)elem1->elem)), mv(*((I2i_t *)elem2->elem)))
			)
		)
	);
break;

case VECTOR_TAG:
	Glue<E2Elem>::push(
		L, 
		new E2Elem(
			MV_TAG,
			new mv(
				rc(mv(*((e2_t *)elem1->elem)), mv(*((vector *)elem2->elem)))
			)
		)
	);
break;

case BIVECTOR_TAG:
	Glue<E2Elem>::push(
		L, 
		new E2Elem(
			MV_TAG,
			new mv(
				rc(mv(*((e2_t *)elem1->elem)), mv(*((bivector *)elem2->elem)))
			)
		)
	);
break;

case ROTOR_TAG:
	Glue<E2Elem>::push(
		L, 
		new E2Elem(
			MV_TAG,
			new mv(
				rc(mv(*((e2_t *)elem1->elem)), mv(*((rotor *)elem2->elem)))
			)
		)
	);
break;

case DOUBLE_TAG:
	Glue<E2Elem>::push(
		L, 
		new E2Elem(
			MV_TAG,
			new mv(
				rc(mv(*((e2_t *)elem1->elem)), *((double *)elem2))
			)
		)
	);
break;
}
break;

case I2_TAG:
switch(tag2) {
case MV_TAG:
	Glue<E2Elem>::push(
		L, 
		new E2Elem(
			MV_TAG,
			new mv(
				rc(mv(*((I2_t *)elem1->elem)), mv(*((mv *)elem2->elem)))
			)
		)
	);
break;

case E1_TAG:
	Glue<E2Elem>::push(
		L, 
		new E2Elem(
			MV_TAG,
			new mv(
				rc(mv(*((I2_t *)elem1->elem)), mv(*((e1_t *)elem2->elem)))
			)
		)
	);
break;

case E2_TAG:
	Glue<E2Elem>::push(
		L, 
		new E2Elem(
			MV_TAG,
			new mv(
				rc(mv(*((I2_t *)elem1->elem)), mv(*((e2_t *)elem2->elem)))
			)
		)
	);
break;

case I2_TAG:
	Glue<E2Elem>::push(
		L, 
		new E2Elem(
			MV_TAG,
			new mv(
				rc(mv(*((I2_t *)elem1->elem)), mv(*((I2_t *)elem2->elem)))
			)
		)
	);
break;

case I2i_TAG:
	Glue<E2Elem>::push(
		L, 
		new E2Elem(
			MV_TAG,
			new mv(
				rc(mv(*((I2_t *)elem1->elem)), mv(*((I2i_t *)elem2->elem)))
			)
		)
	);
break;

case VECTOR_TAG:
	Glue<E2Elem>::push(
		L, 
		new E2Elem(
			MV_TAG,
			new mv(
				rc(mv(*((I2_t *)elem1->elem)), mv(*((vector *)elem2->elem)))
			)
		)
	);
break;

case BIVECTOR_TAG:
				lua::push<double>(
					L, 
					rc(*((I2_t *)elem1->elem), *((bivector *)elem2->elem))
				);
			
break;

case ROTOR_TAG:
	Glue<E2Elem>::push(
		L, 
		new E2Elem(
			MV_TAG,
			new mv(
				rc(mv(*((I2_t *)elem1->elem)), mv(*((rotor *)elem2->elem)))
			)
		)
	);
break;

case DOUBLE_TAG:
	Glue<E2Elem>::push(
		L, 
		new E2Elem(
			MV_TAG,
			new mv(
				rc(mv(*((I2_t *)elem1->elem)), *((double *)elem2))
			)
		)
	);
break;
}
break;

case I2i_TAG:
switch(tag2) {
case MV_TAG:
	Glue<E2Elem>::push(
		L, 
		new E2Elem(
			MV_TAG,
			new mv(
				rc(mv(*((I2i_t *)elem1->elem)), mv(*((mv *)elem2->elem)))
			)
		)
	);
break;

case E1_TAG:
	Glue<E2Elem>::push(
		L, 
		new E2Elem(
			MV_TAG,
			new mv(
				rc(mv(*((I2i_t *)elem1->elem)), mv(*((e1_t *)elem2->elem)))
			)
		)
	);
break;

case E2_TAG:
	Glue<E2Elem>::push(
		L, 
		new E2Elem(
			MV_TAG,
			new mv(
				rc(mv(*((I2i_t *)elem1->elem)), mv(*((e2_t *)elem2->elem)))
			)
		)
	);
break;

case I2_TAG:
	Glue<E2Elem>::push(
		L, 
		new E2Elem(
			MV_TAG,
			new mv(
				rc(mv(*((I2i_t *)elem1->elem)), mv(*((I2_t *)elem2->elem)))
			)
		)
	);
break;

case I2i_TAG:
	Glue<E2Elem>::push(
		L, 
		new E2Elem(
			MV_TAG,
			new mv(
				rc(mv(*((I2i_t *)elem1->elem)), mv(*((I2i_t *)elem2->elem)))
			)
		)
	);
break;

case VECTOR_TAG:
	Glue<E2Elem>::push(
		L, 
		new E2Elem(
			MV_TAG,
			new mv(
				rc(mv(*((I2i_t *)elem1->elem)), mv(*((vector *)elem2->elem)))
			)
		)
	);
break;

case BIVECTOR_TAG:
	Glue<E2Elem>::push(
		L, 
		new E2Elem(
			MV_TAG,
			new mv(
				rc(mv(*((I2i_t *)elem1->elem)), mv(*((bivector *)elem2->elem)))
			)
		)
	);
break;

case ROTOR_TAG:
	Glue<E2Elem>::push(
		L, 
		new E2Elem(
			MV_TAG,
			new mv(
				rc(mv(*((I2i_t *)elem1->elem)), mv(*((rotor *)elem2->elem)))
			)
		)
	);
break;

case DOUBLE_TAG:
	Glue<E2Elem>::push(
		L, 
		new E2Elem(
			MV_TAG,
			new mv(
				rc(mv(*((I2i_t *)elem1->elem)), *((double *)elem2))
			)
		)
	);
break;
}
break;

case VECTOR_TAG:
switch(tag2) {
case MV_TAG:
	Glue<E2Elem>::push(
		L, 
		new E2Elem(
			MV_TAG,
			new mv(
				rc(mv(*((vector *)elem1->elem)), mv(*((mv *)elem2->elem)))
			)
		)
	);
break;

case E1_TAG:
	Glue<E2Elem>::push(
		L, 
		new E2Elem(
			MV_TAG,
			new mv(
				rc(mv(*((vector *)elem1->elem)), mv(*((e1_t *)elem2->elem)))
			)
		)
	);
break;

case E2_TAG:
	Glue<E2Elem>::push(
		L, 
		new E2Elem(
			MV_TAG,
			new mv(
				rc(mv(*((vector *)elem1->elem)), mv(*((e2_t *)elem2->elem)))
			)
		)
	);
break;

case I2_TAG:
	Glue<E2Elem>::push(
		L, 
		new E2Elem(
			MV_TAG,
			new mv(
				rc(mv(*((vector *)elem1->elem)), mv(*((I2_t *)elem2->elem)))
			)
		)
	);
break;

case I2i_TAG:
	Glue<E2Elem>::push(
		L, 
		new E2Elem(
			MV_TAG,
			new mv(
				rc(mv(*((vector *)elem1->elem)), mv(*((I2i_t *)elem2->elem)))
			)
		)
	);
break;

case VECTOR_TAG:
				lua::push<double>(
					L, 
					rc(*((vector *)elem1->elem), *((vector *)elem2->elem))
				);
			
break;

case BIVECTOR_TAG:
				lua::push<double>(
					L, 
					rc(*((vector *)elem1->elem), *((bivector *)elem2->elem))
				);
			
break;

case ROTOR_TAG:
	Glue<E2Elem>::push(
		L, 
		new E2Elem(
			VECTOR_TAG,
			new vector(
				rc(*((vector *)elem1->elem), *((rotor *)elem2->elem))
			)
		)
	);
break;

case DOUBLE_TAG:
	Glue<E2Elem>::push(
		L, 
		new E2Elem(
			MV_TAG,
			new mv(
				rc(mv(*((vector *)elem1->elem)), *((double *)elem2))
			)
		)
	);
break;
}
break;

case BIVECTOR_TAG:
switch(tag2) {
case MV_TAG:
	Glue<E2Elem>::push(
		L, 
		new E2Elem(
			MV_TAG,
			new mv(
				rc(mv(*((bivector *)elem1->elem)), mv(*((mv *)elem2->elem)))
			)
		)
	);
break;

case E1_TAG:
	Glue<E2Elem>::push(
		L, 
		new E2Elem(
			MV_TAG,
			new mv(
				rc(mv(*((bivector *)elem1->elem)), mv(*((e1_t *)elem2->elem)))
			)
		)
	);
break;

case E2_TAG:
	Glue<E2Elem>::push(
		L, 
		new E2Elem(
			MV_TAG,
			new mv(
				rc(mv(*((bivector *)elem1->elem)), mv(*((e2_t *)elem2->elem)))
			)
		)
	);
break;

case I2_TAG:
	Glue<E2Elem>::push(
		L, 
		new E2Elem(
			MV_TAG,
			new mv(
				rc(mv(*((bivector *)elem1->elem)), mv(*((I2_t *)elem2->elem)))
			)
		)
	);
break;

case I2i_TAG:
	Glue<E2Elem>::push(
		L, 
		new E2Elem(
			MV_TAG,
			new mv(
				rc(mv(*((bivector *)elem1->elem)), mv(*((I2i_t *)elem2->elem)))
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
				rc(*((bivector *)elem1->elem), *((vector *)elem2->elem))
			)
		)
	);
break;

case BIVECTOR_TAG:
	Glue<E2Elem>::push(
		L, 
		new E2Elem(
			MV_TAG,
			new mv(
				rc(mv(*((bivector *)elem1->elem)), mv(*((bivector *)elem2->elem)))
			)
		)
	);
break;

case ROTOR_TAG:
	Glue<E2Elem>::push(
		L, 
		new E2Elem(
			MV_TAG,
			new mv(
				rc(mv(*((bivector *)elem1->elem)), mv(*((rotor *)elem2->elem)))
			)
		)
	);
break;

case DOUBLE_TAG:
	Glue<E2Elem>::push(
		L, 
		new E2Elem(
			MV_TAG,
			new mv(
				rc(mv(*((bivector *)elem1->elem)), *((double *)elem2))
			)
		)
	);
break;
}
break;

case ROTOR_TAG:
switch(tag2) {
case MV_TAG:
	Glue<E2Elem>::push(
		L, 
		new E2Elem(
			MV_TAG,
			new mv(
				rc(mv(*((rotor *)elem1->elem)), mv(*((mv *)elem2->elem)))
			)
		)
	);
break;

case E1_TAG:
	Glue<E2Elem>::push(
		L, 
		new E2Elem(
			MV_TAG,
			new mv(
				rc(mv(*((rotor *)elem1->elem)), mv(*((e1_t *)elem2->elem)))
			)
		)
	);
break;

case E2_TAG:
	Glue<E2Elem>::push(
		L, 
		new E2Elem(
			MV_TAG,
			new mv(
				rc(mv(*((rotor *)elem1->elem)), mv(*((e2_t *)elem2->elem)))
			)
		)
	);
break;

case I2_TAG:
	Glue<E2Elem>::push(
		L, 
		new E2Elem(
			MV_TAG,
			new mv(
				rc(mv(*((rotor *)elem1->elem)), mv(*((I2_t *)elem2->elem)))
			)
		)
	);
break;

case I2i_TAG:
	Glue<E2Elem>::push(
		L, 
		new E2Elem(
			MV_TAG,
			new mv(
				rc(mv(*((rotor *)elem1->elem)), mv(*((I2i_t *)elem2->elem)))
			)
		)
	);
break;

case VECTOR_TAG:
	Glue<E2Elem>::push(
		L, 
		new E2Elem(
			MV_TAG,
			new mv(
				rc(mv(*((rotor *)elem1->elem)), mv(*((vector *)elem2->elem)))
			)
		)
	);
break;

case BIVECTOR_TAG:
				lua::push<double>(
					L, 
					rc(*((rotor *)elem1->elem), *((bivector *)elem2->elem))
				);
			
break;

case ROTOR_TAG:
	Glue<E2Elem>::push(
		L, 
		new E2Elem(
			MV_TAG,
			new mv(
				rc(mv(*((rotor *)elem1->elem)), mv(*((rotor *)elem2->elem)))
			)
		)
	);
break;

case DOUBLE_TAG:
	Glue<E2Elem>::push(
		L, 
		new E2Elem(
			MV_TAG,
			new mv(
				rc(mv(*((rotor *)elem1->elem)), *((double *)elem2))
			)
		)
	);
break;
}
break;

case DOUBLE_TAG:
switch(tag2) {
case MV_TAG:
	Glue<E2Elem>::push(
		L, 
		new E2Elem(
			MV_TAG,
			new mv(
				rc(*((double *)elem1), mv(*((mv *)elem2->elem)))
			)
		)
	);
break;

case E1_TAG:
	Glue<E2Elem>::push(
		L, 
		new E2Elem(
			MV_TAG,
			new mv(
				rc(*((double *)elem1), mv(*((e1_t *)elem2->elem)))
			)
		)
	);
break;

case E2_TAG:
	Glue<E2Elem>::push(
		L, 
		new E2Elem(
			MV_TAG,
			new mv(
				rc(*((double *)elem1), mv(*((e2_t *)elem2->elem)))
			)
		)
	);
break;

case I2_TAG:
	Glue<E2Elem>::push(
		L, 
		new E2Elem(
			MV_TAG,
			new mv(
				rc(*((double *)elem1), mv(*((I2_t *)elem2->elem)))
			)
		)
	);
break;

case I2i_TAG:
	Glue<E2Elem>::push(
		L, 
		new E2Elem(
			MV_TAG,
			new mv(
				rc(*((double *)elem1), mv(*((I2i_t *)elem2->elem)))
			)
		)
	);
break;

case VECTOR_TAG:
	Glue<E2Elem>::push(
		L, 
		new E2Elem(
			MV_TAG,
			new mv(
				rc(*((double *)elem1), mv(*((vector *)elem2->elem)))
			)
		)
	);
break;

case BIVECTOR_TAG:
	Glue<E2Elem>::push(
		L, 
		new E2Elem(
			MV_TAG,
			new mv(
				rc(*((double *)elem1), mv(*((bivector *)elem2->elem)))
			)
		)
	);
break;

case ROTOR_TAG:
	Glue<E2Elem>::push(
		L, 
		new E2Elem(
			MV_TAG,
			new mv(
				rc(*((double *)elem1), mv(*((rotor *)elem2->elem)))
			)
		)
	);
break;

case DOUBLE_TAG:
	Glue<E2Elem>::push(
		L, 
		new E2Elem(
			MV_TAG,
			new mv(
				rc(*((double *)elem1), *((double *)elem2))
			)
		)
	);
break;
}
break;
}
return 1;
}


#ifdef __cplusplus
}
#endif