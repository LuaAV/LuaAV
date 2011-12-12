// THIS FILE GENERATED AUTOMATICALLY, DO NOT EDIT
#include "lua_e3ga.h"

#ifdef __cplusplus
extern "C" {
#endif




int e3space_add(lua_State *L) {
	double d1, d2;
	E3Tag tag1;
	E3Tag tag2;
	E3Elem *elem1 = Glue<E3Elem>::to(L, 1);
	E3Elem *elem2 = Glue<E3Elem>::to(L, 2);
	
	
	if(!elem1) {
		if(lua::is<double>(L, 1)) {
			d1 = lua::to<double>(L, 1);
			elem1 = (E3Elem *)(&d1);
			tag1 = DOUBLE_TAG;
		}
		else {
			luaL_error(L, "e3.add: invalid arguments");
		}
	}
	else {
		tag1 = elem1->tag;
	}
	
	if(!elem2) {
		if(lua::is<double>(L, 2)) {
			d2 = lua::to<double>(L, 2);
			elem2 = (E3Elem *)(&d2);
			tag2 = DOUBLE_TAG;
		}
		else {
			luaL_error(L, "e3.add: invalid arguments");
		}
	}
	else {
		tag2 = elem2->tag;
	}
	
switch(tag1) {
case MV_TAG:
switch(tag2) {
case MV_TAG:
	Glue<E3Elem>::push(
		L, 
		new E3Elem(
			MV_TAG,
			new mv(
				add(*((mv *)elem1->elem), *((mv *)elem2->elem))
			)
		)
	);
break;

case E1_TAG:
	Glue<E3Elem>::push(
		L, 
		new E3Elem(
			MV_TAG,
			new mv(
				add(mv(*((mv *)elem1->elem)), mv(*((e1_t *)elem2->elem)))
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
				add(mv(*((mv *)elem1->elem)), mv(*((e2_t *)elem2->elem)))
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
				add(mv(*((mv *)elem1->elem)), mv(*((e3_t *)elem2->elem)))
			)
		)
	);
break;

case I3_TAG:
	Glue<E3Elem>::push(
		L, 
		new E3Elem(
			MV_TAG,
			new mv(
				add(mv(*((mv *)elem1->elem)), mv(*((I3_t *)elem2->elem)))
			)
		)
	);
break;

case VECTOR_TAG:
	Glue<E3Elem>::push(
		L, 
		new E3Elem(
			MV_TAG,
			new mv(
				add(mv(*((mv *)elem1->elem)), mv(*((vector *)elem2->elem)))
			)
		)
	);
break;

case BIVECTOR_TAG:
	Glue<E3Elem>::push(
		L, 
		new E3Elem(
			MV_TAG,
			new mv(
				add(mv(*((mv *)elem1->elem)), mv(*((bivector *)elem2->elem)))
			)
		)
	);
break;

case TRIVECTOR_TAG:
	Glue<E3Elem>::push(
		L, 
		new E3Elem(
			MV_TAG,
			new mv(
				add(mv(*((mv *)elem1->elem)), mv(*((trivector *)elem2->elem)))
			)
		)
	);
break;

case ROTOR_TAG:
	Glue<E3Elem>::push(
		L, 
		new E3Elem(
			MV_TAG,
			new mv(
				add(mv(*((mv *)elem1->elem)), mv(*((rotor *)elem2->elem)))
			)
		)
	);
break;

case ODDVERSOR_TAG:
	Glue<E3Elem>::push(
		L, 
		new E3Elem(
			MV_TAG,
			new mv(
				add(mv(*((mv *)elem1->elem)), mv(*((oddVersor *)elem2->elem)))
			)
		)
	);
break;

case DOUBLE_TAG:
	Glue<E3Elem>::push(
		L, 
		new E3Elem(
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
	Glue<E3Elem>::push(
		L, 
		new E3Elem(
			MV_TAG,
			new mv(
				add(mv(*((e1_t *)elem1->elem)), mv(*((mv *)elem2->elem)))
			)
		)
	);
break;

case E1_TAG:
	Glue<E3Elem>::push(
		L, 
		new E3Elem(
			MV_TAG,
			new mv(
				add(mv(*((e1_t *)elem1->elem)), mv(*((e1_t *)elem2->elem)))
			)
		)
	);
break;

case E2_TAG:
	Glue<E3Elem>::push(
		L, 
		new E3Elem(
			VECTOR_TAG,
			new vector(
				add(*((e1_t *)elem1->elem), *((e2_t *)elem2->elem))
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
				add(mv(*((e1_t *)elem1->elem)), mv(*((e3_t *)elem2->elem)))
			)
		)
	);
break;

case I3_TAG:
	Glue<E3Elem>::push(
		L, 
		new E3Elem(
			ODDVERSOR_TAG,
			new oddVersor(
				add(*((e1_t *)elem1->elem), *((I3_t *)elem2->elem))
			)
		)
	);
break;

case VECTOR_TAG:
	Glue<E3Elem>::push(
		L, 
		new E3Elem(
			MV_TAG,
			new mv(
				add(mv(*((e1_t *)elem1->elem)), mv(*((vector *)elem2->elem)))
			)
		)
	);
break;

case BIVECTOR_TAG:
	Glue<E3Elem>::push(
		L, 
		new E3Elem(
			MV_TAG,
			new mv(
				add(mv(*((e1_t *)elem1->elem)), mv(*((bivector *)elem2->elem)))
			)
		)
	);
break;

case TRIVECTOR_TAG:
	Glue<E3Elem>::push(
		L, 
		new E3Elem(
			MV_TAG,
			new mv(
				add(mv(*((e1_t *)elem1->elem)), mv(*((trivector *)elem2->elem)))
			)
		)
	);
break;

case ROTOR_TAG:
	Glue<E3Elem>::push(
		L, 
		new E3Elem(
			MV_TAG,
			new mv(
				add(mv(*((e1_t *)elem1->elem)), mv(*((rotor *)elem2->elem)))
			)
		)
	);
break;

case ODDVERSOR_TAG:
	Glue<E3Elem>::push(
		L, 
		new E3Elem(
			MV_TAG,
			new mv(
				add(mv(*((e1_t *)elem1->elem)), mv(*((oddVersor *)elem2->elem)))
			)
		)
	);
break;

case DOUBLE_TAG:
	Glue<E3Elem>::push(
		L, 
		new E3Elem(
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
	Glue<E3Elem>::push(
		L, 
		new E3Elem(
			MV_TAG,
			new mv(
				add(mv(*((e2_t *)elem1->elem)), mv(*((mv *)elem2->elem)))
			)
		)
	);
break;

case E1_TAG:
	Glue<E3Elem>::push(
		L, 
		new E3Elem(
			MV_TAG,
			new mv(
				add(mv(*((e2_t *)elem1->elem)), mv(*((e1_t *)elem2->elem)))
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
				add(mv(*((e2_t *)elem1->elem)), mv(*((e2_t *)elem2->elem)))
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
				add(mv(*((e2_t *)elem1->elem)), mv(*((e3_t *)elem2->elem)))
			)
		)
	);
break;

case I3_TAG:
	Glue<E3Elem>::push(
		L, 
		new E3Elem(
			MV_TAG,
			new mv(
				add(mv(*((e2_t *)elem1->elem)), mv(*((I3_t *)elem2->elem)))
			)
		)
	);
break;

case VECTOR_TAG:
	Glue<E3Elem>::push(
		L, 
		new E3Elem(
			MV_TAG,
			new mv(
				add(mv(*((e2_t *)elem1->elem)), mv(*((vector *)elem2->elem)))
			)
		)
	);
break;

case BIVECTOR_TAG:
	Glue<E3Elem>::push(
		L, 
		new E3Elem(
			MV_TAG,
			new mv(
				add(mv(*((e2_t *)elem1->elem)), mv(*((bivector *)elem2->elem)))
			)
		)
	);
break;

case TRIVECTOR_TAG:
	Glue<E3Elem>::push(
		L, 
		new E3Elem(
			MV_TAG,
			new mv(
				add(mv(*((e2_t *)elem1->elem)), mv(*((trivector *)elem2->elem)))
			)
		)
	);
break;

case ROTOR_TAG:
	Glue<E3Elem>::push(
		L, 
		new E3Elem(
			MV_TAG,
			new mv(
				add(mv(*((e2_t *)elem1->elem)), mv(*((rotor *)elem2->elem)))
			)
		)
	);
break;

case ODDVERSOR_TAG:
	Glue<E3Elem>::push(
		L, 
		new E3Elem(
			MV_TAG,
			new mv(
				add(mv(*((e2_t *)elem1->elem)), mv(*((oddVersor *)elem2->elem)))
			)
		)
	);
break;

case DOUBLE_TAG:
	Glue<E3Elem>::push(
		L, 
		new E3Elem(
			MV_TAG,
			new mv(
				add(mv(*((e2_t *)elem1->elem)), *((double *)elem2))
			)
		)
	);
break;
}
break;

case E3_TAG:
switch(tag2) {
case MV_TAG:
	Glue<E3Elem>::push(
		L, 
		new E3Elem(
			MV_TAG,
			new mv(
				add(mv(*((e3_t *)elem1->elem)), mv(*((mv *)elem2->elem)))
			)
		)
	);
break;

case E1_TAG:
	Glue<E3Elem>::push(
		L, 
		new E3Elem(
			MV_TAG,
			new mv(
				add(mv(*((e3_t *)elem1->elem)), mv(*((e1_t *)elem2->elem)))
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
				add(mv(*((e3_t *)elem1->elem)), mv(*((e2_t *)elem2->elem)))
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
				add(mv(*((e3_t *)elem1->elem)), mv(*((e3_t *)elem2->elem)))
			)
		)
	);
break;

case I3_TAG:
	Glue<E3Elem>::push(
		L, 
		new E3Elem(
			MV_TAG,
			new mv(
				add(mv(*((e3_t *)elem1->elem)), mv(*((I3_t *)elem2->elem)))
			)
		)
	);
break;

case VECTOR_TAG:
	Glue<E3Elem>::push(
		L, 
		new E3Elem(
			MV_TAG,
			new mv(
				add(mv(*((e3_t *)elem1->elem)), mv(*((vector *)elem2->elem)))
			)
		)
	);
break;

case BIVECTOR_TAG:
	Glue<E3Elem>::push(
		L, 
		new E3Elem(
			MV_TAG,
			new mv(
				add(mv(*((e3_t *)elem1->elem)), mv(*((bivector *)elem2->elem)))
			)
		)
	);
break;

case TRIVECTOR_TAG:
	Glue<E3Elem>::push(
		L, 
		new E3Elem(
			MV_TAG,
			new mv(
				add(mv(*((e3_t *)elem1->elem)), mv(*((trivector *)elem2->elem)))
			)
		)
	);
break;

case ROTOR_TAG:
	Glue<E3Elem>::push(
		L, 
		new E3Elem(
			MV_TAG,
			new mv(
				add(mv(*((e3_t *)elem1->elem)), mv(*((rotor *)elem2->elem)))
			)
		)
	);
break;

case ODDVERSOR_TAG:
	Glue<E3Elem>::push(
		L, 
		new E3Elem(
			MV_TAG,
			new mv(
				add(mv(*((e3_t *)elem1->elem)), mv(*((oddVersor *)elem2->elem)))
			)
		)
	);
break;

case DOUBLE_TAG:
	Glue<E3Elem>::push(
		L, 
		new E3Elem(
			MV_TAG,
			new mv(
				add(mv(*((e3_t *)elem1->elem)), *((double *)elem2))
			)
		)
	);
break;
}
break;

case I3_TAG:
switch(tag2) {
case MV_TAG:
	Glue<E3Elem>::push(
		L, 
		new E3Elem(
			MV_TAG,
			new mv(
				add(mv(*((I3_t *)elem1->elem)), mv(*((mv *)elem2->elem)))
			)
		)
	);
break;

case E1_TAG:
	Glue<E3Elem>::push(
		L, 
		new E3Elem(
			MV_TAG,
			new mv(
				add(mv(*((I3_t *)elem1->elem)), mv(*((e1_t *)elem2->elem)))
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
				add(mv(*((I3_t *)elem1->elem)), mv(*((e2_t *)elem2->elem)))
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
				add(mv(*((I3_t *)elem1->elem)), mv(*((e3_t *)elem2->elem)))
			)
		)
	);
break;

case I3_TAG:
	Glue<E3Elem>::push(
		L, 
		new E3Elem(
			MV_TAG,
			new mv(
				add(mv(*((I3_t *)elem1->elem)), mv(*((I3_t *)elem2->elem)))
			)
		)
	);
break;

case VECTOR_TAG:
	Glue<E3Elem>::push(
		L, 
		new E3Elem(
			MV_TAG,
			new mv(
				add(mv(*((I3_t *)elem1->elem)), mv(*((vector *)elem2->elem)))
			)
		)
	);
break;

case BIVECTOR_TAG:
	Glue<E3Elem>::push(
		L, 
		new E3Elem(
			MV_TAG,
			new mv(
				add(mv(*((I3_t *)elem1->elem)), mv(*((bivector *)elem2->elem)))
			)
		)
	);
break;

case TRIVECTOR_TAG:
	Glue<E3Elem>::push(
		L, 
		new E3Elem(
			MV_TAG,
			new mv(
				add(mv(*((I3_t *)elem1->elem)), mv(*((trivector *)elem2->elem)))
			)
		)
	);
break;

case ROTOR_TAG:
	Glue<E3Elem>::push(
		L, 
		new E3Elem(
			MV_TAG,
			new mv(
				add(mv(*((I3_t *)elem1->elem)), mv(*((rotor *)elem2->elem)))
			)
		)
	);
break;

case ODDVERSOR_TAG:
	Glue<E3Elem>::push(
		L, 
		new E3Elem(
			MV_TAG,
			new mv(
				add(mv(*((I3_t *)elem1->elem)), mv(*((oddVersor *)elem2->elem)))
			)
		)
	);
break;

case DOUBLE_TAG:
	Glue<E3Elem>::push(
		L, 
		new E3Elem(
			MV_TAG,
			new mv(
				add(mv(*((I3_t *)elem1->elem)), *((double *)elem2))
			)
		)
	);
break;
}
break;

case VECTOR_TAG:
switch(tag2) {
case MV_TAG:
	Glue<E3Elem>::push(
		L, 
		new E3Elem(
			MV_TAG,
			new mv(
				add(mv(*((vector *)elem1->elem)), mv(*((mv *)elem2->elem)))
			)
		)
	);
break;

case E1_TAG:
	Glue<E3Elem>::push(
		L, 
		new E3Elem(
			MV_TAG,
			new mv(
				add(mv(*((vector *)elem1->elem)), mv(*((e1_t *)elem2->elem)))
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
				add(mv(*((vector *)elem1->elem)), mv(*((e2_t *)elem2->elem)))
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
				add(mv(*((vector *)elem1->elem)), mv(*((e3_t *)elem2->elem)))
			)
		)
	);
break;

case I3_TAG:
	Glue<E3Elem>::push(
		L, 
		new E3Elem(
			MV_TAG,
			new mv(
				add(mv(*((vector *)elem1->elem)), mv(*((I3_t *)elem2->elem)))
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
				add(*((vector *)elem1->elem), *((vector *)elem2->elem))
			)
		)
	);
break;

case BIVECTOR_TAG:
	Glue<E3Elem>::push(
		L, 
		new E3Elem(
			MV_TAG,
			new mv(
				add(mv(*((vector *)elem1->elem)), mv(*((bivector *)elem2->elem)))
			)
		)
	);
break;

case TRIVECTOR_TAG:
	Glue<E3Elem>::push(
		L, 
		new E3Elem(
			ODDVERSOR_TAG,
			new oddVersor(
				add(*((vector *)elem1->elem), *((trivector *)elem2->elem))
			)
		)
	);
break;

case ROTOR_TAG:
	Glue<E3Elem>::push(
		L, 
		new E3Elem(
			MV_TAG,
			new mv(
				add(mv(*((vector *)elem1->elem)), mv(*((rotor *)elem2->elem)))
			)
		)
	);
break;

case ODDVERSOR_TAG:
	Glue<E3Elem>::push(
		L, 
		new E3Elem(
			MV_TAG,
			new mv(
				add(mv(*((vector *)elem1->elem)), mv(*((oddVersor *)elem2->elem)))
			)
		)
	);
break;

case DOUBLE_TAG:
	Glue<E3Elem>::push(
		L, 
		new E3Elem(
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
	Glue<E3Elem>::push(
		L, 
		new E3Elem(
			MV_TAG,
			new mv(
				add(mv(*((bivector *)elem1->elem)), mv(*((mv *)elem2->elem)))
			)
		)
	);
break;

case E1_TAG:
	Glue<E3Elem>::push(
		L, 
		new E3Elem(
			MV_TAG,
			new mv(
				add(mv(*((bivector *)elem1->elem)), mv(*((e1_t *)elem2->elem)))
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
				add(mv(*((bivector *)elem1->elem)), mv(*((e2_t *)elem2->elem)))
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
				add(mv(*((bivector *)elem1->elem)), mv(*((e3_t *)elem2->elem)))
			)
		)
	);
break;

case I3_TAG:
	Glue<E3Elem>::push(
		L, 
		new E3Elem(
			MV_TAG,
			new mv(
				add(mv(*((bivector *)elem1->elem)), mv(*((I3_t *)elem2->elem)))
			)
		)
	);
break;

case VECTOR_TAG:
	Glue<E3Elem>::push(
		L, 
		new E3Elem(
			MV_TAG,
			new mv(
				add(mv(*((bivector *)elem1->elem)), mv(*((vector *)elem2->elem)))
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
				add(*((bivector *)elem1->elem), *((bivector *)elem2->elem))
			)
		)
	);
break;

case TRIVECTOR_TAG:
	Glue<E3Elem>::push(
		L, 
		new E3Elem(
			MV_TAG,
			new mv(
				add(mv(*((bivector *)elem1->elem)), mv(*((trivector *)elem2->elem)))
			)
		)
	);
break;

case ROTOR_TAG:
	Glue<E3Elem>::push(
		L, 
		new E3Elem(
			MV_TAG,
			new mv(
				add(mv(*((bivector *)elem1->elem)), mv(*((rotor *)elem2->elem)))
			)
		)
	);
break;

case ODDVERSOR_TAG:
	Glue<E3Elem>::push(
		L, 
		new E3Elem(
			MV_TAG,
			new mv(
				add(mv(*((bivector *)elem1->elem)), mv(*((oddVersor *)elem2->elem)))
			)
		)
	);
break;

case DOUBLE_TAG:
	Glue<E3Elem>::push(
		L, 
		new E3Elem(
			MV_TAG,
			new mv(
				add(mv(*((bivector *)elem1->elem)), *((double *)elem2))
			)
		)
	);
break;
}
break;

case TRIVECTOR_TAG:
switch(tag2) {
case MV_TAG:
	Glue<E3Elem>::push(
		L, 
		new E3Elem(
			MV_TAG,
			new mv(
				add(mv(*((trivector *)elem1->elem)), mv(*((mv *)elem2->elem)))
			)
		)
	);
break;

case E1_TAG:
	Glue<E3Elem>::push(
		L, 
		new E3Elem(
			MV_TAG,
			new mv(
				add(mv(*((trivector *)elem1->elem)), mv(*((e1_t *)elem2->elem)))
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
				add(mv(*((trivector *)elem1->elem)), mv(*((e2_t *)elem2->elem)))
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
				add(mv(*((trivector *)elem1->elem)), mv(*((e3_t *)elem2->elem)))
			)
		)
	);
break;

case I3_TAG:
	Glue<E3Elem>::push(
		L, 
		new E3Elem(
			MV_TAG,
			new mv(
				add(mv(*((trivector *)elem1->elem)), mv(*((I3_t *)elem2->elem)))
			)
		)
	);
break;

case VECTOR_TAG:
	Glue<E3Elem>::push(
		L, 
		new E3Elem(
			MV_TAG,
			new mv(
				add(mv(*((trivector *)elem1->elem)), mv(*((vector *)elem2->elem)))
			)
		)
	);
break;

case BIVECTOR_TAG:
	Glue<E3Elem>::push(
		L, 
		new E3Elem(
			MV_TAG,
			new mv(
				add(mv(*((trivector *)elem1->elem)), mv(*((bivector *)elem2->elem)))
			)
		)
	);
break;

case TRIVECTOR_TAG:
	Glue<E3Elem>::push(
		L, 
		new E3Elem(
			MV_TAG,
			new mv(
				add(mv(*((trivector *)elem1->elem)), mv(*((trivector *)elem2->elem)))
			)
		)
	);
break;

case ROTOR_TAG:
	Glue<E3Elem>::push(
		L, 
		new E3Elem(
			MV_TAG,
			new mv(
				add(mv(*((trivector *)elem1->elem)), mv(*((rotor *)elem2->elem)))
			)
		)
	);
break;

case ODDVERSOR_TAG:
	Glue<E3Elem>::push(
		L, 
		new E3Elem(
			MV_TAG,
			new mv(
				add(mv(*((trivector *)elem1->elem)), mv(*((oddVersor *)elem2->elem)))
			)
		)
	);
break;

case DOUBLE_TAG:
	Glue<E3Elem>::push(
		L, 
		new E3Elem(
			MV_TAG,
			new mv(
				add(mv(*((trivector *)elem1->elem)), *((double *)elem2))
			)
		)
	);
break;
}
break;

case ROTOR_TAG:
switch(tag2) {
case MV_TAG:
	Glue<E3Elem>::push(
		L, 
		new E3Elem(
			MV_TAG,
			new mv(
				add(mv(*((rotor *)elem1->elem)), mv(*((mv *)elem2->elem)))
			)
		)
	);
break;

case E1_TAG:
	Glue<E3Elem>::push(
		L, 
		new E3Elem(
			MV_TAG,
			new mv(
				add(mv(*((rotor *)elem1->elem)), mv(*((e1_t *)elem2->elem)))
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
				add(mv(*((rotor *)elem1->elem)), mv(*((e2_t *)elem2->elem)))
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
				add(mv(*((rotor *)elem1->elem)), mv(*((e3_t *)elem2->elem)))
			)
		)
	);
break;

case I3_TAG:
	Glue<E3Elem>::push(
		L, 
		new E3Elem(
			MV_TAG,
			new mv(
				add(mv(*((rotor *)elem1->elem)), mv(*((I3_t *)elem2->elem)))
			)
		)
	);
break;

case VECTOR_TAG:
	Glue<E3Elem>::push(
		L, 
		new E3Elem(
			MV_TAG,
			new mv(
				add(mv(*((rotor *)elem1->elem)), mv(*((vector *)elem2->elem)))
			)
		)
	);
break;

case BIVECTOR_TAG:
	Glue<E3Elem>::push(
		L, 
		new E3Elem(
			ROTOR_TAG,
			new rotor(
				add(*((rotor *)elem1->elem), *((bivector *)elem2->elem))
			)
		)
	);
break;

case TRIVECTOR_TAG:
	Glue<E3Elem>::push(
		L, 
		new E3Elem(
			MV_TAG,
			new mv(
				add(mv(*((rotor *)elem1->elem)), mv(*((trivector *)elem2->elem)))
			)
		)
	);
break;

case ROTOR_TAG:
	Glue<E3Elem>::push(
		L, 
		new E3Elem(
			MV_TAG,
			new mv(
				add(mv(*((rotor *)elem1->elem)), mv(*((rotor *)elem2->elem)))
			)
		)
	);
break;

case ODDVERSOR_TAG:
	Glue<E3Elem>::push(
		L, 
		new E3Elem(
			MV_TAG,
			new mv(
				add(mv(*((rotor *)elem1->elem)), mv(*((oddVersor *)elem2->elem)))
			)
		)
	);
break;

case DOUBLE_TAG:
	Glue<E3Elem>::push(
		L, 
		new E3Elem(
			MV_TAG,
			new mv(
				add(mv(*((rotor *)elem1->elem)), *((double *)elem2))
			)
		)
	);
break;
}
break;

case ODDVERSOR_TAG:
switch(tag2) {
case MV_TAG:
	Glue<E3Elem>::push(
		L, 
		new E3Elem(
			MV_TAG,
			new mv(
				add(mv(*((oddVersor *)elem1->elem)), mv(*((mv *)elem2->elem)))
			)
		)
	);
break;

case E1_TAG:
	Glue<E3Elem>::push(
		L, 
		new E3Elem(
			MV_TAG,
			new mv(
				add(mv(*((oddVersor *)elem1->elem)), mv(*((e1_t *)elem2->elem)))
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
				add(mv(*((oddVersor *)elem1->elem)), mv(*((e2_t *)elem2->elem)))
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
				add(mv(*((oddVersor *)elem1->elem)), mv(*((e3_t *)elem2->elem)))
			)
		)
	);
break;

case I3_TAG:
	Glue<E3Elem>::push(
		L, 
		new E3Elem(
			MV_TAG,
			new mv(
				add(mv(*((oddVersor *)elem1->elem)), mv(*((I3_t *)elem2->elem)))
			)
		)
	);
break;

case VECTOR_TAG:
	Glue<E3Elem>::push(
		L, 
		new E3Elem(
			MV_TAG,
			new mv(
				add(mv(*((oddVersor *)elem1->elem)), mv(*((vector *)elem2->elem)))
			)
		)
	);
break;

case BIVECTOR_TAG:
	Glue<E3Elem>::push(
		L, 
		new E3Elem(
			MV_TAG,
			new mv(
				add(mv(*((oddVersor *)elem1->elem)), mv(*((bivector *)elem2->elem)))
			)
		)
	);
break;

case TRIVECTOR_TAG:
	Glue<E3Elem>::push(
		L, 
		new E3Elem(
			MV_TAG,
			new mv(
				add(mv(*((oddVersor *)elem1->elem)), mv(*((trivector *)elem2->elem)))
			)
		)
	);
break;

case ROTOR_TAG:
	Glue<E3Elem>::push(
		L, 
		new E3Elem(
			MV_TAG,
			new mv(
				add(mv(*((oddVersor *)elem1->elem)), mv(*((rotor *)elem2->elem)))
			)
		)
	);
break;

case ODDVERSOR_TAG:
	Glue<E3Elem>::push(
		L, 
		new E3Elem(
			MV_TAG,
			new mv(
				add(mv(*((oddVersor *)elem1->elem)), mv(*((oddVersor *)elem2->elem)))
			)
		)
	);
break;

case DOUBLE_TAG:
	Glue<E3Elem>::push(
		L, 
		new E3Elem(
			MV_TAG,
			new mv(
				add(mv(*((oddVersor *)elem1->elem)), *((double *)elem2))
			)
		)
	);
break;
}
break;

case DOUBLE_TAG:
switch(tag2) {
case MV_TAG:
	Glue<E3Elem>::push(
		L, 
		new E3Elem(
			MV_TAG,
			new mv(
				add(*((double *)elem1), mv(*((mv *)elem2->elem)))
			)
		)
	);
break;

case E1_TAG:
	Glue<E3Elem>::push(
		L, 
		new E3Elem(
			MV_TAG,
			new mv(
				add(*((double *)elem1), mv(*((e1_t *)elem2->elem)))
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
				add(*((double *)elem1), mv(*((e2_t *)elem2->elem)))
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
				add(*((double *)elem1), mv(*((e3_t *)elem2->elem)))
			)
		)
	);
break;

case I3_TAG:
	Glue<E3Elem>::push(
		L, 
		new E3Elem(
			MV_TAG,
			new mv(
				add(*((double *)elem1), mv(*((I3_t *)elem2->elem)))
			)
		)
	);
break;

case VECTOR_TAG:
	Glue<E3Elem>::push(
		L, 
		new E3Elem(
			MV_TAG,
			new mv(
				add(*((double *)elem1), mv(*((vector *)elem2->elem)))
			)
		)
	);
break;

case BIVECTOR_TAG:
	Glue<E3Elem>::push(
		L, 
		new E3Elem(
			MV_TAG,
			new mv(
				add(*((double *)elem1), mv(*((bivector *)elem2->elem)))
			)
		)
	);
break;

case TRIVECTOR_TAG:
	Glue<E3Elem>::push(
		L, 
		new E3Elem(
			MV_TAG,
			new mv(
				add(*((double *)elem1), mv(*((trivector *)elem2->elem)))
			)
		)
	);
break;

case ROTOR_TAG:
	Glue<E3Elem>::push(
		L, 
		new E3Elem(
			MV_TAG,
			new mv(
				add(*((double *)elem1), mv(*((rotor *)elem2->elem)))
			)
		)
	);
break;

case ODDVERSOR_TAG:
	Glue<E3Elem>::push(
		L, 
		new E3Elem(
			MV_TAG,
			new mv(
				add(*((double *)elem1), mv(*((oddVersor *)elem2->elem)))
			)
		)
	);
break;

case DOUBLE_TAG:
	Glue<E3Elem>::push(
		L, 
		new E3Elem(
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


int e3space_subtract(lua_State *L) {
	double d1, d2;
	E3Tag tag1;
	E3Tag tag2;
	E3Elem *elem1 = Glue<E3Elem>::to(L, 1);
	E3Elem *elem2 = Glue<E3Elem>::to(L, 2);
	
	
	if(!elem1) {
		if(lua::is<double>(L, 1)) {
			d1 = lua::to<double>(L, 1);
			elem1 = (E3Elem *)(&d1);
			tag1 = DOUBLE_TAG;
		}
		else {
			luaL_error(L, "e3.subtract: invalid arguments");
		}
	}
	else {
		tag1 = elem1->tag;
	}
	
	if(!elem2) {
		if(lua::is<double>(L, 2)) {
			d2 = lua::to<double>(L, 2);
			elem2 = (E3Elem *)(&d2);
			tag2 = DOUBLE_TAG;
		}
		else {
			luaL_error(L, "e3.subtract: invalid arguments");
		}
	}
	else {
		tag2 = elem2->tag;
	}
	
switch(tag1) {
case MV_TAG:
switch(tag2) {
case MV_TAG:
	Glue<E3Elem>::push(
		L, 
		new E3Elem(
			MV_TAG,
			new mv(
				subtract(*((mv *)elem1->elem), *((mv *)elem2->elem))
			)
		)
	);
break;

case E1_TAG:
	Glue<E3Elem>::push(
		L, 
		new E3Elem(
			MV_TAG,
			new mv(
				subtract(mv(*((mv *)elem1->elem)), mv(*((e1_t *)elem2->elem)))
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
				subtract(mv(*((mv *)elem1->elem)), mv(*((e2_t *)elem2->elem)))
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
				subtract(mv(*((mv *)elem1->elem)), mv(*((e3_t *)elem2->elem)))
			)
		)
	);
break;

case I3_TAG:
	Glue<E3Elem>::push(
		L, 
		new E3Elem(
			MV_TAG,
			new mv(
				subtract(mv(*((mv *)elem1->elem)), mv(*((I3_t *)elem2->elem)))
			)
		)
	);
break;

case VECTOR_TAG:
	Glue<E3Elem>::push(
		L, 
		new E3Elem(
			MV_TAG,
			new mv(
				subtract(mv(*((mv *)elem1->elem)), mv(*((vector *)elem2->elem)))
			)
		)
	);
break;

case BIVECTOR_TAG:
	Glue<E3Elem>::push(
		L, 
		new E3Elem(
			MV_TAG,
			new mv(
				subtract(mv(*((mv *)elem1->elem)), mv(*((bivector *)elem2->elem)))
			)
		)
	);
break;

case TRIVECTOR_TAG:
	Glue<E3Elem>::push(
		L, 
		new E3Elem(
			MV_TAG,
			new mv(
				subtract(mv(*((mv *)elem1->elem)), mv(*((trivector *)elem2->elem)))
			)
		)
	);
break;

case ROTOR_TAG:
	Glue<E3Elem>::push(
		L, 
		new E3Elem(
			MV_TAG,
			new mv(
				subtract(mv(*((mv *)elem1->elem)), mv(*((rotor *)elem2->elem)))
			)
		)
	);
break;

case ODDVERSOR_TAG:
	Glue<E3Elem>::push(
		L, 
		new E3Elem(
			MV_TAG,
			new mv(
				subtract(mv(*((mv *)elem1->elem)), mv(*((oddVersor *)elem2->elem)))
			)
		)
	);
break;

case DOUBLE_TAG:
	Glue<E3Elem>::push(
		L, 
		new E3Elem(
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
	Glue<E3Elem>::push(
		L, 
		new E3Elem(
			MV_TAG,
			new mv(
				subtract(mv(*((e1_t *)elem1->elem)), mv(*((mv *)elem2->elem)))
			)
		)
	);
break;

case E1_TAG:
	Glue<E3Elem>::push(
		L, 
		new E3Elem(
			MV_TAG,
			new mv(
				subtract(mv(*((e1_t *)elem1->elem)), mv(*((e1_t *)elem2->elem)))
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
				subtract(mv(*((e1_t *)elem1->elem)), mv(*((e2_t *)elem2->elem)))
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
				subtract(mv(*((e1_t *)elem1->elem)), mv(*((e3_t *)elem2->elem)))
			)
		)
	);
break;

case I3_TAG:
	Glue<E3Elem>::push(
		L, 
		new E3Elem(
			MV_TAG,
			new mv(
				subtract(mv(*((e1_t *)elem1->elem)), mv(*((I3_t *)elem2->elem)))
			)
		)
	);
break;

case VECTOR_TAG:
	Glue<E3Elem>::push(
		L, 
		new E3Elem(
			MV_TAG,
			new mv(
				subtract(mv(*((e1_t *)elem1->elem)), mv(*((vector *)elem2->elem)))
			)
		)
	);
break;

case BIVECTOR_TAG:
	Glue<E3Elem>::push(
		L, 
		new E3Elem(
			MV_TAG,
			new mv(
				subtract(mv(*((e1_t *)elem1->elem)), mv(*((bivector *)elem2->elem)))
			)
		)
	);
break;

case TRIVECTOR_TAG:
	Glue<E3Elem>::push(
		L, 
		new E3Elem(
			MV_TAG,
			new mv(
				subtract(mv(*((e1_t *)elem1->elem)), mv(*((trivector *)elem2->elem)))
			)
		)
	);
break;

case ROTOR_TAG:
	Glue<E3Elem>::push(
		L, 
		new E3Elem(
			MV_TAG,
			new mv(
				subtract(mv(*((e1_t *)elem1->elem)), mv(*((rotor *)elem2->elem)))
			)
		)
	);
break;

case ODDVERSOR_TAG:
	Glue<E3Elem>::push(
		L, 
		new E3Elem(
			MV_TAG,
			new mv(
				subtract(mv(*((e1_t *)elem1->elem)), mv(*((oddVersor *)elem2->elem)))
			)
		)
	);
break;

case DOUBLE_TAG:
	Glue<E3Elem>::push(
		L, 
		new E3Elem(
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
	Glue<E3Elem>::push(
		L, 
		new E3Elem(
			MV_TAG,
			new mv(
				subtract(mv(*((e2_t *)elem1->elem)), mv(*((mv *)elem2->elem)))
			)
		)
	);
break;

case E1_TAG:
	Glue<E3Elem>::push(
		L, 
		new E3Elem(
			MV_TAG,
			new mv(
				subtract(mv(*((e2_t *)elem1->elem)), mv(*((e1_t *)elem2->elem)))
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
				subtract(mv(*((e2_t *)elem1->elem)), mv(*((e2_t *)elem2->elem)))
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
				subtract(mv(*((e2_t *)elem1->elem)), mv(*((e3_t *)elem2->elem)))
			)
		)
	);
break;

case I3_TAG:
	Glue<E3Elem>::push(
		L, 
		new E3Elem(
			MV_TAG,
			new mv(
				subtract(mv(*((e2_t *)elem1->elem)), mv(*((I3_t *)elem2->elem)))
			)
		)
	);
break;

case VECTOR_TAG:
	Glue<E3Elem>::push(
		L, 
		new E3Elem(
			MV_TAG,
			new mv(
				subtract(mv(*((e2_t *)elem1->elem)), mv(*((vector *)elem2->elem)))
			)
		)
	);
break;

case BIVECTOR_TAG:
	Glue<E3Elem>::push(
		L, 
		new E3Elem(
			MV_TAG,
			new mv(
				subtract(mv(*((e2_t *)elem1->elem)), mv(*((bivector *)elem2->elem)))
			)
		)
	);
break;

case TRIVECTOR_TAG:
	Glue<E3Elem>::push(
		L, 
		new E3Elem(
			MV_TAG,
			new mv(
				subtract(mv(*((e2_t *)elem1->elem)), mv(*((trivector *)elem2->elem)))
			)
		)
	);
break;

case ROTOR_TAG:
	Glue<E3Elem>::push(
		L, 
		new E3Elem(
			MV_TAG,
			new mv(
				subtract(mv(*((e2_t *)elem1->elem)), mv(*((rotor *)elem2->elem)))
			)
		)
	);
break;

case ODDVERSOR_TAG:
	Glue<E3Elem>::push(
		L, 
		new E3Elem(
			MV_TAG,
			new mv(
				subtract(mv(*((e2_t *)elem1->elem)), mv(*((oddVersor *)elem2->elem)))
			)
		)
	);
break;

case DOUBLE_TAG:
	Glue<E3Elem>::push(
		L, 
		new E3Elem(
			MV_TAG,
			new mv(
				subtract(mv(*((e2_t *)elem1->elem)), *((double *)elem2))
			)
		)
	);
break;
}
break;

case E3_TAG:
switch(tag2) {
case MV_TAG:
	Glue<E3Elem>::push(
		L, 
		new E3Elem(
			MV_TAG,
			new mv(
				subtract(mv(*((e3_t *)elem1->elem)), mv(*((mv *)elem2->elem)))
			)
		)
	);
break;

case E1_TAG:
	Glue<E3Elem>::push(
		L, 
		new E3Elem(
			MV_TAG,
			new mv(
				subtract(mv(*((e3_t *)elem1->elem)), mv(*((e1_t *)elem2->elem)))
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
				subtract(mv(*((e3_t *)elem1->elem)), mv(*((e2_t *)elem2->elem)))
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
				subtract(mv(*((e3_t *)elem1->elem)), mv(*((e3_t *)elem2->elem)))
			)
		)
	);
break;

case I3_TAG:
	Glue<E3Elem>::push(
		L, 
		new E3Elem(
			MV_TAG,
			new mv(
				subtract(mv(*((e3_t *)elem1->elem)), mv(*((I3_t *)elem2->elem)))
			)
		)
	);
break;

case VECTOR_TAG:
	Glue<E3Elem>::push(
		L, 
		new E3Elem(
			MV_TAG,
			new mv(
				subtract(mv(*((e3_t *)elem1->elem)), mv(*((vector *)elem2->elem)))
			)
		)
	);
break;

case BIVECTOR_TAG:
	Glue<E3Elem>::push(
		L, 
		new E3Elem(
			MV_TAG,
			new mv(
				subtract(mv(*((e3_t *)elem1->elem)), mv(*((bivector *)elem2->elem)))
			)
		)
	);
break;

case TRIVECTOR_TAG:
	Glue<E3Elem>::push(
		L, 
		new E3Elem(
			MV_TAG,
			new mv(
				subtract(mv(*((e3_t *)elem1->elem)), mv(*((trivector *)elem2->elem)))
			)
		)
	);
break;

case ROTOR_TAG:
	Glue<E3Elem>::push(
		L, 
		new E3Elem(
			MV_TAG,
			new mv(
				subtract(mv(*((e3_t *)elem1->elem)), mv(*((rotor *)elem2->elem)))
			)
		)
	);
break;

case ODDVERSOR_TAG:
	Glue<E3Elem>::push(
		L, 
		new E3Elem(
			MV_TAG,
			new mv(
				subtract(mv(*((e3_t *)elem1->elem)), mv(*((oddVersor *)elem2->elem)))
			)
		)
	);
break;

case DOUBLE_TAG:
	Glue<E3Elem>::push(
		L, 
		new E3Elem(
			MV_TAG,
			new mv(
				subtract(mv(*((e3_t *)elem1->elem)), *((double *)elem2))
			)
		)
	);
break;
}
break;

case I3_TAG:
switch(tag2) {
case MV_TAG:
	Glue<E3Elem>::push(
		L, 
		new E3Elem(
			MV_TAG,
			new mv(
				subtract(mv(*((I3_t *)elem1->elem)), mv(*((mv *)elem2->elem)))
			)
		)
	);
break;

case E1_TAG:
	Glue<E3Elem>::push(
		L, 
		new E3Elem(
			MV_TAG,
			new mv(
				subtract(mv(*((I3_t *)elem1->elem)), mv(*((e1_t *)elem2->elem)))
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
				subtract(mv(*((I3_t *)elem1->elem)), mv(*((e2_t *)elem2->elem)))
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
				subtract(mv(*((I3_t *)elem1->elem)), mv(*((e3_t *)elem2->elem)))
			)
		)
	);
break;

case I3_TAG:
	Glue<E3Elem>::push(
		L, 
		new E3Elem(
			MV_TAG,
			new mv(
				subtract(mv(*((I3_t *)elem1->elem)), mv(*((I3_t *)elem2->elem)))
			)
		)
	);
break;

case VECTOR_TAG:
	Glue<E3Elem>::push(
		L, 
		new E3Elem(
			MV_TAG,
			new mv(
				subtract(mv(*((I3_t *)elem1->elem)), mv(*((vector *)elem2->elem)))
			)
		)
	);
break;

case BIVECTOR_TAG:
	Glue<E3Elem>::push(
		L, 
		new E3Elem(
			MV_TAG,
			new mv(
				subtract(mv(*((I3_t *)elem1->elem)), mv(*((bivector *)elem2->elem)))
			)
		)
	);
break;

case TRIVECTOR_TAG:
	Glue<E3Elem>::push(
		L, 
		new E3Elem(
			MV_TAG,
			new mv(
				subtract(mv(*((I3_t *)elem1->elem)), mv(*((trivector *)elem2->elem)))
			)
		)
	);
break;

case ROTOR_TAG:
	Glue<E3Elem>::push(
		L, 
		new E3Elem(
			MV_TAG,
			new mv(
				subtract(mv(*((I3_t *)elem1->elem)), mv(*((rotor *)elem2->elem)))
			)
		)
	);
break;

case ODDVERSOR_TAG:
	Glue<E3Elem>::push(
		L, 
		new E3Elem(
			MV_TAG,
			new mv(
				subtract(mv(*((I3_t *)elem1->elem)), mv(*((oddVersor *)elem2->elem)))
			)
		)
	);
break;

case DOUBLE_TAG:
	Glue<E3Elem>::push(
		L, 
		new E3Elem(
			MV_TAG,
			new mv(
				subtract(mv(*((I3_t *)elem1->elem)), *((double *)elem2))
			)
		)
	);
break;
}
break;

case VECTOR_TAG:
switch(tag2) {
case MV_TAG:
	Glue<E3Elem>::push(
		L, 
		new E3Elem(
			MV_TAG,
			new mv(
				subtract(mv(*((vector *)elem1->elem)), mv(*((mv *)elem2->elem)))
			)
		)
	);
break;

case E1_TAG:
	Glue<E3Elem>::push(
		L, 
		new E3Elem(
			MV_TAG,
			new mv(
				subtract(mv(*((vector *)elem1->elem)), mv(*((e1_t *)elem2->elem)))
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
				subtract(mv(*((vector *)elem1->elem)), mv(*((e2_t *)elem2->elem)))
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
				subtract(mv(*((vector *)elem1->elem)), mv(*((e3_t *)elem2->elem)))
			)
		)
	);
break;

case I3_TAG:
	Glue<E3Elem>::push(
		L, 
		new E3Elem(
			MV_TAG,
			new mv(
				subtract(mv(*((vector *)elem1->elem)), mv(*((I3_t *)elem2->elem)))
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
				subtract(*((vector *)elem1->elem), *((vector *)elem2->elem))
			)
		)
	);
break;

case BIVECTOR_TAG:
	Glue<E3Elem>::push(
		L, 
		new E3Elem(
			MV_TAG,
			new mv(
				subtract(mv(*((vector *)elem1->elem)), mv(*((bivector *)elem2->elem)))
			)
		)
	);
break;

case TRIVECTOR_TAG:
	Glue<E3Elem>::push(
		L, 
		new E3Elem(
			ODDVERSOR_TAG,
			new oddVersor(
				subtract(*((vector *)elem1->elem), *((trivector *)elem2->elem))
			)
		)
	);
break;

case ROTOR_TAG:
	Glue<E3Elem>::push(
		L, 
		new E3Elem(
			MV_TAG,
			new mv(
				subtract(mv(*((vector *)elem1->elem)), mv(*((rotor *)elem2->elem)))
			)
		)
	);
break;

case ODDVERSOR_TAG:
	Glue<E3Elem>::push(
		L, 
		new E3Elem(
			MV_TAG,
			new mv(
				subtract(mv(*((vector *)elem1->elem)), mv(*((oddVersor *)elem2->elem)))
			)
		)
	);
break;

case DOUBLE_TAG:
	Glue<E3Elem>::push(
		L, 
		new E3Elem(
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
	Glue<E3Elem>::push(
		L, 
		new E3Elem(
			MV_TAG,
			new mv(
				subtract(mv(*((bivector *)elem1->elem)), mv(*((mv *)elem2->elem)))
			)
		)
	);
break;

case E1_TAG:
	Glue<E3Elem>::push(
		L, 
		new E3Elem(
			MV_TAG,
			new mv(
				subtract(mv(*((bivector *)elem1->elem)), mv(*((e1_t *)elem2->elem)))
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
				subtract(mv(*((bivector *)elem1->elem)), mv(*((e2_t *)elem2->elem)))
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
				subtract(mv(*((bivector *)elem1->elem)), mv(*((e3_t *)elem2->elem)))
			)
		)
	);
break;

case I3_TAG:
	Glue<E3Elem>::push(
		L, 
		new E3Elem(
			MV_TAG,
			new mv(
				subtract(mv(*((bivector *)elem1->elem)), mv(*((I3_t *)elem2->elem)))
			)
		)
	);
break;

case VECTOR_TAG:
	Glue<E3Elem>::push(
		L, 
		new E3Elem(
			MV_TAG,
			new mv(
				subtract(mv(*((bivector *)elem1->elem)), mv(*((vector *)elem2->elem)))
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
				subtract(*((bivector *)elem1->elem), *((bivector *)elem2->elem))
			)
		)
	);
break;

case TRIVECTOR_TAG:
	Glue<E3Elem>::push(
		L, 
		new E3Elem(
			MV_TAG,
			new mv(
				subtract(mv(*((bivector *)elem1->elem)), mv(*((trivector *)elem2->elem)))
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
				subtract(*((bivector *)elem1->elem), *((rotor *)elem2->elem))
			)
		)
	);
break;

case ODDVERSOR_TAG:
	Glue<E3Elem>::push(
		L, 
		new E3Elem(
			MV_TAG,
			new mv(
				subtract(mv(*((bivector *)elem1->elem)), mv(*((oddVersor *)elem2->elem)))
			)
		)
	);
break;

case DOUBLE_TAG:
	Glue<E3Elem>::push(
		L, 
		new E3Elem(
			MV_TAG,
			new mv(
				subtract(mv(*((bivector *)elem1->elem)), *((double *)elem2))
			)
		)
	);
break;
}
break;

case TRIVECTOR_TAG:
switch(tag2) {
case MV_TAG:
	Glue<E3Elem>::push(
		L, 
		new E3Elem(
			MV_TAG,
			new mv(
				subtract(mv(*((trivector *)elem1->elem)), mv(*((mv *)elem2->elem)))
			)
		)
	);
break;

case E1_TAG:
	Glue<E3Elem>::push(
		L, 
		new E3Elem(
			MV_TAG,
			new mv(
				subtract(mv(*((trivector *)elem1->elem)), mv(*((e1_t *)elem2->elem)))
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
				subtract(mv(*((trivector *)elem1->elem)), mv(*((e2_t *)elem2->elem)))
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
				subtract(mv(*((trivector *)elem1->elem)), mv(*((e3_t *)elem2->elem)))
			)
		)
	);
break;

case I3_TAG:
	Glue<E3Elem>::push(
		L, 
		new E3Elem(
			MV_TAG,
			new mv(
				subtract(mv(*((trivector *)elem1->elem)), mv(*((I3_t *)elem2->elem)))
			)
		)
	);
break;

case VECTOR_TAG:
	Glue<E3Elem>::push(
		L, 
		new E3Elem(
			MV_TAG,
			new mv(
				subtract(mv(*((trivector *)elem1->elem)), mv(*((vector *)elem2->elem)))
			)
		)
	);
break;

case BIVECTOR_TAG:
	Glue<E3Elem>::push(
		L, 
		new E3Elem(
			MV_TAG,
			new mv(
				subtract(mv(*((trivector *)elem1->elem)), mv(*((bivector *)elem2->elem)))
			)
		)
	);
break;

case TRIVECTOR_TAG:
	Glue<E3Elem>::push(
		L, 
		new E3Elem(
			MV_TAG,
			new mv(
				subtract(mv(*((trivector *)elem1->elem)), mv(*((trivector *)elem2->elem)))
			)
		)
	);
break;

case ROTOR_TAG:
	Glue<E3Elem>::push(
		L, 
		new E3Elem(
			MV_TAG,
			new mv(
				subtract(mv(*((trivector *)elem1->elem)), mv(*((rotor *)elem2->elem)))
			)
		)
	);
break;

case ODDVERSOR_TAG:
	Glue<E3Elem>::push(
		L, 
		new E3Elem(
			MV_TAG,
			new mv(
				subtract(mv(*((trivector *)elem1->elem)), mv(*((oddVersor *)elem2->elem)))
			)
		)
	);
break;

case DOUBLE_TAG:
	Glue<E3Elem>::push(
		L, 
		new E3Elem(
			MV_TAG,
			new mv(
				subtract(mv(*((trivector *)elem1->elem)), *((double *)elem2))
			)
		)
	);
break;
}
break;

case ROTOR_TAG:
switch(tag2) {
case MV_TAG:
	Glue<E3Elem>::push(
		L, 
		new E3Elem(
			MV_TAG,
			new mv(
				subtract(mv(*((rotor *)elem1->elem)), mv(*((mv *)elem2->elem)))
			)
		)
	);
break;

case E1_TAG:
	Glue<E3Elem>::push(
		L, 
		new E3Elem(
			MV_TAG,
			new mv(
				subtract(mv(*((rotor *)elem1->elem)), mv(*((e1_t *)elem2->elem)))
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
				subtract(mv(*((rotor *)elem1->elem)), mv(*((e2_t *)elem2->elem)))
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
				subtract(mv(*((rotor *)elem1->elem)), mv(*((e3_t *)elem2->elem)))
			)
		)
	);
break;

case I3_TAG:
	Glue<E3Elem>::push(
		L, 
		new E3Elem(
			MV_TAG,
			new mv(
				subtract(mv(*((rotor *)elem1->elem)), mv(*((I3_t *)elem2->elem)))
			)
		)
	);
break;

case VECTOR_TAG:
	Glue<E3Elem>::push(
		L, 
		new E3Elem(
			MV_TAG,
			new mv(
				subtract(mv(*((rotor *)elem1->elem)), mv(*((vector *)elem2->elem)))
			)
		)
	);
break;

case BIVECTOR_TAG:
	Glue<E3Elem>::push(
		L, 
		new E3Elem(
			MV_TAG,
			new mv(
				subtract(mv(*((rotor *)elem1->elem)), mv(*((bivector *)elem2->elem)))
			)
		)
	);
break;

case TRIVECTOR_TAG:
	Glue<E3Elem>::push(
		L, 
		new E3Elem(
			MV_TAG,
			new mv(
				subtract(mv(*((rotor *)elem1->elem)), mv(*((trivector *)elem2->elem)))
			)
		)
	);
break;

case ROTOR_TAG:
	Glue<E3Elem>::push(
		L, 
		new E3Elem(
			MV_TAG,
			new mv(
				subtract(mv(*((rotor *)elem1->elem)), mv(*((rotor *)elem2->elem)))
			)
		)
	);
break;

case ODDVERSOR_TAG:
	Glue<E3Elem>::push(
		L, 
		new E3Elem(
			MV_TAG,
			new mv(
				subtract(mv(*((rotor *)elem1->elem)), mv(*((oddVersor *)elem2->elem)))
			)
		)
	);
break;

case DOUBLE_TAG:
	Glue<E3Elem>::push(
		L, 
		new E3Elem(
			MV_TAG,
			new mv(
				subtract(mv(*((rotor *)elem1->elem)), *((double *)elem2))
			)
		)
	);
break;
}
break;

case ODDVERSOR_TAG:
switch(tag2) {
case MV_TAG:
	Glue<E3Elem>::push(
		L, 
		new E3Elem(
			MV_TAG,
			new mv(
				subtract(mv(*((oddVersor *)elem1->elem)), mv(*((mv *)elem2->elem)))
			)
		)
	);
break;

case E1_TAG:
	Glue<E3Elem>::push(
		L, 
		new E3Elem(
			MV_TAG,
			new mv(
				subtract(mv(*((oddVersor *)elem1->elem)), mv(*((e1_t *)elem2->elem)))
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
				subtract(mv(*((oddVersor *)elem1->elem)), mv(*((e2_t *)elem2->elem)))
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
				subtract(mv(*((oddVersor *)elem1->elem)), mv(*((e3_t *)elem2->elem)))
			)
		)
	);
break;

case I3_TAG:
	Glue<E3Elem>::push(
		L, 
		new E3Elem(
			MV_TAG,
			new mv(
				subtract(mv(*((oddVersor *)elem1->elem)), mv(*((I3_t *)elem2->elem)))
			)
		)
	);
break;

case VECTOR_TAG:
	Glue<E3Elem>::push(
		L, 
		new E3Elem(
			MV_TAG,
			new mv(
				subtract(mv(*((oddVersor *)elem1->elem)), mv(*((vector *)elem2->elem)))
			)
		)
	);
break;

case BIVECTOR_TAG:
	Glue<E3Elem>::push(
		L, 
		new E3Elem(
			MV_TAG,
			new mv(
				subtract(mv(*((oddVersor *)elem1->elem)), mv(*((bivector *)elem2->elem)))
			)
		)
	);
break;

case TRIVECTOR_TAG:
	Glue<E3Elem>::push(
		L, 
		new E3Elem(
			MV_TAG,
			new mv(
				subtract(mv(*((oddVersor *)elem1->elem)), mv(*((trivector *)elem2->elem)))
			)
		)
	);
break;

case ROTOR_TAG:
	Glue<E3Elem>::push(
		L, 
		new E3Elem(
			MV_TAG,
			new mv(
				subtract(mv(*((oddVersor *)elem1->elem)), mv(*((rotor *)elem2->elem)))
			)
		)
	);
break;

case ODDVERSOR_TAG:
	Glue<E3Elem>::push(
		L, 
		new E3Elem(
			MV_TAG,
			new mv(
				subtract(mv(*((oddVersor *)elem1->elem)), mv(*((oddVersor *)elem2->elem)))
			)
		)
	);
break;

case DOUBLE_TAG:
	Glue<E3Elem>::push(
		L, 
		new E3Elem(
			MV_TAG,
			new mv(
				subtract(mv(*((oddVersor *)elem1->elem)), *((double *)elem2))
			)
		)
	);
break;
}
break;

case DOUBLE_TAG:
switch(tag2) {
case MV_TAG:
	Glue<E3Elem>::push(
		L, 
		new E3Elem(
			MV_TAG,
			new mv(
				subtract(*((double *)elem1), mv(*((mv *)elem2->elem)))
			)
		)
	);
break;

case E1_TAG:
	Glue<E3Elem>::push(
		L, 
		new E3Elem(
			MV_TAG,
			new mv(
				subtract(*((double *)elem1), mv(*((e1_t *)elem2->elem)))
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
				subtract(*((double *)elem1), mv(*((e2_t *)elem2->elem)))
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
				subtract(*((double *)elem1), mv(*((e3_t *)elem2->elem)))
			)
		)
	);
break;

case I3_TAG:
	Glue<E3Elem>::push(
		L, 
		new E3Elem(
			MV_TAG,
			new mv(
				subtract(*((double *)elem1), mv(*((I3_t *)elem2->elem)))
			)
		)
	);
break;

case VECTOR_TAG:
	Glue<E3Elem>::push(
		L, 
		new E3Elem(
			MV_TAG,
			new mv(
				subtract(*((double *)elem1), mv(*((vector *)elem2->elem)))
			)
		)
	);
break;

case BIVECTOR_TAG:
	Glue<E3Elem>::push(
		L, 
		new E3Elem(
			MV_TAG,
			new mv(
				subtract(*((double *)elem1), mv(*((bivector *)elem2->elem)))
			)
		)
	);
break;

case TRIVECTOR_TAG:
	Glue<E3Elem>::push(
		L, 
		new E3Elem(
			MV_TAG,
			new mv(
				subtract(*((double *)elem1), mv(*((trivector *)elem2->elem)))
			)
		)
	);
break;

case ROTOR_TAG:
	Glue<E3Elem>::push(
		L, 
		new E3Elem(
			MV_TAG,
			new mv(
				subtract(*((double *)elem1), mv(*((rotor *)elem2->elem)))
			)
		)
	);
break;

case ODDVERSOR_TAG:
	Glue<E3Elem>::push(
		L, 
		new E3Elem(
			MV_TAG,
			new mv(
				subtract(*((double *)elem1), mv(*((oddVersor *)elem2->elem)))
			)
		)
	);
break;

case DOUBLE_TAG:
	Glue<E3Elem>::push(
		L, 
		new E3Elem(
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


int e3space_hip(lua_State *L) {
	double d1, d2;
	E3Tag tag1;
	E3Tag tag2;
	E3Elem *elem1 = Glue<E3Elem>::to(L, 1);
	E3Elem *elem2 = Glue<E3Elem>::to(L, 2);
	
	
	if(!elem1) {
		if(lua::is<double>(L, 1)) {
			d1 = lua::to<double>(L, 1);
			elem1 = (E3Elem *)(&d1);
			tag1 = DOUBLE_TAG;
		}
		else {
			luaL_error(L, "e3.hip: invalid arguments");
		}
	}
	else {
		tag1 = elem1->tag;
	}
	
	if(!elem2) {
		if(lua::is<double>(L, 2)) {
			d2 = lua::to<double>(L, 2);
			elem2 = (E3Elem *)(&d2);
			tag2 = DOUBLE_TAG;
		}
		else {
			luaL_error(L, "e3.hip: invalid arguments");
		}
	}
	else {
		tag2 = elem2->tag;
	}
	
switch(tag1) {
case MV_TAG:
switch(tag2) {
case MV_TAG:
	Glue<E3Elem>::push(
		L, 
		new E3Elem(
			MV_TAG,
			new mv(
				hip(*((mv *)elem1->elem), *((mv *)elem2->elem))
			)
		)
	);
break;

case E1_TAG:
	Glue<E3Elem>::push(
		L, 
		new E3Elem(
			MV_TAG,
			new mv(
				hip(mv(*((mv *)elem1->elem)), mv(*((e1_t *)elem2->elem)))
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
				hip(mv(*((mv *)elem1->elem)), mv(*((e2_t *)elem2->elem)))
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
				hip(mv(*((mv *)elem1->elem)), mv(*((e3_t *)elem2->elem)))
			)
		)
	);
break;

case I3_TAG:
	Glue<E3Elem>::push(
		L, 
		new E3Elem(
			MV_TAG,
			new mv(
				hip(mv(*((mv *)elem1->elem)), mv(*((I3_t *)elem2->elem)))
			)
		)
	);
break;

case VECTOR_TAG:
	Glue<E3Elem>::push(
		L, 
		new E3Elem(
			MV_TAG,
			new mv(
				hip(mv(*((mv *)elem1->elem)), mv(*((vector *)elem2->elem)))
			)
		)
	);
break;

case BIVECTOR_TAG:
	Glue<E3Elem>::push(
		L, 
		new E3Elem(
			MV_TAG,
			new mv(
				hip(mv(*((mv *)elem1->elem)), mv(*((bivector *)elem2->elem)))
			)
		)
	);
break;

case TRIVECTOR_TAG:
	Glue<E3Elem>::push(
		L, 
		new E3Elem(
			MV_TAG,
			new mv(
				hip(mv(*((mv *)elem1->elem)), mv(*((trivector *)elem2->elem)))
			)
		)
	);
break;

case ROTOR_TAG:
	Glue<E3Elem>::push(
		L, 
		new E3Elem(
			MV_TAG,
			new mv(
				hip(mv(*((mv *)elem1->elem)), mv(*((rotor *)elem2->elem)))
			)
		)
	);
break;

case ODDVERSOR_TAG:
	Glue<E3Elem>::push(
		L, 
		new E3Elem(
			MV_TAG,
			new mv(
				hip(mv(*((mv *)elem1->elem)), mv(*((oddVersor *)elem2->elem)))
			)
		)
	);
break;

case DOUBLE_TAG:
	Glue<E3Elem>::push(
		L, 
		new E3Elem(
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
	Glue<E3Elem>::push(
		L, 
		new E3Elem(
			MV_TAG,
			new mv(
				hip(mv(*((e1_t *)elem1->elem)), mv(*((mv *)elem2->elem)))
			)
		)
	);
break;

case E1_TAG:
	Glue<E3Elem>::push(
		L, 
		new E3Elem(
			MV_TAG,
			new mv(
				hip(mv(*((e1_t *)elem1->elem)), mv(*((e1_t *)elem2->elem)))
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
				hip(mv(*((e1_t *)elem1->elem)), mv(*((e2_t *)elem2->elem)))
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
				hip(mv(*((e1_t *)elem1->elem)), mv(*((e3_t *)elem2->elem)))
			)
		)
	);
break;

case I3_TAG:
	Glue<E3Elem>::push(
		L, 
		new E3Elem(
			MV_TAG,
			new mv(
				hip(mv(*((e1_t *)elem1->elem)), mv(*((I3_t *)elem2->elem)))
			)
		)
	);
break;

case VECTOR_TAG:
	Glue<E3Elem>::push(
		L, 
		new E3Elem(
			MV_TAG,
			new mv(
				hip(mv(*((e1_t *)elem1->elem)), mv(*((vector *)elem2->elem)))
			)
		)
	);
break;

case BIVECTOR_TAG:
	Glue<E3Elem>::push(
		L, 
		new E3Elem(
			MV_TAG,
			new mv(
				hip(mv(*((e1_t *)elem1->elem)), mv(*((bivector *)elem2->elem)))
			)
		)
	);
break;

case TRIVECTOR_TAG:
	Glue<E3Elem>::push(
		L, 
		new E3Elem(
			MV_TAG,
			new mv(
				hip(mv(*((e1_t *)elem1->elem)), mv(*((trivector *)elem2->elem)))
			)
		)
	);
break;

case ROTOR_TAG:
	Glue<E3Elem>::push(
		L, 
		new E3Elem(
			MV_TAG,
			new mv(
				hip(mv(*((e1_t *)elem1->elem)), mv(*((rotor *)elem2->elem)))
			)
		)
	);
break;

case ODDVERSOR_TAG:
	Glue<E3Elem>::push(
		L, 
		new E3Elem(
			MV_TAG,
			new mv(
				hip(mv(*((e1_t *)elem1->elem)), mv(*((oddVersor *)elem2->elem)))
			)
		)
	);
break;

case DOUBLE_TAG:
	Glue<E3Elem>::push(
		L, 
		new E3Elem(
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
	Glue<E3Elem>::push(
		L, 
		new E3Elem(
			MV_TAG,
			new mv(
				hip(mv(*((e2_t *)elem1->elem)), mv(*((mv *)elem2->elem)))
			)
		)
	);
break;

case E1_TAG:
	Glue<E3Elem>::push(
		L, 
		new E3Elem(
			MV_TAG,
			new mv(
				hip(mv(*((e2_t *)elem1->elem)), mv(*((e1_t *)elem2->elem)))
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
				hip(mv(*((e2_t *)elem1->elem)), mv(*((e2_t *)elem2->elem)))
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
				hip(mv(*((e2_t *)elem1->elem)), mv(*((e3_t *)elem2->elem)))
			)
		)
	);
break;

case I3_TAG:
	Glue<E3Elem>::push(
		L, 
		new E3Elem(
			BIVECTOR_TAG,
			new bivector(
				hip(*((e2_t *)elem1->elem), *((I3_t *)elem2->elem))
			)
		)
	);
break;

case VECTOR_TAG:
	Glue<E3Elem>::push(
		L, 
		new E3Elem(
			MV_TAG,
			new mv(
				hip(mv(*((e2_t *)elem1->elem)), mv(*((vector *)elem2->elem)))
			)
		)
	);
break;

case BIVECTOR_TAG:
	Glue<E3Elem>::push(
		L, 
		new E3Elem(
			MV_TAG,
			new mv(
				hip(mv(*((e2_t *)elem1->elem)), mv(*((bivector *)elem2->elem)))
			)
		)
	);
break;

case TRIVECTOR_TAG:
	Glue<E3Elem>::push(
		L, 
		new E3Elem(
			MV_TAG,
			new mv(
				hip(mv(*((e2_t *)elem1->elem)), mv(*((trivector *)elem2->elem)))
			)
		)
	);
break;

case ROTOR_TAG:
	Glue<E3Elem>::push(
		L, 
		new E3Elem(
			MV_TAG,
			new mv(
				hip(mv(*((e2_t *)elem1->elem)), mv(*((rotor *)elem2->elem)))
			)
		)
	);
break;

case ODDVERSOR_TAG:
	Glue<E3Elem>::push(
		L, 
		new E3Elem(
			MV_TAG,
			new mv(
				hip(mv(*((e2_t *)elem1->elem)), mv(*((oddVersor *)elem2->elem)))
			)
		)
	);
break;

case DOUBLE_TAG:
	Glue<E3Elem>::push(
		L, 
		new E3Elem(
			MV_TAG,
			new mv(
				hip(mv(*((e2_t *)elem1->elem)), *((double *)elem2))
			)
		)
	);
break;
}
break;

case E3_TAG:
switch(tag2) {
case MV_TAG:
	Glue<E3Elem>::push(
		L, 
		new E3Elem(
			MV_TAG,
			new mv(
				hip(mv(*((e3_t *)elem1->elem)), mv(*((mv *)elem2->elem)))
			)
		)
	);
break;

case E1_TAG:
	Glue<E3Elem>::push(
		L, 
		new E3Elem(
			MV_TAG,
			new mv(
				hip(mv(*((e3_t *)elem1->elem)), mv(*((e1_t *)elem2->elem)))
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
				hip(mv(*((e3_t *)elem1->elem)), mv(*((e2_t *)elem2->elem)))
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
				hip(mv(*((e3_t *)elem1->elem)), mv(*((e3_t *)elem2->elem)))
			)
		)
	);
break;

case I3_TAG:
	Glue<E3Elem>::push(
		L, 
		new E3Elem(
			MV_TAG,
			new mv(
				hip(mv(*((e3_t *)elem1->elem)), mv(*((I3_t *)elem2->elem)))
			)
		)
	);
break;

case VECTOR_TAG:
	Glue<E3Elem>::push(
		L, 
		new E3Elem(
			MV_TAG,
			new mv(
				hip(mv(*((e3_t *)elem1->elem)), mv(*((vector *)elem2->elem)))
			)
		)
	);
break;

case BIVECTOR_TAG:
	Glue<E3Elem>::push(
		L, 
		new E3Elem(
			MV_TAG,
			new mv(
				hip(mv(*((e3_t *)elem1->elem)), mv(*((bivector *)elem2->elem)))
			)
		)
	);
break;

case TRIVECTOR_TAG:
	Glue<E3Elem>::push(
		L, 
		new E3Elem(
			MV_TAG,
			new mv(
				hip(mv(*((e3_t *)elem1->elem)), mv(*((trivector *)elem2->elem)))
			)
		)
	);
break;

case ROTOR_TAG:
	Glue<E3Elem>::push(
		L, 
		new E3Elem(
			MV_TAG,
			new mv(
				hip(mv(*((e3_t *)elem1->elem)), mv(*((rotor *)elem2->elem)))
			)
		)
	);
break;

case ODDVERSOR_TAG:
	Glue<E3Elem>::push(
		L, 
		new E3Elem(
			MV_TAG,
			new mv(
				hip(mv(*((e3_t *)elem1->elem)), mv(*((oddVersor *)elem2->elem)))
			)
		)
	);
break;

case DOUBLE_TAG:
	Glue<E3Elem>::push(
		L, 
		new E3Elem(
			MV_TAG,
			new mv(
				hip(mv(*((e3_t *)elem1->elem)), *((double *)elem2))
			)
		)
	);
break;
}
break;

case I3_TAG:
switch(tag2) {
case MV_TAG:
	Glue<E3Elem>::push(
		L, 
		new E3Elem(
			MV_TAG,
			new mv(
				hip(mv(*((I3_t *)elem1->elem)), mv(*((mv *)elem2->elem)))
			)
		)
	);
break;

case E1_TAG:
	Glue<E3Elem>::push(
		L, 
		new E3Elem(
			MV_TAG,
			new mv(
				hip(mv(*((I3_t *)elem1->elem)), mv(*((e1_t *)elem2->elem)))
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
				hip(mv(*((I3_t *)elem1->elem)), mv(*((e2_t *)elem2->elem)))
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
				hip(mv(*((I3_t *)elem1->elem)), mv(*((e3_t *)elem2->elem)))
			)
		)
	);
break;

case I3_TAG:
	Glue<E3Elem>::push(
		L, 
		new E3Elem(
			MV_TAG,
			new mv(
				hip(mv(*((I3_t *)elem1->elem)), mv(*((I3_t *)elem2->elem)))
			)
		)
	);
break;

case VECTOR_TAG:
	Glue<E3Elem>::push(
		L, 
		new E3Elem(
			MV_TAG,
			new mv(
				hip(mv(*((I3_t *)elem1->elem)), mv(*((vector *)elem2->elem)))
			)
		)
	);
break;

case BIVECTOR_TAG:
	Glue<E3Elem>::push(
		L, 
		new E3Elem(
			MV_TAG,
			new mv(
				hip(mv(*((I3_t *)elem1->elem)), mv(*((bivector *)elem2->elem)))
			)
		)
	);
break;

case TRIVECTOR_TAG:
	Glue<E3Elem>::push(
		L, 
		new E3Elem(
			MV_TAG,
			new mv(
				hip(mv(*((I3_t *)elem1->elem)), mv(*((trivector *)elem2->elem)))
			)
		)
	);
break;

case ROTOR_TAG:
	Glue<E3Elem>::push(
		L, 
		new E3Elem(
			MV_TAG,
			new mv(
				hip(mv(*((I3_t *)elem1->elem)), mv(*((rotor *)elem2->elem)))
			)
		)
	);
break;

case ODDVERSOR_TAG:
	Glue<E3Elem>::push(
		L, 
		new E3Elem(
			MV_TAG,
			new mv(
				hip(mv(*((I3_t *)elem1->elem)), mv(*((oddVersor *)elem2->elem)))
			)
		)
	);
break;

case DOUBLE_TAG:
	Glue<E3Elem>::push(
		L, 
		new E3Elem(
			MV_TAG,
			new mv(
				hip(mv(*((I3_t *)elem1->elem)), *((double *)elem2))
			)
		)
	);
break;
}
break;

case VECTOR_TAG:
switch(tag2) {
case MV_TAG:
	Glue<E3Elem>::push(
		L, 
		new E3Elem(
			MV_TAG,
			new mv(
				hip(mv(*((vector *)elem1->elem)), mv(*((mv *)elem2->elem)))
			)
		)
	);
break;

case E1_TAG:
	Glue<E3Elem>::push(
		L, 
		new E3Elem(
			MV_TAG,
			new mv(
				hip(mv(*((vector *)elem1->elem)), mv(*((e1_t *)elem2->elem)))
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
				hip(mv(*((vector *)elem1->elem)), mv(*((e2_t *)elem2->elem)))
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
				hip(mv(*((vector *)elem1->elem)), mv(*((e3_t *)elem2->elem)))
			)
		)
	);
break;

case I3_TAG:
	Glue<E3Elem>::push(
		L, 
		new E3Elem(
			MV_TAG,
			new mv(
				hip(mv(*((vector *)elem1->elem)), mv(*((I3_t *)elem2->elem)))
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
	Glue<E3Elem>::push(
		L, 
		new E3Elem(
			VECTOR_TAG,
			new vector(
				hip(*((vector *)elem1->elem), *((bivector *)elem2->elem))
			)
		)
	);
break;

case TRIVECTOR_TAG:
	Glue<E3Elem>::push(
		L, 
		new E3Elem(
			MV_TAG,
			new mv(
				hip(mv(*((vector *)elem1->elem)), mv(*((trivector *)elem2->elem)))
			)
		)
	);
break;

case ROTOR_TAG:
	Glue<E3Elem>::push(
		L, 
		new E3Elem(
			VECTOR_TAG,
			new vector(
				hip(*((vector *)elem1->elem), *((rotor *)elem2->elem))
			)
		)
	);
break;

case ODDVERSOR_TAG:
	Glue<E3Elem>::push(
		L, 
		new E3Elem(
			MV_TAG,
			new mv(
				hip(mv(*((vector *)elem1->elem)), mv(*((oddVersor *)elem2->elem)))
			)
		)
	);
break;

case DOUBLE_TAG:
	Glue<E3Elem>::push(
		L, 
		new E3Elem(
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
	Glue<E3Elem>::push(
		L, 
		new E3Elem(
			MV_TAG,
			new mv(
				hip(mv(*((bivector *)elem1->elem)), mv(*((mv *)elem2->elem)))
			)
		)
	);
break;

case E1_TAG:
	Glue<E3Elem>::push(
		L, 
		new E3Elem(
			MV_TAG,
			new mv(
				hip(mv(*((bivector *)elem1->elem)), mv(*((e1_t *)elem2->elem)))
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
				hip(mv(*((bivector *)elem1->elem)), mv(*((e2_t *)elem2->elem)))
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
				hip(mv(*((bivector *)elem1->elem)), mv(*((e3_t *)elem2->elem)))
			)
		)
	);
break;

case I3_TAG:
	Glue<E3Elem>::push(
		L, 
		new E3Elem(
			MV_TAG,
			new mv(
				hip(mv(*((bivector *)elem1->elem)), mv(*((I3_t *)elem2->elem)))
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
				hip(*((bivector *)elem1->elem), *((vector *)elem2->elem))
			)
		)
	);
break;

case BIVECTOR_TAG:
	Glue<E3Elem>::push(
		L, 
		new E3Elem(
			MV_TAG,
			new mv(
				hip(mv(*((bivector *)elem1->elem)), mv(*((bivector *)elem2->elem)))
			)
		)
	);
break;

case TRIVECTOR_TAG:
	Glue<E3Elem>::push(
		L, 
		new E3Elem(
			MV_TAG,
			new mv(
				hip(mv(*((bivector *)elem1->elem)), mv(*((trivector *)elem2->elem)))
			)
		)
	);
break;

case ROTOR_TAG:
	Glue<E3Elem>::push(
		L, 
		new E3Elem(
			MV_TAG,
			new mv(
				hip(mv(*((bivector *)elem1->elem)), mv(*((rotor *)elem2->elem)))
			)
		)
	);
break;

case ODDVERSOR_TAG:
	Glue<E3Elem>::push(
		L, 
		new E3Elem(
			MV_TAG,
			new mv(
				hip(mv(*((bivector *)elem1->elem)), mv(*((oddVersor *)elem2->elem)))
			)
		)
	);
break;

case DOUBLE_TAG:
	Glue<E3Elem>::push(
		L, 
		new E3Elem(
			MV_TAG,
			new mv(
				hip(mv(*((bivector *)elem1->elem)), *((double *)elem2))
			)
		)
	);
break;
}
break;

case TRIVECTOR_TAG:
switch(tag2) {
case MV_TAG:
	Glue<E3Elem>::push(
		L, 
		new E3Elem(
			MV_TAG,
			new mv(
				hip(mv(*((trivector *)elem1->elem)), mv(*((mv *)elem2->elem)))
			)
		)
	);
break;

case E1_TAG:
	Glue<E3Elem>::push(
		L, 
		new E3Elem(
			MV_TAG,
			new mv(
				hip(mv(*((trivector *)elem1->elem)), mv(*((e1_t *)elem2->elem)))
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
				hip(mv(*((trivector *)elem1->elem)), mv(*((e2_t *)elem2->elem)))
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
				hip(mv(*((trivector *)elem1->elem)), mv(*((e3_t *)elem2->elem)))
			)
		)
	);
break;

case I3_TAG:
	Glue<E3Elem>::push(
		L, 
		new E3Elem(
			MV_TAG,
			new mv(
				hip(mv(*((trivector *)elem1->elem)), mv(*((I3_t *)elem2->elem)))
			)
		)
	);
break;

case VECTOR_TAG:
	Glue<E3Elem>::push(
		L, 
		new E3Elem(
			MV_TAG,
			new mv(
				hip(mv(*((trivector *)elem1->elem)), mv(*((vector *)elem2->elem)))
			)
		)
	);
break;

case BIVECTOR_TAG:
	Glue<E3Elem>::push(
		L, 
		new E3Elem(
			MV_TAG,
			new mv(
				hip(mv(*((trivector *)elem1->elem)), mv(*((bivector *)elem2->elem)))
			)
		)
	);
break;

case TRIVECTOR_TAG:
				lua::push<double>(
					L, 
					hip(*((trivector *)elem1->elem), *((trivector *)elem2->elem))
				);
			
break;

case ROTOR_TAG:
	Glue<E3Elem>::push(
		L, 
		new E3Elem(
			MV_TAG,
			new mv(
				hip(mv(*((trivector *)elem1->elem)), mv(*((rotor *)elem2->elem)))
			)
		)
	);
break;

case ODDVERSOR_TAG:
	Glue<E3Elem>::push(
		L, 
		new E3Elem(
			MV_TAG,
			new mv(
				hip(mv(*((trivector *)elem1->elem)), mv(*((oddVersor *)elem2->elem)))
			)
		)
	);
break;

case DOUBLE_TAG:
	Glue<E3Elem>::push(
		L, 
		new E3Elem(
			MV_TAG,
			new mv(
				hip(mv(*((trivector *)elem1->elem)), *((double *)elem2))
			)
		)
	);
break;
}
break;

case ROTOR_TAG:
switch(tag2) {
case MV_TAG:
	Glue<E3Elem>::push(
		L, 
		new E3Elem(
			MV_TAG,
			new mv(
				hip(mv(*((rotor *)elem1->elem)), mv(*((mv *)elem2->elem)))
			)
		)
	);
break;

case E1_TAG:
	Glue<E3Elem>::push(
		L, 
		new E3Elem(
			MV_TAG,
			new mv(
				hip(mv(*((rotor *)elem1->elem)), mv(*((e1_t *)elem2->elem)))
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
				hip(mv(*((rotor *)elem1->elem)), mv(*((e2_t *)elem2->elem)))
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
				hip(mv(*((rotor *)elem1->elem)), mv(*((e3_t *)elem2->elem)))
			)
		)
	);
break;

case I3_TAG:
	Glue<E3Elem>::push(
		L, 
		new E3Elem(
			MV_TAG,
			new mv(
				hip(mv(*((rotor *)elem1->elem)), mv(*((I3_t *)elem2->elem)))
			)
		)
	);
break;

case VECTOR_TAG:
	Glue<E3Elem>::push(
		L, 
		new E3Elem(
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

case TRIVECTOR_TAG:
	Glue<E3Elem>::push(
		L, 
		new E3Elem(
			VECTOR_TAG,
			new vector(
				hip(*((rotor *)elem1->elem), *((trivector *)elem2->elem))
			)
		)
	);
break;

case ROTOR_TAG:
	Glue<E3Elem>::push(
		L, 
		new E3Elem(
			MV_TAG,
			new mv(
				hip(mv(*((rotor *)elem1->elem)), mv(*((rotor *)elem2->elem)))
			)
		)
	);
break;

case ODDVERSOR_TAG:
	Glue<E3Elem>::push(
		L, 
		new E3Elem(
			MV_TAG,
			new mv(
				hip(mv(*((rotor *)elem1->elem)), mv(*((oddVersor *)elem2->elem)))
			)
		)
	);
break;

case DOUBLE_TAG:
	Glue<E3Elem>::push(
		L, 
		new E3Elem(
			MV_TAG,
			new mv(
				hip(mv(*((rotor *)elem1->elem)), *((double *)elem2))
			)
		)
	);
break;
}
break;

case ODDVERSOR_TAG:
switch(tag2) {
case MV_TAG:
	Glue<E3Elem>::push(
		L, 
		new E3Elem(
			MV_TAG,
			new mv(
				hip(mv(*((oddVersor *)elem1->elem)), mv(*((mv *)elem2->elem)))
			)
		)
	);
break;

case E1_TAG:
	Glue<E3Elem>::push(
		L, 
		new E3Elem(
			MV_TAG,
			new mv(
				hip(mv(*((oddVersor *)elem1->elem)), mv(*((e1_t *)elem2->elem)))
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
				hip(mv(*((oddVersor *)elem1->elem)), mv(*((e2_t *)elem2->elem)))
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
				hip(mv(*((oddVersor *)elem1->elem)), mv(*((e3_t *)elem2->elem)))
			)
		)
	);
break;

case I3_TAG:
	Glue<E3Elem>::push(
		L, 
		new E3Elem(
			MV_TAG,
			new mv(
				hip(mv(*((oddVersor *)elem1->elem)), mv(*((I3_t *)elem2->elem)))
			)
		)
	);
break;

case VECTOR_TAG:
	Glue<E3Elem>::push(
		L, 
		new E3Elem(
			MV_TAG,
			new mv(
				hip(mv(*((oddVersor *)elem1->elem)), mv(*((vector *)elem2->elem)))
			)
		)
	);
break;

case BIVECTOR_TAG:
	Glue<E3Elem>::push(
		L, 
		new E3Elem(
			MV_TAG,
			new mv(
				hip(mv(*((oddVersor *)elem1->elem)), mv(*((bivector *)elem2->elem)))
			)
		)
	);
break;

case TRIVECTOR_TAG:
	Glue<E3Elem>::push(
		L, 
		new E3Elem(
			MV_TAG,
			new mv(
				hip(mv(*((oddVersor *)elem1->elem)), mv(*((trivector *)elem2->elem)))
			)
		)
	);
break;

case ROTOR_TAG:
	Glue<E3Elem>::push(
		L, 
		new E3Elem(
			MV_TAG,
			new mv(
				hip(mv(*((oddVersor *)elem1->elem)), mv(*((rotor *)elem2->elem)))
			)
		)
	);
break;

case ODDVERSOR_TAG:
	Glue<E3Elem>::push(
		L, 
		new E3Elem(
			MV_TAG,
			new mv(
				hip(mv(*((oddVersor *)elem1->elem)), mv(*((oddVersor *)elem2->elem)))
			)
		)
	);
break;

case DOUBLE_TAG:
	Glue<E3Elem>::push(
		L, 
		new E3Elem(
			MV_TAG,
			new mv(
				hip(mv(*((oddVersor *)elem1->elem)), *((double *)elem2))
			)
		)
	);
break;
}
break;

case DOUBLE_TAG:
switch(tag2) {
case MV_TAG:
	Glue<E3Elem>::push(
		L, 
		new E3Elem(
			MV_TAG,
			new mv(
				hip(*((double *)elem1), mv(*((mv *)elem2->elem)))
			)
		)
	);
break;

case E1_TAG:
	Glue<E3Elem>::push(
		L, 
		new E3Elem(
			MV_TAG,
			new mv(
				hip(*((double *)elem1), mv(*((e1_t *)elem2->elem)))
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
				hip(*((double *)elem1), mv(*((e2_t *)elem2->elem)))
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
				hip(*((double *)elem1), mv(*((e3_t *)elem2->elem)))
			)
		)
	);
break;

case I3_TAG:
	Glue<E3Elem>::push(
		L, 
		new E3Elem(
			MV_TAG,
			new mv(
				hip(*((double *)elem1), mv(*((I3_t *)elem2->elem)))
			)
		)
	);
break;

case VECTOR_TAG:
	Glue<E3Elem>::push(
		L, 
		new E3Elem(
			MV_TAG,
			new mv(
				hip(*((double *)elem1), mv(*((vector *)elem2->elem)))
			)
		)
	);
break;

case BIVECTOR_TAG:
	Glue<E3Elem>::push(
		L, 
		new E3Elem(
			MV_TAG,
			new mv(
				hip(*((double *)elem1), mv(*((bivector *)elem2->elem)))
			)
		)
	);
break;

case TRIVECTOR_TAG:
	Glue<E3Elem>::push(
		L, 
		new E3Elem(
			MV_TAG,
			new mv(
				hip(*((double *)elem1), mv(*((trivector *)elem2->elem)))
			)
		)
	);
break;

case ROTOR_TAG:
	Glue<E3Elem>::push(
		L, 
		new E3Elem(
			MV_TAG,
			new mv(
				hip(*((double *)elem1), mv(*((rotor *)elem2->elem)))
			)
		)
	);
break;

case ODDVERSOR_TAG:
	Glue<E3Elem>::push(
		L, 
		new E3Elem(
			MV_TAG,
			new mv(
				hip(*((double *)elem1), mv(*((oddVersor *)elem2->elem)))
			)
		)
	);
break;

case DOUBLE_TAG:
	Glue<E3Elem>::push(
		L, 
		new E3Elem(
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


int e3space_mhip(lua_State *L) {
	double d1, d2;
	E3Tag tag1;
	E3Tag tag2;
	E3Elem *elem1 = Glue<E3Elem>::to(L, 1);
	E3Elem *elem2 = Glue<E3Elem>::to(L, 2);
	
	
	if(!elem1) {
		if(lua::is<double>(L, 1)) {
			d1 = lua::to<double>(L, 1);
			elem1 = (E3Elem *)(&d1);
			tag1 = DOUBLE_TAG;
		}
		else {
			luaL_error(L, "e3.mhip: invalid arguments");
		}
	}
	else {
		tag1 = elem1->tag;
	}
	
	if(!elem2) {
		if(lua::is<double>(L, 2)) {
			d2 = lua::to<double>(L, 2);
			elem2 = (E3Elem *)(&d2);
			tag2 = DOUBLE_TAG;
		}
		else {
			luaL_error(L, "e3.mhip: invalid arguments");
		}
	}
	else {
		tag2 = elem2->tag;
	}
	
switch(tag1) {
case MV_TAG:
switch(tag2) {
case MV_TAG:
	Glue<E3Elem>::push(
		L, 
		new E3Elem(
			MV_TAG,
			new mv(
				mhip(*((mv *)elem1->elem), *((mv *)elem2->elem))
			)
		)
	);
break;

case E1_TAG:
	Glue<E3Elem>::push(
		L, 
		new E3Elem(
			MV_TAG,
			new mv(
				mhip(mv(*((mv *)elem1->elem)), mv(*((e1_t *)elem2->elem)))
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
				mhip(mv(*((mv *)elem1->elem)), mv(*((e2_t *)elem2->elem)))
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
				mhip(mv(*((mv *)elem1->elem)), mv(*((e3_t *)elem2->elem)))
			)
		)
	);
break;

case I3_TAG:
	Glue<E3Elem>::push(
		L, 
		new E3Elem(
			MV_TAG,
			new mv(
				mhip(mv(*((mv *)elem1->elem)), mv(*((I3_t *)elem2->elem)))
			)
		)
	);
break;

case VECTOR_TAG:
	Glue<E3Elem>::push(
		L, 
		new E3Elem(
			MV_TAG,
			new mv(
				mhip(mv(*((mv *)elem1->elem)), mv(*((vector *)elem2->elem)))
			)
		)
	);
break;

case BIVECTOR_TAG:
	Glue<E3Elem>::push(
		L, 
		new E3Elem(
			MV_TAG,
			new mv(
				mhip(mv(*((mv *)elem1->elem)), mv(*((bivector *)elem2->elem)))
			)
		)
	);
break;

case TRIVECTOR_TAG:
	Glue<E3Elem>::push(
		L, 
		new E3Elem(
			MV_TAG,
			new mv(
				mhip(mv(*((mv *)elem1->elem)), mv(*((trivector *)elem2->elem)))
			)
		)
	);
break;

case ROTOR_TAG:
	Glue<E3Elem>::push(
		L, 
		new E3Elem(
			MV_TAG,
			new mv(
				mhip(mv(*((mv *)elem1->elem)), mv(*((rotor *)elem2->elem)))
			)
		)
	);
break;

case ODDVERSOR_TAG:
	Glue<E3Elem>::push(
		L, 
		new E3Elem(
			MV_TAG,
			new mv(
				mhip(mv(*((mv *)elem1->elem)), mv(*((oddVersor *)elem2->elem)))
			)
		)
	);
break;

case DOUBLE_TAG:
	Glue<E3Elem>::push(
		L, 
		new E3Elem(
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
	Glue<E3Elem>::push(
		L, 
		new E3Elem(
			MV_TAG,
			new mv(
				mhip(mv(*((e1_t *)elem1->elem)), mv(*((mv *)elem2->elem)))
			)
		)
	);
break;

case E1_TAG:
	Glue<E3Elem>::push(
		L, 
		new E3Elem(
			MV_TAG,
			new mv(
				mhip(mv(*((e1_t *)elem1->elem)), mv(*((e1_t *)elem2->elem)))
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
				mhip(mv(*((e1_t *)elem1->elem)), mv(*((e2_t *)elem2->elem)))
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
				mhip(mv(*((e1_t *)elem1->elem)), mv(*((e3_t *)elem2->elem)))
			)
		)
	);
break;

case I3_TAG:
	Glue<E3Elem>::push(
		L, 
		new E3Elem(
			MV_TAG,
			new mv(
				mhip(mv(*((e1_t *)elem1->elem)), mv(*((I3_t *)elem2->elem)))
			)
		)
	);
break;

case VECTOR_TAG:
	Glue<E3Elem>::push(
		L, 
		new E3Elem(
			MV_TAG,
			new mv(
				mhip(mv(*((e1_t *)elem1->elem)), mv(*((vector *)elem2->elem)))
			)
		)
	);
break;

case BIVECTOR_TAG:
	Glue<E3Elem>::push(
		L, 
		new E3Elem(
			MV_TAG,
			new mv(
				mhip(mv(*((e1_t *)elem1->elem)), mv(*((bivector *)elem2->elem)))
			)
		)
	);
break;

case TRIVECTOR_TAG:
	Glue<E3Elem>::push(
		L, 
		new E3Elem(
			MV_TAG,
			new mv(
				mhip(mv(*((e1_t *)elem1->elem)), mv(*((trivector *)elem2->elem)))
			)
		)
	);
break;

case ROTOR_TAG:
	Glue<E3Elem>::push(
		L, 
		new E3Elem(
			MV_TAG,
			new mv(
				mhip(mv(*((e1_t *)elem1->elem)), mv(*((rotor *)elem2->elem)))
			)
		)
	);
break;

case ODDVERSOR_TAG:
	Glue<E3Elem>::push(
		L, 
		new E3Elem(
			MV_TAG,
			new mv(
				mhip(mv(*((e1_t *)elem1->elem)), mv(*((oddVersor *)elem2->elem)))
			)
		)
	);
break;

case DOUBLE_TAG:
	Glue<E3Elem>::push(
		L, 
		new E3Elem(
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
	Glue<E3Elem>::push(
		L, 
		new E3Elem(
			MV_TAG,
			new mv(
				mhip(mv(*((e2_t *)elem1->elem)), mv(*((mv *)elem2->elem)))
			)
		)
	);
break;

case E1_TAG:
	Glue<E3Elem>::push(
		L, 
		new E3Elem(
			MV_TAG,
			new mv(
				mhip(mv(*((e2_t *)elem1->elem)), mv(*((e1_t *)elem2->elem)))
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
				mhip(mv(*((e2_t *)elem1->elem)), mv(*((e2_t *)elem2->elem)))
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
				mhip(mv(*((e2_t *)elem1->elem)), mv(*((e3_t *)elem2->elem)))
			)
		)
	);
break;

case I3_TAG:
	Glue<E3Elem>::push(
		L, 
		new E3Elem(
			MV_TAG,
			new mv(
				mhip(mv(*((e2_t *)elem1->elem)), mv(*((I3_t *)elem2->elem)))
			)
		)
	);
break;

case VECTOR_TAG:
	Glue<E3Elem>::push(
		L, 
		new E3Elem(
			MV_TAG,
			new mv(
				mhip(mv(*((e2_t *)elem1->elem)), mv(*((vector *)elem2->elem)))
			)
		)
	);
break;

case BIVECTOR_TAG:
	Glue<E3Elem>::push(
		L, 
		new E3Elem(
			MV_TAG,
			new mv(
				mhip(mv(*((e2_t *)elem1->elem)), mv(*((bivector *)elem2->elem)))
			)
		)
	);
break;

case TRIVECTOR_TAG:
	Glue<E3Elem>::push(
		L, 
		new E3Elem(
			MV_TAG,
			new mv(
				mhip(mv(*((e2_t *)elem1->elem)), mv(*((trivector *)elem2->elem)))
			)
		)
	);
break;

case ROTOR_TAG:
	Glue<E3Elem>::push(
		L, 
		new E3Elem(
			MV_TAG,
			new mv(
				mhip(mv(*((e2_t *)elem1->elem)), mv(*((rotor *)elem2->elem)))
			)
		)
	);
break;

case ODDVERSOR_TAG:
	Glue<E3Elem>::push(
		L, 
		new E3Elem(
			MV_TAG,
			new mv(
				mhip(mv(*((e2_t *)elem1->elem)), mv(*((oddVersor *)elem2->elem)))
			)
		)
	);
break;

case DOUBLE_TAG:
	Glue<E3Elem>::push(
		L, 
		new E3Elem(
			MV_TAG,
			new mv(
				mhip(mv(*((e2_t *)elem1->elem)), *((double *)elem2))
			)
		)
	);
break;
}
break;

case E3_TAG:
switch(tag2) {
case MV_TAG:
	Glue<E3Elem>::push(
		L, 
		new E3Elem(
			MV_TAG,
			new mv(
				mhip(mv(*((e3_t *)elem1->elem)), mv(*((mv *)elem2->elem)))
			)
		)
	);
break;

case E1_TAG:
	Glue<E3Elem>::push(
		L, 
		new E3Elem(
			MV_TAG,
			new mv(
				mhip(mv(*((e3_t *)elem1->elem)), mv(*((e1_t *)elem2->elem)))
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
				mhip(mv(*((e3_t *)elem1->elem)), mv(*((e2_t *)elem2->elem)))
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
				mhip(mv(*((e3_t *)elem1->elem)), mv(*((e3_t *)elem2->elem)))
			)
		)
	);
break;

case I3_TAG:
	Glue<E3Elem>::push(
		L, 
		new E3Elem(
			MV_TAG,
			new mv(
				mhip(mv(*((e3_t *)elem1->elem)), mv(*((I3_t *)elem2->elem)))
			)
		)
	);
break;

case VECTOR_TAG:
	Glue<E3Elem>::push(
		L, 
		new E3Elem(
			MV_TAG,
			new mv(
				mhip(mv(*((e3_t *)elem1->elem)), mv(*((vector *)elem2->elem)))
			)
		)
	);
break;

case BIVECTOR_TAG:
	Glue<E3Elem>::push(
		L, 
		new E3Elem(
			MV_TAG,
			new mv(
				mhip(mv(*((e3_t *)elem1->elem)), mv(*((bivector *)elem2->elem)))
			)
		)
	);
break;

case TRIVECTOR_TAG:
	Glue<E3Elem>::push(
		L, 
		new E3Elem(
			MV_TAG,
			new mv(
				mhip(mv(*((e3_t *)elem1->elem)), mv(*((trivector *)elem2->elem)))
			)
		)
	);
break;

case ROTOR_TAG:
	Glue<E3Elem>::push(
		L, 
		new E3Elem(
			MV_TAG,
			new mv(
				mhip(mv(*((e3_t *)elem1->elem)), mv(*((rotor *)elem2->elem)))
			)
		)
	);
break;

case ODDVERSOR_TAG:
	Glue<E3Elem>::push(
		L, 
		new E3Elem(
			MV_TAG,
			new mv(
				mhip(mv(*((e3_t *)elem1->elem)), mv(*((oddVersor *)elem2->elem)))
			)
		)
	);
break;

case DOUBLE_TAG:
	Glue<E3Elem>::push(
		L, 
		new E3Elem(
			MV_TAG,
			new mv(
				mhip(mv(*((e3_t *)elem1->elem)), *((double *)elem2))
			)
		)
	);
break;
}
break;

case I3_TAG:
switch(tag2) {
case MV_TAG:
	Glue<E3Elem>::push(
		L, 
		new E3Elem(
			MV_TAG,
			new mv(
				mhip(mv(*((I3_t *)elem1->elem)), mv(*((mv *)elem2->elem)))
			)
		)
	);
break;

case E1_TAG:
	Glue<E3Elem>::push(
		L, 
		new E3Elem(
			MV_TAG,
			new mv(
				mhip(mv(*((I3_t *)elem1->elem)), mv(*((e1_t *)elem2->elem)))
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
				mhip(mv(*((I3_t *)elem1->elem)), mv(*((e2_t *)elem2->elem)))
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
				mhip(mv(*((I3_t *)elem1->elem)), mv(*((e3_t *)elem2->elem)))
			)
		)
	);
break;

case I3_TAG:
				lua::push<double>(
					L, 
					mhip(*((I3_t *)elem1->elem), *((I3_t *)elem2->elem))
				);
			
break;

case VECTOR_TAG:
	Glue<E3Elem>::push(
		L, 
		new E3Elem(
			MV_TAG,
			new mv(
				mhip(mv(*((I3_t *)elem1->elem)), mv(*((vector *)elem2->elem)))
			)
		)
	);
break;

case BIVECTOR_TAG:
	Glue<E3Elem>::push(
		L, 
		new E3Elem(
			MV_TAG,
			new mv(
				mhip(mv(*((I3_t *)elem1->elem)), mv(*((bivector *)elem2->elem)))
			)
		)
	);
break;

case TRIVECTOR_TAG:
	Glue<E3Elem>::push(
		L, 
		new E3Elem(
			MV_TAG,
			new mv(
				mhip(mv(*((I3_t *)elem1->elem)), mv(*((trivector *)elem2->elem)))
			)
		)
	);
break;

case ROTOR_TAG:
	Glue<E3Elem>::push(
		L, 
		new E3Elem(
			MV_TAG,
			new mv(
				mhip(mv(*((I3_t *)elem1->elem)), mv(*((rotor *)elem2->elem)))
			)
		)
	);
break;

case ODDVERSOR_TAG:
	Glue<E3Elem>::push(
		L, 
		new E3Elem(
			MV_TAG,
			new mv(
				mhip(mv(*((I3_t *)elem1->elem)), mv(*((oddVersor *)elem2->elem)))
			)
		)
	);
break;

case DOUBLE_TAG:
	Glue<E3Elem>::push(
		L, 
		new E3Elem(
			MV_TAG,
			new mv(
				mhip(mv(*((I3_t *)elem1->elem)), *((double *)elem2))
			)
		)
	);
break;
}
break;

case VECTOR_TAG:
switch(tag2) {
case MV_TAG:
	Glue<E3Elem>::push(
		L, 
		new E3Elem(
			MV_TAG,
			new mv(
				mhip(mv(*((vector *)elem1->elem)), mv(*((mv *)elem2->elem)))
			)
		)
	);
break;

case E1_TAG:
	Glue<E3Elem>::push(
		L, 
		new E3Elem(
			MV_TAG,
			new mv(
				mhip(mv(*((vector *)elem1->elem)), mv(*((e1_t *)elem2->elem)))
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
				mhip(mv(*((vector *)elem1->elem)), mv(*((e2_t *)elem2->elem)))
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
				mhip(mv(*((vector *)elem1->elem)), mv(*((e3_t *)elem2->elem)))
			)
		)
	);
break;

case I3_TAG:
	Glue<E3Elem>::push(
		L, 
		new E3Elem(
			MV_TAG,
			new mv(
				mhip(mv(*((vector *)elem1->elem)), mv(*((I3_t *)elem2->elem)))
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
	Glue<E3Elem>::push(
		L, 
		new E3Elem(
			VECTOR_TAG,
			new vector(
				mhip(*((vector *)elem1->elem), *((bivector *)elem2->elem))
			)
		)
	);
break;

case TRIVECTOR_TAG:
	Glue<E3Elem>::push(
		L, 
		new E3Elem(
			MV_TAG,
			new mv(
				mhip(mv(*((vector *)elem1->elem)), mv(*((trivector *)elem2->elem)))
			)
		)
	);
break;

case ROTOR_TAG:
	Glue<E3Elem>::push(
		L, 
		new E3Elem(
			VECTOR_TAG,
			new vector(
				mhip(*((vector *)elem1->elem), *((rotor *)elem2->elem))
			)
		)
	);
break;

case ODDVERSOR_TAG:
	Glue<E3Elem>::push(
		L, 
		new E3Elem(
			MV_TAG,
			new mv(
				mhip(mv(*((vector *)elem1->elem)), mv(*((oddVersor *)elem2->elem)))
			)
		)
	);
break;

case DOUBLE_TAG:
	Glue<E3Elem>::push(
		L, 
		new E3Elem(
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
	Glue<E3Elem>::push(
		L, 
		new E3Elem(
			MV_TAG,
			new mv(
				mhip(mv(*((bivector *)elem1->elem)), mv(*((mv *)elem2->elem)))
			)
		)
	);
break;

case E1_TAG:
	Glue<E3Elem>::push(
		L, 
		new E3Elem(
			MV_TAG,
			new mv(
				mhip(mv(*((bivector *)elem1->elem)), mv(*((e1_t *)elem2->elem)))
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
				mhip(mv(*((bivector *)elem1->elem)), mv(*((e2_t *)elem2->elem)))
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
				mhip(mv(*((bivector *)elem1->elem)), mv(*((e3_t *)elem2->elem)))
			)
		)
	);
break;

case I3_TAG:
	Glue<E3Elem>::push(
		L, 
		new E3Elem(
			MV_TAG,
			new mv(
				mhip(mv(*((bivector *)elem1->elem)), mv(*((I3_t *)elem2->elem)))
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
				mhip(*((bivector *)elem1->elem), *((vector *)elem2->elem))
			)
		)
	);
break;

case BIVECTOR_TAG:
	Glue<E3Elem>::push(
		L, 
		new E3Elem(
			MV_TAG,
			new mv(
				mhip(mv(*((bivector *)elem1->elem)), mv(*((bivector *)elem2->elem)))
			)
		)
	);
break;

case TRIVECTOR_TAG:
	Glue<E3Elem>::push(
		L, 
		new E3Elem(
			MV_TAG,
			new mv(
				mhip(mv(*((bivector *)elem1->elem)), mv(*((trivector *)elem2->elem)))
			)
		)
	);
break;

case ROTOR_TAG:
	Glue<E3Elem>::push(
		L, 
		new E3Elem(
			MV_TAG,
			new mv(
				mhip(mv(*((bivector *)elem1->elem)), mv(*((rotor *)elem2->elem)))
			)
		)
	);
break;

case ODDVERSOR_TAG:
	Glue<E3Elem>::push(
		L, 
		new E3Elem(
			MV_TAG,
			new mv(
				mhip(mv(*((bivector *)elem1->elem)), mv(*((oddVersor *)elem2->elem)))
			)
		)
	);
break;

case DOUBLE_TAG:
	Glue<E3Elem>::push(
		L, 
		new E3Elem(
			MV_TAG,
			new mv(
				mhip(mv(*((bivector *)elem1->elem)), *((double *)elem2))
			)
		)
	);
break;
}
break;

case TRIVECTOR_TAG:
switch(tag2) {
case MV_TAG:
	Glue<E3Elem>::push(
		L, 
		new E3Elem(
			MV_TAG,
			new mv(
				mhip(mv(*((trivector *)elem1->elem)), mv(*((mv *)elem2->elem)))
			)
		)
	);
break;

case E1_TAG:
	Glue<E3Elem>::push(
		L, 
		new E3Elem(
			MV_TAG,
			new mv(
				mhip(mv(*((trivector *)elem1->elem)), mv(*((e1_t *)elem2->elem)))
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
				mhip(mv(*((trivector *)elem1->elem)), mv(*((e2_t *)elem2->elem)))
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
				mhip(mv(*((trivector *)elem1->elem)), mv(*((e3_t *)elem2->elem)))
			)
		)
	);
break;

case I3_TAG:
	Glue<E3Elem>::push(
		L, 
		new E3Elem(
			MV_TAG,
			new mv(
				mhip(mv(*((trivector *)elem1->elem)), mv(*((I3_t *)elem2->elem)))
			)
		)
	);
break;

case VECTOR_TAG:
	Glue<E3Elem>::push(
		L, 
		new E3Elem(
			MV_TAG,
			new mv(
				mhip(mv(*((trivector *)elem1->elem)), mv(*((vector *)elem2->elem)))
			)
		)
	);
break;

case BIVECTOR_TAG:
	Glue<E3Elem>::push(
		L, 
		new E3Elem(
			MV_TAG,
			new mv(
				mhip(mv(*((trivector *)elem1->elem)), mv(*((bivector *)elem2->elem)))
			)
		)
	);
break;

case TRIVECTOR_TAG:
				lua::push<double>(
					L, 
					mhip(*((trivector *)elem1->elem), *((trivector *)elem2->elem))
				);
			
break;

case ROTOR_TAG:
	Glue<E3Elem>::push(
		L, 
		new E3Elem(
			MV_TAG,
			new mv(
				mhip(mv(*((trivector *)elem1->elem)), mv(*((rotor *)elem2->elem)))
			)
		)
	);
break;

case ODDVERSOR_TAG:
	Glue<E3Elem>::push(
		L, 
		new E3Elem(
			MV_TAG,
			new mv(
				mhip(mv(*((trivector *)elem1->elem)), mv(*((oddVersor *)elem2->elem)))
			)
		)
	);
break;

case DOUBLE_TAG:
	Glue<E3Elem>::push(
		L, 
		new E3Elem(
			MV_TAG,
			new mv(
				mhip(mv(*((trivector *)elem1->elem)), *((double *)elem2))
			)
		)
	);
break;
}
break;

case ROTOR_TAG:
switch(tag2) {
case MV_TAG:
	Glue<E3Elem>::push(
		L, 
		new E3Elem(
			MV_TAG,
			new mv(
				mhip(mv(*((rotor *)elem1->elem)), mv(*((mv *)elem2->elem)))
			)
		)
	);
break;

case E1_TAG:
	Glue<E3Elem>::push(
		L, 
		new E3Elem(
			MV_TAG,
			new mv(
				mhip(mv(*((rotor *)elem1->elem)), mv(*((e1_t *)elem2->elem)))
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
				mhip(mv(*((rotor *)elem1->elem)), mv(*((e2_t *)elem2->elem)))
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
				mhip(mv(*((rotor *)elem1->elem)), mv(*((e3_t *)elem2->elem)))
			)
		)
	);
break;

case I3_TAG:
	Glue<E3Elem>::push(
		L, 
		new E3Elem(
			MV_TAG,
			new mv(
				mhip(mv(*((rotor *)elem1->elem)), mv(*((I3_t *)elem2->elem)))
			)
		)
	);
break;

case VECTOR_TAG:
	Glue<E3Elem>::push(
		L, 
		new E3Elem(
			MV_TAG,
			new mv(
				mhip(mv(*((rotor *)elem1->elem)), mv(*((vector *)elem2->elem)))
			)
		)
	);
break;

case BIVECTOR_TAG:
	Glue<E3Elem>::push(
		L, 
		new E3Elem(
			ROTOR_TAG,
			new rotor(
				mhip(*((rotor *)elem1->elem), *((bivector *)elem2->elem))
			)
		)
	);
break;

case TRIVECTOR_TAG:
	Glue<E3Elem>::push(
		L, 
		new E3Elem(
			ODDVERSOR_TAG,
			new oddVersor(
				mhip(*((rotor *)elem1->elem), *((trivector *)elem2->elem))
			)
		)
	);
break;

case ROTOR_TAG:
	Glue<E3Elem>::push(
		L, 
		new E3Elem(
			MV_TAG,
			new mv(
				mhip(mv(*((rotor *)elem1->elem)), mv(*((rotor *)elem2->elem)))
			)
		)
	);
break;

case ODDVERSOR_TAG:
	Glue<E3Elem>::push(
		L, 
		new E3Elem(
			MV_TAG,
			new mv(
				mhip(mv(*((rotor *)elem1->elem)), mv(*((oddVersor *)elem2->elem)))
			)
		)
	);
break;

case DOUBLE_TAG:
	Glue<E3Elem>::push(
		L, 
		new E3Elem(
			MV_TAG,
			new mv(
				mhip(mv(*((rotor *)elem1->elem)), *((double *)elem2))
			)
		)
	);
break;
}
break;

case ODDVERSOR_TAG:
switch(tag2) {
case MV_TAG:
	Glue<E3Elem>::push(
		L, 
		new E3Elem(
			MV_TAG,
			new mv(
				mhip(mv(*((oddVersor *)elem1->elem)), mv(*((mv *)elem2->elem)))
			)
		)
	);
break;

case E1_TAG:
	Glue<E3Elem>::push(
		L, 
		new E3Elem(
			MV_TAG,
			new mv(
				mhip(mv(*((oddVersor *)elem1->elem)), mv(*((e1_t *)elem2->elem)))
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
				mhip(mv(*((oddVersor *)elem1->elem)), mv(*((e2_t *)elem2->elem)))
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
				mhip(mv(*((oddVersor *)elem1->elem)), mv(*((e3_t *)elem2->elem)))
			)
		)
	);
break;

case I3_TAG:
	Glue<E3Elem>::push(
		L, 
		new E3Elem(
			MV_TAG,
			new mv(
				mhip(mv(*((oddVersor *)elem1->elem)), mv(*((I3_t *)elem2->elem)))
			)
		)
	);
break;

case VECTOR_TAG:
	Glue<E3Elem>::push(
		L, 
		new E3Elem(
			MV_TAG,
			new mv(
				mhip(mv(*((oddVersor *)elem1->elem)), mv(*((vector *)elem2->elem)))
			)
		)
	);
break;

case BIVECTOR_TAG:
	Glue<E3Elem>::push(
		L, 
		new E3Elem(
			MV_TAG,
			new mv(
				mhip(mv(*((oddVersor *)elem1->elem)), mv(*((bivector *)elem2->elem)))
			)
		)
	);
break;

case TRIVECTOR_TAG:
	Glue<E3Elem>::push(
		L, 
		new E3Elem(
			MV_TAG,
			new mv(
				mhip(mv(*((oddVersor *)elem1->elem)), mv(*((trivector *)elem2->elem)))
			)
		)
	);
break;

case ROTOR_TAG:
	Glue<E3Elem>::push(
		L, 
		new E3Elem(
			MV_TAG,
			new mv(
				mhip(mv(*((oddVersor *)elem1->elem)), mv(*((rotor *)elem2->elem)))
			)
		)
	);
break;

case ODDVERSOR_TAG:
	Glue<E3Elem>::push(
		L, 
		new E3Elem(
			MV_TAG,
			new mv(
				mhip(mv(*((oddVersor *)elem1->elem)), mv(*((oddVersor *)elem2->elem)))
			)
		)
	);
break;

case DOUBLE_TAG:
	Glue<E3Elem>::push(
		L, 
		new E3Elem(
			MV_TAG,
			new mv(
				mhip(mv(*((oddVersor *)elem1->elem)), *((double *)elem2))
			)
		)
	);
break;
}
break;

case DOUBLE_TAG:
switch(tag2) {
case MV_TAG:
	Glue<E3Elem>::push(
		L, 
		new E3Elem(
			MV_TAG,
			new mv(
				mhip(*((double *)elem1), mv(*((mv *)elem2->elem)))
			)
		)
	);
break;

case E1_TAG:
	Glue<E3Elem>::push(
		L, 
		new E3Elem(
			MV_TAG,
			new mv(
				mhip(*((double *)elem1), mv(*((e1_t *)elem2->elem)))
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
				mhip(*((double *)elem1), mv(*((e2_t *)elem2->elem)))
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
				mhip(*((double *)elem1), mv(*((e3_t *)elem2->elem)))
			)
		)
	);
break;

case I3_TAG:
	Glue<E3Elem>::push(
		L, 
		new E3Elem(
			MV_TAG,
			new mv(
				mhip(*((double *)elem1), mv(*((I3_t *)elem2->elem)))
			)
		)
	);
break;

case VECTOR_TAG:
	Glue<E3Elem>::push(
		L, 
		new E3Elem(
			MV_TAG,
			new mv(
				mhip(*((double *)elem1), mv(*((vector *)elem2->elem)))
			)
		)
	);
break;

case BIVECTOR_TAG:
	Glue<E3Elem>::push(
		L, 
		new E3Elem(
			MV_TAG,
			new mv(
				mhip(*((double *)elem1), mv(*((bivector *)elem2->elem)))
			)
		)
	);
break;

case TRIVECTOR_TAG:
	Glue<E3Elem>::push(
		L, 
		new E3Elem(
			MV_TAG,
			new mv(
				mhip(*((double *)elem1), mv(*((trivector *)elem2->elem)))
			)
		)
	);
break;

case ROTOR_TAG:
	Glue<E3Elem>::push(
		L, 
		new E3Elem(
			MV_TAG,
			new mv(
				mhip(*((double *)elem1), mv(*((rotor *)elem2->elem)))
			)
		)
	);
break;

case ODDVERSOR_TAG:
	Glue<E3Elem>::push(
		L, 
		new E3Elem(
			MV_TAG,
			new mv(
				mhip(*((double *)elem1), mv(*((oddVersor *)elem2->elem)))
			)
		)
	);
break;

case DOUBLE_TAG:
	Glue<E3Elem>::push(
		L, 
		new E3Elem(
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


int e3space_op(lua_State *L) {
	double d1, d2;
	E3Tag tag1;
	E3Tag tag2;
	E3Elem *elem1 = Glue<E3Elem>::to(L, 1);
	E3Elem *elem2 = Glue<E3Elem>::to(L, 2);
	
	
	if(!elem1) {
		if(lua::is<double>(L, 1)) {
			d1 = lua::to<double>(L, 1);
			elem1 = (E3Elem *)(&d1);
			tag1 = DOUBLE_TAG;
		}
		else {
			luaL_error(L, "e3.op: invalid arguments");
		}
	}
	else {
		tag1 = elem1->tag;
	}
	
	if(!elem2) {
		if(lua::is<double>(L, 2)) {
			d2 = lua::to<double>(L, 2);
			elem2 = (E3Elem *)(&d2);
			tag2 = DOUBLE_TAG;
		}
		else {
			luaL_error(L, "e3.op: invalid arguments");
		}
	}
	else {
		tag2 = elem2->tag;
	}
	
switch(tag1) {
case MV_TAG:
switch(tag2) {
case MV_TAG:
	Glue<E3Elem>::push(
		L, 
		new E3Elem(
			MV_TAG,
			new mv(
				op(*((mv *)elem1->elem), *((mv *)elem2->elem))
			)
		)
	);
break;

case E1_TAG:
	Glue<E3Elem>::push(
		L, 
		new E3Elem(
			MV_TAG,
			new mv(
				op(mv(*((mv *)elem1->elem)), mv(*((e1_t *)elem2->elem)))
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
				op(mv(*((mv *)elem1->elem)), mv(*((e2_t *)elem2->elem)))
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
				op(mv(*((mv *)elem1->elem)), mv(*((e3_t *)elem2->elem)))
			)
		)
	);
break;

case I3_TAG:
	Glue<E3Elem>::push(
		L, 
		new E3Elem(
			MV_TAG,
			new mv(
				op(mv(*((mv *)elem1->elem)), mv(*((I3_t *)elem2->elem)))
			)
		)
	);
break;

case VECTOR_TAG:
	Glue<E3Elem>::push(
		L, 
		new E3Elem(
			MV_TAG,
			new mv(
				op(mv(*((mv *)elem1->elem)), mv(*((vector *)elem2->elem)))
			)
		)
	);
break;

case BIVECTOR_TAG:
	Glue<E3Elem>::push(
		L, 
		new E3Elem(
			MV_TAG,
			new mv(
				op(mv(*((mv *)elem1->elem)), mv(*((bivector *)elem2->elem)))
			)
		)
	);
break;

case TRIVECTOR_TAG:
	Glue<E3Elem>::push(
		L, 
		new E3Elem(
			MV_TAG,
			new mv(
				op(mv(*((mv *)elem1->elem)), mv(*((trivector *)elem2->elem)))
			)
		)
	);
break;

case ROTOR_TAG:
	Glue<E3Elem>::push(
		L, 
		new E3Elem(
			MV_TAG,
			new mv(
				op(mv(*((mv *)elem1->elem)), mv(*((rotor *)elem2->elem)))
			)
		)
	);
break;

case ODDVERSOR_TAG:
	Glue<E3Elem>::push(
		L, 
		new E3Elem(
			MV_TAG,
			new mv(
				op(mv(*((mv *)elem1->elem)), mv(*((oddVersor *)elem2->elem)))
			)
		)
	);
break;

case DOUBLE_TAG:
	Glue<E3Elem>::push(
		L, 
		new E3Elem(
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
	Glue<E3Elem>::push(
		L, 
		new E3Elem(
			MV_TAG,
			new mv(
				op(mv(*((e1_t *)elem1->elem)), mv(*((mv *)elem2->elem)))
			)
		)
	);
break;

case E1_TAG:
				lua::push<double>(
					L, 
					op(*((e1_t *)elem1->elem), *((e1_t *)elem2->elem))
				);
			
break;

case E2_TAG:
	Glue<E3Elem>::push(
		L, 
		new E3Elem(
			BIVECTOR_TAG,
			new bivector(
				op(*((e1_t *)elem1->elem), *((e2_t *)elem2->elem))
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
				op(mv(*((e1_t *)elem1->elem)), mv(*((e3_t *)elem2->elem)))
			)
		)
	);
break;

case I3_TAG:
	Glue<E3Elem>::push(
		L, 
		new E3Elem(
			MV_TAG,
			new mv(
				op(mv(*((e1_t *)elem1->elem)), mv(*((I3_t *)elem2->elem)))
			)
		)
	);
break;

case VECTOR_TAG:
	Glue<E3Elem>::push(
		L, 
		new E3Elem(
			MV_TAG,
			new mv(
				op(mv(*((e1_t *)elem1->elem)), mv(*((vector *)elem2->elem)))
			)
		)
	);
break;

case BIVECTOR_TAG:
	Glue<E3Elem>::push(
		L, 
		new E3Elem(
			MV_TAG,
			new mv(
				op(mv(*((e1_t *)elem1->elem)), mv(*((bivector *)elem2->elem)))
			)
		)
	);
break;

case TRIVECTOR_TAG:
	Glue<E3Elem>::push(
		L, 
		new E3Elem(
			MV_TAG,
			new mv(
				op(mv(*((e1_t *)elem1->elem)), mv(*((trivector *)elem2->elem)))
			)
		)
	);
break;

case ROTOR_TAG:
	Glue<E3Elem>::push(
		L, 
		new E3Elem(
			MV_TAG,
			new mv(
				op(mv(*((e1_t *)elem1->elem)), mv(*((rotor *)elem2->elem)))
			)
		)
	);
break;

case ODDVERSOR_TAG:
	Glue<E3Elem>::push(
		L, 
		new E3Elem(
			MV_TAG,
			new mv(
				op(mv(*((e1_t *)elem1->elem)), mv(*((oddVersor *)elem2->elem)))
			)
		)
	);
break;

case DOUBLE_TAG:
	Glue<E3Elem>::push(
		L, 
		new E3Elem(
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
	Glue<E3Elem>::push(
		L, 
		new E3Elem(
			MV_TAG,
			new mv(
				op(mv(*((e2_t *)elem1->elem)), mv(*((mv *)elem2->elem)))
			)
		)
	);
break;

case E1_TAG:
	Glue<E3Elem>::push(
		L, 
		new E3Elem(
			MV_TAG,
			new mv(
				op(mv(*((e2_t *)elem1->elem)), mv(*((e1_t *)elem2->elem)))
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
				op(mv(*((e2_t *)elem1->elem)), mv(*((e2_t *)elem2->elem)))
			)
		)
	);
break;

case E3_TAG:
	Glue<E3Elem>::push(
		L, 
		new E3Elem(
			BIVECTOR_TAG,
			new bivector(
				op(*((e2_t *)elem1->elem), *((e3_t *)elem2->elem))
			)
		)
	);
break;

case I3_TAG:
	Glue<E3Elem>::push(
		L, 
		new E3Elem(
			MV_TAG,
			new mv(
				op(mv(*((e2_t *)elem1->elem)), mv(*((I3_t *)elem2->elem)))
			)
		)
	);
break;

case VECTOR_TAG:
	Glue<E3Elem>::push(
		L, 
		new E3Elem(
			MV_TAG,
			new mv(
				op(mv(*((e2_t *)elem1->elem)), mv(*((vector *)elem2->elem)))
			)
		)
	);
break;

case BIVECTOR_TAG:
	Glue<E3Elem>::push(
		L, 
		new E3Elem(
			MV_TAG,
			new mv(
				op(mv(*((e2_t *)elem1->elem)), mv(*((bivector *)elem2->elem)))
			)
		)
	);
break;

case TRIVECTOR_TAG:
	Glue<E3Elem>::push(
		L, 
		new E3Elem(
			MV_TAG,
			new mv(
				op(mv(*((e2_t *)elem1->elem)), mv(*((trivector *)elem2->elem)))
			)
		)
	);
break;

case ROTOR_TAG:
	Glue<E3Elem>::push(
		L, 
		new E3Elem(
			MV_TAG,
			new mv(
				op(mv(*((e2_t *)elem1->elem)), mv(*((rotor *)elem2->elem)))
			)
		)
	);
break;

case ODDVERSOR_TAG:
	Glue<E3Elem>::push(
		L, 
		new E3Elem(
			MV_TAG,
			new mv(
				op(mv(*((e2_t *)elem1->elem)), mv(*((oddVersor *)elem2->elem)))
			)
		)
	);
break;

case DOUBLE_TAG:
	Glue<E3Elem>::push(
		L, 
		new E3Elem(
			MV_TAG,
			new mv(
				op(mv(*((e2_t *)elem1->elem)), *((double *)elem2))
			)
		)
	);
break;
}
break;

case E3_TAG:
switch(tag2) {
case MV_TAG:
	Glue<E3Elem>::push(
		L, 
		new E3Elem(
			MV_TAG,
			new mv(
				op(mv(*((e3_t *)elem1->elem)), mv(*((mv *)elem2->elem)))
			)
		)
	);
break;

case E1_TAG:
	Glue<E3Elem>::push(
		L, 
		new E3Elem(
			MV_TAG,
			new mv(
				op(mv(*((e3_t *)elem1->elem)), mv(*((e1_t *)elem2->elem)))
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
				op(mv(*((e3_t *)elem1->elem)), mv(*((e2_t *)elem2->elem)))
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
				op(mv(*((e3_t *)elem1->elem)), mv(*((e3_t *)elem2->elem)))
			)
		)
	);
break;

case I3_TAG:
	Glue<E3Elem>::push(
		L, 
		new E3Elem(
			MV_TAG,
			new mv(
				op(mv(*((e3_t *)elem1->elem)), mv(*((I3_t *)elem2->elem)))
			)
		)
	);
break;

case VECTOR_TAG:
	Glue<E3Elem>::push(
		L, 
		new E3Elem(
			MV_TAG,
			new mv(
				op(mv(*((e3_t *)elem1->elem)), mv(*((vector *)elem2->elem)))
			)
		)
	);
break;

case BIVECTOR_TAG:
	Glue<E3Elem>::push(
		L, 
		new E3Elem(
			MV_TAG,
			new mv(
				op(mv(*((e3_t *)elem1->elem)), mv(*((bivector *)elem2->elem)))
			)
		)
	);
break;

case TRIVECTOR_TAG:
	Glue<E3Elem>::push(
		L, 
		new E3Elem(
			MV_TAG,
			new mv(
				op(mv(*((e3_t *)elem1->elem)), mv(*((trivector *)elem2->elem)))
			)
		)
	);
break;

case ROTOR_TAG:
	Glue<E3Elem>::push(
		L, 
		new E3Elem(
			MV_TAG,
			new mv(
				op(mv(*((e3_t *)elem1->elem)), mv(*((rotor *)elem2->elem)))
			)
		)
	);
break;

case ODDVERSOR_TAG:
	Glue<E3Elem>::push(
		L, 
		new E3Elem(
			MV_TAG,
			new mv(
				op(mv(*((e3_t *)elem1->elem)), mv(*((oddVersor *)elem2->elem)))
			)
		)
	);
break;

case DOUBLE_TAG:
	Glue<E3Elem>::push(
		L, 
		new E3Elem(
			MV_TAG,
			new mv(
				op(mv(*((e3_t *)elem1->elem)), *((double *)elem2))
			)
		)
	);
break;
}
break;

case I3_TAG:
switch(tag2) {
case MV_TAG:
	Glue<E3Elem>::push(
		L, 
		new E3Elem(
			MV_TAG,
			new mv(
				op(mv(*((I3_t *)elem1->elem)), mv(*((mv *)elem2->elem)))
			)
		)
	);
break;

case E1_TAG:
	Glue<E3Elem>::push(
		L, 
		new E3Elem(
			MV_TAG,
			new mv(
				op(mv(*((I3_t *)elem1->elem)), mv(*((e1_t *)elem2->elem)))
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
				op(mv(*((I3_t *)elem1->elem)), mv(*((e2_t *)elem2->elem)))
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
				op(mv(*((I3_t *)elem1->elem)), mv(*((e3_t *)elem2->elem)))
			)
		)
	);
break;

case I3_TAG:
	Glue<E3Elem>::push(
		L, 
		new E3Elem(
			MV_TAG,
			new mv(
				op(mv(*((I3_t *)elem1->elem)), mv(*((I3_t *)elem2->elem)))
			)
		)
	);
break;

case VECTOR_TAG:
	Glue<E3Elem>::push(
		L, 
		new E3Elem(
			MV_TAG,
			new mv(
				op(mv(*((I3_t *)elem1->elem)), mv(*((vector *)elem2->elem)))
			)
		)
	);
break;

case BIVECTOR_TAG:
	Glue<E3Elem>::push(
		L, 
		new E3Elem(
			MV_TAG,
			new mv(
				op(mv(*((I3_t *)elem1->elem)), mv(*((bivector *)elem2->elem)))
			)
		)
	);
break;

case TRIVECTOR_TAG:
	Glue<E3Elem>::push(
		L, 
		new E3Elem(
			MV_TAG,
			new mv(
				op(mv(*((I3_t *)elem1->elem)), mv(*((trivector *)elem2->elem)))
			)
		)
	);
break;

case ROTOR_TAG:
	Glue<E3Elem>::push(
		L, 
		new E3Elem(
			MV_TAG,
			new mv(
				op(mv(*((I3_t *)elem1->elem)), mv(*((rotor *)elem2->elem)))
			)
		)
	);
break;

case ODDVERSOR_TAG:
	Glue<E3Elem>::push(
		L, 
		new E3Elem(
			MV_TAG,
			new mv(
				op(mv(*((I3_t *)elem1->elem)), mv(*((oddVersor *)elem2->elem)))
			)
		)
	);
break;

case DOUBLE_TAG:
	Glue<E3Elem>::push(
		L, 
		new E3Elem(
			MV_TAG,
			new mv(
				op(mv(*((I3_t *)elem1->elem)), *((double *)elem2))
			)
		)
	);
break;
}
break;

case VECTOR_TAG:
switch(tag2) {
case MV_TAG:
	Glue<E3Elem>::push(
		L, 
		new E3Elem(
			MV_TAG,
			new mv(
				op(mv(*((vector *)elem1->elem)), mv(*((mv *)elem2->elem)))
			)
		)
	);
break;

case E1_TAG:
	Glue<E3Elem>::push(
		L, 
		new E3Elem(
			MV_TAG,
			new mv(
				op(mv(*((vector *)elem1->elem)), mv(*((e1_t *)elem2->elem)))
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
				op(mv(*((vector *)elem1->elem)), mv(*((e2_t *)elem2->elem)))
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
				op(mv(*((vector *)elem1->elem)), mv(*((e3_t *)elem2->elem)))
			)
		)
	);
break;

case I3_TAG:
	Glue<E3Elem>::push(
		L, 
		new E3Elem(
			MV_TAG,
			new mv(
				op(mv(*((vector *)elem1->elem)), mv(*((I3_t *)elem2->elem)))
			)
		)
	);
break;

case VECTOR_TAG:
	Glue<E3Elem>::push(
		L, 
		new E3Elem(
			BIVECTOR_TAG,
			new bivector(
				op(*((vector *)elem1->elem), *((vector *)elem2->elem))
			)
		)
	);
break;

case BIVECTOR_TAG:
	Glue<E3Elem>::push(
		L, 
		new E3Elem(
			MV_TAG,
			new mv(
				op(mv(*((vector *)elem1->elem)), mv(*((bivector *)elem2->elem)))
			)
		)
	);
break;

case TRIVECTOR_TAG:
				lua::push<double>(
					L, 
					op(*((vector *)elem1->elem), *((trivector *)elem2->elem))
				);
			
break;

case ROTOR_TAG:
	Glue<E3Elem>::push(
		L, 
		new E3Elem(
			ODDVERSOR_TAG,
			new oddVersor(
				op(*((vector *)elem1->elem), *((rotor *)elem2->elem))
			)
		)
	);
break;

case ODDVERSOR_TAG:
	Glue<E3Elem>::push(
		L, 
		new E3Elem(
			MV_TAG,
			new mv(
				op(mv(*((vector *)elem1->elem)), mv(*((oddVersor *)elem2->elem)))
			)
		)
	);
break;

case DOUBLE_TAG:
	Glue<E3Elem>::push(
		L, 
		new E3Elem(
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
	Glue<E3Elem>::push(
		L, 
		new E3Elem(
			MV_TAG,
			new mv(
				op(mv(*((bivector *)elem1->elem)), mv(*((mv *)elem2->elem)))
			)
		)
	);
break;

case E1_TAG:
	Glue<E3Elem>::push(
		L, 
		new E3Elem(
			MV_TAG,
			new mv(
				op(mv(*((bivector *)elem1->elem)), mv(*((e1_t *)elem2->elem)))
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
				op(mv(*((bivector *)elem1->elem)), mv(*((e2_t *)elem2->elem)))
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
				op(mv(*((bivector *)elem1->elem)), mv(*((e3_t *)elem2->elem)))
			)
		)
	);
break;

case I3_TAG:
	Glue<E3Elem>::push(
		L, 
		new E3Elem(
			MV_TAG,
			new mv(
				op(mv(*((bivector *)elem1->elem)), mv(*((I3_t *)elem2->elem)))
			)
		)
	);
break;

case VECTOR_TAG:
	Glue<E3Elem>::push(
		L, 
		new E3Elem(
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

case TRIVECTOR_TAG:
	Glue<E3Elem>::push(
		L, 
		new E3Elem(
			MV_TAG,
			new mv(
				op(mv(*((bivector *)elem1->elem)), mv(*((trivector *)elem2->elem)))
			)
		)
	);
break;

case ROTOR_TAG:
	Glue<E3Elem>::push(
		L, 
		new E3Elem(
			MV_TAG,
			new mv(
				op(mv(*((bivector *)elem1->elem)), mv(*((rotor *)elem2->elem)))
			)
		)
	);
break;

case ODDVERSOR_TAG:
	Glue<E3Elem>::push(
		L, 
		new E3Elem(
			MV_TAG,
			new mv(
				op(mv(*((bivector *)elem1->elem)), mv(*((oddVersor *)elem2->elem)))
			)
		)
	);
break;

case DOUBLE_TAG:
	Glue<E3Elem>::push(
		L, 
		new E3Elem(
			MV_TAG,
			new mv(
				op(mv(*((bivector *)elem1->elem)), *((double *)elem2))
			)
		)
	);
break;
}
break;

case TRIVECTOR_TAG:
switch(tag2) {
case MV_TAG:
	Glue<E3Elem>::push(
		L, 
		new E3Elem(
			MV_TAG,
			new mv(
				op(mv(*((trivector *)elem1->elem)), mv(*((mv *)elem2->elem)))
			)
		)
	);
break;

case E1_TAG:
	Glue<E3Elem>::push(
		L, 
		new E3Elem(
			MV_TAG,
			new mv(
				op(mv(*((trivector *)elem1->elem)), mv(*((e1_t *)elem2->elem)))
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
				op(mv(*((trivector *)elem1->elem)), mv(*((e2_t *)elem2->elem)))
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
				op(mv(*((trivector *)elem1->elem)), mv(*((e3_t *)elem2->elem)))
			)
		)
	);
break;

case I3_TAG:
	Glue<E3Elem>::push(
		L, 
		new E3Elem(
			MV_TAG,
			new mv(
				op(mv(*((trivector *)elem1->elem)), mv(*((I3_t *)elem2->elem)))
			)
		)
	);
break;

case VECTOR_TAG:
	Glue<E3Elem>::push(
		L, 
		new E3Elem(
			MV_TAG,
			new mv(
				op(mv(*((trivector *)elem1->elem)), mv(*((vector *)elem2->elem)))
			)
		)
	);
break;

case BIVECTOR_TAG:
	Glue<E3Elem>::push(
		L, 
		new E3Elem(
			MV_TAG,
			new mv(
				op(mv(*((trivector *)elem1->elem)), mv(*((bivector *)elem2->elem)))
			)
		)
	);
break;

case TRIVECTOR_TAG:
	Glue<E3Elem>::push(
		L, 
		new E3Elem(
			MV_TAG,
			new mv(
				op(mv(*((trivector *)elem1->elem)), mv(*((trivector *)elem2->elem)))
			)
		)
	);
break;

case ROTOR_TAG:
	Glue<E3Elem>::push(
		L, 
		new E3Elem(
			MV_TAG,
			new mv(
				op(mv(*((trivector *)elem1->elem)), mv(*((rotor *)elem2->elem)))
			)
		)
	);
break;

case ODDVERSOR_TAG:
	Glue<E3Elem>::push(
		L, 
		new E3Elem(
			MV_TAG,
			new mv(
				op(mv(*((trivector *)elem1->elem)), mv(*((oddVersor *)elem2->elem)))
			)
		)
	);
break;

case DOUBLE_TAG:
	Glue<E3Elem>::push(
		L, 
		new E3Elem(
			MV_TAG,
			new mv(
				op(mv(*((trivector *)elem1->elem)), *((double *)elem2))
			)
		)
	);
break;
}
break;

case ROTOR_TAG:
switch(tag2) {
case MV_TAG:
	Glue<E3Elem>::push(
		L, 
		new E3Elem(
			MV_TAG,
			new mv(
				op(mv(*((rotor *)elem1->elem)), mv(*((mv *)elem2->elem)))
			)
		)
	);
break;

case E1_TAG:
	Glue<E3Elem>::push(
		L, 
		new E3Elem(
			MV_TAG,
			new mv(
				op(mv(*((rotor *)elem1->elem)), mv(*((e1_t *)elem2->elem)))
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
				op(mv(*((rotor *)elem1->elem)), mv(*((e2_t *)elem2->elem)))
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
				op(mv(*((rotor *)elem1->elem)), mv(*((e3_t *)elem2->elem)))
			)
		)
	);
break;

case I3_TAG:
	Glue<E3Elem>::push(
		L, 
		new E3Elem(
			MV_TAG,
			new mv(
				op(mv(*((rotor *)elem1->elem)), mv(*((I3_t *)elem2->elem)))
			)
		)
	);
break;

case VECTOR_TAG:
	Glue<E3Elem>::push(
		L, 
		new E3Elem(
			MV_TAG,
			new mv(
				op(mv(*((rotor *)elem1->elem)), mv(*((vector *)elem2->elem)))
			)
		)
	);
break;

case BIVECTOR_TAG:
	Glue<E3Elem>::push(
		L, 
		new E3Elem(
			MV_TAG,
			new mv(
				op(mv(*((rotor *)elem1->elem)), mv(*((bivector *)elem2->elem)))
			)
		)
	);
break;

case TRIVECTOR_TAG:
	Glue<E3Elem>::push(
		L, 
		new E3Elem(
			MV_TAG,
			new mv(
				op(mv(*((rotor *)elem1->elem)), mv(*((trivector *)elem2->elem)))
			)
		)
	);
break;

case ROTOR_TAG:
	Glue<E3Elem>::push(
		L, 
		new E3Elem(
			MV_TAG,
			new mv(
				op(mv(*((rotor *)elem1->elem)), mv(*((rotor *)elem2->elem)))
			)
		)
	);
break;

case ODDVERSOR_TAG:
	Glue<E3Elem>::push(
		L, 
		new E3Elem(
			MV_TAG,
			new mv(
				op(mv(*((rotor *)elem1->elem)), mv(*((oddVersor *)elem2->elem)))
			)
		)
	);
break;

case DOUBLE_TAG:
	Glue<E3Elem>::push(
		L, 
		new E3Elem(
			MV_TAG,
			new mv(
				op(mv(*((rotor *)elem1->elem)), *((double *)elem2))
			)
		)
	);
break;
}
break;

case ODDVERSOR_TAG:
switch(tag2) {
case MV_TAG:
	Glue<E3Elem>::push(
		L, 
		new E3Elem(
			MV_TAG,
			new mv(
				op(mv(*((oddVersor *)elem1->elem)), mv(*((mv *)elem2->elem)))
			)
		)
	);
break;

case E1_TAG:
	Glue<E3Elem>::push(
		L, 
		new E3Elem(
			MV_TAG,
			new mv(
				op(mv(*((oddVersor *)elem1->elem)), mv(*((e1_t *)elem2->elem)))
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
				op(mv(*((oddVersor *)elem1->elem)), mv(*((e2_t *)elem2->elem)))
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
				op(mv(*((oddVersor *)elem1->elem)), mv(*((e3_t *)elem2->elem)))
			)
		)
	);
break;

case I3_TAG:
	Glue<E3Elem>::push(
		L, 
		new E3Elem(
			MV_TAG,
			new mv(
				op(mv(*((oddVersor *)elem1->elem)), mv(*((I3_t *)elem2->elem)))
			)
		)
	);
break;

case VECTOR_TAG:
	Glue<E3Elem>::push(
		L, 
		new E3Elem(
			MV_TAG,
			new mv(
				op(mv(*((oddVersor *)elem1->elem)), mv(*((vector *)elem2->elem)))
			)
		)
	);
break;

case BIVECTOR_TAG:
	Glue<E3Elem>::push(
		L, 
		new E3Elem(
			MV_TAG,
			new mv(
				op(mv(*((oddVersor *)elem1->elem)), mv(*((bivector *)elem2->elem)))
			)
		)
	);
break;

case TRIVECTOR_TAG:
	Glue<E3Elem>::push(
		L, 
		new E3Elem(
			MV_TAG,
			new mv(
				op(mv(*((oddVersor *)elem1->elem)), mv(*((trivector *)elem2->elem)))
			)
		)
	);
break;

case ROTOR_TAG:
	Glue<E3Elem>::push(
		L, 
		new E3Elem(
			MV_TAG,
			new mv(
				op(mv(*((oddVersor *)elem1->elem)), mv(*((rotor *)elem2->elem)))
			)
		)
	);
break;

case ODDVERSOR_TAG:
	Glue<E3Elem>::push(
		L, 
		new E3Elem(
			MV_TAG,
			new mv(
				op(mv(*((oddVersor *)elem1->elem)), mv(*((oddVersor *)elem2->elem)))
			)
		)
	);
break;

case DOUBLE_TAG:
	Glue<E3Elem>::push(
		L, 
		new E3Elem(
			MV_TAG,
			new mv(
				op(mv(*((oddVersor *)elem1->elem)), *((double *)elem2))
			)
		)
	);
break;
}
break;

case DOUBLE_TAG:
switch(tag2) {
case MV_TAG:
	Glue<E3Elem>::push(
		L, 
		new E3Elem(
			MV_TAG,
			new mv(
				op(*((double *)elem1), mv(*((mv *)elem2->elem)))
			)
		)
	);
break;

case E1_TAG:
	Glue<E3Elem>::push(
		L, 
		new E3Elem(
			MV_TAG,
			new mv(
				op(*((double *)elem1), mv(*((e1_t *)elem2->elem)))
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
				op(*((double *)elem1), mv(*((e2_t *)elem2->elem)))
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
				op(*((double *)elem1), mv(*((e3_t *)elem2->elem)))
			)
		)
	);
break;

case I3_TAG:
	Glue<E3Elem>::push(
		L, 
		new E3Elem(
			MV_TAG,
			new mv(
				op(*((double *)elem1), mv(*((I3_t *)elem2->elem)))
			)
		)
	);
break;

case VECTOR_TAG:
	Glue<E3Elem>::push(
		L, 
		new E3Elem(
			MV_TAG,
			new mv(
				op(*((double *)elem1), mv(*((vector *)elem2->elem)))
			)
		)
	);
break;

case BIVECTOR_TAG:
	Glue<E3Elem>::push(
		L, 
		new E3Elem(
			MV_TAG,
			new mv(
				op(*((double *)elem1), mv(*((bivector *)elem2->elem)))
			)
		)
	);
break;

case TRIVECTOR_TAG:
	Glue<E3Elem>::push(
		L, 
		new E3Elem(
			MV_TAG,
			new mv(
				op(*((double *)elem1), mv(*((trivector *)elem2->elem)))
			)
		)
	);
break;

case ROTOR_TAG:
	Glue<E3Elem>::push(
		L, 
		new E3Elem(
			MV_TAG,
			new mv(
				op(*((double *)elem1), mv(*((rotor *)elem2->elem)))
			)
		)
	);
break;

case ODDVERSOR_TAG:
	Glue<E3Elem>::push(
		L, 
		new E3Elem(
			MV_TAG,
			new mv(
				op(*((double *)elem1), mv(*((oddVersor *)elem2->elem)))
			)
		)
	);
break;

case DOUBLE_TAG:
	Glue<E3Elem>::push(
		L, 
		new E3Elem(
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


int e3space_gp(lua_State *L) {
	double d1, d2;
	E3Tag tag1;
	E3Tag tag2;
	E3Elem *elem1 = Glue<E3Elem>::to(L, 1);
	E3Elem *elem2 = Glue<E3Elem>::to(L, 2);
	
	
	if(!elem1) {
		if(lua::is<double>(L, 1)) {
			d1 = lua::to<double>(L, 1);
			elem1 = (E3Elem *)(&d1);
			tag1 = DOUBLE_TAG;
		}
		else {
			luaL_error(L, "e3.gp: invalid arguments");
		}
	}
	else {
		tag1 = elem1->tag;
	}
	
	if(!elem2) {
		if(lua::is<double>(L, 2)) {
			d2 = lua::to<double>(L, 2);
			elem2 = (E3Elem *)(&d2);
			tag2 = DOUBLE_TAG;
		}
		else {
			luaL_error(L, "e3.gp: invalid arguments");
		}
	}
	else {
		tag2 = elem2->tag;
	}
	
switch(tag1) {
case MV_TAG:
switch(tag2) {
case MV_TAG:
	Glue<E3Elem>::push(
		L, 
		new E3Elem(
			MV_TAG,
			new mv(
				gp(*((mv *)elem1->elem), *((mv *)elem2->elem))
			)
		)
	);
break;

case E1_TAG:
	Glue<E3Elem>::push(
		L, 
		new E3Elem(
			MV_TAG,
			new mv(
				gp(mv(*((mv *)elem1->elem)), mv(*((e1_t *)elem2->elem)))
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
				gp(mv(*((mv *)elem1->elem)), mv(*((e2_t *)elem2->elem)))
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
				gp(mv(*((mv *)elem1->elem)), mv(*((e3_t *)elem2->elem)))
			)
		)
	);
break;

case I3_TAG:
	Glue<E3Elem>::push(
		L, 
		new E3Elem(
			MV_TAG,
			new mv(
				gp(mv(*((mv *)elem1->elem)), mv(*((I3_t *)elem2->elem)))
			)
		)
	);
break;

case VECTOR_TAG:
	Glue<E3Elem>::push(
		L, 
		new E3Elem(
			MV_TAG,
			new mv(
				gp(mv(*((mv *)elem1->elem)), mv(*((vector *)elem2->elem)))
			)
		)
	);
break;

case BIVECTOR_TAG:
	Glue<E3Elem>::push(
		L, 
		new E3Elem(
			MV_TAG,
			new mv(
				gp(mv(*((mv *)elem1->elem)), mv(*((bivector *)elem2->elem)))
			)
		)
	);
break;

case TRIVECTOR_TAG:
	Glue<E3Elem>::push(
		L, 
		new E3Elem(
			MV_TAG,
			new mv(
				gp(mv(*((mv *)elem1->elem)), mv(*((trivector *)elem2->elem)))
			)
		)
	);
break;

case ROTOR_TAG:
	Glue<E3Elem>::push(
		L, 
		new E3Elem(
			MV_TAG,
			new mv(
				gp(mv(*((mv *)elem1->elem)), mv(*((rotor *)elem2->elem)))
			)
		)
	);
break;

case ODDVERSOR_TAG:
	Glue<E3Elem>::push(
		L, 
		new E3Elem(
			MV_TAG,
			new mv(
				gp(mv(*((mv *)elem1->elem)), mv(*((oddVersor *)elem2->elem)))
			)
		)
	);
break;

case DOUBLE_TAG:
	Glue<E3Elem>::push(
		L, 
		new E3Elem(
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
	Glue<E3Elem>::push(
		L, 
		new E3Elem(
			MV_TAG,
			new mv(
				gp(mv(*((e1_t *)elem1->elem)), mv(*((mv *)elem2->elem)))
			)
		)
	);
break;

case E1_TAG:
	Glue<E3Elem>::push(
		L, 
		new E3Elem(
			MV_TAG,
			new mv(
				gp(mv(*((e1_t *)elem1->elem)), mv(*((e1_t *)elem2->elem)))
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
				gp(mv(*((e1_t *)elem1->elem)), mv(*((e2_t *)elem2->elem)))
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
				gp(mv(*((e1_t *)elem1->elem)), mv(*((e3_t *)elem2->elem)))
			)
		)
	);
break;

case I3_TAG:
	Glue<E3Elem>::push(
		L, 
		new E3Elem(
			MV_TAG,
			new mv(
				gp(mv(*((e1_t *)elem1->elem)), mv(*((I3_t *)elem2->elem)))
			)
		)
	);
break;

case VECTOR_TAG:
	Glue<E3Elem>::push(
		L, 
		new E3Elem(
			MV_TAG,
			new mv(
				gp(mv(*((e1_t *)elem1->elem)), mv(*((vector *)elem2->elem)))
			)
		)
	);
break;

case BIVECTOR_TAG:
	Glue<E3Elem>::push(
		L, 
		new E3Elem(
			MV_TAG,
			new mv(
				gp(mv(*((e1_t *)elem1->elem)), mv(*((bivector *)elem2->elem)))
			)
		)
	);
break;

case TRIVECTOR_TAG:
	Glue<E3Elem>::push(
		L, 
		new E3Elem(
			MV_TAG,
			new mv(
				gp(mv(*((e1_t *)elem1->elem)), mv(*((trivector *)elem2->elem)))
			)
		)
	);
break;

case ROTOR_TAG:
	Glue<E3Elem>::push(
		L, 
		new E3Elem(
			ODDVERSOR_TAG,
			new oddVersor(
				gp(*((e1_t *)elem1->elem), *((rotor *)elem2->elem))
			)
		)
	);
break;

case ODDVERSOR_TAG:
	Glue<E3Elem>::push(
		L, 
		new E3Elem(
			MV_TAG,
			new mv(
				gp(mv(*((e1_t *)elem1->elem)), mv(*((oddVersor *)elem2->elem)))
			)
		)
	);
break;

case DOUBLE_TAG:
	Glue<E3Elem>::push(
		L, 
		new E3Elem(
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
	Glue<E3Elem>::push(
		L, 
		new E3Elem(
			MV_TAG,
			new mv(
				gp(mv(*((e2_t *)elem1->elem)), mv(*((mv *)elem2->elem)))
			)
		)
	);
break;

case E1_TAG:
	Glue<E3Elem>::push(
		L, 
		new E3Elem(
			MV_TAG,
			new mv(
				gp(mv(*((e2_t *)elem1->elem)), mv(*((e1_t *)elem2->elem)))
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
				gp(mv(*((e2_t *)elem1->elem)), mv(*((e2_t *)elem2->elem)))
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
				gp(mv(*((e2_t *)elem1->elem)), mv(*((e3_t *)elem2->elem)))
			)
		)
	);
break;

case I3_TAG:
	Glue<E3Elem>::push(
		L, 
		new E3Elem(
			MV_TAG,
			new mv(
				gp(mv(*((e2_t *)elem1->elem)), mv(*((I3_t *)elem2->elem)))
			)
		)
	);
break;

case VECTOR_TAG:
	Glue<E3Elem>::push(
		L, 
		new E3Elem(
			MV_TAG,
			new mv(
				gp(mv(*((e2_t *)elem1->elem)), mv(*((vector *)elem2->elem)))
			)
		)
	);
break;

case BIVECTOR_TAG:
	Glue<E3Elem>::push(
		L, 
		new E3Elem(
			MV_TAG,
			new mv(
				gp(mv(*((e2_t *)elem1->elem)), mv(*((bivector *)elem2->elem)))
			)
		)
	);
break;

case TRIVECTOR_TAG:
	Glue<E3Elem>::push(
		L, 
		new E3Elem(
			MV_TAG,
			new mv(
				gp(mv(*((e2_t *)elem1->elem)), mv(*((trivector *)elem2->elem)))
			)
		)
	);
break;

case ROTOR_TAG:
	Glue<E3Elem>::push(
		L, 
		new E3Elem(
			MV_TAG,
			new mv(
				gp(mv(*((e2_t *)elem1->elem)), mv(*((rotor *)elem2->elem)))
			)
		)
	);
break;

case ODDVERSOR_TAG:
	Glue<E3Elem>::push(
		L, 
		new E3Elem(
			MV_TAG,
			new mv(
				gp(mv(*((e2_t *)elem1->elem)), mv(*((oddVersor *)elem2->elem)))
			)
		)
	);
break;

case DOUBLE_TAG:
	Glue<E3Elem>::push(
		L, 
		new E3Elem(
			MV_TAG,
			new mv(
				gp(mv(*((e2_t *)elem1->elem)), *((double *)elem2))
			)
		)
	);
break;
}
break;

case E3_TAG:
switch(tag2) {
case MV_TAG:
	Glue<E3Elem>::push(
		L, 
		new E3Elem(
			MV_TAG,
			new mv(
				gp(mv(*((e3_t *)elem1->elem)), mv(*((mv *)elem2->elem)))
			)
		)
	);
break;

case E1_TAG:
	Glue<E3Elem>::push(
		L, 
		new E3Elem(
			MV_TAG,
			new mv(
				gp(mv(*((e3_t *)elem1->elem)), mv(*((e1_t *)elem2->elem)))
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
				gp(mv(*((e3_t *)elem1->elem)), mv(*((e2_t *)elem2->elem)))
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
				gp(mv(*((e3_t *)elem1->elem)), mv(*((e3_t *)elem2->elem)))
			)
		)
	);
break;

case I3_TAG:
	Glue<E3Elem>::push(
		L, 
		new E3Elem(
			MV_TAG,
			new mv(
				gp(mv(*((e3_t *)elem1->elem)), mv(*((I3_t *)elem2->elem)))
			)
		)
	);
break;

case VECTOR_TAG:
	Glue<E3Elem>::push(
		L, 
		new E3Elem(
			MV_TAG,
			new mv(
				gp(mv(*((e3_t *)elem1->elem)), mv(*((vector *)elem2->elem)))
			)
		)
	);
break;

case BIVECTOR_TAG:
	Glue<E3Elem>::push(
		L, 
		new E3Elem(
			MV_TAG,
			new mv(
				gp(mv(*((e3_t *)elem1->elem)), mv(*((bivector *)elem2->elem)))
			)
		)
	);
break;

case TRIVECTOR_TAG:
	Glue<E3Elem>::push(
		L, 
		new E3Elem(
			MV_TAG,
			new mv(
				gp(mv(*((e3_t *)elem1->elem)), mv(*((trivector *)elem2->elem)))
			)
		)
	);
break;

case ROTOR_TAG:
	Glue<E3Elem>::push(
		L, 
		new E3Elem(
			MV_TAG,
			new mv(
				gp(mv(*((e3_t *)elem1->elem)), mv(*((rotor *)elem2->elem)))
			)
		)
	);
break;

case ODDVERSOR_TAG:
	Glue<E3Elem>::push(
		L, 
		new E3Elem(
			MV_TAG,
			new mv(
				gp(mv(*((e3_t *)elem1->elem)), mv(*((oddVersor *)elem2->elem)))
			)
		)
	);
break;

case DOUBLE_TAG:
	Glue<E3Elem>::push(
		L, 
		new E3Elem(
			MV_TAG,
			new mv(
				gp(mv(*((e3_t *)elem1->elem)), *((double *)elem2))
			)
		)
	);
break;
}
break;

case I3_TAG:
switch(tag2) {
case MV_TAG:
	Glue<E3Elem>::push(
		L, 
		new E3Elem(
			MV_TAG,
			new mv(
				gp(mv(*((I3_t *)elem1->elem)), mv(*((mv *)elem2->elem)))
			)
		)
	);
break;

case E1_TAG:
	Glue<E3Elem>::push(
		L, 
		new E3Elem(
			MV_TAG,
			new mv(
				gp(mv(*((I3_t *)elem1->elem)), mv(*((e1_t *)elem2->elem)))
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
				gp(mv(*((I3_t *)elem1->elem)), mv(*((e2_t *)elem2->elem)))
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
				gp(mv(*((I3_t *)elem1->elem)), mv(*((e3_t *)elem2->elem)))
			)
		)
	);
break;

case I3_TAG:
	Glue<E3Elem>::push(
		L, 
		new E3Elem(
			MV_TAG,
			new mv(
				gp(mv(*((I3_t *)elem1->elem)), mv(*((I3_t *)elem2->elem)))
			)
		)
	);
break;

case VECTOR_TAG:
	Glue<E3Elem>::push(
		L, 
		new E3Elem(
			MV_TAG,
			new mv(
				gp(mv(*((I3_t *)elem1->elem)), mv(*((vector *)elem2->elem)))
			)
		)
	);
break;

case BIVECTOR_TAG:
	Glue<E3Elem>::push(
		L, 
		new E3Elem(
			MV_TAG,
			new mv(
				gp(mv(*((I3_t *)elem1->elem)), mv(*((bivector *)elem2->elem)))
			)
		)
	);
break;

case TRIVECTOR_TAG:
	Glue<E3Elem>::push(
		L, 
		new E3Elem(
			MV_TAG,
			new mv(
				gp(mv(*((I3_t *)elem1->elem)), mv(*((trivector *)elem2->elem)))
			)
		)
	);
break;

case ROTOR_TAG:
	Glue<E3Elem>::push(
		L, 
		new E3Elem(
			ODDVERSOR_TAG,
			new oddVersor(
				gp(*((I3_t *)elem1->elem), *((rotor *)elem2->elem))
			)
		)
	);
break;

case ODDVERSOR_TAG:
	Glue<E3Elem>::push(
		L, 
		new E3Elem(
			MV_TAG,
			new mv(
				gp(mv(*((I3_t *)elem1->elem)), mv(*((oddVersor *)elem2->elem)))
			)
		)
	);
break;

case DOUBLE_TAG:
	Glue<E3Elem>::push(
		L, 
		new E3Elem(
			MV_TAG,
			new mv(
				gp(mv(*((I3_t *)elem1->elem)), *((double *)elem2))
			)
		)
	);
break;
}
break;

case VECTOR_TAG:
switch(tag2) {
case MV_TAG:
	Glue<E3Elem>::push(
		L, 
		new E3Elem(
			MV_TAG,
			new mv(
				gp(mv(*((vector *)elem1->elem)), mv(*((mv *)elem2->elem)))
			)
		)
	);
break;

case E1_TAG:
	Glue<E3Elem>::push(
		L, 
		new E3Elem(
			MV_TAG,
			new mv(
				gp(mv(*((vector *)elem1->elem)), mv(*((e1_t *)elem2->elem)))
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
				gp(mv(*((vector *)elem1->elem)), mv(*((e2_t *)elem2->elem)))
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
				gp(mv(*((vector *)elem1->elem)), mv(*((e3_t *)elem2->elem)))
			)
		)
	);
break;

case I3_TAG:
	Glue<E3Elem>::push(
		L, 
		new E3Elem(
			MV_TAG,
			new mv(
				gp(mv(*((vector *)elem1->elem)), mv(*((I3_t *)elem2->elem)))
			)
		)
	);
break;

case VECTOR_TAG:
	Glue<E3Elem>::push(
		L, 
		new E3Elem(
			ROTOR_TAG,
			new rotor(
				gp(*((vector *)elem1->elem), *((vector *)elem2->elem))
			)
		)
	);
break;

case BIVECTOR_TAG:
	Glue<E3Elem>::push(
		L, 
		new E3Elem(
			MV_TAG,
			new mv(
				gp(mv(*((vector *)elem1->elem)), mv(*((bivector *)elem2->elem)))
			)
		)
	);
break;

case TRIVECTOR_TAG:
	Glue<E3Elem>::push(
		L, 
		new E3Elem(
			MV_TAG,
			new mv(
				gp(mv(*((vector *)elem1->elem)), mv(*((trivector *)elem2->elem)))
			)
		)
	);
break;

case ROTOR_TAG:
	Glue<E3Elem>::push(
		L, 
		new E3Elem(
			ODDVERSOR_TAG,
			new oddVersor(
				gp(*((vector *)elem1->elem), *((rotor *)elem2->elem))
			)
		)
	);
break;

case ODDVERSOR_TAG:
	Glue<E3Elem>::push(
		L, 
		new E3Elem(
			MV_TAG,
			new mv(
				gp(mv(*((vector *)elem1->elem)), mv(*((oddVersor *)elem2->elem)))
			)
		)
	);
break;

case DOUBLE_TAG:
	Glue<E3Elem>::push(
		L, 
		new E3Elem(
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
	Glue<E3Elem>::push(
		L, 
		new E3Elem(
			MV_TAG,
			new mv(
				gp(mv(*((bivector *)elem1->elem)), mv(*((mv *)elem2->elem)))
			)
		)
	);
break;

case E1_TAG:
	Glue<E3Elem>::push(
		L, 
		new E3Elem(
			ODDVERSOR_TAG,
			new oddVersor(
				gp(*((bivector *)elem1->elem), *((e1_t *)elem2->elem))
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
				gp(mv(*((bivector *)elem1->elem)), mv(*((e2_t *)elem2->elem)))
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
				gp(mv(*((bivector *)elem1->elem)), mv(*((e3_t *)elem2->elem)))
			)
		)
	);
break;

case I3_TAG:
	Glue<E3Elem>::push(
		L, 
		new E3Elem(
			MV_TAG,
			new mv(
				gp(mv(*((bivector *)elem1->elem)), mv(*((I3_t *)elem2->elem)))
			)
		)
	);
break;

case VECTOR_TAG:
	Glue<E3Elem>::push(
		L, 
		new E3Elem(
			MV_TAG,
			new mv(
				gp(mv(*((bivector *)elem1->elem)), mv(*((vector *)elem2->elem)))
			)
		)
	);
break;

case BIVECTOR_TAG:
	Glue<E3Elem>::push(
		L, 
		new E3Elem(
			ROTOR_TAG,
			new rotor(
				gp(*((bivector *)elem1->elem), *((bivector *)elem2->elem))
			)
		)
	);
break;

case TRIVECTOR_TAG:
	Glue<E3Elem>::push(
		L, 
		new E3Elem(
			MV_TAG,
			new mv(
				gp(mv(*((bivector *)elem1->elem)), mv(*((trivector *)elem2->elem)))
			)
		)
	);
break;

case ROTOR_TAG:
	Glue<E3Elem>::push(
		L, 
		new E3Elem(
			MV_TAG,
			new mv(
				gp(mv(*((bivector *)elem1->elem)), mv(*((rotor *)elem2->elem)))
			)
		)
	);
break;

case ODDVERSOR_TAG:
	Glue<E3Elem>::push(
		L, 
		new E3Elem(
			MV_TAG,
			new mv(
				gp(mv(*((bivector *)elem1->elem)), mv(*((oddVersor *)elem2->elem)))
			)
		)
	);
break;

case DOUBLE_TAG:
	Glue<E3Elem>::push(
		L, 
		new E3Elem(
			MV_TAG,
			new mv(
				gp(mv(*((bivector *)elem1->elem)), *((double *)elem2))
			)
		)
	);
break;
}
break;

case TRIVECTOR_TAG:
switch(tag2) {
case MV_TAG:
	Glue<E3Elem>::push(
		L, 
		new E3Elem(
			MV_TAG,
			new mv(
				gp(mv(*((trivector *)elem1->elem)), mv(*((mv *)elem2->elem)))
			)
		)
	);
break;

case E1_TAG:
	Glue<E3Elem>::push(
		L, 
		new E3Elem(
			MV_TAG,
			new mv(
				gp(mv(*((trivector *)elem1->elem)), mv(*((e1_t *)elem2->elem)))
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
				gp(mv(*((trivector *)elem1->elem)), mv(*((e2_t *)elem2->elem)))
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
				gp(mv(*((trivector *)elem1->elem)), mv(*((e3_t *)elem2->elem)))
			)
		)
	);
break;

case I3_TAG:
	Glue<E3Elem>::push(
		L, 
		new E3Elem(
			MV_TAG,
			new mv(
				gp(mv(*((trivector *)elem1->elem)), mv(*((I3_t *)elem2->elem)))
			)
		)
	);
break;

case VECTOR_TAG:
	Glue<E3Elem>::push(
		L, 
		new E3Elem(
			MV_TAG,
			new mv(
				gp(mv(*((trivector *)elem1->elem)), mv(*((vector *)elem2->elem)))
			)
		)
	);
break;

case BIVECTOR_TAG:
	Glue<E3Elem>::push(
		L, 
		new E3Elem(
			MV_TAG,
			new mv(
				gp(mv(*((trivector *)elem1->elem)), mv(*((bivector *)elem2->elem)))
			)
		)
	);
break;

case TRIVECTOR_TAG:
	Glue<E3Elem>::push(
		L, 
		new E3Elem(
			MV_TAG,
			new mv(
				gp(mv(*((trivector *)elem1->elem)), mv(*((trivector *)elem2->elem)))
			)
		)
	);
break;

case ROTOR_TAG:
	Glue<E3Elem>::push(
		L, 
		new E3Elem(
			MV_TAG,
			new mv(
				gp(mv(*((trivector *)elem1->elem)), mv(*((rotor *)elem2->elem)))
			)
		)
	);
break;

case ODDVERSOR_TAG:
	Glue<E3Elem>::push(
		L, 
		new E3Elem(
			MV_TAG,
			new mv(
				gp(mv(*((trivector *)elem1->elem)), mv(*((oddVersor *)elem2->elem)))
			)
		)
	);
break;

case DOUBLE_TAG:
	Glue<E3Elem>::push(
		L, 
		new E3Elem(
			MV_TAG,
			new mv(
				gp(mv(*((trivector *)elem1->elem)), *((double *)elem2))
			)
		)
	);
break;
}
break;

case ROTOR_TAG:
switch(tag2) {
case MV_TAG:
	Glue<E3Elem>::push(
		L, 
		new E3Elem(
			MV_TAG,
			new mv(
				gp(mv(*((rotor *)elem1->elem)), mv(*((mv *)elem2->elem)))
			)
		)
	);
break;

case E1_TAG:
	Glue<E3Elem>::push(
		L, 
		new E3Elem(
			MV_TAG,
			new mv(
				gp(mv(*((rotor *)elem1->elem)), mv(*((e1_t *)elem2->elem)))
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
				gp(mv(*((rotor *)elem1->elem)), mv(*((e2_t *)elem2->elem)))
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
				gp(mv(*((rotor *)elem1->elem)), mv(*((e3_t *)elem2->elem)))
			)
		)
	);
break;

case I3_TAG:
	Glue<E3Elem>::push(
		L, 
		new E3Elem(
			MV_TAG,
			new mv(
				gp(mv(*((rotor *)elem1->elem)), mv(*((I3_t *)elem2->elem)))
			)
		)
	);
break;

case VECTOR_TAG:
	Glue<E3Elem>::push(
		L, 
		new E3Elem(
			ODDVERSOR_TAG,
			new oddVersor(
				gp(*((rotor *)elem1->elem), *((vector *)elem2->elem))
			)
		)
	);
break;

case BIVECTOR_TAG:
	Glue<E3Elem>::push(
		L, 
		new E3Elem(
			MV_TAG,
			new mv(
				gp(mv(*((rotor *)elem1->elem)), mv(*((bivector *)elem2->elem)))
			)
		)
	);
break;

case TRIVECTOR_TAG:
	Glue<E3Elem>::push(
		L, 
		new E3Elem(
			MV_TAG,
			new mv(
				gp(mv(*((rotor *)elem1->elem)), mv(*((trivector *)elem2->elem)))
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
				gp(*((rotor *)elem1->elem), *((rotor *)elem2->elem))
			)
		)
	);
break;

case ODDVERSOR_TAG:
	Glue<E3Elem>::push(
		L, 
		new E3Elem(
			MV_TAG,
			new mv(
				gp(mv(*((rotor *)elem1->elem)), mv(*((oddVersor *)elem2->elem)))
			)
		)
	);
break;

case DOUBLE_TAG:
	Glue<E3Elem>::push(
		L, 
		new E3Elem(
			MV_TAG,
			new mv(
				gp(mv(*((rotor *)elem1->elem)), *((double *)elem2))
			)
		)
	);
break;
}
break;

case ODDVERSOR_TAG:
switch(tag2) {
case MV_TAG:
	Glue<E3Elem>::push(
		L, 
		new E3Elem(
			MV_TAG,
			new mv(
				gp(mv(*((oddVersor *)elem1->elem)), mv(*((mv *)elem2->elem)))
			)
		)
	);
break;

case E1_TAG:
	Glue<E3Elem>::push(
		L, 
		new E3Elem(
			MV_TAG,
			new mv(
				gp(mv(*((oddVersor *)elem1->elem)), mv(*((e1_t *)elem2->elem)))
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
				gp(mv(*((oddVersor *)elem1->elem)), mv(*((e2_t *)elem2->elem)))
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
				gp(mv(*((oddVersor *)elem1->elem)), mv(*((e3_t *)elem2->elem)))
			)
		)
	);
break;

case I3_TAG:
	Glue<E3Elem>::push(
		L, 
		new E3Elem(
			MV_TAG,
			new mv(
				gp(mv(*((oddVersor *)elem1->elem)), mv(*((I3_t *)elem2->elem)))
			)
		)
	);
break;

case VECTOR_TAG:
	Glue<E3Elem>::push(
		L, 
		new E3Elem(
			MV_TAG,
			new mv(
				gp(mv(*((oddVersor *)elem1->elem)), mv(*((vector *)elem2->elem)))
			)
		)
	);
break;

case BIVECTOR_TAG:
	Glue<E3Elem>::push(
		L, 
		new E3Elem(
			MV_TAG,
			new mv(
				gp(mv(*((oddVersor *)elem1->elem)), mv(*((bivector *)elem2->elem)))
			)
		)
	);
break;

case TRIVECTOR_TAG:
	Glue<E3Elem>::push(
		L, 
		new E3Elem(
			MV_TAG,
			new mv(
				gp(mv(*((oddVersor *)elem1->elem)), mv(*((trivector *)elem2->elem)))
			)
		)
	);
break;

case ROTOR_TAG:
	Glue<E3Elem>::push(
		L, 
		new E3Elem(
			MV_TAG,
			new mv(
				gp(mv(*((oddVersor *)elem1->elem)), mv(*((rotor *)elem2->elem)))
			)
		)
	);
break;

case ODDVERSOR_TAG:
	Glue<E3Elem>::push(
		L, 
		new E3Elem(
			MV_TAG,
			new mv(
				gp(mv(*((oddVersor *)elem1->elem)), mv(*((oddVersor *)elem2->elem)))
			)
		)
	);
break;

case DOUBLE_TAG:
	Glue<E3Elem>::push(
		L, 
		new E3Elem(
			MV_TAG,
			new mv(
				gp(mv(*((oddVersor *)elem1->elem)), *((double *)elem2))
			)
		)
	);
break;
}
break;

case DOUBLE_TAG:
switch(tag2) {
case MV_TAG:
	Glue<E3Elem>::push(
		L, 
		new E3Elem(
			MV_TAG,
			new mv(
				gp(*((double *)elem1), *((mv *)elem2->elem))
			)
		)
	);
break;

case E1_TAG:
	Glue<E3Elem>::push(
		L, 
		new E3Elem(
			MV_TAG,
			new mv(
				gp(*((double *)elem1), *((e1_t *)elem2->elem))
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
				gp(*((double *)elem1), *((e2_t *)elem2->elem))
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
				gp(*((double *)elem1), *((e3_t *)elem2->elem))
			)
		)
	);
break;

case I3_TAG:
	Glue<E3Elem>::push(
		L, 
		new E3Elem(
			MV_TAG,
			new mv(
				gp(*((double *)elem1), *((I3_t *)elem2->elem))
			)
		)
	);
break;

case VECTOR_TAG:
	/*Glue<E3Elem>::push(
		L, 
		new E3Elem(
			VECTOR_TAG,
			new vector(
				gp(*((double *)elem1), *((vector *)elem2->elem))
			)
		)
	);
	*/
break;

case BIVECTOR_TAG:
	Glue<E3Elem>::push(
		L, 
		new E3Elem(
			MV_TAG,
			new mv(
				gp(*((double *)elem1), *((bivector *)elem2->elem))
			)
		)
	);
break;

case TRIVECTOR_TAG:
	Glue<E3Elem>::push(
		L, 
		new E3Elem(
			MV_TAG,
			new mv(
				gp(*((double *)elem1), *((trivector *)elem2->elem))
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
				gp(*((double *)elem1), *((rotor *)elem2->elem))
			)
		)
	);
break;

case ODDVERSOR_TAG:
	Glue<E3Elem>::push(
		L, 
		new E3Elem(
			MV_TAG,
			new mv(
				gp(*((double *)elem1), *((oddVersor *)elem2->elem))
			)
		)
	);
break;

case DOUBLE_TAG:
	Glue<E3Elem>::push(
		L, 
		new E3Elem(
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


int e3space_lc(lua_State *L) {
	double d1, d2;
	E3Tag tag1;
	E3Tag tag2;
	E3Elem *elem1 = Glue<E3Elem>::to(L, 1);
	E3Elem *elem2 = Glue<E3Elem>::to(L, 2);
	
	
	if(!elem1) {
		if(lua::is<double>(L, 1)) {
			d1 = lua::to<double>(L, 1);
			elem1 = (E3Elem *)(&d1);
			tag1 = DOUBLE_TAG;
		}
		else {
			luaL_error(L, "e3.lc: invalid arguments");
		}
	}
	else {
		tag1 = elem1->tag;
	}
	
	if(!elem2) {
		if(lua::is<double>(L, 2)) {
			d2 = lua::to<double>(L, 2);
			elem2 = (E3Elem *)(&d2);
			tag2 = DOUBLE_TAG;
		}
		else {
			luaL_error(L, "e3.lc: invalid arguments");
		}
	}
	else {
		tag2 = elem2->tag;
	}
	
switch(tag1) {
case MV_TAG:
switch(tag2) {
case MV_TAG:
	Glue<E3Elem>::push(
		L, 
		new E3Elem(
			MV_TAG,
			new mv(
				lc(*((mv *)elem1->elem), *((mv *)elem2->elem))
			)
		)
	);
break;

case E1_TAG:
	Glue<E3Elem>::push(
		L, 
		new E3Elem(
			MV_TAG,
			new mv(
				lc(mv(*((mv *)elem1->elem)), mv(*((e1_t *)elem2->elem)))
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
				lc(mv(*((mv *)elem1->elem)), mv(*((e2_t *)elem2->elem)))
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
				lc(mv(*((mv *)elem1->elem)), mv(*((e3_t *)elem2->elem)))
			)
		)
	);
break;

case I3_TAG:
	Glue<E3Elem>::push(
		L, 
		new E3Elem(
			MV_TAG,
			new mv(
				lc(mv(*((mv *)elem1->elem)), mv(*((I3_t *)elem2->elem)))
			)
		)
	);
break;

case VECTOR_TAG:
	Glue<E3Elem>::push(
		L, 
		new E3Elem(
			MV_TAG,
			new mv(
				lc(mv(*((mv *)elem1->elem)), mv(*((vector *)elem2->elem)))
			)
		)
	);
break;

case BIVECTOR_TAG:
	Glue<E3Elem>::push(
		L, 
		new E3Elem(
			MV_TAG,
			new mv(
				lc(mv(*((mv *)elem1->elem)), mv(*((bivector *)elem2->elem)))
			)
		)
	);
break;

case TRIVECTOR_TAG:
	Glue<E3Elem>::push(
		L, 
		new E3Elem(
			MV_TAG,
			new mv(
				lc(mv(*((mv *)elem1->elem)), mv(*((trivector *)elem2->elem)))
			)
		)
	);
break;

case ROTOR_TAG:
	Glue<E3Elem>::push(
		L, 
		new E3Elem(
			MV_TAG,
			new mv(
				lc(mv(*((mv *)elem1->elem)), mv(*((rotor *)elem2->elem)))
			)
		)
	);
break;

case ODDVERSOR_TAG:
	Glue<E3Elem>::push(
		L, 
		new E3Elem(
			MV_TAG,
			new mv(
				lc(mv(*((mv *)elem1->elem)), mv(*((oddVersor *)elem2->elem)))
			)
		)
	);
break;

case DOUBLE_TAG:
	Glue<E3Elem>::push(
		L, 
		new E3Elem(
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
	Glue<E3Elem>::push(
		L, 
		new E3Elem(
			MV_TAG,
			new mv(
				lc(mv(*((e1_t *)elem1->elem)), mv(*((mv *)elem2->elem)))
			)
		)
	);
break;

case E1_TAG:
	Glue<E3Elem>::push(
		L, 
		new E3Elem(
			MV_TAG,
			new mv(
				lc(mv(*((e1_t *)elem1->elem)), mv(*((e1_t *)elem2->elem)))
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
				lc(mv(*((e1_t *)elem1->elem)), mv(*((e2_t *)elem2->elem)))
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
				lc(mv(*((e1_t *)elem1->elem)), mv(*((e3_t *)elem2->elem)))
			)
		)
	);
break;

case I3_TAG:
	Glue<E3Elem>::push(
		L, 
		new E3Elem(
			MV_TAG,
			new mv(
				lc(mv(*((e1_t *)elem1->elem)), mv(*((I3_t *)elem2->elem)))
			)
		)
	);
break;

case VECTOR_TAG:
	Glue<E3Elem>::push(
		L, 
		new E3Elem(
			MV_TAG,
			new mv(
				lc(mv(*((e1_t *)elem1->elem)), mv(*((vector *)elem2->elem)))
			)
		)
	);
break;

case BIVECTOR_TAG:
	Glue<E3Elem>::push(
		L, 
		new E3Elem(
			MV_TAG,
			new mv(
				lc(mv(*((e1_t *)elem1->elem)), mv(*((bivector *)elem2->elem)))
			)
		)
	);
break;

case TRIVECTOR_TAG:
	Glue<E3Elem>::push(
		L, 
		new E3Elem(
			MV_TAG,
			new mv(
				lc(mv(*((e1_t *)elem1->elem)), mv(*((trivector *)elem2->elem)))
			)
		)
	);
break;

case ROTOR_TAG:
	Glue<E3Elem>::push(
		L, 
		new E3Elem(
			MV_TAG,
			new mv(
				lc(mv(*((e1_t *)elem1->elem)), mv(*((rotor *)elem2->elem)))
			)
		)
	);
break;

case ODDVERSOR_TAG:
	Glue<E3Elem>::push(
		L, 
		new E3Elem(
			MV_TAG,
			new mv(
				lc(mv(*((e1_t *)elem1->elem)), mv(*((oddVersor *)elem2->elem)))
			)
		)
	);
break;

case DOUBLE_TAG:
	Glue<E3Elem>::push(
		L, 
		new E3Elem(
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
	Glue<E3Elem>::push(
		L, 
		new E3Elem(
			MV_TAG,
			new mv(
				lc(mv(*((e2_t *)elem1->elem)), mv(*((mv *)elem2->elem)))
			)
		)
	);
break;

case E1_TAG:
	Glue<E3Elem>::push(
		L, 
		new E3Elem(
			MV_TAG,
			new mv(
				lc(mv(*((e2_t *)elem1->elem)), mv(*((e1_t *)elem2->elem)))
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
				lc(mv(*((e2_t *)elem1->elem)), mv(*((e2_t *)elem2->elem)))
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
				lc(mv(*((e2_t *)elem1->elem)), mv(*((e3_t *)elem2->elem)))
			)
		)
	);
break;

case I3_TAG:
	Glue<E3Elem>::push(
		L, 
		new E3Elem(
			MV_TAG,
			new mv(
				lc(mv(*((e2_t *)elem1->elem)), mv(*((I3_t *)elem2->elem)))
			)
		)
	);
break;

case VECTOR_TAG:
	Glue<E3Elem>::push(
		L, 
		new E3Elem(
			MV_TAG,
			new mv(
				lc(mv(*((e2_t *)elem1->elem)), mv(*((vector *)elem2->elem)))
			)
		)
	);
break;

case BIVECTOR_TAG:
	Glue<E3Elem>::push(
		L, 
		new E3Elem(
			MV_TAG,
			new mv(
				lc(mv(*((e2_t *)elem1->elem)), mv(*((bivector *)elem2->elem)))
			)
		)
	);
break;

case TRIVECTOR_TAG:
	Glue<E3Elem>::push(
		L, 
		new E3Elem(
			MV_TAG,
			new mv(
				lc(mv(*((e2_t *)elem1->elem)), mv(*((trivector *)elem2->elem)))
			)
		)
	);
break;

case ROTOR_TAG:
	Glue<E3Elem>::push(
		L, 
		new E3Elem(
			MV_TAG,
			new mv(
				lc(mv(*((e2_t *)elem1->elem)), mv(*((rotor *)elem2->elem)))
			)
		)
	);
break;

case ODDVERSOR_TAG:
	Glue<E3Elem>::push(
		L, 
		new E3Elem(
			MV_TAG,
			new mv(
				lc(mv(*((e2_t *)elem1->elem)), mv(*((oddVersor *)elem2->elem)))
			)
		)
	);
break;

case DOUBLE_TAG:
	Glue<E3Elem>::push(
		L, 
		new E3Elem(
			MV_TAG,
			new mv(
				lc(mv(*((e2_t *)elem1->elem)), *((double *)elem2))
			)
		)
	);
break;
}
break;

case E3_TAG:
switch(tag2) {
case MV_TAG:
	Glue<E3Elem>::push(
		L, 
		new E3Elem(
			MV_TAG,
			new mv(
				lc(mv(*((e3_t *)elem1->elem)), mv(*((mv *)elem2->elem)))
			)
		)
	);
break;

case E1_TAG:
	Glue<E3Elem>::push(
		L, 
		new E3Elem(
			MV_TAG,
			new mv(
				lc(mv(*((e3_t *)elem1->elem)), mv(*((e1_t *)elem2->elem)))
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
				lc(mv(*((e3_t *)elem1->elem)), mv(*((e2_t *)elem2->elem)))
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
				lc(mv(*((e3_t *)elem1->elem)), mv(*((e3_t *)elem2->elem)))
			)
		)
	);
break;

case I3_TAG:
	Glue<E3Elem>::push(
		L, 
		new E3Elem(
			MV_TAG,
			new mv(
				lc(mv(*((e3_t *)elem1->elem)), mv(*((I3_t *)elem2->elem)))
			)
		)
	);
break;

case VECTOR_TAG:
	Glue<E3Elem>::push(
		L, 
		new E3Elem(
			MV_TAG,
			new mv(
				lc(mv(*((e3_t *)elem1->elem)), mv(*((vector *)elem2->elem)))
			)
		)
	);
break;

case BIVECTOR_TAG:
	Glue<E3Elem>::push(
		L, 
		new E3Elem(
			MV_TAG,
			new mv(
				lc(mv(*((e3_t *)elem1->elem)), mv(*((bivector *)elem2->elem)))
			)
		)
	);
break;

case TRIVECTOR_TAG:
	Glue<E3Elem>::push(
		L, 
		new E3Elem(
			MV_TAG,
			new mv(
				lc(mv(*((e3_t *)elem1->elem)), mv(*((trivector *)elem2->elem)))
			)
		)
	);
break;

case ROTOR_TAG:
	Glue<E3Elem>::push(
		L, 
		new E3Elem(
			MV_TAG,
			new mv(
				lc(mv(*((e3_t *)elem1->elem)), mv(*((rotor *)elem2->elem)))
			)
		)
	);
break;

case ODDVERSOR_TAG:
	Glue<E3Elem>::push(
		L, 
		new E3Elem(
			MV_TAG,
			new mv(
				lc(mv(*((e3_t *)elem1->elem)), mv(*((oddVersor *)elem2->elem)))
			)
		)
	);
break;

case DOUBLE_TAG:
	Glue<E3Elem>::push(
		L, 
		new E3Elem(
			MV_TAG,
			new mv(
				lc(mv(*((e3_t *)elem1->elem)), *((double *)elem2))
			)
		)
	);
break;
}
break;

case I3_TAG:
switch(tag2) {
case MV_TAG:
	Glue<E3Elem>::push(
		L, 
		new E3Elem(
			MV_TAG,
			new mv(
				lc(mv(*((I3_t *)elem1->elem)), mv(*((mv *)elem2->elem)))
			)
		)
	);
break;

case E1_TAG:
	Glue<E3Elem>::push(
		L, 
		new E3Elem(
			MV_TAG,
			new mv(
				lc(mv(*((I3_t *)elem1->elem)), mv(*((e1_t *)elem2->elem)))
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
				lc(mv(*((I3_t *)elem1->elem)), mv(*((e2_t *)elem2->elem)))
			)
		)
	);
break;

case E3_TAG:
				lua::push<double>(
					L, 
					lc(*((I3_t *)elem1->elem), *((e3_t *)elem2->elem))
				);
			
break;

case I3_TAG:
	Glue<E3Elem>::push(
		L, 
		new E3Elem(
			MV_TAG,
			new mv(
				lc(mv(*((I3_t *)elem1->elem)), mv(*((I3_t *)elem2->elem)))
			)
		)
	);
break;

case VECTOR_TAG:
	Glue<E3Elem>::push(
		L, 
		new E3Elem(
			MV_TAG,
			new mv(
				lc(mv(*((I3_t *)elem1->elem)), mv(*((vector *)elem2->elem)))
			)
		)
	);
break;

case BIVECTOR_TAG:
	Glue<E3Elem>::push(
		L, 
		new E3Elem(
			MV_TAG,
			new mv(
				lc(mv(*((I3_t *)elem1->elem)), mv(*((bivector *)elem2->elem)))
			)
		)
	);
break;

case TRIVECTOR_TAG:
	Glue<E3Elem>::push(
		L, 
		new E3Elem(
			MV_TAG,
			new mv(
				lc(mv(*((I3_t *)elem1->elem)), mv(*((trivector *)elem2->elem)))
			)
		)
	);
break;

case ROTOR_TAG:
	Glue<E3Elem>::push(
		L, 
		new E3Elem(
			MV_TAG,
			new mv(
				lc(mv(*((I3_t *)elem1->elem)), mv(*((rotor *)elem2->elem)))
			)
		)
	);
break;

case ODDVERSOR_TAG:
	Glue<E3Elem>::push(
		L, 
		new E3Elem(
			MV_TAG,
			new mv(
				lc(mv(*((I3_t *)elem1->elem)), mv(*((oddVersor *)elem2->elem)))
			)
		)
	);
break;

case DOUBLE_TAG:
	Glue<E3Elem>::push(
		L, 
		new E3Elem(
			MV_TAG,
			new mv(
				lc(mv(*((I3_t *)elem1->elem)), *((double *)elem2))
			)
		)
	);
break;
}
break;

case VECTOR_TAG:
switch(tag2) {
case MV_TAG:
	Glue<E3Elem>::push(
		L, 
		new E3Elem(
			MV_TAG,
			new mv(
				lc(mv(*((vector *)elem1->elem)), mv(*((mv *)elem2->elem)))
			)
		)
	);
break;

case E1_TAG:
	Glue<E3Elem>::push(
		L, 
		new E3Elem(
			MV_TAG,
			new mv(
				lc(mv(*((vector *)elem1->elem)), mv(*((e1_t *)elem2->elem)))
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
				lc(mv(*((vector *)elem1->elem)), mv(*((e2_t *)elem2->elem)))
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
				lc(mv(*((vector *)elem1->elem)), mv(*((e3_t *)elem2->elem)))
			)
		)
	);
break;

case I3_TAG:
	Glue<E3Elem>::push(
		L, 
		new E3Elem(
			MV_TAG,
			new mv(
				lc(mv(*((vector *)elem1->elem)), mv(*((I3_t *)elem2->elem)))
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
	Glue<E3Elem>::push(
		L, 
		new E3Elem(
			VECTOR_TAG,
			new vector(
				lc(*((vector *)elem1->elem), *((bivector *)elem2->elem))
			)
		)
	);
break;

case TRIVECTOR_TAG:
	Glue<E3Elem>::push(
		L, 
		new E3Elem(
			MV_TAG,
			new mv(
				lc(mv(*((vector *)elem1->elem)), mv(*((trivector *)elem2->elem)))
			)
		)
	);
break;

case ROTOR_TAG:
	Glue<E3Elem>::push(
		L, 
		new E3Elem(
			VECTOR_TAG,
			new vector(
				lc(*((vector *)elem1->elem), *((rotor *)elem2->elem))
			)
		)
	);
break;

case ODDVERSOR_TAG:
	Glue<E3Elem>::push(
		L, 
		new E3Elem(
			MV_TAG,
			new mv(
				lc(mv(*((vector *)elem1->elem)), mv(*((oddVersor *)elem2->elem)))
			)
		)
	);
break;

case DOUBLE_TAG:
	Glue<E3Elem>::push(
		L, 
		new E3Elem(
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
	Glue<E3Elem>::push(
		L, 
		new E3Elem(
			MV_TAG,
			new mv(
				lc(mv(*((bivector *)elem1->elem)), mv(*((mv *)elem2->elem)))
			)
		)
	);
break;

case E1_TAG:
	Glue<E3Elem>::push(
		L, 
		new E3Elem(
			MV_TAG,
			new mv(
				lc(mv(*((bivector *)elem1->elem)), mv(*((e1_t *)elem2->elem)))
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
				lc(mv(*((bivector *)elem1->elem)), mv(*((e2_t *)elem2->elem)))
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
				lc(mv(*((bivector *)elem1->elem)), mv(*((e3_t *)elem2->elem)))
			)
		)
	);
break;

case I3_TAG:
	Glue<E3Elem>::push(
		L, 
		new E3Elem(
			MV_TAG,
			new mv(
				lc(mv(*((bivector *)elem1->elem)), mv(*((I3_t *)elem2->elem)))
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
	Glue<E3Elem>::push(
		L, 
		new E3Elem(
			MV_TAG,
			new mv(
				lc(mv(*((bivector *)elem1->elem)), mv(*((bivector *)elem2->elem)))
			)
		)
	);
break;

case TRIVECTOR_TAG:
	Glue<E3Elem>::push(
		L, 
		new E3Elem(
			MV_TAG,
			new mv(
				lc(mv(*((bivector *)elem1->elem)), mv(*((trivector *)elem2->elem)))
			)
		)
	);
break;

case ROTOR_TAG:
	Glue<E3Elem>::push(
		L, 
		new E3Elem(
			MV_TAG,
			new mv(
				lc(mv(*((bivector *)elem1->elem)), mv(*((rotor *)elem2->elem)))
			)
		)
	);
break;

case ODDVERSOR_TAG:
	Glue<E3Elem>::push(
		L, 
		new E3Elem(
			MV_TAG,
			new mv(
				lc(mv(*((bivector *)elem1->elem)), mv(*((oddVersor *)elem2->elem)))
			)
		)
	);
break;

case DOUBLE_TAG:
	Glue<E3Elem>::push(
		L, 
		new E3Elem(
			MV_TAG,
			new mv(
				lc(mv(*((bivector *)elem1->elem)), *((double *)elem2))
			)
		)
	);
break;
}
break;

case TRIVECTOR_TAG:
switch(tag2) {
case MV_TAG:
	Glue<E3Elem>::push(
		L, 
		new E3Elem(
			MV_TAG,
			new mv(
				lc(mv(*((trivector *)elem1->elem)), mv(*((mv *)elem2->elem)))
			)
		)
	);
break;

case E1_TAG:
	Glue<E3Elem>::push(
		L, 
		new E3Elem(
			MV_TAG,
			new mv(
				lc(mv(*((trivector *)elem1->elem)), mv(*((e1_t *)elem2->elem)))
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
				lc(mv(*((trivector *)elem1->elem)), mv(*((e2_t *)elem2->elem)))
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
				lc(mv(*((trivector *)elem1->elem)), mv(*((e3_t *)elem2->elem)))
			)
		)
	);
break;

case I3_TAG:
	Glue<E3Elem>::push(
		L, 
		new E3Elem(
			MV_TAG,
			new mv(
				lc(mv(*((trivector *)elem1->elem)), mv(*((I3_t *)elem2->elem)))
			)
		)
	);
break;

case VECTOR_TAG:
	Glue<E3Elem>::push(
		L, 
		new E3Elem(
			MV_TAG,
			new mv(
				lc(mv(*((trivector *)elem1->elem)), mv(*((vector *)elem2->elem)))
			)
		)
	);
break;

case BIVECTOR_TAG:
	Glue<E3Elem>::push(
		L, 
		new E3Elem(
			MV_TAG,
			new mv(
				lc(mv(*((trivector *)elem1->elem)), mv(*((bivector *)elem2->elem)))
			)
		)
	);
break;

case TRIVECTOR_TAG:
				lua::push<double>(
					L, 
					lc(*((trivector *)elem1->elem), *((trivector *)elem2->elem))
				);
			
break;

case ROTOR_TAG:
	Glue<E3Elem>::push(
		L, 
		new E3Elem(
			MV_TAG,
			new mv(
				lc(mv(*((trivector *)elem1->elem)), mv(*((rotor *)elem2->elem)))
			)
		)
	);
break;

case ODDVERSOR_TAG:
	Glue<E3Elem>::push(
		L, 
		new E3Elem(
			MV_TAG,
			new mv(
				lc(mv(*((trivector *)elem1->elem)), mv(*((oddVersor *)elem2->elem)))
			)
		)
	);
break;

case DOUBLE_TAG:
	Glue<E3Elem>::push(
		L, 
		new E3Elem(
			MV_TAG,
			new mv(
				lc(mv(*((trivector *)elem1->elem)), *((double *)elem2))
			)
		)
	);
break;
}
break;

case ROTOR_TAG:
switch(tag2) {
case MV_TAG:
	Glue<E3Elem>::push(
		L, 
		new E3Elem(
			MV_TAG,
			new mv(
				lc(mv(*((rotor *)elem1->elem)), mv(*((mv *)elem2->elem)))
			)
		)
	);
break;

case E1_TAG:
	Glue<E3Elem>::push(
		L, 
		new E3Elem(
			MV_TAG,
			new mv(
				lc(mv(*((rotor *)elem1->elem)), mv(*((e1_t *)elem2->elem)))
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
				lc(mv(*((rotor *)elem1->elem)), mv(*((e2_t *)elem2->elem)))
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
				lc(mv(*((rotor *)elem1->elem)), mv(*((e3_t *)elem2->elem)))
			)
		)
	);
break;

case I3_TAG:
	Glue<E3Elem>::push(
		L, 
		new E3Elem(
			MV_TAG,
			new mv(
				lc(mv(*((rotor *)elem1->elem)), mv(*((I3_t *)elem2->elem)))
			)
		)
	);
break;

case VECTOR_TAG:
	Glue<E3Elem>::push(
		L, 
		new E3Elem(
			MV_TAG,
			new mv(
				lc(mv(*((rotor *)elem1->elem)), mv(*((vector *)elem2->elem)))
			)
		)
	);
break;

case BIVECTOR_TAG:
	Glue<E3Elem>::push(
		L, 
		new E3Elem(
			ROTOR_TAG,
			new rotor(
				lc(*((rotor *)elem1->elem), *((bivector *)elem2->elem))
			)
		)
	);
break;

case TRIVECTOR_TAG:
	Glue<E3Elem>::push(
		L, 
		new E3Elem(
			ODDVERSOR_TAG,
			new oddVersor(
				lc(*((rotor *)elem1->elem), *((trivector *)elem2->elem))
			)
		)
	);
break;

case ROTOR_TAG:
	Glue<E3Elem>::push(
		L, 
		new E3Elem(
			MV_TAG,
			new mv(
				lc(mv(*((rotor *)elem1->elem)), mv(*((rotor *)elem2->elem)))
			)
		)
	);
break;

case ODDVERSOR_TAG:
	Glue<E3Elem>::push(
		L, 
		new E3Elem(
			MV_TAG,
			new mv(
				lc(mv(*((rotor *)elem1->elem)), mv(*((oddVersor *)elem2->elem)))
			)
		)
	);
break;

case DOUBLE_TAG:
	Glue<E3Elem>::push(
		L, 
		new E3Elem(
			MV_TAG,
			new mv(
				lc(mv(*((rotor *)elem1->elem)), *((double *)elem2))
			)
		)
	);
break;
}
break;

case ODDVERSOR_TAG:
switch(tag2) {
case MV_TAG:
	Glue<E3Elem>::push(
		L, 
		new E3Elem(
			MV_TAG,
			new mv(
				lc(mv(*((oddVersor *)elem1->elem)), mv(*((mv *)elem2->elem)))
			)
		)
	);
break;

case E1_TAG:
	Glue<E3Elem>::push(
		L, 
		new E3Elem(
			MV_TAG,
			new mv(
				lc(mv(*((oddVersor *)elem1->elem)), mv(*((e1_t *)elem2->elem)))
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
				lc(mv(*((oddVersor *)elem1->elem)), mv(*((e2_t *)elem2->elem)))
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
				lc(mv(*((oddVersor *)elem1->elem)), mv(*((e3_t *)elem2->elem)))
			)
		)
	);
break;

case I3_TAG:
	Glue<E3Elem>::push(
		L, 
		new E3Elem(
			MV_TAG,
			new mv(
				lc(mv(*((oddVersor *)elem1->elem)), mv(*((I3_t *)elem2->elem)))
			)
		)
	);
break;

case VECTOR_TAG:
	Glue<E3Elem>::push(
		L, 
		new E3Elem(
			MV_TAG,
			new mv(
				lc(mv(*((oddVersor *)elem1->elem)), mv(*((vector *)elem2->elem)))
			)
		)
	);
break;

case BIVECTOR_TAG:
	Glue<E3Elem>::push(
		L, 
		new E3Elem(
			MV_TAG,
			new mv(
				lc(mv(*((oddVersor *)elem1->elem)), mv(*((bivector *)elem2->elem)))
			)
		)
	);
break;

case TRIVECTOR_TAG:
	Glue<E3Elem>::push(
		L, 
		new E3Elem(
			MV_TAG,
			new mv(
				lc(mv(*((oddVersor *)elem1->elem)), mv(*((trivector *)elem2->elem)))
			)
		)
	);
break;

case ROTOR_TAG:
	Glue<E3Elem>::push(
		L, 
		new E3Elem(
			MV_TAG,
			new mv(
				lc(mv(*((oddVersor *)elem1->elem)), mv(*((rotor *)elem2->elem)))
			)
		)
	);
break;

case ODDVERSOR_TAG:
	Glue<E3Elem>::push(
		L, 
		new E3Elem(
			MV_TAG,
			new mv(
				lc(mv(*((oddVersor *)elem1->elem)), mv(*((oddVersor *)elem2->elem)))
			)
		)
	);
break;

case DOUBLE_TAG:
	Glue<E3Elem>::push(
		L, 
		new E3Elem(
			MV_TAG,
			new mv(
				lc(mv(*((oddVersor *)elem1->elem)), *((double *)elem2))
			)
		)
	);
break;
}
break;

case DOUBLE_TAG:
switch(tag2) {
case MV_TAG:
	Glue<E3Elem>::push(
		L, 
		new E3Elem(
			MV_TAG,
			new mv(
				lc(*((double *)elem1), mv(*((mv *)elem2->elem)))
			)
		)
	);
break;

case E1_TAG:
	Glue<E3Elem>::push(
		L, 
		new E3Elem(
			MV_TAG,
			new mv(
				lc(*((double *)elem1), mv(*((e1_t *)elem2->elem)))
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
				lc(*((double *)elem1), mv(*((e2_t *)elem2->elem)))
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
				lc(*((double *)elem1), mv(*((e3_t *)elem2->elem)))
			)
		)
	);
break;

case I3_TAG:
	Glue<E3Elem>::push(
		L, 
		new E3Elem(
			MV_TAG,
			new mv(
				lc(*((double *)elem1), mv(*((I3_t *)elem2->elem)))
			)
		)
	);
break;

case VECTOR_TAG:
	Glue<E3Elem>::push(
		L, 
		new E3Elem(
			MV_TAG,
			new mv(
				lc(*((double *)elem1), mv(*((vector *)elem2->elem)))
			)
		)
	);
break;

case BIVECTOR_TAG:
	Glue<E3Elem>::push(
		L, 
		new E3Elem(
			MV_TAG,
			new mv(
				lc(*((double *)elem1), mv(*((bivector *)elem2->elem)))
			)
		)
	);
break;

case TRIVECTOR_TAG:
	Glue<E3Elem>::push(
		L, 
		new E3Elem(
			MV_TAG,
			new mv(
				lc(*((double *)elem1), mv(*((trivector *)elem2->elem)))
			)
		)
	);
break;

case ROTOR_TAG:
	Glue<E3Elem>::push(
		L, 
		new E3Elem(
			MV_TAG,
			new mv(
				lc(*((double *)elem1), mv(*((rotor *)elem2->elem)))
			)
		)
	);
break;

case ODDVERSOR_TAG:
	Glue<E3Elem>::push(
		L, 
		new E3Elem(
			MV_TAG,
			new mv(
				lc(*((double *)elem1), mv(*((oddVersor *)elem2->elem)))
			)
		)
	);
break;

case DOUBLE_TAG:
	Glue<E3Elem>::push(
		L, 
		new E3Elem(
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


int e3space_rc(lua_State *L) {
	double d1, d2;
	E3Tag tag1;
	E3Tag tag2;
	E3Elem *elem1 = Glue<E3Elem>::to(L, 1);
	E3Elem *elem2 = Glue<E3Elem>::to(L, 2);
	
	
	if(!elem1) {
		if(lua::is<double>(L, 1)) {
			d1 = lua::to<double>(L, 1);
			elem1 = (E3Elem *)(&d1);
			tag1 = DOUBLE_TAG;
		}
		else {
			luaL_error(L, "e3.rc: invalid arguments");
		}
	}
	else {
		tag1 = elem1->tag;
	}
	
	if(!elem2) {
		if(lua::is<double>(L, 2)) {
			d2 = lua::to<double>(L, 2);
			elem2 = (E3Elem *)(&d2);
			tag2 = DOUBLE_TAG;
		}
		else {
			luaL_error(L, "e3.rc: invalid arguments");
		}
	}
	else {
		tag2 = elem2->tag;
	}
	
switch(tag1) {
case MV_TAG:
switch(tag2) {
case MV_TAG:
	Glue<E3Elem>::push(
		L, 
		new E3Elem(
			MV_TAG,
			new mv(
				rc(*((mv *)elem1->elem), *((mv *)elem2->elem))
			)
		)
	);
break;

case E1_TAG:
	Glue<E3Elem>::push(
		L, 
		new E3Elem(
			MV_TAG,
			new mv(
				rc(mv(*((mv *)elem1->elem)), mv(*((e1_t *)elem2->elem)))
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
				rc(mv(*((mv *)elem1->elem)), mv(*((e2_t *)elem2->elem)))
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
				rc(mv(*((mv *)elem1->elem)), mv(*((e3_t *)elem2->elem)))
			)
		)
	);
break;

case I3_TAG:
	Glue<E3Elem>::push(
		L, 
		new E3Elem(
			MV_TAG,
			new mv(
				rc(mv(*((mv *)elem1->elem)), mv(*((I3_t *)elem2->elem)))
			)
		)
	);
break;

case VECTOR_TAG:
	Glue<E3Elem>::push(
		L, 
		new E3Elem(
			MV_TAG,
			new mv(
				rc(mv(*((mv *)elem1->elem)), mv(*((vector *)elem2->elem)))
			)
		)
	);
break;

case BIVECTOR_TAG:
	Glue<E3Elem>::push(
		L, 
		new E3Elem(
			MV_TAG,
			new mv(
				rc(mv(*((mv *)elem1->elem)), mv(*((bivector *)elem2->elem)))
			)
		)
	);
break;

case TRIVECTOR_TAG:
	Glue<E3Elem>::push(
		L, 
		new E3Elem(
			MV_TAG,
			new mv(
				rc(mv(*((mv *)elem1->elem)), mv(*((trivector *)elem2->elem)))
			)
		)
	);
break;

case ROTOR_TAG:
	Glue<E3Elem>::push(
		L, 
		new E3Elem(
			MV_TAG,
			new mv(
				rc(mv(*((mv *)elem1->elem)), mv(*((rotor *)elem2->elem)))
			)
		)
	);
break;

case ODDVERSOR_TAG:
	Glue<E3Elem>::push(
		L, 
		new E3Elem(
			MV_TAG,
			new mv(
				rc(mv(*((mv *)elem1->elem)), mv(*((oddVersor *)elem2->elem)))
			)
		)
	);
break;

case DOUBLE_TAG:
	Glue<E3Elem>::push(
		L, 
		new E3Elem(
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
	Glue<E3Elem>::push(
		L, 
		new E3Elem(
			MV_TAG,
			new mv(
				rc(mv(*((e1_t *)elem1->elem)), mv(*((mv *)elem2->elem)))
			)
		)
	);
break;

case E1_TAG:
				lua::push<double>(
					L, 
					rc(*((e1_t *)elem1->elem), *((e1_t *)elem2->elem))
				);
			
break;

case E2_TAG:
	Glue<E3Elem>::push(
		L, 
		new E3Elem(
			MV_TAG,
			new mv(
				rc(mv(*((e1_t *)elem1->elem)), mv(*((e2_t *)elem2->elem)))
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
				rc(mv(*((e1_t *)elem1->elem)), mv(*((e3_t *)elem2->elem)))
			)
		)
	);
break;

case I3_TAG:
	Glue<E3Elem>::push(
		L, 
		new E3Elem(
			MV_TAG,
			new mv(
				rc(mv(*((e1_t *)elem1->elem)), mv(*((I3_t *)elem2->elem)))
			)
		)
	);
break;

case VECTOR_TAG:
	Glue<E3Elem>::push(
		L, 
		new E3Elem(
			MV_TAG,
			new mv(
				rc(mv(*((e1_t *)elem1->elem)), mv(*((vector *)elem2->elem)))
			)
		)
	);
break;

case BIVECTOR_TAG:
	Glue<E3Elem>::push(
		L, 
		new E3Elem(
			MV_TAG,
			new mv(
				rc(mv(*((e1_t *)elem1->elem)), mv(*((bivector *)elem2->elem)))
			)
		)
	);
break;

case TRIVECTOR_TAG:
	Glue<E3Elem>::push(
		L, 
		new E3Elem(
			MV_TAG,
			new mv(
				rc(mv(*((e1_t *)elem1->elem)), mv(*((trivector *)elem2->elem)))
			)
		)
	);
break;

case ROTOR_TAG:
	Glue<E3Elem>::push(
		L, 
		new E3Elem(
			MV_TAG,
			new mv(
				rc(mv(*((e1_t *)elem1->elem)), mv(*((rotor *)elem2->elem)))
			)
		)
	);
break;

case ODDVERSOR_TAG:
	Glue<E3Elem>::push(
		L, 
		new E3Elem(
			MV_TAG,
			new mv(
				rc(mv(*((e1_t *)elem1->elem)), mv(*((oddVersor *)elem2->elem)))
			)
		)
	);
break;

case DOUBLE_TAG:
	Glue<E3Elem>::push(
		L, 
		new E3Elem(
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
	Glue<E3Elem>::push(
		L, 
		new E3Elem(
			MV_TAG,
			new mv(
				rc(mv(*((e2_t *)elem1->elem)), mv(*((mv *)elem2->elem)))
			)
		)
	);
break;

case E1_TAG:
	Glue<E3Elem>::push(
		L, 
		new E3Elem(
			MV_TAG,
			new mv(
				rc(mv(*((e2_t *)elem1->elem)), mv(*((e1_t *)elem2->elem)))
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
				rc(mv(*((e2_t *)elem1->elem)), mv(*((e2_t *)elem2->elem)))
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
				rc(mv(*((e2_t *)elem1->elem)), mv(*((e3_t *)elem2->elem)))
			)
		)
	);
break;

case I3_TAG:
	Glue<E3Elem>::push(
		L, 
		new E3Elem(
			MV_TAG,
			new mv(
				rc(mv(*((e2_t *)elem1->elem)), mv(*((I3_t *)elem2->elem)))
			)
		)
	);
break;

case VECTOR_TAG:
	Glue<E3Elem>::push(
		L, 
		new E3Elem(
			MV_TAG,
			new mv(
				rc(mv(*((e2_t *)elem1->elem)), mv(*((vector *)elem2->elem)))
			)
		)
	);
break;

case BIVECTOR_TAG:
	Glue<E3Elem>::push(
		L, 
		new E3Elem(
			MV_TAG,
			new mv(
				rc(mv(*((e2_t *)elem1->elem)), mv(*((bivector *)elem2->elem)))
			)
		)
	);
break;

case TRIVECTOR_TAG:
	Glue<E3Elem>::push(
		L, 
		new E3Elem(
			MV_TAG,
			new mv(
				rc(mv(*((e2_t *)elem1->elem)), mv(*((trivector *)elem2->elem)))
			)
		)
	);
break;

case ROTOR_TAG:
	Glue<E3Elem>::push(
		L, 
		new E3Elem(
			MV_TAG,
			new mv(
				rc(mv(*((e2_t *)elem1->elem)), mv(*((rotor *)elem2->elem)))
			)
		)
	);
break;

case ODDVERSOR_TAG:
	Glue<E3Elem>::push(
		L, 
		new E3Elem(
			MV_TAG,
			new mv(
				rc(mv(*((e2_t *)elem1->elem)), mv(*((oddVersor *)elem2->elem)))
			)
		)
	);
break;

case DOUBLE_TAG:
	Glue<E3Elem>::push(
		L, 
		new E3Elem(
			MV_TAG,
			new mv(
				rc(mv(*((e2_t *)elem1->elem)), *((double *)elem2))
			)
		)
	);
break;
}
break;

case E3_TAG:
switch(tag2) {
case MV_TAG:
	Glue<E3Elem>::push(
		L, 
		new E3Elem(
			MV_TAG,
			new mv(
				rc(mv(*((e3_t *)elem1->elem)), mv(*((mv *)elem2->elem)))
			)
		)
	);
break;

case E1_TAG:
	Glue<E3Elem>::push(
		L, 
		new E3Elem(
			MV_TAG,
			new mv(
				rc(mv(*((e3_t *)elem1->elem)), mv(*((e1_t *)elem2->elem)))
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
				rc(mv(*((e3_t *)elem1->elem)), mv(*((e2_t *)elem2->elem)))
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
				rc(mv(*((e3_t *)elem1->elem)), mv(*((e3_t *)elem2->elem)))
			)
		)
	);
break;

case I3_TAG:
	Glue<E3Elem>::push(
		L, 
		new E3Elem(
			MV_TAG,
			new mv(
				rc(mv(*((e3_t *)elem1->elem)), mv(*((I3_t *)elem2->elem)))
			)
		)
	);
break;

case VECTOR_TAG:
	Glue<E3Elem>::push(
		L, 
		new E3Elem(
			MV_TAG,
			new mv(
				rc(mv(*((e3_t *)elem1->elem)), mv(*((vector *)elem2->elem)))
			)
		)
	);
break;

case BIVECTOR_TAG:
	Glue<E3Elem>::push(
		L, 
		new E3Elem(
			MV_TAG,
			new mv(
				rc(mv(*((e3_t *)elem1->elem)), mv(*((bivector *)elem2->elem)))
			)
		)
	);
break;

case TRIVECTOR_TAG:
	Glue<E3Elem>::push(
		L, 
		new E3Elem(
			MV_TAG,
			new mv(
				rc(mv(*((e3_t *)elem1->elem)), mv(*((trivector *)elem2->elem)))
			)
		)
	);
break;

case ROTOR_TAG:
	Glue<E3Elem>::push(
		L, 
		new E3Elem(
			MV_TAG,
			new mv(
				rc(mv(*((e3_t *)elem1->elem)), mv(*((rotor *)elem2->elem)))
			)
		)
	);
break;

case ODDVERSOR_TAG:
	Glue<E3Elem>::push(
		L, 
		new E3Elem(
			MV_TAG,
			new mv(
				rc(mv(*((e3_t *)elem1->elem)), mv(*((oddVersor *)elem2->elem)))
			)
		)
	);
break;

case DOUBLE_TAG:
	Glue<E3Elem>::push(
		L, 
		new E3Elem(
			MV_TAG,
			new mv(
				rc(mv(*((e3_t *)elem1->elem)), *((double *)elem2))
			)
		)
	);
break;
}
break;

case I3_TAG:
switch(tag2) {
case MV_TAG:
	Glue<E3Elem>::push(
		L, 
		new E3Elem(
			MV_TAG,
			new mv(
				rc(mv(*((I3_t *)elem1->elem)), mv(*((mv *)elem2->elem)))
			)
		)
	);
break;

case E1_TAG:
	Glue<E3Elem>::push(
		L, 
		new E3Elem(
			MV_TAG,
			new mv(
				rc(mv(*((I3_t *)elem1->elem)), mv(*((e1_t *)elem2->elem)))
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
				rc(mv(*((I3_t *)elem1->elem)), mv(*((e2_t *)elem2->elem)))
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
				rc(mv(*((I3_t *)elem1->elem)), mv(*((e3_t *)elem2->elem)))
			)
		)
	);
break;

case I3_TAG:
	Glue<E3Elem>::push(
		L, 
		new E3Elem(
			MV_TAG,
			new mv(
				rc(mv(*((I3_t *)elem1->elem)), mv(*((I3_t *)elem2->elem)))
			)
		)
	);
break;

case VECTOR_TAG:
	Glue<E3Elem>::push(
		L, 
		new E3Elem(
			MV_TAG,
			new mv(
				rc(mv(*((I3_t *)elem1->elem)), mv(*((vector *)elem2->elem)))
			)
		)
	);
break;

case BIVECTOR_TAG:
	Glue<E3Elem>::push(
		L, 
		new E3Elem(
			MV_TAG,
			new mv(
				rc(mv(*((I3_t *)elem1->elem)), mv(*((bivector *)elem2->elem)))
			)
		)
	);
break;

case TRIVECTOR_TAG:
	Glue<E3Elem>::push(
		L, 
		new E3Elem(
			MV_TAG,
			new mv(
				rc(mv(*((I3_t *)elem1->elem)), mv(*((trivector *)elem2->elem)))
			)
		)
	);
break;

case ROTOR_TAG:
	Glue<E3Elem>::push(
		L, 
		new E3Elem(
			MV_TAG,
			new mv(
				rc(mv(*((I3_t *)elem1->elem)), mv(*((rotor *)elem2->elem)))
			)
		)
	);
break;

case ODDVERSOR_TAG:
	Glue<E3Elem>::push(
		L, 
		new E3Elem(
			MV_TAG,
			new mv(
				rc(mv(*((I3_t *)elem1->elem)), mv(*((oddVersor *)elem2->elem)))
			)
		)
	);
break;

case DOUBLE_TAG:
	Glue<E3Elem>::push(
		L, 
		new E3Elem(
			MV_TAG,
			new mv(
				rc(mv(*((I3_t *)elem1->elem)), *((double *)elem2))
			)
		)
	);
break;
}
break;

case VECTOR_TAG:
switch(tag2) {
case MV_TAG:
	Glue<E3Elem>::push(
		L, 
		new E3Elem(
			MV_TAG,
			new mv(
				rc(mv(*((vector *)elem1->elem)), mv(*((mv *)elem2->elem)))
			)
		)
	);
break;

case E1_TAG:
	Glue<E3Elem>::push(
		L, 
		new E3Elem(
			MV_TAG,
			new mv(
				rc(mv(*((vector *)elem1->elem)), mv(*((e1_t *)elem2->elem)))
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
				rc(mv(*((vector *)elem1->elem)), mv(*((e2_t *)elem2->elem)))
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
				rc(mv(*((vector *)elem1->elem)), mv(*((e3_t *)elem2->elem)))
			)
		)
	);
break;

case I3_TAG:
	Glue<E3Elem>::push(
		L, 
		new E3Elem(
			MV_TAG,
			new mv(
				rc(mv(*((vector *)elem1->elem)), mv(*((I3_t *)elem2->elem)))
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

case TRIVECTOR_TAG:
	Glue<E3Elem>::push(
		L, 
		new E3Elem(
			MV_TAG,
			new mv(
				rc(mv(*((vector *)elem1->elem)), mv(*((trivector *)elem2->elem)))
			)
		)
	);
break;

case ROTOR_TAG:
	Glue<E3Elem>::push(
		L, 
		new E3Elem(
			VECTOR_TAG,
			new vector(
				rc(*((vector *)elem1->elem), *((rotor *)elem2->elem))
			)
		)
	);
break;

case ODDVERSOR_TAG:
	Glue<E3Elem>::push(
		L, 
		new E3Elem(
			MV_TAG,
			new mv(
				rc(mv(*((vector *)elem1->elem)), mv(*((oddVersor *)elem2->elem)))
			)
		)
	);
break;

case DOUBLE_TAG:
	Glue<E3Elem>::push(
		L, 
		new E3Elem(
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
	Glue<E3Elem>::push(
		L, 
		new E3Elem(
			MV_TAG,
			new mv(
				rc(mv(*((bivector *)elem1->elem)), mv(*((mv *)elem2->elem)))
			)
		)
	);
break;

case E1_TAG:
	Glue<E3Elem>::push(
		L, 
		new E3Elem(
			MV_TAG,
			new mv(
				rc(mv(*((bivector *)elem1->elem)), mv(*((e1_t *)elem2->elem)))
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
				rc(mv(*((bivector *)elem1->elem)), mv(*((e2_t *)elem2->elem)))
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
				rc(mv(*((bivector *)elem1->elem)), mv(*((e3_t *)elem2->elem)))
			)
		)
	);
break;

case I3_TAG:
	Glue<E3Elem>::push(
		L, 
		new E3Elem(
			MV_TAG,
			new mv(
				rc(mv(*((bivector *)elem1->elem)), mv(*((I3_t *)elem2->elem)))
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
				rc(*((bivector *)elem1->elem), *((vector *)elem2->elem))
			)
		)
	);
break;

case BIVECTOR_TAG:
	Glue<E3Elem>::push(
		L, 
		new E3Elem(
			MV_TAG,
			new mv(
				rc(mv(*((bivector *)elem1->elem)), mv(*((bivector *)elem2->elem)))
			)
		)
	);
break;

case TRIVECTOR_TAG:
	Glue<E3Elem>::push(
		L, 
		new E3Elem(
			MV_TAG,
			new mv(
				rc(mv(*((bivector *)elem1->elem)), mv(*((trivector *)elem2->elem)))
			)
		)
	);
break;

case ROTOR_TAG:
	Glue<E3Elem>::push(
		L, 
		new E3Elem(
			MV_TAG,
			new mv(
				rc(mv(*((bivector *)elem1->elem)), mv(*((rotor *)elem2->elem)))
			)
		)
	);
break;

case ODDVERSOR_TAG:
	Glue<E3Elem>::push(
		L, 
		new E3Elem(
			MV_TAG,
			new mv(
				rc(mv(*((bivector *)elem1->elem)), mv(*((oddVersor *)elem2->elem)))
			)
		)
	);
break;

case DOUBLE_TAG:
	Glue<E3Elem>::push(
		L, 
		new E3Elem(
			MV_TAG,
			new mv(
				rc(mv(*((bivector *)elem1->elem)), *((double *)elem2))
			)
		)
	);
break;
}
break;

case TRIVECTOR_TAG:
switch(tag2) {
case MV_TAG:
	Glue<E3Elem>::push(
		L, 
		new E3Elem(
			MV_TAG,
			new mv(
				rc(mv(*((trivector *)elem1->elem)), mv(*((mv *)elem2->elem)))
			)
		)
	);
break;

case E1_TAG:
	Glue<E3Elem>::push(
		L, 
		new E3Elem(
			MV_TAG,
			new mv(
				rc(mv(*((trivector *)elem1->elem)), mv(*((e1_t *)elem2->elem)))
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
				rc(mv(*((trivector *)elem1->elem)), mv(*((e2_t *)elem2->elem)))
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
				rc(mv(*((trivector *)elem1->elem)), mv(*((e3_t *)elem2->elem)))
			)
		)
	);
break;

case I3_TAG:
	Glue<E3Elem>::push(
		L, 
		new E3Elem(
			MV_TAG,
			new mv(
				rc(mv(*((trivector *)elem1->elem)), mv(*((I3_t *)elem2->elem)))
			)
		)
	);
break;

case VECTOR_TAG:
	Glue<E3Elem>::push(
		L, 
		new E3Elem(
			MV_TAG,
			new mv(
				rc(mv(*((trivector *)elem1->elem)), mv(*((vector *)elem2->elem)))
			)
		)
	);
break;

case BIVECTOR_TAG:
	Glue<E3Elem>::push(
		L, 
		new E3Elem(
			MV_TAG,
			new mv(
				rc(mv(*((trivector *)elem1->elem)), mv(*((bivector *)elem2->elem)))
			)
		)
	);
break;

case TRIVECTOR_TAG:
				lua::push<double>(
					L, 
					rc(*((trivector *)elem1->elem), *((trivector *)elem2->elem))
				);
			
break;

case ROTOR_TAG:
	Glue<E3Elem>::push(
		L, 
		new E3Elem(
			MV_TAG,
			new mv(
				rc(mv(*((trivector *)elem1->elem)), mv(*((rotor *)elem2->elem)))
			)
		)
	);
break;

case ODDVERSOR_TAG:
	Glue<E3Elem>::push(
		L, 
		new E3Elem(
			MV_TAG,
			new mv(
				rc(mv(*((trivector *)elem1->elem)), mv(*((oddVersor *)elem2->elem)))
			)
		)
	);
break;

case DOUBLE_TAG:
	Glue<E3Elem>::push(
		L, 
		new E3Elem(
			MV_TAG,
			new mv(
				rc(mv(*((trivector *)elem1->elem)), *((double *)elem2))
			)
		)
	);
break;
}
break;

case ROTOR_TAG:
switch(tag2) {
case MV_TAG:
	Glue<E3Elem>::push(
		L, 
		new E3Elem(
			MV_TAG,
			new mv(
				rc(mv(*((rotor *)elem1->elem)), mv(*((mv *)elem2->elem)))
			)
		)
	);
break;

case E1_TAG:
	Glue<E3Elem>::push(
		L, 
		new E3Elem(
			MV_TAG,
			new mv(
				rc(mv(*((rotor *)elem1->elem)), mv(*((e1_t *)elem2->elem)))
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
				rc(mv(*((rotor *)elem1->elem)), mv(*((e2_t *)elem2->elem)))
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
				rc(mv(*((rotor *)elem1->elem)), mv(*((e3_t *)elem2->elem)))
			)
		)
	);
break;

case I3_TAG:
	Glue<E3Elem>::push(
		L, 
		new E3Elem(
			MV_TAG,
			new mv(
				rc(mv(*((rotor *)elem1->elem)), mv(*((I3_t *)elem2->elem)))
			)
		)
	);
break;

case VECTOR_TAG:
	Glue<E3Elem>::push(
		L, 
		new E3Elem(
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

case TRIVECTOR_TAG:
				lua::push<double>(
					L, 
					rc(*((rotor *)elem1->elem), *((trivector *)elem2->elem))
				);
			
break;

case ROTOR_TAG:
	Glue<E3Elem>::push(
		L, 
		new E3Elem(
			MV_TAG,
			new mv(
				rc(mv(*((rotor *)elem1->elem)), mv(*((rotor *)elem2->elem)))
			)
		)
	);
break;

case ODDVERSOR_TAG:
	Glue<E3Elem>::push(
		L, 
		new E3Elem(
			MV_TAG,
			new mv(
				rc(mv(*((rotor *)elem1->elem)), mv(*((oddVersor *)elem2->elem)))
			)
		)
	);
break;

case DOUBLE_TAG:
	Glue<E3Elem>::push(
		L, 
		new E3Elem(
			MV_TAG,
			new mv(
				rc(mv(*((rotor *)elem1->elem)), *((double *)elem2))
			)
		)
	);
break;
}
break;

case ODDVERSOR_TAG:
switch(tag2) {
case MV_TAG:
	Glue<E3Elem>::push(
		L, 
		new E3Elem(
			MV_TAG,
			new mv(
				rc(mv(*((oddVersor *)elem1->elem)), mv(*((mv *)elem2->elem)))
			)
		)
	);
break;

case E1_TAG:
	Glue<E3Elem>::push(
		L, 
		new E3Elem(
			MV_TAG,
			new mv(
				rc(mv(*((oddVersor *)elem1->elem)), mv(*((e1_t *)elem2->elem)))
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
				rc(mv(*((oddVersor *)elem1->elem)), mv(*((e2_t *)elem2->elem)))
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
				rc(mv(*((oddVersor *)elem1->elem)), mv(*((e3_t *)elem2->elem)))
			)
		)
	);
break;

case I3_TAG:
	Glue<E3Elem>::push(
		L, 
		new E3Elem(
			MV_TAG,
			new mv(
				rc(mv(*((oddVersor *)elem1->elem)), mv(*((I3_t *)elem2->elem)))
			)
		)
	);
break;

case VECTOR_TAG:
	Glue<E3Elem>::push(
		L, 
		new E3Elem(
			MV_TAG,
			new mv(
				rc(mv(*((oddVersor *)elem1->elem)), mv(*((vector *)elem2->elem)))
			)
		)
	);
break;

case BIVECTOR_TAG:
	Glue<E3Elem>::push(
		L, 
		new E3Elem(
			MV_TAG,
			new mv(
				rc(mv(*((oddVersor *)elem1->elem)), mv(*((bivector *)elem2->elem)))
			)
		)
	);
break;

case TRIVECTOR_TAG:
	Glue<E3Elem>::push(
		L, 
		new E3Elem(
			MV_TAG,
			new mv(
				rc(mv(*((oddVersor *)elem1->elem)), mv(*((trivector *)elem2->elem)))
			)
		)
	);
break;

case ROTOR_TAG:
	Glue<E3Elem>::push(
		L, 
		new E3Elem(
			MV_TAG,
			new mv(
				rc(mv(*((oddVersor *)elem1->elem)), mv(*((rotor *)elem2->elem)))
			)
		)
	);
break;

case ODDVERSOR_TAG:
	Glue<E3Elem>::push(
		L, 
		new E3Elem(
			MV_TAG,
			new mv(
				rc(mv(*((oddVersor *)elem1->elem)), mv(*((oddVersor *)elem2->elem)))
			)
		)
	);
break;

case DOUBLE_TAG:
	Glue<E3Elem>::push(
		L, 
		new E3Elem(
			MV_TAG,
			new mv(
				rc(mv(*((oddVersor *)elem1->elem)), *((double *)elem2))
			)
		)
	);
break;
}
break;

case DOUBLE_TAG:
switch(tag2) {
case MV_TAG:
	Glue<E3Elem>::push(
		L, 
		new E3Elem(
			MV_TAG,
			new mv(
				rc(*((double *)elem1), mv(*((mv *)elem2->elem)))
			)
		)
	);
break;

case E1_TAG:
	Glue<E3Elem>::push(
		L, 
		new E3Elem(
			MV_TAG,
			new mv(
				rc(*((double *)elem1), mv(*((e1_t *)elem2->elem)))
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
				rc(*((double *)elem1), mv(*((e2_t *)elem2->elem)))
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
				rc(*((double *)elem1), mv(*((e3_t *)elem2->elem)))
			)
		)
	);
break;

case I3_TAG:
	Glue<E3Elem>::push(
		L, 
		new E3Elem(
			MV_TAG,
			new mv(
				rc(*((double *)elem1), mv(*((I3_t *)elem2->elem)))
			)
		)
	);
break;

case VECTOR_TAG:
	Glue<E3Elem>::push(
		L, 
		new E3Elem(
			MV_TAG,
			new mv(
				rc(*((double *)elem1), mv(*((vector *)elem2->elem)))
			)
		)
	);
break;

case BIVECTOR_TAG:
	Glue<E3Elem>::push(
		L, 
		new E3Elem(
			MV_TAG,
			new mv(
				rc(*((double *)elem1), mv(*((bivector *)elem2->elem)))
			)
		)
	);
break;

case TRIVECTOR_TAG:
	Glue<E3Elem>::push(
		L, 
		new E3Elem(
			MV_TAG,
			new mv(
				rc(*((double *)elem1), mv(*((trivector *)elem2->elem)))
			)
		)
	);
break;

case ROTOR_TAG:
	Glue<E3Elem>::push(
		L, 
		new E3Elem(
			MV_TAG,
			new mv(
				rc(*((double *)elem1), mv(*((rotor *)elem2->elem)))
			)
		)
	);
break;

case ODDVERSOR_TAG:
	Glue<E3Elem>::push(
		L, 
		new E3Elem(
			MV_TAG,
			new mv(
				rc(*((double *)elem1), mv(*((oddVersor *)elem2->elem)))
			)
		)
	);
break;

case DOUBLE_TAG:
	Glue<E3Elem>::push(
		L, 
		new E3Elem(
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