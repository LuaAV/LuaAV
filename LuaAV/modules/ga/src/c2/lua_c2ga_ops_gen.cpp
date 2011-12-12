// THIS FILE GENERATED AUTOMATICALLY, DO NOT EDIT
#include "lua_c2ga.h"

#ifdef __cplusplus
extern "C" {
#endif




int c2space_gp(lua_State *L) {
	double d1, d2;
	C2Tag tag1;
	C2Tag tag2;
	C2Elem *elem1 = Glue<C2Elem>::to(L, 1);
	C2Elem *elem2 = Glue<C2Elem>::to(L, 2);
	
	
	if(!elem1) {
		if(lua::is<double>(L, 1)) {
			d1 = lua::to<double>(L, 1);
			elem1 = (C2Elem *)(&d1);
			tag1 = DOUBLE_TAG;
		}
		else {
			luaL_error(L, "c2.gp: invalid arguments");
		}
	}
	else {
		tag1 = elem1->tag;
	}
	
	if(!elem2) {
		if(lua::is<double>(L, 2)) {
			d2 = lua::to<double>(L, 2);
			elem2 = (C2Elem *)(&d2);
			tag2 = DOUBLE_TAG;
		}
		else {
			luaL_error(L, "c2.gp: invalid arguments");
		}
	}
	else {
		tag2 = elem2->tag;
	}
	
switch(tag1) {
case MV_TAG:
switch(tag2) {
case MV_TAG:
	Glue<C2Elem>::push(
		L, 
		new C2Elem(
			MV_TAG,
			new mv(
				gp(*((mv *)elem1->elem), *((mv *)elem2->elem))
			)
		)
	);
break;

case NO_TAG:
	Glue<C2Elem>::push(
		L, 
		new C2Elem(
			MV_TAG,
			new mv(
				gp(mv(*((mv *)elem1->elem)), mv(*((no_t *)elem2->elem)))
			)
		)
	);
break;

case E1_TAG:
	Glue<C2Elem>::push(
		L, 
		new C2Elem(
			MV_TAG,
			new mv(
				gp(mv(*((mv *)elem1->elem)), mv(*((e1_t *)elem2->elem)))
			)
		)
	);
break;

case E2_TAG:
	Glue<C2Elem>::push(
		L, 
		new C2Elem(
			MV_TAG,
			new mv(
				gp(mv(*((mv *)elem1->elem)), mv(*((e2_t *)elem2->elem)))
			)
		)
	);
break;

case NI_TAG:
	Glue<C2Elem>::push(
		L, 
		new C2Elem(
			MV_TAG,
			new mv(
				gp(mv(*((mv *)elem1->elem)), mv(*((ni_t *)elem2->elem)))
			)
		)
	);
break;

case NONI_TAG:
	Glue<C2Elem>::push(
		L, 
		new C2Elem(
			MV_TAG,
			new mv(
				gp(mv(*((mv *)elem1->elem)), mv(*((noni_t *)elem2->elem)))
			)
		)
	);
break;

case I2_TAG:
	Glue<C2Elem>::push(
		L, 
		new C2Elem(
			MV_TAG,
			new mv(
				gp(mv(*((mv *)elem1->elem)), mv(*((I2_t *)elem2->elem)))
			)
		)
	);
break;

case I4_TAG:
	Glue<C2Elem>::push(
		L, 
		new C2Elem(
			MV_TAG,
			new mv(
				gp(mv(*((mv *)elem1->elem)), mv(*((I4_t *)elem2->elem)))
			)
		)
	);
break;

case I4i_TAG:
	Glue<C2Elem>::push(
		L, 
		new C2Elem(
			MV_TAG,
			new mv(
				gp(mv(*((mv *)elem1->elem)), mv(*((I4i_t *)elem2->elem)))
			)
		)
	);
break;

case VECTORE2GA_TAG:
	Glue<C2Elem>::push(
		L, 
		new C2Elem(
			MV_TAG,
			new mv(
				gp(mv(*((mv *)elem1->elem)), mv(*((vectorE2GA *)elem2->elem)))
			)
		)
	);
break;

case BIVECTORE2GA_TAG:
	Glue<C2Elem>::push(
		L, 
		new C2Elem(
			MV_TAG,
			new mv(
				gp(mv(*((mv *)elem1->elem)), mv(*((bivectorE2GA *)elem2->elem)))
			)
		)
	);
break;

case ROTORE2GA_TAG:
	Glue<C2Elem>::push(
		L, 
		new C2Elem(
			MV_TAG,
			new mv(
				gp(mv(*((mv *)elem1->elem)), mv(*((rotorE2GA *)elem2->elem)))
			)
		)
	);
break;

case NORMALIZEDPOINT_TAG:
	Glue<C2Elem>::push(
		L, 
		new C2Elem(
			MV_TAG,
			new mv(
				gp(mv(*((mv *)elem1->elem)), mv(*((normalizedPoint *)elem2->elem)))
			)
		)
	);
break;

case DUALCIRCLE_TAG:
	Glue<C2Elem>::push(
		L, 
		new C2Elem(
			MV_TAG,
			new mv(
				gp(mv(*((mv *)elem1->elem)), mv(*((dualCircle *)elem2->elem)))
			)
		)
	);
break;

case FREEVECTOR_TAG:
	Glue<C2Elem>::push(
		L, 
		new C2Elem(
			MV_TAG,
			new mv(
				gp(mv(*((mv *)elem1->elem)), mv(*((freeVector *)elem2->elem)))
			)
		)
	);
break;

case FREEBIVECTOR_TAG:
	Glue<C2Elem>::push(
		L, 
		new C2Elem(
			MV_TAG,
			new mv(
				gp(mv(*((mv *)elem1->elem)), mv(*((freeBivector *)elem2->elem)))
			)
		)
	);
break;

case CIRCLE_TAG:
	Glue<C2Elem>::push(
		L, 
		new C2Elem(
			MV_TAG,
			new mv(
				gp(mv(*((mv *)elem1->elem)), mv(*((circle *)elem2->elem)))
			)
		)
	);
break;

case LINE_TAG:
	Glue<C2Elem>::push(
		L, 
		new C2Elem(
			MV_TAG,
			new mv(
				gp(mv(*((mv *)elem1->elem)), mv(*((line *)elem2->elem)))
			)
		)
	);
break;

case PSEUDOSCALAR_TAG:
	Glue<C2Elem>::push(
		L, 
		new C2Elem(
			MV_TAG,
			new mv(
				gp(mv(*((mv *)elem1->elem)), mv(*((pseudoscalar *)elem2->elem)))
			)
		)
	);
break;

case NORMALIZEDTRANSLATOR_TAG:
	Glue<C2Elem>::push(
		L, 
		new C2Elem(
			MV_TAG,
			new mv(
				gp(mv(*((mv *)elem1->elem)), mv(*((normalizedTranslator *)elem2->elem)))
			)
		)
	);
break;

case TRANSLATOR_TAG:
	Glue<C2Elem>::push(
		L, 
		new C2Elem(
			MV_TAG,
			new mv(
				gp(mv(*((mv *)elem1->elem)), mv(*((translator *)elem2->elem)))
			)
		)
	);
break;

case EVENVERSOR_TAG:
	Glue<C2Elem>::push(
		L, 
		new C2Elem(
			MV_TAG,
			new mv(
				gp(mv(*((mv *)elem1->elem)), mv(*((evenVersor *)elem2->elem)))
			)
		)
	);
break;

case ODDVERSOR_TAG:
	Glue<C2Elem>::push(
		L, 
		new C2Elem(
			MV_TAG,
			new mv(
				gp(mv(*((mv *)elem1->elem)), mv(*((oddVersor *)elem2->elem)))
			)
		)
	);
break;

case DOUBLE_TAG:
	Glue<C2Elem>::push(
		L, 
		new C2Elem(
			MV_TAG,
			new mv(
				gp(mv(*((mv *)elem1->elem)), *((double *)elem2))
			)
		)
	);
break;
}
break;

case NO_TAG:
switch(tag2) {
case MV_TAG:
	Glue<C2Elem>::push(
		L, 
		new C2Elem(
			MV_TAG,
			new mv(
				gp(mv(*((no_t *)elem1->elem)), mv(*((mv *)elem2->elem)))
			)
		)
	);
break;

case NO_TAG:
	Glue<C2Elem>::push(
		L, 
		new C2Elem(
			MV_TAG,
			new mv(
				gp(mv(*((no_t *)elem1->elem)), mv(*((no_t *)elem2->elem)))
			)
		)
	);
break;

case E1_TAG:
	Glue<C2Elem>::push(
		L, 
		new C2Elem(
			MV_TAG,
			new mv(
				gp(mv(*((no_t *)elem1->elem)), mv(*((e1_t *)elem2->elem)))
			)
		)
	);
break;

case E2_TAG:
	Glue<C2Elem>::push(
		L, 
		new C2Elem(
			MV_TAG,
			new mv(
				gp(mv(*((no_t *)elem1->elem)), mv(*((e2_t *)elem2->elem)))
			)
		)
	);
break;

case NI_TAG:
	Glue<C2Elem>::push(
		L, 
		new C2Elem(
			EVENVERSOR_TAG,
			new evenVersor(
				gp(*((no_t *)elem1->elem), *((ni_t *)elem2->elem))
			)
		)
	);
break;

case NONI_TAG:
	Glue<C2Elem>::push(
		L, 
		new C2Elem(
			MV_TAG,
			new mv(
				gp(mv(*((no_t *)elem1->elem)), mv(*((noni_t *)elem2->elem)))
			)
		)
	);
break;

case I2_TAG:
	Glue<C2Elem>::push(
		L, 
		new C2Elem(
			MV_TAG,
			new mv(
				gp(mv(*((no_t *)elem1->elem)), mv(*((I2_t *)elem2->elem)))
			)
		)
	);
break;

case I4_TAG:
	Glue<C2Elem>::push(
		L, 
		new C2Elem(
			MV_TAG,
			new mv(
				gp(mv(*((no_t *)elem1->elem)), mv(*((I4_t *)elem2->elem)))
			)
		)
	);
break;

case I4i_TAG:
	Glue<C2Elem>::push(
		L, 
		new C2Elem(
			MV_TAG,
			new mv(
				gp(mv(*((no_t *)elem1->elem)), mv(*((I4i_t *)elem2->elem)))
			)
		)
	);
break;

case VECTORE2GA_TAG:
	Glue<C2Elem>::push(
		L, 
		new C2Elem(
			MV_TAG,
			new mv(
				gp(mv(*((no_t *)elem1->elem)), mv(*((vectorE2GA *)elem2->elem)))
			)
		)
	);
break;

case BIVECTORE2GA_TAG:
	Glue<C2Elem>::push(
		L, 
		new C2Elem(
			MV_TAG,
			new mv(
				gp(mv(*((no_t *)elem1->elem)), mv(*((bivectorE2GA *)elem2->elem)))
			)
		)
	);
break;

case ROTORE2GA_TAG:
	Glue<C2Elem>::push(
		L, 
		new C2Elem(
			MV_TAG,
			new mv(
				gp(mv(*((no_t *)elem1->elem)), mv(*((rotorE2GA *)elem2->elem)))
			)
		)
	);
break;

case NORMALIZEDPOINT_TAG:
	Glue<C2Elem>::push(
		L, 
		new C2Elem(
			MV_TAG,
			new mv(
				gp(mv(*((no_t *)elem1->elem)), mv(*((normalizedPoint *)elem2->elem)))
			)
		)
	);
break;

case DUALCIRCLE_TAG:
	Glue<C2Elem>::push(
		L, 
		new C2Elem(
			MV_TAG,
			new mv(
				gp(mv(*((no_t *)elem1->elem)), mv(*((dualCircle *)elem2->elem)))
			)
		)
	);
break;

case FREEVECTOR_TAG:
	Glue<C2Elem>::push(
		L, 
		new C2Elem(
			MV_TAG,
			new mv(
				gp(mv(*((no_t *)elem1->elem)), mv(*((freeVector *)elem2->elem)))
			)
		)
	);
break;

case FREEBIVECTOR_TAG:
	Glue<C2Elem>::push(
		L, 
		new C2Elem(
			MV_TAG,
			new mv(
				gp(mv(*((no_t *)elem1->elem)), mv(*((freeBivector *)elem2->elem)))
			)
		)
	);
break;

case CIRCLE_TAG:
	Glue<C2Elem>::push(
		L, 
		new C2Elem(
			MV_TAG,
			new mv(
				gp(mv(*((no_t *)elem1->elem)), mv(*((circle *)elem2->elem)))
			)
		)
	);
break;

case LINE_TAG:
	Glue<C2Elem>::push(
		L, 
		new C2Elem(
			MV_TAG,
			new mv(
				gp(mv(*((no_t *)elem1->elem)), mv(*((line *)elem2->elem)))
			)
		)
	);
break;

case PSEUDOSCALAR_TAG:
	Glue<C2Elem>::push(
		L, 
		new C2Elem(
			MV_TAG,
			new mv(
				gp(mv(*((no_t *)elem1->elem)), mv(*((pseudoscalar *)elem2->elem)))
			)
		)
	);
break;

case NORMALIZEDTRANSLATOR_TAG:
	Glue<C2Elem>::push(
		L, 
		new C2Elem(
			MV_TAG,
			new mv(
				gp(mv(*((no_t *)elem1->elem)), mv(*((normalizedTranslator *)elem2->elem)))
			)
		)
	);
break;

case TRANSLATOR_TAG:
	Glue<C2Elem>::push(
		L, 
		new C2Elem(
			MV_TAG,
			new mv(
				gp(mv(*((no_t *)elem1->elem)), mv(*((translator *)elem2->elem)))
			)
		)
	);
break;

case EVENVERSOR_TAG:
	Glue<C2Elem>::push(
		L, 
		new C2Elem(
			MV_TAG,
			new mv(
				gp(mv(*((no_t *)elem1->elem)), mv(*((evenVersor *)elem2->elem)))
			)
		)
	);
break;

case ODDVERSOR_TAG:
	Glue<C2Elem>::push(
		L, 
		new C2Elem(
			MV_TAG,
			new mv(
				gp(mv(*((no_t *)elem1->elem)), mv(*((oddVersor *)elem2->elem)))
			)
		)
	);
break;

case DOUBLE_TAG:
	Glue<C2Elem>::push(
		L, 
		new C2Elem(
			MV_TAG,
			new mv(
				gp(mv(*((no_t *)elem1->elem)), *((double *)elem2))
			)
		)
	);
break;
}
break;

case E1_TAG:
switch(tag2) {
case MV_TAG:
	Glue<C2Elem>::push(
		L, 
		new C2Elem(
			MV_TAG,
			new mv(
				gp(mv(*((e1_t *)elem1->elem)), mv(*((mv *)elem2->elem)))
			)
		)
	);
break;

case NO_TAG:
	Glue<C2Elem>::push(
		L, 
		new C2Elem(
			MV_TAG,
			new mv(
				gp(mv(*((e1_t *)elem1->elem)), mv(*((no_t *)elem2->elem)))
			)
		)
	);
break;

case E1_TAG:
	Glue<C2Elem>::push(
		L, 
		new C2Elem(
			MV_TAG,
			new mv(
				gp(mv(*((e1_t *)elem1->elem)), mv(*((e1_t *)elem2->elem)))
			)
		)
	);
break;

case E2_TAG:
	Glue<C2Elem>::push(
		L, 
		new C2Elem(
			I2_TAG,
			new I2_t(
				gp(*((e1_t *)elem1->elem), *((e2_t *)elem2->elem))
			)
		)
	);
break;

case NI_TAG:
	Glue<C2Elem>::push(
		L, 
		new C2Elem(
			MV_TAG,
			new mv(
				gp(mv(*((e1_t *)elem1->elem)), mv(*((ni_t *)elem2->elem)))
			)
		)
	);
break;

case NONI_TAG:
	Glue<C2Elem>::push(
		L, 
		new C2Elem(
			MV_TAG,
			new mv(
				gp(mv(*((e1_t *)elem1->elem)), mv(*((noni_t *)elem2->elem)))
			)
		)
	);
break;

case I2_TAG:
	Glue<C2Elem>::push(
		L, 
		new C2Elem(
			MV_TAG,
			new mv(
				gp(mv(*((e1_t *)elem1->elem)), mv(*((I2_t *)elem2->elem)))
			)
		)
	);
break;

case I4_TAG:
	Glue<C2Elem>::push(
		L, 
		new C2Elem(
			MV_TAG,
			new mv(
				gp(mv(*((e1_t *)elem1->elem)), mv(*((I4_t *)elem2->elem)))
			)
		)
	);
break;

case I4i_TAG:
	Glue<C2Elem>::push(
		L, 
		new C2Elem(
			MV_TAG,
			new mv(
				gp(mv(*((e1_t *)elem1->elem)), mv(*((I4i_t *)elem2->elem)))
			)
		)
	);
break;

case VECTORE2GA_TAG:
	Glue<C2Elem>::push(
		L, 
		new C2Elem(
			MV_TAG,
			new mv(
				gp(mv(*((e1_t *)elem1->elem)), mv(*((vectorE2GA *)elem2->elem)))
			)
		)
	);
break;

case BIVECTORE2GA_TAG:
	Glue<C2Elem>::push(
		L, 
		new C2Elem(
			MV_TAG,
			new mv(
				gp(mv(*((e1_t *)elem1->elem)), mv(*((bivectorE2GA *)elem2->elem)))
			)
		)
	);
break;

case ROTORE2GA_TAG:
	Glue<C2Elem>::push(
		L, 
		new C2Elem(
			MV_TAG,
			new mv(
				gp(mv(*((e1_t *)elem1->elem)), mv(*((rotorE2GA *)elem2->elem)))
			)
		)
	);
break;

case NORMALIZEDPOINT_TAG:
	Glue<C2Elem>::push(
		L, 
		new C2Elem(
			MV_TAG,
			new mv(
				gp(mv(*((e1_t *)elem1->elem)), mv(*((normalizedPoint *)elem2->elem)))
			)
		)
	);
break;

case DUALCIRCLE_TAG:
	Glue<C2Elem>::push(
		L, 
		new C2Elem(
			MV_TAG,
			new mv(
				gp(mv(*((e1_t *)elem1->elem)), mv(*((dualCircle *)elem2->elem)))
			)
		)
	);
break;

case FREEVECTOR_TAG:
	Glue<C2Elem>::push(
		L, 
		new C2Elem(
			MV_TAG,
			new mv(
				gp(mv(*((e1_t *)elem1->elem)), mv(*((freeVector *)elem2->elem)))
			)
		)
	);
break;

case FREEBIVECTOR_TAG:
	Glue<C2Elem>::push(
		L, 
		new C2Elem(
			MV_TAG,
			new mv(
				gp(mv(*((e1_t *)elem1->elem)), mv(*((freeBivector *)elem2->elem)))
			)
		)
	);
break;

case CIRCLE_TAG:
	Glue<C2Elem>::push(
		L, 
		new C2Elem(
			MV_TAG,
			new mv(
				gp(mv(*((e1_t *)elem1->elem)), mv(*((circle *)elem2->elem)))
			)
		)
	);
break;

case LINE_TAG:
	Glue<C2Elem>::push(
		L, 
		new C2Elem(
			MV_TAG,
			new mv(
				gp(mv(*((e1_t *)elem1->elem)), mv(*((line *)elem2->elem)))
			)
		)
	);
break;

case PSEUDOSCALAR_TAG:
	Glue<C2Elem>::push(
		L, 
		new C2Elem(
			MV_TAG,
			new mv(
				gp(mv(*((e1_t *)elem1->elem)), mv(*((pseudoscalar *)elem2->elem)))
			)
		)
	);
break;

case NORMALIZEDTRANSLATOR_TAG:
	Glue<C2Elem>::push(
		L, 
		new C2Elem(
			MV_TAG,
			new mv(
				gp(mv(*((e1_t *)elem1->elem)), mv(*((normalizedTranslator *)elem2->elem)))
			)
		)
	);
break;

case TRANSLATOR_TAG:
	Glue<C2Elem>::push(
		L, 
		new C2Elem(
			MV_TAG,
			new mv(
				gp(mv(*((e1_t *)elem1->elem)), mv(*((translator *)elem2->elem)))
			)
		)
	);
break;

case EVENVERSOR_TAG:
	Glue<C2Elem>::push(
		L, 
		new C2Elem(
			MV_TAG,
			new mv(
				gp(mv(*((e1_t *)elem1->elem)), mv(*((evenVersor *)elem2->elem)))
			)
		)
	);
break;

case ODDVERSOR_TAG:
	Glue<C2Elem>::push(
		L, 
		new C2Elem(
			MV_TAG,
			new mv(
				gp(mv(*((e1_t *)elem1->elem)), mv(*((oddVersor *)elem2->elem)))
			)
		)
	);
break;

case DOUBLE_TAG:
	Glue<C2Elem>::push(
		L, 
		new C2Elem(
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
	Glue<C2Elem>::push(
		L, 
		new C2Elem(
			MV_TAG,
			new mv(
				gp(mv(*((e2_t *)elem1->elem)), mv(*((mv *)elem2->elem)))
			)
		)
	);
break;

case NO_TAG:
	Glue<C2Elem>::push(
		L, 
		new C2Elem(
			MV_TAG,
			new mv(
				gp(mv(*((e2_t *)elem1->elem)), mv(*((no_t *)elem2->elem)))
			)
		)
	);
break;

case E1_TAG:
	Glue<C2Elem>::push(
		L, 
		new C2Elem(
			MV_TAG,
			new mv(
				gp(mv(*((e2_t *)elem1->elem)), mv(*((e1_t *)elem2->elem)))
			)
		)
	);
break;

case E2_TAG:
	Glue<C2Elem>::push(
		L, 
		new C2Elem(
			MV_TAG,
			new mv(
				gp(mv(*((e2_t *)elem1->elem)), mv(*((e2_t *)elem2->elem)))
			)
		)
	);
break;

case NI_TAG:
	Glue<C2Elem>::push(
		L, 
		new C2Elem(
			MV_TAG,
			new mv(
				gp(mv(*((e2_t *)elem1->elem)), mv(*((ni_t *)elem2->elem)))
			)
		)
	);
break;

case NONI_TAG:
	Glue<C2Elem>::push(
		L, 
		new C2Elem(
			MV_TAG,
			new mv(
				gp(mv(*((e2_t *)elem1->elem)), mv(*((noni_t *)elem2->elem)))
			)
		)
	);
break;

case I2_TAG:
	Glue<C2Elem>::push(
		L, 
		new C2Elem(
			MV_TAG,
			new mv(
				gp(mv(*((e2_t *)elem1->elem)), mv(*((I2_t *)elem2->elem)))
			)
		)
	);
break;

case I4_TAG:
	Glue<C2Elem>::push(
		L, 
		new C2Elem(
			MV_TAG,
			new mv(
				gp(mv(*((e2_t *)elem1->elem)), mv(*((I4_t *)elem2->elem)))
			)
		)
	);
break;

case I4i_TAG:
	Glue<C2Elem>::push(
		L, 
		new C2Elem(
			MV_TAG,
			new mv(
				gp(mv(*((e2_t *)elem1->elem)), mv(*((I4i_t *)elem2->elem)))
			)
		)
	);
break;

case VECTORE2GA_TAG:
	Glue<C2Elem>::push(
		L, 
		new C2Elem(
			MV_TAG,
			new mv(
				gp(mv(*((e2_t *)elem1->elem)), mv(*((vectorE2GA *)elem2->elem)))
			)
		)
	);
break;

case BIVECTORE2GA_TAG:
	Glue<C2Elem>::push(
		L, 
		new C2Elem(
			MV_TAG,
			new mv(
				gp(mv(*((e2_t *)elem1->elem)), mv(*((bivectorE2GA *)elem2->elem)))
			)
		)
	);
break;

case ROTORE2GA_TAG:
	Glue<C2Elem>::push(
		L, 
		new C2Elem(
			MV_TAG,
			new mv(
				gp(mv(*((e2_t *)elem1->elem)), mv(*((rotorE2GA *)elem2->elem)))
			)
		)
	);
break;

case NORMALIZEDPOINT_TAG:
	Glue<C2Elem>::push(
		L, 
		new C2Elem(
			MV_TAG,
			new mv(
				gp(mv(*((e2_t *)elem1->elem)), mv(*((normalizedPoint *)elem2->elem)))
			)
		)
	);
break;

case DUALCIRCLE_TAG:
	Glue<C2Elem>::push(
		L, 
		new C2Elem(
			MV_TAG,
			new mv(
				gp(mv(*((e2_t *)elem1->elem)), mv(*((dualCircle *)elem2->elem)))
			)
		)
	);
break;

case FREEVECTOR_TAG:
	Glue<C2Elem>::push(
		L, 
		new C2Elem(
			MV_TAG,
			new mv(
				gp(mv(*((e2_t *)elem1->elem)), mv(*((freeVector *)elem2->elem)))
			)
		)
	);
break;

case FREEBIVECTOR_TAG:
	Glue<C2Elem>::push(
		L, 
		new C2Elem(
			MV_TAG,
			new mv(
				gp(mv(*((e2_t *)elem1->elem)), mv(*((freeBivector *)elem2->elem)))
			)
		)
	);
break;

case CIRCLE_TAG:
	Glue<C2Elem>::push(
		L, 
		new C2Elem(
			MV_TAG,
			new mv(
				gp(mv(*((e2_t *)elem1->elem)), mv(*((circle *)elem2->elem)))
			)
		)
	);
break;

case LINE_TAG:
	Glue<C2Elem>::push(
		L, 
		new C2Elem(
			MV_TAG,
			new mv(
				gp(mv(*((e2_t *)elem1->elem)), mv(*((line *)elem2->elem)))
			)
		)
	);
break;

case PSEUDOSCALAR_TAG:
	Glue<C2Elem>::push(
		L, 
		new C2Elem(
			MV_TAG,
			new mv(
				gp(mv(*((e2_t *)elem1->elem)), mv(*((pseudoscalar *)elem2->elem)))
			)
		)
	);
break;

case NORMALIZEDTRANSLATOR_TAG:
	Glue<C2Elem>::push(
		L, 
		new C2Elem(
			MV_TAG,
			new mv(
				gp(mv(*((e2_t *)elem1->elem)), mv(*((normalizedTranslator *)elem2->elem)))
			)
		)
	);
break;

case TRANSLATOR_TAG:
	Glue<C2Elem>::push(
		L, 
		new C2Elem(
			MV_TAG,
			new mv(
				gp(mv(*((e2_t *)elem1->elem)), mv(*((translator *)elem2->elem)))
			)
		)
	);
break;

case EVENVERSOR_TAG:
	Glue<C2Elem>::push(
		L, 
		new C2Elem(
			MV_TAG,
			new mv(
				gp(mv(*((e2_t *)elem1->elem)), mv(*((evenVersor *)elem2->elem)))
			)
		)
	);
break;

case ODDVERSOR_TAG:
	Glue<C2Elem>::push(
		L, 
		new C2Elem(
			MV_TAG,
			new mv(
				gp(mv(*((e2_t *)elem1->elem)), mv(*((oddVersor *)elem2->elem)))
			)
		)
	);
break;

case DOUBLE_TAG:
	Glue<C2Elem>::push(
		L, 
		new C2Elem(
			MV_TAG,
			new mv(
				gp(mv(*((e2_t *)elem1->elem)), *((double *)elem2))
			)
		)
	);
break;
}
break;

case NI_TAG:
switch(tag2) {
case MV_TAG:
	Glue<C2Elem>::push(
		L, 
		new C2Elem(
			MV_TAG,
			new mv(
				gp(mv(*((ni_t *)elem1->elem)), mv(*((mv *)elem2->elem)))
			)
		)
	);
break;

case NO_TAG:
	Glue<C2Elem>::push(
		L, 
		new C2Elem(
			MV_TAG,
			new mv(
				gp(mv(*((ni_t *)elem1->elem)), mv(*((no_t *)elem2->elem)))
			)
		)
	);
break;

case E1_TAG:
	Glue<C2Elem>::push(
		L, 
		new C2Elem(
			MV_TAG,
			new mv(
				gp(mv(*((ni_t *)elem1->elem)), mv(*((e1_t *)elem2->elem)))
			)
		)
	);
break;

case E2_TAG:
	Glue<C2Elem>::push(
		L, 
		new C2Elem(
			MV_TAG,
			new mv(
				gp(mv(*((ni_t *)elem1->elem)), mv(*((e2_t *)elem2->elem)))
			)
		)
	);
break;

case NI_TAG:
	Glue<C2Elem>::push(
		L, 
		new C2Elem(
			MV_TAG,
			new mv(
				gp(mv(*((ni_t *)elem1->elem)), mv(*((ni_t *)elem2->elem)))
			)
		)
	);
break;

case NONI_TAG:
	Glue<C2Elem>::push(
		L, 
		new C2Elem(
			MV_TAG,
			new mv(
				gp(mv(*((ni_t *)elem1->elem)), mv(*((noni_t *)elem2->elem)))
			)
		)
	);
break;

case I2_TAG:
	Glue<C2Elem>::push(
		L, 
		new C2Elem(
			MV_TAG,
			new mv(
				gp(mv(*((ni_t *)elem1->elem)), mv(*((I2_t *)elem2->elem)))
			)
		)
	);
break;

case I4_TAG:
	Glue<C2Elem>::push(
		L, 
		new C2Elem(
			MV_TAG,
			new mv(
				gp(mv(*((ni_t *)elem1->elem)), mv(*((I4_t *)elem2->elem)))
			)
		)
	);
break;

case I4i_TAG:
	Glue<C2Elem>::push(
		L, 
		new C2Elem(
			MV_TAG,
			new mv(
				gp(mv(*((ni_t *)elem1->elem)), mv(*((I4i_t *)elem2->elem)))
			)
		)
	);
break;

case VECTORE2GA_TAG:
	Glue<C2Elem>::push(
		L, 
		new C2Elem(
			MV_TAG,
			new mv(
				gp(mv(*((ni_t *)elem1->elem)), mv(*((vectorE2GA *)elem2->elem)))
			)
		)
	);
break;

case BIVECTORE2GA_TAG:
	Glue<C2Elem>::push(
		L, 
		new C2Elem(
			MV_TAG,
			new mv(
				gp(mv(*((ni_t *)elem1->elem)), mv(*((bivectorE2GA *)elem2->elem)))
			)
		)
	);
break;

case ROTORE2GA_TAG:
	Glue<C2Elem>::push(
		L, 
		new C2Elem(
			MV_TAG,
			new mv(
				gp(mv(*((ni_t *)elem1->elem)), mv(*((rotorE2GA *)elem2->elem)))
			)
		)
	);
break;

case NORMALIZEDPOINT_TAG:
	Glue<C2Elem>::push(
		L, 
		new C2Elem(
			MV_TAG,
			new mv(
				gp(mv(*((ni_t *)elem1->elem)), mv(*((normalizedPoint *)elem2->elem)))
			)
		)
	);
break;

case DUALCIRCLE_TAG:
	Glue<C2Elem>::push(
		L, 
		new C2Elem(
			MV_TAG,
			new mv(
				gp(mv(*((ni_t *)elem1->elem)), mv(*((dualCircle *)elem2->elem)))
			)
		)
	);
break;

case FREEVECTOR_TAG:
	Glue<C2Elem>::push(
		L, 
		new C2Elem(
			MV_TAG,
			new mv(
				gp(mv(*((ni_t *)elem1->elem)), mv(*((freeVector *)elem2->elem)))
			)
		)
	);
break;

case FREEBIVECTOR_TAG:
	Glue<C2Elem>::push(
		L, 
		new C2Elem(
			MV_TAG,
			new mv(
				gp(mv(*((ni_t *)elem1->elem)), mv(*((freeBivector *)elem2->elem)))
			)
		)
	);
break;

case CIRCLE_TAG:
	Glue<C2Elem>::push(
		L, 
		new C2Elem(
			MV_TAG,
			new mv(
				gp(mv(*((ni_t *)elem1->elem)), mv(*((circle *)elem2->elem)))
			)
		)
	);
break;

case LINE_TAG:
	Glue<C2Elem>::push(
		L, 
		new C2Elem(
			MV_TAG,
			new mv(
				gp(mv(*((ni_t *)elem1->elem)), mv(*((line *)elem2->elem)))
			)
		)
	);
break;

case PSEUDOSCALAR_TAG:
	Glue<C2Elem>::push(
		L, 
		new C2Elem(
			MV_TAG,
			new mv(
				gp(mv(*((ni_t *)elem1->elem)), mv(*((pseudoscalar *)elem2->elem)))
			)
		)
	);
break;

case NORMALIZEDTRANSLATOR_TAG:
	Glue<C2Elem>::push(
		L, 
		new C2Elem(
			MV_TAG,
			new mv(
				gp(mv(*((ni_t *)elem1->elem)), mv(*((normalizedTranslator *)elem2->elem)))
			)
		)
	);
break;

case TRANSLATOR_TAG:
	Glue<C2Elem>::push(
		L, 
		new C2Elem(
			MV_TAG,
			new mv(
				gp(mv(*((ni_t *)elem1->elem)), mv(*((translator *)elem2->elem)))
			)
		)
	);
break;

case EVENVERSOR_TAG:
	Glue<C2Elem>::push(
		L, 
		new C2Elem(
			MV_TAG,
			new mv(
				gp(mv(*((ni_t *)elem1->elem)), mv(*((evenVersor *)elem2->elem)))
			)
		)
	);
break;

case ODDVERSOR_TAG:
	Glue<C2Elem>::push(
		L, 
		new C2Elem(
			MV_TAG,
			new mv(
				gp(mv(*((ni_t *)elem1->elem)), mv(*((oddVersor *)elem2->elem)))
			)
		)
	);
break;

case DOUBLE_TAG:
	Glue<C2Elem>::push(
		L, 
		new C2Elem(
			MV_TAG,
			new mv(
				gp(mv(*((ni_t *)elem1->elem)), *((double *)elem2))
			)
		)
	);
break;
}
break;

case NONI_TAG:
switch(tag2) {
case MV_TAG:
	Glue<C2Elem>::push(
		L, 
		new C2Elem(
			MV_TAG,
			new mv(
				gp(mv(*((noni_t *)elem1->elem)), mv(*((mv *)elem2->elem)))
			)
		)
	);
break;

case NO_TAG:
	Glue<C2Elem>::push(
		L, 
		new C2Elem(
			MV_TAG,
			new mv(
				gp(mv(*((noni_t *)elem1->elem)), mv(*((no_t *)elem2->elem)))
			)
		)
	);
break;

case E1_TAG:
	Glue<C2Elem>::push(
		L, 
		new C2Elem(
			MV_TAG,
			new mv(
				gp(mv(*((noni_t *)elem1->elem)), mv(*((e1_t *)elem2->elem)))
			)
		)
	);
break;

case E2_TAG:
	Glue<C2Elem>::push(
		L, 
		new C2Elem(
			MV_TAG,
			new mv(
				gp(mv(*((noni_t *)elem1->elem)), mv(*((e2_t *)elem2->elem)))
			)
		)
	);
break;

case NI_TAG:
	Glue<C2Elem>::push(
		L, 
		new C2Elem(
			MV_TAG,
			new mv(
				gp(mv(*((noni_t *)elem1->elem)), mv(*((ni_t *)elem2->elem)))
			)
		)
	);
break;

case NONI_TAG:
	Glue<C2Elem>::push(
		L, 
		new C2Elem(
			MV_TAG,
			new mv(
				gp(mv(*((noni_t *)elem1->elem)), mv(*((noni_t *)elem2->elem)))
			)
		)
	);
break;

case I2_TAG:
	Glue<C2Elem>::push(
		L, 
		new C2Elem(
			MV_TAG,
			new mv(
				gp(mv(*((noni_t *)elem1->elem)), mv(*((I2_t *)elem2->elem)))
			)
		)
	);
break;

case I4_TAG:
	Glue<C2Elem>::push(
		L, 
		new C2Elem(
			MV_TAG,
			new mv(
				gp(mv(*((noni_t *)elem1->elem)), mv(*((I4_t *)elem2->elem)))
			)
		)
	);
break;

case I4i_TAG:
	Glue<C2Elem>::push(
		L, 
		new C2Elem(
			MV_TAG,
			new mv(
				gp(mv(*((noni_t *)elem1->elem)), mv(*((I4i_t *)elem2->elem)))
			)
		)
	);
break;

case VECTORE2GA_TAG:
	Glue<C2Elem>::push(
		L, 
		new C2Elem(
			MV_TAG,
			new mv(
				gp(mv(*((noni_t *)elem1->elem)), mv(*((vectorE2GA *)elem2->elem)))
			)
		)
	);
break;

case BIVECTORE2GA_TAG:
	Glue<C2Elem>::push(
		L, 
		new C2Elem(
			MV_TAG,
			new mv(
				gp(mv(*((noni_t *)elem1->elem)), mv(*((bivectorE2GA *)elem2->elem)))
			)
		)
	);
break;

case ROTORE2GA_TAG:
	Glue<C2Elem>::push(
		L, 
		new C2Elem(
			MV_TAG,
			new mv(
				gp(mv(*((noni_t *)elem1->elem)), mv(*((rotorE2GA *)elem2->elem)))
			)
		)
	);
break;

case NORMALIZEDPOINT_TAG:
	Glue<C2Elem>::push(
		L, 
		new C2Elem(
			MV_TAG,
			new mv(
				gp(mv(*((noni_t *)elem1->elem)), mv(*((normalizedPoint *)elem2->elem)))
			)
		)
	);
break;

case DUALCIRCLE_TAG:
	Glue<C2Elem>::push(
		L, 
		new C2Elem(
			MV_TAG,
			new mv(
				gp(mv(*((noni_t *)elem1->elem)), mv(*((dualCircle *)elem2->elem)))
			)
		)
	);
break;

case FREEVECTOR_TAG:
	Glue<C2Elem>::push(
		L, 
		new C2Elem(
			MV_TAG,
			new mv(
				gp(mv(*((noni_t *)elem1->elem)), mv(*((freeVector *)elem2->elem)))
			)
		)
	);
break;

case FREEBIVECTOR_TAG:
	Glue<C2Elem>::push(
		L, 
		new C2Elem(
			MV_TAG,
			new mv(
				gp(mv(*((noni_t *)elem1->elem)), mv(*((freeBivector *)elem2->elem)))
			)
		)
	);
break;

case CIRCLE_TAG:
	Glue<C2Elem>::push(
		L, 
		new C2Elem(
			MV_TAG,
			new mv(
				gp(mv(*((noni_t *)elem1->elem)), mv(*((circle *)elem2->elem)))
			)
		)
	);
break;

case LINE_TAG:
	Glue<C2Elem>::push(
		L, 
		new C2Elem(
			MV_TAG,
			new mv(
				gp(mv(*((noni_t *)elem1->elem)), mv(*((line *)elem2->elem)))
			)
		)
	);
break;

case PSEUDOSCALAR_TAG:
	Glue<C2Elem>::push(
		L, 
		new C2Elem(
			MV_TAG,
			new mv(
				gp(mv(*((noni_t *)elem1->elem)), mv(*((pseudoscalar *)elem2->elem)))
			)
		)
	);
break;

case NORMALIZEDTRANSLATOR_TAG:
	Glue<C2Elem>::push(
		L, 
		new C2Elem(
			MV_TAG,
			new mv(
				gp(mv(*((noni_t *)elem1->elem)), mv(*((normalizedTranslator *)elem2->elem)))
			)
		)
	);
break;

case TRANSLATOR_TAG:
	Glue<C2Elem>::push(
		L, 
		new C2Elem(
			MV_TAG,
			new mv(
				gp(mv(*((noni_t *)elem1->elem)), mv(*((translator *)elem2->elem)))
			)
		)
	);
break;

case EVENVERSOR_TAG:
	Glue<C2Elem>::push(
		L, 
		new C2Elem(
			MV_TAG,
			new mv(
				gp(mv(*((noni_t *)elem1->elem)), mv(*((evenVersor *)elem2->elem)))
			)
		)
	);
break;

case ODDVERSOR_TAG:
	Glue<C2Elem>::push(
		L, 
		new C2Elem(
			MV_TAG,
			new mv(
				gp(mv(*((noni_t *)elem1->elem)), mv(*((oddVersor *)elem2->elem)))
			)
		)
	);
break;

case DOUBLE_TAG:
	Glue<C2Elem>::push(
		L, 
		new C2Elem(
			MV_TAG,
			new mv(
				gp(mv(*((noni_t *)elem1->elem)), *((double *)elem2))
			)
		)
	);
break;
}
break;

case I2_TAG:
switch(tag2) {
case MV_TAG:
	Glue<C2Elem>::push(
		L, 
		new C2Elem(
			MV_TAG,
			new mv(
				gp(mv(*((I2_t *)elem1->elem)), mv(*((mv *)elem2->elem)))
			)
		)
	);
break;

case NO_TAG:
	Glue<C2Elem>::push(
		L, 
		new C2Elem(
			MV_TAG,
			new mv(
				gp(mv(*((I2_t *)elem1->elem)), mv(*((no_t *)elem2->elem)))
			)
		)
	);
break;

case E1_TAG:
	Glue<C2Elem>::push(
		L, 
		new C2Elem(
			MV_TAG,
			new mv(
				gp(mv(*((I2_t *)elem1->elem)), mv(*((e1_t *)elem2->elem)))
			)
		)
	);
break;

case E2_TAG:
	Glue<C2Elem>::push(
		L, 
		new C2Elem(
			MV_TAG,
			new mv(
				gp(mv(*((I2_t *)elem1->elem)), mv(*((e2_t *)elem2->elem)))
			)
		)
	);
break;

case NI_TAG:
	Glue<C2Elem>::push(
		L, 
		new C2Elem(
			MV_TAG,
			new mv(
				gp(mv(*((I2_t *)elem1->elem)), mv(*((ni_t *)elem2->elem)))
			)
		)
	);
break;

case NONI_TAG:
	Glue<C2Elem>::push(
		L, 
		new C2Elem(
			MV_TAG,
			new mv(
				gp(mv(*((I2_t *)elem1->elem)), mv(*((noni_t *)elem2->elem)))
			)
		)
	);
break;

case I2_TAG:
	Glue<C2Elem>::push(
		L, 
		new C2Elem(
			MV_TAG,
			new mv(
				gp(mv(*((I2_t *)elem1->elem)), mv(*((I2_t *)elem2->elem)))
			)
		)
	);
break;

case I4_TAG:
	Glue<C2Elem>::push(
		L, 
		new C2Elem(
			MV_TAG,
			new mv(
				gp(mv(*((I2_t *)elem1->elem)), mv(*((I4_t *)elem2->elem)))
			)
		)
	);
break;

case I4i_TAG:
	Glue<C2Elem>::push(
		L, 
		new C2Elem(
			MV_TAG,
			new mv(
				gp(mv(*((I2_t *)elem1->elem)), mv(*((I4i_t *)elem2->elem)))
			)
		)
	);
break;

case VECTORE2GA_TAG:
	Glue<C2Elem>::push(
		L, 
		new C2Elem(
			MV_TAG,
			new mv(
				gp(mv(*((I2_t *)elem1->elem)), mv(*((vectorE2GA *)elem2->elem)))
			)
		)
	);
break;

case BIVECTORE2GA_TAG:
	Glue<C2Elem>::push(
		L, 
		new C2Elem(
			MV_TAG,
			new mv(
				gp(mv(*((I2_t *)elem1->elem)), mv(*((bivectorE2GA *)elem2->elem)))
			)
		)
	);
break;

case ROTORE2GA_TAG:
	Glue<C2Elem>::push(
		L, 
		new C2Elem(
			MV_TAG,
			new mv(
				gp(mv(*((I2_t *)elem1->elem)), mv(*((rotorE2GA *)elem2->elem)))
			)
		)
	);
break;

case NORMALIZEDPOINT_TAG:
	Glue<C2Elem>::push(
		L, 
		new C2Elem(
			MV_TAG,
			new mv(
				gp(mv(*((I2_t *)elem1->elem)), mv(*((normalizedPoint *)elem2->elem)))
			)
		)
	);
break;

case DUALCIRCLE_TAG:
	Glue<C2Elem>::push(
		L, 
		new C2Elem(
			MV_TAG,
			new mv(
				gp(mv(*((I2_t *)elem1->elem)), mv(*((dualCircle *)elem2->elem)))
			)
		)
	);
break;

case FREEVECTOR_TAG:
	Glue<C2Elem>::push(
		L, 
		new C2Elem(
			MV_TAG,
			new mv(
				gp(mv(*((I2_t *)elem1->elem)), mv(*((freeVector *)elem2->elem)))
			)
		)
	);
break;

case FREEBIVECTOR_TAG:
	Glue<C2Elem>::push(
		L, 
		new C2Elem(
			MV_TAG,
			new mv(
				gp(mv(*((I2_t *)elem1->elem)), mv(*((freeBivector *)elem2->elem)))
			)
		)
	);
break;

case CIRCLE_TAG:
	Glue<C2Elem>::push(
		L, 
		new C2Elem(
			MV_TAG,
			new mv(
				gp(mv(*((I2_t *)elem1->elem)), mv(*((circle *)elem2->elem)))
			)
		)
	);
break;

case LINE_TAG:
	Glue<C2Elem>::push(
		L, 
		new C2Elem(
			MV_TAG,
			new mv(
				gp(mv(*((I2_t *)elem1->elem)), mv(*((line *)elem2->elem)))
			)
		)
	);
break;

case PSEUDOSCALAR_TAG:
	Glue<C2Elem>::push(
		L, 
		new C2Elem(
			MV_TAG,
			new mv(
				gp(mv(*((I2_t *)elem1->elem)), mv(*((pseudoscalar *)elem2->elem)))
			)
		)
	);
break;

case NORMALIZEDTRANSLATOR_TAG:
	Glue<C2Elem>::push(
		L, 
		new C2Elem(
			MV_TAG,
			new mv(
				gp(mv(*((I2_t *)elem1->elem)), mv(*((normalizedTranslator *)elem2->elem)))
			)
		)
	);
break;

case TRANSLATOR_TAG:
	Glue<C2Elem>::push(
		L, 
		new C2Elem(
			MV_TAG,
			new mv(
				gp(mv(*((I2_t *)elem1->elem)), mv(*((translator *)elem2->elem)))
			)
		)
	);
break;

case EVENVERSOR_TAG:
	Glue<C2Elem>::push(
		L, 
		new C2Elem(
			MV_TAG,
			new mv(
				gp(mv(*((I2_t *)elem1->elem)), mv(*((evenVersor *)elem2->elem)))
			)
		)
	);
break;

case ODDVERSOR_TAG:
	Glue<C2Elem>::push(
		L, 
		new C2Elem(
			MV_TAG,
			new mv(
				gp(mv(*((I2_t *)elem1->elem)), mv(*((oddVersor *)elem2->elem)))
			)
		)
	);
break;

case DOUBLE_TAG:
	Glue<C2Elem>::push(
		L, 
		new C2Elem(
			MV_TAG,
			new mv(
				gp(mv(*((I2_t *)elem1->elem)), *((double *)elem2))
			)
		)
	);
break;
}
break;

case I4_TAG:
switch(tag2) {
case MV_TAG:
	Glue<C2Elem>::push(
		L, 
		new C2Elem(
			MV_TAG,
			new mv(
				gp(mv(*((I4_t *)elem1->elem)), mv(*((mv *)elem2->elem)))
			)
		)
	);
break;

case NO_TAG:
	Glue<C2Elem>::push(
		L, 
		new C2Elem(
			MV_TAG,
			new mv(
				gp(mv(*((I4_t *)elem1->elem)), mv(*((no_t *)elem2->elem)))
			)
		)
	);
break;

case E1_TAG:
	Glue<C2Elem>::push(
		L, 
		new C2Elem(
			MV_TAG,
			new mv(
				gp(mv(*((I4_t *)elem1->elem)), mv(*((e1_t *)elem2->elem)))
			)
		)
	);
break;

case E2_TAG:
	Glue<C2Elem>::push(
		L, 
		new C2Elem(
			MV_TAG,
			new mv(
				gp(mv(*((I4_t *)elem1->elem)), mv(*((e2_t *)elem2->elem)))
			)
		)
	);
break;

case NI_TAG:
	Glue<C2Elem>::push(
		L, 
		new C2Elem(
			MV_TAG,
			new mv(
				gp(mv(*((I4_t *)elem1->elem)), mv(*((ni_t *)elem2->elem)))
			)
		)
	);
break;

case NONI_TAG:
	Glue<C2Elem>::push(
		L, 
		new C2Elem(
			MV_TAG,
			new mv(
				gp(mv(*((I4_t *)elem1->elem)), mv(*((noni_t *)elem2->elem)))
			)
		)
	);
break;

case I2_TAG:
	Glue<C2Elem>::push(
		L, 
		new C2Elem(
			MV_TAG,
			new mv(
				gp(mv(*((I4_t *)elem1->elem)), mv(*((I2_t *)elem2->elem)))
			)
		)
	);
break;

case I4_TAG:
	Glue<C2Elem>::push(
		L, 
		new C2Elem(
			MV_TAG,
			new mv(
				gp(mv(*((I4_t *)elem1->elem)), mv(*((I4_t *)elem2->elem)))
			)
		)
	);
break;

case I4i_TAG:
	Glue<C2Elem>::push(
		L, 
		new C2Elem(
			MV_TAG,
			new mv(
				gp(mv(*((I4_t *)elem1->elem)), mv(*((I4i_t *)elem2->elem)))
			)
		)
	);
break;

case VECTORE2GA_TAG:
	Glue<C2Elem>::push(
		L, 
		new C2Elem(
			MV_TAG,
			new mv(
				gp(mv(*((I4_t *)elem1->elem)), mv(*((vectorE2GA *)elem2->elem)))
			)
		)
	);
break;

case BIVECTORE2GA_TAG:
	Glue<C2Elem>::push(
		L, 
		new C2Elem(
			MV_TAG,
			new mv(
				gp(mv(*((I4_t *)elem1->elem)), mv(*((bivectorE2GA *)elem2->elem)))
			)
		)
	);
break;

case ROTORE2GA_TAG:
	Glue<C2Elem>::push(
		L, 
		new C2Elem(
			MV_TAG,
			new mv(
				gp(mv(*((I4_t *)elem1->elem)), mv(*((rotorE2GA *)elem2->elem)))
			)
		)
	);
break;

case NORMALIZEDPOINT_TAG:
	Glue<C2Elem>::push(
		L, 
		new C2Elem(
			MV_TAG,
			new mv(
				gp(mv(*((I4_t *)elem1->elem)), mv(*((normalizedPoint *)elem2->elem)))
			)
		)
	);
break;

case DUALCIRCLE_TAG:
	Glue<C2Elem>::push(
		L, 
		new C2Elem(
			MV_TAG,
			new mv(
				gp(mv(*((I4_t *)elem1->elem)), mv(*((dualCircle *)elem2->elem)))
			)
		)
	);
break;

case FREEVECTOR_TAG:
	Glue<C2Elem>::push(
		L, 
		new C2Elem(
			MV_TAG,
			new mv(
				gp(mv(*((I4_t *)elem1->elem)), mv(*((freeVector *)elem2->elem)))
			)
		)
	);
break;

case FREEBIVECTOR_TAG:
	Glue<C2Elem>::push(
		L, 
		new C2Elem(
			MV_TAG,
			new mv(
				gp(mv(*((I4_t *)elem1->elem)), mv(*((freeBivector *)elem2->elem)))
			)
		)
	);
break;

case CIRCLE_TAG:
	Glue<C2Elem>::push(
		L, 
		new C2Elem(
			MV_TAG,
			new mv(
				gp(mv(*((I4_t *)elem1->elem)), mv(*((circle *)elem2->elem)))
			)
		)
	);
break;

case LINE_TAG:
	Glue<C2Elem>::push(
		L, 
		new C2Elem(
			MV_TAG,
			new mv(
				gp(mv(*((I4_t *)elem1->elem)), mv(*((line *)elem2->elem)))
			)
		)
	);
break;

case PSEUDOSCALAR_TAG:
	Glue<C2Elem>::push(
		L, 
		new C2Elem(
			MV_TAG,
			new mv(
				gp(mv(*((I4_t *)elem1->elem)), mv(*((pseudoscalar *)elem2->elem)))
			)
		)
	);
break;

case NORMALIZEDTRANSLATOR_TAG:
	Glue<C2Elem>::push(
		L, 
		new C2Elem(
			MV_TAG,
			new mv(
				gp(mv(*((I4_t *)elem1->elem)), mv(*((normalizedTranslator *)elem2->elem)))
			)
		)
	);
break;

case TRANSLATOR_TAG:
	Glue<C2Elem>::push(
		L, 
		new C2Elem(
			MV_TAG,
			new mv(
				gp(mv(*((I4_t *)elem1->elem)), mv(*((translator *)elem2->elem)))
			)
		)
	);
break;

case EVENVERSOR_TAG:
	Glue<C2Elem>::push(
		L, 
		new C2Elem(
			MV_TAG,
			new mv(
				gp(mv(*((I4_t *)elem1->elem)), mv(*((evenVersor *)elem2->elem)))
			)
		)
	);
break;

case ODDVERSOR_TAG:
	Glue<C2Elem>::push(
		L, 
		new C2Elem(
			MV_TAG,
			new mv(
				gp(mv(*((I4_t *)elem1->elem)), mv(*((oddVersor *)elem2->elem)))
			)
		)
	);
break;

case DOUBLE_TAG:
	Glue<C2Elem>::push(
		L, 
		new C2Elem(
			MV_TAG,
			new mv(
				gp(mv(*((I4_t *)elem1->elem)), *((double *)elem2))
			)
		)
	);
break;
}
break;

case I4i_TAG:
switch(tag2) {
case MV_TAG:
	Glue<C2Elem>::push(
		L, 
		new C2Elem(
			MV_TAG,
			new mv(
				gp(mv(*((I4i_t *)elem1->elem)), mv(*((mv *)elem2->elem)))
			)
		)
	);
break;

case NO_TAG:
	Glue<C2Elem>::push(
		L, 
		new C2Elem(
			MV_TAG,
			new mv(
				gp(mv(*((I4i_t *)elem1->elem)), mv(*((no_t *)elem2->elem)))
			)
		)
	);
break;

case E1_TAG:
	Glue<C2Elem>::push(
		L, 
		new C2Elem(
			MV_TAG,
			new mv(
				gp(mv(*((I4i_t *)elem1->elem)), mv(*((e1_t *)elem2->elem)))
			)
		)
	);
break;

case E2_TAG:
	Glue<C2Elem>::push(
		L, 
		new C2Elem(
			MV_TAG,
			new mv(
				gp(mv(*((I4i_t *)elem1->elem)), mv(*((e2_t *)elem2->elem)))
			)
		)
	);
break;

case NI_TAG:
	Glue<C2Elem>::push(
		L, 
		new C2Elem(
			MV_TAG,
			new mv(
				gp(mv(*((I4i_t *)elem1->elem)), mv(*((ni_t *)elem2->elem)))
			)
		)
	);
break;

case NONI_TAG:
	Glue<C2Elem>::push(
		L, 
		new C2Elem(
			MV_TAG,
			new mv(
				gp(mv(*((I4i_t *)elem1->elem)), mv(*((noni_t *)elem2->elem)))
			)
		)
	);
break;

case I2_TAG:
	Glue<C2Elem>::push(
		L, 
		new C2Elem(
			MV_TAG,
			new mv(
				gp(mv(*((I4i_t *)elem1->elem)), mv(*((I2_t *)elem2->elem)))
			)
		)
	);
break;

case I4_TAG:
	Glue<C2Elem>::push(
		L, 
		new C2Elem(
			MV_TAG,
			new mv(
				gp(mv(*((I4i_t *)elem1->elem)), mv(*((I4_t *)elem2->elem)))
			)
		)
	);
break;

case I4i_TAG:
	Glue<C2Elem>::push(
		L, 
		new C2Elem(
			MV_TAG,
			new mv(
				gp(mv(*((I4i_t *)elem1->elem)), mv(*((I4i_t *)elem2->elem)))
			)
		)
	);
break;

case VECTORE2GA_TAG:
	Glue<C2Elem>::push(
		L, 
		new C2Elem(
			MV_TAG,
			new mv(
				gp(mv(*((I4i_t *)elem1->elem)), mv(*((vectorE2GA *)elem2->elem)))
			)
		)
	);
break;

case BIVECTORE2GA_TAG:
	Glue<C2Elem>::push(
		L, 
		new C2Elem(
			MV_TAG,
			new mv(
				gp(mv(*((I4i_t *)elem1->elem)), mv(*((bivectorE2GA *)elem2->elem)))
			)
		)
	);
break;

case ROTORE2GA_TAG:
	Glue<C2Elem>::push(
		L, 
		new C2Elem(
			MV_TAG,
			new mv(
				gp(mv(*((I4i_t *)elem1->elem)), mv(*((rotorE2GA *)elem2->elem)))
			)
		)
	);
break;

case NORMALIZEDPOINT_TAG:
	Glue<C2Elem>::push(
		L, 
		new C2Elem(
			MV_TAG,
			new mv(
				gp(mv(*((I4i_t *)elem1->elem)), mv(*((normalizedPoint *)elem2->elem)))
			)
		)
	);
break;

case DUALCIRCLE_TAG:
	Glue<C2Elem>::push(
		L, 
		new C2Elem(
			MV_TAG,
			new mv(
				gp(mv(*((I4i_t *)elem1->elem)), mv(*((dualCircle *)elem2->elem)))
			)
		)
	);
break;

case FREEVECTOR_TAG:
	Glue<C2Elem>::push(
		L, 
		new C2Elem(
			MV_TAG,
			new mv(
				gp(mv(*((I4i_t *)elem1->elem)), mv(*((freeVector *)elem2->elem)))
			)
		)
	);
break;

case FREEBIVECTOR_TAG:
	Glue<C2Elem>::push(
		L, 
		new C2Elem(
			MV_TAG,
			new mv(
				gp(mv(*((I4i_t *)elem1->elem)), mv(*((freeBivector *)elem2->elem)))
			)
		)
	);
break;

case CIRCLE_TAG:
	Glue<C2Elem>::push(
		L, 
		new C2Elem(
			MV_TAG,
			new mv(
				gp(mv(*((I4i_t *)elem1->elem)), mv(*((circle *)elem2->elem)))
			)
		)
	);
break;

case LINE_TAG:
	Glue<C2Elem>::push(
		L, 
		new C2Elem(
			MV_TAG,
			new mv(
				gp(mv(*((I4i_t *)elem1->elem)), mv(*((line *)elem2->elem)))
			)
		)
	);
break;

case PSEUDOSCALAR_TAG:
	Glue<C2Elem>::push(
		L, 
		new C2Elem(
			MV_TAG,
			new mv(
				gp(mv(*((I4i_t *)elem1->elem)), mv(*((pseudoscalar *)elem2->elem)))
			)
		)
	);
break;

case NORMALIZEDTRANSLATOR_TAG:
	Glue<C2Elem>::push(
		L, 
		new C2Elem(
			MV_TAG,
			new mv(
				gp(mv(*((I4i_t *)elem1->elem)), mv(*((normalizedTranslator *)elem2->elem)))
			)
		)
	);
break;

case TRANSLATOR_TAG:
	Glue<C2Elem>::push(
		L, 
		new C2Elem(
			MV_TAG,
			new mv(
				gp(mv(*((I4i_t *)elem1->elem)), mv(*((translator *)elem2->elem)))
			)
		)
	);
break;

case EVENVERSOR_TAG:
	Glue<C2Elem>::push(
		L, 
		new C2Elem(
			MV_TAG,
			new mv(
				gp(mv(*((I4i_t *)elem1->elem)), mv(*((evenVersor *)elem2->elem)))
			)
		)
	);
break;

case ODDVERSOR_TAG:
	Glue<C2Elem>::push(
		L, 
		new C2Elem(
			MV_TAG,
			new mv(
				gp(mv(*((I4i_t *)elem1->elem)), mv(*((oddVersor *)elem2->elem)))
			)
		)
	);
break;

case DOUBLE_TAG:
	Glue<C2Elem>::push(
		L, 
		new C2Elem(
			MV_TAG,
			new mv(
				gp(mv(*((I4i_t *)elem1->elem)), *((double *)elem2))
			)
		)
	);
break;
}
break;

case VECTORE2GA_TAG:
switch(tag2) {
case MV_TAG:
	Glue<C2Elem>::push(
		L, 
		new C2Elem(
			MV_TAG,
			new mv(
				gp(mv(*((vectorE2GA *)elem1->elem)), mv(*((mv *)elem2->elem)))
			)
		)
	);
break;

case NO_TAG:
	Glue<C2Elem>::push(
		L, 
		new C2Elem(
			MV_TAG,
			new mv(
				gp(mv(*((vectorE2GA *)elem1->elem)), mv(*((no_t *)elem2->elem)))
			)
		)
	);
break;

case E1_TAG:
	Glue<C2Elem>::push(
		L, 
		new C2Elem(
			MV_TAG,
			new mv(
				gp(mv(*((vectorE2GA *)elem1->elem)), mv(*((e1_t *)elem2->elem)))
			)
		)
	);
break;

case E2_TAG:
	Glue<C2Elem>::push(
		L, 
		new C2Elem(
			MV_TAG,
			new mv(
				gp(mv(*((vectorE2GA *)elem1->elem)), mv(*((e2_t *)elem2->elem)))
			)
		)
	);
break;

case NI_TAG:
	Glue<C2Elem>::push(
		L, 
		new C2Elem(
			MV_TAG,
			new mv(
				gp(mv(*((vectorE2GA *)elem1->elem)), mv(*((ni_t *)elem2->elem)))
			)
		)
	);
break;

case NONI_TAG:
	Glue<C2Elem>::push(
		L, 
		new C2Elem(
			MV_TAG,
			new mv(
				gp(mv(*((vectorE2GA *)elem1->elem)), mv(*((noni_t *)elem2->elem)))
			)
		)
	);
break;

case I2_TAG:
	Glue<C2Elem>::push(
		L, 
		new C2Elem(
			MV_TAG,
			new mv(
				gp(mv(*((vectorE2GA *)elem1->elem)), mv(*((I2_t *)elem2->elem)))
			)
		)
	);
break;

case I4_TAG:
	Glue<C2Elem>::push(
		L, 
		new C2Elem(
			MV_TAG,
			new mv(
				gp(mv(*((vectorE2GA *)elem1->elem)), mv(*((I4_t *)elem2->elem)))
			)
		)
	);
break;

case I4i_TAG:
	Glue<C2Elem>::push(
		L, 
		new C2Elem(
			MV_TAG,
			new mv(
				gp(mv(*((vectorE2GA *)elem1->elem)), mv(*((I4i_t *)elem2->elem)))
			)
		)
	);
break;

case VECTORE2GA_TAG:
	Glue<C2Elem>::push(
		L, 
		new C2Elem(
			ROTORE2GA_TAG,
			new rotorE2GA(
				gp(*((vectorE2GA *)elem1->elem), *((vectorE2GA *)elem2->elem))
			)
		)
	);
break;

case BIVECTORE2GA_TAG:
	Glue<C2Elem>::push(
		L, 
		new C2Elem(
			MV_TAG,
			new mv(
				gp(mv(*((vectorE2GA *)elem1->elem)), mv(*((bivectorE2GA *)elem2->elem)))
			)
		)
	);
break;

case ROTORE2GA_TAG:
	Glue<C2Elem>::push(
		L, 
		new C2Elem(
			VECTORE2GA_TAG,
			new vectorE2GA(
				gp(*((vectorE2GA *)elem1->elem), *((rotorE2GA *)elem2->elem))
			)
		)
	);
break;

case NORMALIZEDPOINT_TAG:
	Glue<C2Elem>::push(
		L, 
		new C2Elem(
			MV_TAG,
			new mv(
				gp(mv(*((vectorE2GA *)elem1->elem)), mv(*((normalizedPoint *)elem2->elem)))
			)
		)
	);
break;

case DUALCIRCLE_TAG:
	Glue<C2Elem>::push(
		L, 
		new C2Elem(
			MV_TAG,
			new mv(
				gp(mv(*((vectorE2GA *)elem1->elem)), mv(*((dualCircle *)elem2->elem)))
			)
		)
	);
break;

case FREEVECTOR_TAG:
	Glue<C2Elem>::push(
		L, 
		new C2Elem(
			MV_TAG,
			new mv(
				gp(mv(*((vectorE2GA *)elem1->elem)), mv(*((freeVector *)elem2->elem)))
			)
		)
	);
break;

case FREEBIVECTOR_TAG:
	Glue<C2Elem>::push(
		L, 
		new C2Elem(
			MV_TAG,
			new mv(
				gp(mv(*((vectorE2GA *)elem1->elem)), mv(*((freeBivector *)elem2->elem)))
			)
		)
	);
break;

case CIRCLE_TAG:
	Glue<C2Elem>::push(
		L, 
		new C2Elem(
			MV_TAG,
			new mv(
				gp(mv(*((vectorE2GA *)elem1->elem)), mv(*((circle *)elem2->elem)))
			)
		)
	);
break;

case LINE_TAG:
	Glue<C2Elem>::push(
		L, 
		new C2Elem(
			MV_TAG,
			new mv(
				gp(mv(*((vectorE2GA *)elem1->elem)), mv(*((line *)elem2->elem)))
			)
		)
	);
break;

case PSEUDOSCALAR_TAG:
	Glue<C2Elem>::push(
		L, 
		new C2Elem(
			MV_TAG,
			new mv(
				gp(mv(*((vectorE2GA *)elem1->elem)), mv(*((pseudoscalar *)elem2->elem)))
			)
		)
	);
break;

case NORMALIZEDTRANSLATOR_TAG:
	Glue<C2Elem>::push(
		L, 
		new C2Elem(
			MV_TAG,
			new mv(
				gp(mv(*((vectorE2GA *)elem1->elem)), mv(*((normalizedTranslator *)elem2->elem)))
			)
		)
	);
break;

case TRANSLATOR_TAG:
	Glue<C2Elem>::push(
		L, 
		new C2Elem(
			MV_TAG,
			new mv(
				gp(mv(*((vectorE2GA *)elem1->elem)), mv(*((translator *)elem2->elem)))
			)
		)
	);
break;

case EVENVERSOR_TAG:
	Glue<C2Elem>::push(
		L, 
		new C2Elem(
			MV_TAG,
			new mv(
				gp(mv(*((vectorE2GA *)elem1->elem)), mv(*((evenVersor *)elem2->elem)))
			)
		)
	);
break;

case ODDVERSOR_TAG:
	Glue<C2Elem>::push(
		L, 
		new C2Elem(
			MV_TAG,
			new mv(
				gp(mv(*((vectorE2GA *)elem1->elem)), mv(*((oddVersor *)elem2->elem)))
			)
		)
	);
break;

case DOUBLE_TAG:
	Glue<C2Elem>::push(
		L, 
		new C2Elem(
			MV_TAG,
			new mv(
				gp(mv(*((vectorE2GA *)elem1->elem)), *((double *)elem2))
			)
		)
	);
break;
}
break;

case BIVECTORE2GA_TAG:
switch(tag2) {
case MV_TAG:
	Glue<C2Elem>::push(
		L, 
		new C2Elem(
			MV_TAG,
			new mv(
				gp(mv(*((bivectorE2GA *)elem1->elem)), mv(*((mv *)elem2->elem)))
			)
		)
	);
break;

case NO_TAG:
	Glue<C2Elem>::push(
		L, 
		new C2Elem(
			MV_TAG,
			new mv(
				gp(mv(*((bivectorE2GA *)elem1->elem)), mv(*((no_t *)elem2->elem)))
			)
		)
	);
break;

case E1_TAG:
	Glue<C2Elem>::push(
		L, 
		new C2Elem(
			MV_TAG,
			new mv(
				gp(mv(*((bivectorE2GA *)elem1->elem)), mv(*((e1_t *)elem2->elem)))
			)
		)
	);
break;

case E2_TAG:
	Glue<C2Elem>::push(
		L, 
		new C2Elem(
			MV_TAG,
			new mv(
				gp(mv(*((bivectorE2GA *)elem1->elem)), mv(*((e2_t *)elem2->elem)))
			)
		)
	);
break;

case NI_TAG:
	Glue<C2Elem>::push(
		L, 
		new C2Elem(
			MV_TAG,
			new mv(
				gp(mv(*((bivectorE2GA *)elem1->elem)), mv(*((ni_t *)elem2->elem)))
			)
		)
	);
break;

case NONI_TAG:
	Glue<C2Elem>::push(
		L, 
		new C2Elem(
			MV_TAG,
			new mv(
				gp(mv(*((bivectorE2GA *)elem1->elem)), mv(*((noni_t *)elem2->elem)))
			)
		)
	);
break;

case I2_TAG:
	Glue<C2Elem>::push(
		L, 
		new C2Elem(
			MV_TAG,
			new mv(
				gp(mv(*((bivectorE2GA *)elem1->elem)), mv(*((I2_t *)elem2->elem)))
			)
		)
	);
break;

case I4_TAG:
	Glue<C2Elem>::push(
		L, 
		new C2Elem(
			MV_TAG,
			new mv(
				gp(mv(*((bivectorE2GA *)elem1->elem)), mv(*((I4_t *)elem2->elem)))
			)
		)
	);
break;

case I4i_TAG:
	Glue<C2Elem>::push(
		L, 
		new C2Elem(
			MV_TAG,
			new mv(
				gp(mv(*((bivectorE2GA *)elem1->elem)), mv(*((I4i_t *)elem2->elem)))
			)
		)
	);
break;

case VECTORE2GA_TAG:
	Glue<C2Elem>::push(
		L, 
		new C2Elem(
			MV_TAG,
			new mv(
				gp(mv(*((bivectorE2GA *)elem1->elem)), mv(*((vectorE2GA *)elem2->elem)))
			)
		)
	);
break;

case BIVECTORE2GA_TAG:
				lua::push<double>(
					L, 
					gp(*((bivectorE2GA *)elem1->elem), *((bivectorE2GA *)elem2->elem))
				);
			
break;

case ROTORE2GA_TAG:
	Glue<C2Elem>::push(
		L, 
		new C2Elem(
			MV_TAG,
			new mv(
				gp(mv(*((bivectorE2GA *)elem1->elem)), mv(*((rotorE2GA *)elem2->elem)))
			)
		)
	);
break;

case NORMALIZEDPOINT_TAG:
	Glue<C2Elem>::push(
		L, 
		new C2Elem(
			MV_TAG,
			new mv(
				gp(mv(*((bivectorE2GA *)elem1->elem)), mv(*((normalizedPoint *)elem2->elem)))
			)
		)
	);
break;

case DUALCIRCLE_TAG:
	Glue<C2Elem>::push(
		L, 
		new C2Elem(
			MV_TAG,
			new mv(
				gp(mv(*((bivectorE2GA *)elem1->elem)), mv(*((dualCircle *)elem2->elem)))
			)
		)
	);
break;

case FREEVECTOR_TAG:
	Glue<C2Elem>::push(
		L, 
		new C2Elem(
			MV_TAG,
			new mv(
				gp(mv(*((bivectorE2GA *)elem1->elem)), mv(*((freeVector *)elem2->elem)))
			)
		)
	);
break;

case FREEBIVECTOR_TAG:
	Glue<C2Elem>::push(
		L, 
		new C2Elem(
			MV_TAG,
			new mv(
				gp(mv(*((bivectorE2GA *)elem1->elem)), mv(*((freeBivector *)elem2->elem)))
			)
		)
	);
break;

case CIRCLE_TAG:
	Glue<C2Elem>::push(
		L, 
		new C2Elem(
			MV_TAG,
			new mv(
				gp(mv(*((bivectorE2GA *)elem1->elem)), mv(*((circle *)elem2->elem)))
			)
		)
	);
break;

case LINE_TAG:
	Glue<C2Elem>::push(
		L, 
		new C2Elem(
			MV_TAG,
			new mv(
				gp(mv(*((bivectorE2GA *)elem1->elem)), mv(*((line *)elem2->elem)))
			)
		)
	);
break;

case PSEUDOSCALAR_TAG:
	Glue<C2Elem>::push(
		L, 
		new C2Elem(
			MV_TAG,
			new mv(
				gp(mv(*((bivectorE2GA *)elem1->elem)), mv(*((pseudoscalar *)elem2->elem)))
			)
		)
	);
break;

case NORMALIZEDTRANSLATOR_TAG:
	Glue<C2Elem>::push(
		L, 
		new C2Elem(
			MV_TAG,
			new mv(
				gp(mv(*((bivectorE2GA *)elem1->elem)), mv(*((normalizedTranslator *)elem2->elem)))
			)
		)
	);
break;

case TRANSLATOR_TAG:
	Glue<C2Elem>::push(
		L, 
		new C2Elem(
			MV_TAG,
			new mv(
				gp(mv(*((bivectorE2GA *)elem1->elem)), mv(*((translator *)elem2->elem)))
			)
		)
	);
break;

case EVENVERSOR_TAG:
	Glue<C2Elem>::push(
		L, 
		new C2Elem(
			MV_TAG,
			new mv(
				gp(mv(*((bivectorE2GA *)elem1->elem)), mv(*((evenVersor *)elem2->elem)))
			)
		)
	);
break;

case ODDVERSOR_TAG:
	Glue<C2Elem>::push(
		L, 
		new C2Elem(
			MV_TAG,
			new mv(
				gp(mv(*((bivectorE2GA *)elem1->elem)), mv(*((oddVersor *)elem2->elem)))
			)
		)
	);
break;

case DOUBLE_TAG:
	Glue<C2Elem>::push(
		L, 
		new C2Elem(
			MV_TAG,
			new mv(
				gp(mv(*((bivectorE2GA *)elem1->elem)), *((double *)elem2))
			)
		)
	);
break;
}
break;

case ROTORE2GA_TAG:
switch(tag2) {
case MV_TAG:
	Glue<C2Elem>::push(
		L, 
		new C2Elem(
			MV_TAG,
			new mv(
				gp(mv(*((rotorE2GA *)elem1->elem)), mv(*((mv *)elem2->elem)))
			)
		)
	);
break;

case NO_TAG:
	Glue<C2Elem>::push(
		L, 
		new C2Elem(
			MV_TAG,
			new mv(
				gp(mv(*((rotorE2GA *)elem1->elem)), mv(*((no_t *)elem2->elem)))
			)
		)
	);
break;

case E1_TAG:
	Glue<C2Elem>::push(
		L, 
		new C2Elem(
			MV_TAG,
			new mv(
				gp(mv(*((rotorE2GA *)elem1->elem)), mv(*((e1_t *)elem2->elem)))
			)
		)
	);
break;

case E2_TAG:
	Glue<C2Elem>::push(
		L, 
		new C2Elem(
			MV_TAG,
			new mv(
				gp(mv(*((rotorE2GA *)elem1->elem)), mv(*((e2_t *)elem2->elem)))
			)
		)
	);
break;

case NI_TAG:
	Glue<C2Elem>::push(
		L, 
		new C2Elem(
			MV_TAG,
			new mv(
				gp(mv(*((rotorE2GA *)elem1->elem)), mv(*((ni_t *)elem2->elem)))
			)
		)
	);
break;

case NONI_TAG:
	Glue<C2Elem>::push(
		L, 
		new C2Elem(
			MV_TAG,
			new mv(
				gp(mv(*((rotorE2GA *)elem1->elem)), mv(*((noni_t *)elem2->elem)))
			)
		)
	);
break;

case I2_TAG:
	Glue<C2Elem>::push(
		L, 
		new C2Elem(
			MV_TAG,
			new mv(
				gp(mv(*((rotorE2GA *)elem1->elem)), mv(*((I2_t *)elem2->elem)))
			)
		)
	);
break;

case I4_TAG:
	Glue<C2Elem>::push(
		L, 
		new C2Elem(
			MV_TAG,
			new mv(
				gp(mv(*((rotorE2GA *)elem1->elem)), mv(*((I4_t *)elem2->elem)))
			)
		)
	);
break;

case I4i_TAG:
	Glue<C2Elem>::push(
		L, 
		new C2Elem(
			MV_TAG,
			new mv(
				gp(mv(*((rotorE2GA *)elem1->elem)), mv(*((I4i_t *)elem2->elem)))
			)
		)
	);
break;

case VECTORE2GA_TAG:
	Glue<C2Elem>::push(
		L, 
		new C2Elem(
			VECTORE2GA_TAG,
			new vectorE2GA(
				gp(*((rotorE2GA *)elem1->elem), *((vectorE2GA *)elem2->elem))
			)
		)
	);
break;

case BIVECTORE2GA_TAG:
	Glue<C2Elem>::push(
		L, 
		new C2Elem(
			MV_TAG,
			new mv(
				gp(mv(*((rotorE2GA *)elem1->elem)), mv(*((bivectorE2GA *)elem2->elem)))
			)
		)
	);
break;

case ROTORE2GA_TAG:
	Glue<C2Elem>::push(
		L, 
		new C2Elem(
			ROTORE2GA_TAG,
			new rotorE2GA(
				gp(*((rotorE2GA *)elem1->elem), *((rotorE2GA *)elem2->elem))
			)
		)
	);
break;

case NORMALIZEDPOINT_TAG:
	Glue<C2Elem>::push(
		L, 
		new C2Elem(
			MV_TAG,
			new mv(
				gp(mv(*((rotorE2GA *)elem1->elem)), mv(*((normalizedPoint *)elem2->elem)))
			)
		)
	);
break;

case DUALCIRCLE_TAG:
	Glue<C2Elem>::push(
		L, 
		new C2Elem(
			MV_TAG,
			new mv(
				gp(mv(*((rotorE2GA *)elem1->elem)), mv(*((dualCircle *)elem2->elem)))
			)
		)
	);
break;

case FREEVECTOR_TAG:
	Glue<C2Elem>::push(
		L, 
		new C2Elem(
			MV_TAG,
			new mv(
				gp(mv(*((rotorE2GA *)elem1->elem)), mv(*((freeVector *)elem2->elem)))
			)
		)
	);
break;

case FREEBIVECTOR_TAG:
	Glue<C2Elem>::push(
		L, 
		new C2Elem(
			MV_TAG,
			new mv(
				gp(mv(*((rotorE2GA *)elem1->elem)), mv(*((freeBivector *)elem2->elem)))
			)
		)
	);
break;

case CIRCLE_TAG:
	Glue<C2Elem>::push(
		L, 
		new C2Elem(
			MV_TAG,
			new mv(
				gp(mv(*((rotorE2GA *)elem1->elem)), mv(*((circle *)elem2->elem)))
			)
		)
	);
break;

case LINE_TAG:
	Glue<C2Elem>::push(
		L, 
		new C2Elem(
			ODDVERSOR_TAG,
			new oddVersor(
				gp(*((rotorE2GA *)elem1->elem), *((line *)elem2->elem))
			)
		)
	);
break;

case PSEUDOSCALAR_TAG:
	Glue<C2Elem>::push(
		L, 
		new C2Elem(
			MV_TAG,
			new mv(
				gp(mv(*((rotorE2GA *)elem1->elem)), mv(*((pseudoscalar *)elem2->elem)))
			)
		)
	);
break;

case NORMALIZEDTRANSLATOR_TAG:
	Glue<C2Elem>::push(
		L, 
		new C2Elem(
			MV_TAG,
			new mv(
				gp(mv(*((rotorE2GA *)elem1->elem)), mv(*((normalizedTranslator *)elem2->elem)))
			)
		)
	);
break;

case TRANSLATOR_TAG:
	Glue<C2Elem>::push(
		L, 
		new C2Elem(
			MV_TAG,
			new mv(
				gp(mv(*((rotorE2GA *)elem1->elem)), mv(*((translator *)elem2->elem)))
			)
		)
	);
break;

case EVENVERSOR_TAG:
	Glue<C2Elem>::push(
		L, 
		new C2Elem(
			MV_TAG,
			new mv(
				gp(mv(*((rotorE2GA *)elem1->elem)), mv(*((evenVersor *)elem2->elem)))
			)
		)
	);
break;

case ODDVERSOR_TAG:
	Glue<C2Elem>::push(
		L, 
		new C2Elem(
			MV_TAG,
			new mv(
				gp(mv(*((rotorE2GA *)elem1->elem)), mv(*((oddVersor *)elem2->elem)))
			)
		)
	);
break;

case DOUBLE_TAG:
	Glue<C2Elem>::push(
		L, 
		new C2Elem(
			MV_TAG,
			new mv(
				gp(mv(*((rotorE2GA *)elem1->elem)), *((double *)elem2))
			)
		)
	);
break;
}
break;

case NORMALIZEDPOINT_TAG:
switch(tag2) {
case MV_TAG:
	Glue<C2Elem>::push(
		L, 
		new C2Elem(
			MV_TAG,
			new mv(
				gp(mv(*((normalizedPoint *)elem1->elem)), mv(*((mv *)elem2->elem)))
			)
		)
	);
break;

case NO_TAG:
	Glue<C2Elem>::push(
		L, 
		new C2Elem(
			MV_TAG,
			new mv(
				gp(mv(*((normalizedPoint *)elem1->elem)), mv(*((no_t *)elem2->elem)))
			)
		)
	);
break;

case E1_TAG:
	Glue<C2Elem>::push(
		L, 
		new C2Elem(
			MV_TAG,
			new mv(
				gp(mv(*((normalizedPoint *)elem1->elem)), mv(*((e1_t *)elem2->elem)))
			)
		)
	);
break;

case E2_TAG:
	Glue<C2Elem>::push(
		L, 
		new C2Elem(
			MV_TAG,
			new mv(
				gp(mv(*((normalizedPoint *)elem1->elem)), mv(*((e2_t *)elem2->elem)))
			)
		)
	);
break;

case NI_TAG:
	Glue<C2Elem>::push(
		L, 
		new C2Elem(
			MV_TAG,
			new mv(
				gp(mv(*((normalizedPoint *)elem1->elem)), mv(*((ni_t *)elem2->elem)))
			)
		)
	);
break;

case NONI_TAG:
	Glue<C2Elem>::push(
		L, 
		new C2Elem(
			MV_TAG,
			new mv(
				gp(mv(*((normalizedPoint *)elem1->elem)), mv(*((noni_t *)elem2->elem)))
			)
		)
	);
break;

case I2_TAG:
	Glue<C2Elem>::push(
		L, 
		new C2Elem(
			MV_TAG,
			new mv(
				gp(mv(*((normalizedPoint *)elem1->elem)), mv(*((I2_t *)elem2->elem)))
			)
		)
	);
break;

case I4_TAG:
	Glue<C2Elem>::push(
		L, 
		new C2Elem(
			MV_TAG,
			new mv(
				gp(mv(*((normalizedPoint *)elem1->elem)), mv(*((I4_t *)elem2->elem)))
			)
		)
	);
break;

case I4i_TAG:
	Glue<C2Elem>::push(
		L, 
		new C2Elem(
			MV_TAG,
			new mv(
				gp(mv(*((normalizedPoint *)elem1->elem)), mv(*((I4i_t *)elem2->elem)))
			)
		)
	);
break;

case VECTORE2GA_TAG:
	Glue<C2Elem>::push(
		L, 
		new C2Elem(
			MV_TAG,
			new mv(
				gp(mv(*((normalizedPoint *)elem1->elem)), mv(*((vectorE2GA *)elem2->elem)))
			)
		)
	);
break;

case BIVECTORE2GA_TAG:
	Glue<C2Elem>::push(
		L, 
		new C2Elem(
			MV_TAG,
			new mv(
				gp(mv(*((normalizedPoint *)elem1->elem)), mv(*((bivectorE2GA *)elem2->elem)))
			)
		)
	);
break;

case ROTORE2GA_TAG:
	Glue<C2Elem>::push(
		L, 
		new C2Elem(
			MV_TAG,
			new mv(
				gp(mv(*((normalizedPoint *)elem1->elem)), mv(*((rotorE2GA *)elem2->elem)))
			)
		)
	);
break;

case NORMALIZEDPOINT_TAG:
	Glue<C2Elem>::push(
		L, 
		new C2Elem(
			EVENVERSOR_TAG,
			new evenVersor(
				gp(*((normalizedPoint *)elem1->elem), *((normalizedPoint *)elem2->elem))
			)
		)
	);
break;

case DUALCIRCLE_TAG:
	Glue<C2Elem>::push(
		L, 
		new C2Elem(
			MV_TAG,
			new mv(
				gp(mv(*((normalizedPoint *)elem1->elem)), mv(*((dualCircle *)elem2->elem)))
			)
		)
	);
break;

case FREEVECTOR_TAG:
	Glue<C2Elem>::push(
		L, 
		new C2Elem(
			MV_TAG,
			new mv(
				gp(mv(*((normalizedPoint *)elem1->elem)), mv(*((freeVector *)elem2->elem)))
			)
		)
	);
break;

case FREEBIVECTOR_TAG:
	Glue<C2Elem>::push(
		L, 
		new C2Elem(
			MV_TAG,
			new mv(
				gp(mv(*((normalizedPoint *)elem1->elem)), mv(*((freeBivector *)elem2->elem)))
			)
		)
	);
break;

case CIRCLE_TAG:
	Glue<C2Elem>::push(
		L, 
		new C2Elem(
			MV_TAG,
			new mv(
				gp(mv(*((normalizedPoint *)elem1->elem)), mv(*((circle *)elem2->elem)))
			)
		)
	);
break;

case LINE_TAG:
	Glue<C2Elem>::push(
		L, 
		new C2Elem(
			MV_TAG,
			new mv(
				gp(mv(*((normalizedPoint *)elem1->elem)), mv(*((line *)elem2->elem)))
			)
		)
	);
break;

case PSEUDOSCALAR_TAG:
	Glue<C2Elem>::push(
		L, 
		new C2Elem(
			MV_TAG,
			new mv(
				gp(mv(*((normalizedPoint *)elem1->elem)), mv(*((pseudoscalar *)elem2->elem)))
			)
		)
	);
break;

case NORMALIZEDTRANSLATOR_TAG:
	Glue<C2Elem>::push(
		L, 
		new C2Elem(
			MV_TAG,
			new mv(
				gp(mv(*((normalizedPoint *)elem1->elem)), mv(*((normalizedTranslator *)elem2->elem)))
			)
		)
	);
break;

case TRANSLATOR_TAG:
	Glue<C2Elem>::push(
		L, 
		new C2Elem(
			MV_TAG,
			new mv(
				gp(mv(*((normalizedPoint *)elem1->elem)), mv(*((translator *)elem2->elem)))
			)
		)
	);
break;

case EVENVERSOR_TAG:
	Glue<C2Elem>::push(
		L, 
		new C2Elem(
			MV_TAG,
			new mv(
				gp(mv(*((normalizedPoint *)elem1->elem)), mv(*((evenVersor *)elem2->elem)))
			)
		)
	);
break;

case ODDVERSOR_TAG:
	Glue<C2Elem>::push(
		L, 
		new C2Elem(
			MV_TAG,
			new mv(
				gp(mv(*((normalizedPoint *)elem1->elem)), mv(*((oddVersor *)elem2->elem)))
			)
		)
	);
break;

case DOUBLE_TAG:
	Glue<C2Elem>::push(
		L, 
		new C2Elem(
			MV_TAG,
			new mv(
				gp(mv(*((normalizedPoint *)elem1->elem)), *((double *)elem2))
			)
		)
	);
break;
}
break;

case DUALCIRCLE_TAG:
switch(tag2) {
case MV_TAG:
	Glue<C2Elem>::push(
		L, 
		new C2Elem(
			MV_TAG,
			new mv(
				gp(mv(*((dualCircle *)elem1->elem)), mv(*((mv *)elem2->elem)))
			)
		)
	);
break;

case NO_TAG:
	Glue<C2Elem>::push(
		L, 
		new C2Elem(
			MV_TAG,
			new mv(
				gp(mv(*((dualCircle *)elem1->elem)), mv(*((no_t *)elem2->elem)))
			)
		)
	);
break;

case E1_TAG:
	Glue<C2Elem>::push(
		L, 
		new C2Elem(
			MV_TAG,
			new mv(
				gp(mv(*((dualCircle *)elem1->elem)), mv(*((e1_t *)elem2->elem)))
			)
		)
	);
break;

case E2_TAG:
	Glue<C2Elem>::push(
		L, 
		new C2Elem(
			MV_TAG,
			new mv(
				gp(mv(*((dualCircle *)elem1->elem)), mv(*((e2_t *)elem2->elem)))
			)
		)
	);
break;

case NI_TAG:
	Glue<C2Elem>::push(
		L, 
		new C2Elem(
			MV_TAG,
			new mv(
				gp(mv(*((dualCircle *)elem1->elem)), mv(*((ni_t *)elem2->elem)))
			)
		)
	);
break;

case NONI_TAG:
	Glue<C2Elem>::push(
		L, 
		new C2Elem(
			MV_TAG,
			new mv(
				gp(mv(*((dualCircle *)elem1->elem)), mv(*((noni_t *)elem2->elem)))
			)
		)
	);
break;

case I2_TAG:
	Glue<C2Elem>::push(
		L, 
		new C2Elem(
			MV_TAG,
			new mv(
				gp(mv(*((dualCircle *)elem1->elem)), mv(*((I2_t *)elem2->elem)))
			)
		)
	);
break;

case I4_TAG:
	Glue<C2Elem>::push(
		L, 
		new C2Elem(
			MV_TAG,
			new mv(
				gp(mv(*((dualCircle *)elem1->elem)), mv(*((I4_t *)elem2->elem)))
			)
		)
	);
break;

case I4i_TAG:
	Glue<C2Elem>::push(
		L, 
		new C2Elem(
			MV_TAG,
			new mv(
				gp(mv(*((dualCircle *)elem1->elem)), mv(*((I4i_t *)elem2->elem)))
			)
		)
	);
break;

case VECTORE2GA_TAG:
	Glue<C2Elem>::push(
		L, 
		new C2Elem(
			MV_TAG,
			new mv(
				gp(mv(*((dualCircle *)elem1->elem)), mv(*((vectorE2GA *)elem2->elem)))
			)
		)
	);
break;

case BIVECTORE2GA_TAG:
	Glue<C2Elem>::push(
		L, 
		new C2Elem(
			MV_TAG,
			new mv(
				gp(mv(*((dualCircle *)elem1->elem)), mv(*((bivectorE2GA *)elem2->elem)))
			)
		)
	);
break;

case ROTORE2GA_TAG:
	Glue<C2Elem>::push(
		L, 
		new C2Elem(
			MV_TAG,
			new mv(
				gp(mv(*((dualCircle *)elem1->elem)), mv(*((rotorE2GA *)elem2->elem)))
			)
		)
	);
break;

case NORMALIZEDPOINT_TAG:
	Glue<C2Elem>::push(
		L, 
		new C2Elem(
			MV_TAG,
			new mv(
				gp(mv(*((dualCircle *)elem1->elem)), mv(*((normalizedPoint *)elem2->elem)))
			)
		)
	);
break;

case DUALCIRCLE_TAG:
	Glue<C2Elem>::push(
		L, 
		new C2Elem(
			MV_TAG,
			new mv(
				gp(mv(*((dualCircle *)elem1->elem)), mv(*((dualCircle *)elem2->elem)))
			)
		)
	);
break;

case FREEVECTOR_TAG:
	Glue<C2Elem>::push(
		L, 
		new C2Elem(
			MV_TAG,
			new mv(
				gp(mv(*((dualCircle *)elem1->elem)), mv(*((freeVector *)elem2->elem)))
			)
		)
	);
break;

case FREEBIVECTOR_TAG:
	Glue<C2Elem>::push(
		L, 
		new C2Elem(
			MV_TAG,
			new mv(
				gp(mv(*((dualCircle *)elem1->elem)), mv(*((freeBivector *)elem2->elem)))
			)
		)
	);
break;

case CIRCLE_TAG:
	Glue<C2Elem>::push(
		L, 
		new C2Elem(
			MV_TAG,
			new mv(
				gp(mv(*((dualCircle *)elem1->elem)), mv(*((circle *)elem2->elem)))
			)
		)
	);
break;

case LINE_TAG:
	Glue<C2Elem>::push(
		L, 
		new C2Elem(
			MV_TAG,
			new mv(
				gp(mv(*((dualCircle *)elem1->elem)), mv(*((line *)elem2->elem)))
			)
		)
	);
break;

case PSEUDOSCALAR_TAG:
	Glue<C2Elem>::push(
		L, 
		new C2Elem(
			MV_TAG,
			new mv(
				gp(mv(*((dualCircle *)elem1->elem)), mv(*((pseudoscalar *)elem2->elem)))
			)
		)
	);
break;

case NORMALIZEDTRANSLATOR_TAG:
	Glue<C2Elem>::push(
		L, 
		new C2Elem(
			MV_TAG,
			new mv(
				gp(mv(*((dualCircle *)elem1->elem)), mv(*((normalizedTranslator *)elem2->elem)))
			)
		)
	);
break;

case TRANSLATOR_TAG:
	Glue<C2Elem>::push(
		L, 
		new C2Elem(
			MV_TAG,
			new mv(
				gp(mv(*((dualCircle *)elem1->elem)), mv(*((translator *)elem2->elem)))
			)
		)
	);
break;

case EVENVERSOR_TAG:
	Glue<C2Elem>::push(
		L, 
		new C2Elem(
			MV_TAG,
			new mv(
				gp(mv(*((dualCircle *)elem1->elem)), mv(*((evenVersor *)elem2->elem)))
			)
		)
	);
break;

case ODDVERSOR_TAG:
	Glue<C2Elem>::push(
		L, 
		new C2Elem(
			MV_TAG,
			new mv(
				gp(mv(*((dualCircle *)elem1->elem)), mv(*((oddVersor *)elem2->elem)))
			)
		)
	);
break;

case DOUBLE_TAG:
	Glue<C2Elem>::push(
		L, 
		new C2Elem(
			MV_TAG,
			new mv(
				gp(mv(*((dualCircle *)elem1->elem)), *((double *)elem2))
			)
		)
	);
break;
}
break;

case FREEVECTOR_TAG:
switch(tag2) {
case MV_TAG:
	Glue<C2Elem>::push(
		L, 
		new C2Elem(
			MV_TAG,
			new mv(
				gp(mv(*((freeVector *)elem1->elem)), mv(*((mv *)elem2->elem)))
			)
		)
	);
break;

case NO_TAG:
	Glue<C2Elem>::push(
		L, 
		new C2Elem(
			MV_TAG,
			new mv(
				gp(mv(*((freeVector *)elem1->elem)), mv(*((no_t *)elem2->elem)))
			)
		)
	);
break;

case E1_TAG:
	Glue<C2Elem>::push(
		L, 
		new C2Elem(
			MV_TAG,
			new mv(
				gp(mv(*((freeVector *)elem1->elem)), mv(*((e1_t *)elem2->elem)))
			)
		)
	);
break;

case E2_TAG:
	Glue<C2Elem>::push(
		L, 
		new C2Elem(
			MV_TAG,
			new mv(
				gp(mv(*((freeVector *)elem1->elem)), mv(*((e2_t *)elem2->elem)))
			)
		)
	);
break;

case NI_TAG:
	Glue<C2Elem>::push(
		L, 
		new C2Elem(
			MV_TAG,
			new mv(
				gp(mv(*((freeVector *)elem1->elem)), mv(*((ni_t *)elem2->elem)))
			)
		)
	);
break;

case NONI_TAG:
	Glue<C2Elem>::push(
		L, 
		new C2Elem(
			MV_TAG,
			new mv(
				gp(mv(*((freeVector *)elem1->elem)), mv(*((noni_t *)elem2->elem)))
			)
		)
	);
break;

case I2_TAG:
	Glue<C2Elem>::push(
		L, 
		new C2Elem(
			MV_TAG,
			new mv(
				gp(mv(*((freeVector *)elem1->elem)), mv(*((I2_t *)elem2->elem)))
			)
		)
	);
break;

case I4_TAG:
	Glue<C2Elem>::push(
		L, 
		new C2Elem(
			MV_TAG,
			new mv(
				gp(mv(*((freeVector *)elem1->elem)), mv(*((I4_t *)elem2->elem)))
			)
		)
	);
break;

case I4i_TAG:
	Glue<C2Elem>::push(
		L, 
		new C2Elem(
			MV_TAG,
			new mv(
				gp(mv(*((freeVector *)elem1->elem)), mv(*((I4i_t *)elem2->elem)))
			)
		)
	);
break;

case VECTORE2GA_TAG:
	Glue<C2Elem>::push(
		L, 
		new C2Elem(
			MV_TAG,
			new mv(
				gp(mv(*((freeVector *)elem1->elem)), mv(*((vectorE2GA *)elem2->elem)))
			)
		)
	);
break;

case BIVECTORE2GA_TAG:
	Glue<C2Elem>::push(
		L, 
		new C2Elem(
			MV_TAG,
			new mv(
				gp(mv(*((freeVector *)elem1->elem)), mv(*((bivectorE2GA *)elem2->elem)))
			)
		)
	);
break;

case ROTORE2GA_TAG:
	Glue<C2Elem>::push(
		L, 
		new C2Elem(
			MV_TAG,
			new mv(
				gp(mv(*((freeVector *)elem1->elem)), mv(*((rotorE2GA *)elem2->elem)))
			)
		)
	);
break;

case NORMALIZEDPOINT_TAG:
	Glue<C2Elem>::push(
		L, 
		new C2Elem(
			MV_TAG,
			new mv(
				gp(mv(*((freeVector *)elem1->elem)), mv(*((normalizedPoint *)elem2->elem)))
			)
		)
	);
break;

case DUALCIRCLE_TAG:
	Glue<C2Elem>::push(
		L, 
		new C2Elem(
			MV_TAG,
			new mv(
				gp(mv(*((freeVector *)elem1->elem)), mv(*((dualCircle *)elem2->elem)))
			)
		)
	);
break;

case FREEVECTOR_TAG:
	Glue<C2Elem>::push(
		L, 
		new C2Elem(
			MV_TAG,
			new mv(
				gp(mv(*((freeVector *)elem1->elem)), mv(*((freeVector *)elem2->elem)))
			)
		)
	);
break;

case FREEBIVECTOR_TAG:
	Glue<C2Elem>::push(
		L, 
		new C2Elem(
			MV_TAG,
			new mv(
				gp(mv(*((freeVector *)elem1->elem)), mv(*((freeBivector *)elem2->elem)))
			)
		)
	);
break;

case CIRCLE_TAG:
	Glue<C2Elem>::push(
		L, 
		new C2Elem(
			MV_TAG,
			new mv(
				gp(mv(*((freeVector *)elem1->elem)), mv(*((circle *)elem2->elem)))
			)
		)
	);
break;

case LINE_TAG:
	Glue<C2Elem>::push(
		L, 
		new C2Elem(
			MV_TAG,
			new mv(
				gp(mv(*((freeVector *)elem1->elem)), mv(*((line *)elem2->elem)))
			)
		)
	);
break;

case PSEUDOSCALAR_TAG:
	Glue<C2Elem>::push(
		L, 
		new C2Elem(
			MV_TAG,
			new mv(
				gp(mv(*((freeVector *)elem1->elem)), mv(*((pseudoscalar *)elem2->elem)))
			)
		)
	);
break;

case NORMALIZEDTRANSLATOR_TAG:
	Glue<C2Elem>::push(
		L, 
		new C2Elem(
			MV_TAG,
			new mv(
				gp(mv(*((freeVector *)elem1->elem)), mv(*((normalizedTranslator *)elem2->elem)))
			)
		)
	);
break;

case TRANSLATOR_TAG:
	Glue<C2Elem>::push(
		L, 
		new C2Elem(
			MV_TAG,
			new mv(
				gp(mv(*((freeVector *)elem1->elem)), mv(*((translator *)elem2->elem)))
			)
		)
	);
break;

case EVENVERSOR_TAG:
	Glue<C2Elem>::push(
		L, 
		new C2Elem(
			MV_TAG,
			new mv(
				gp(mv(*((freeVector *)elem1->elem)), mv(*((evenVersor *)elem2->elem)))
			)
		)
	);
break;

case ODDVERSOR_TAG:
	Glue<C2Elem>::push(
		L, 
		new C2Elem(
			MV_TAG,
			new mv(
				gp(mv(*((freeVector *)elem1->elem)), mv(*((oddVersor *)elem2->elem)))
			)
		)
	);
break;

case DOUBLE_TAG:
	Glue<C2Elem>::push(
		L, 
		new C2Elem(
			MV_TAG,
			new mv(
				gp(mv(*((freeVector *)elem1->elem)), *((double *)elem2))
			)
		)
	);
break;
}
break;

case FREEBIVECTOR_TAG:
switch(tag2) {
case MV_TAG:
	Glue<C2Elem>::push(
		L, 
		new C2Elem(
			MV_TAG,
			new mv(
				gp(mv(*((freeBivector *)elem1->elem)), mv(*((mv *)elem2->elem)))
			)
		)
	);
break;

case NO_TAG:
	Glue<C2Elem>::push(
		L, 
		new C2Elem(
			MV_TAG,
			new mv(
				gp(mv(*((freeBivector *)elem1->elem)), mv(*((no_t *)elem2->elem)))
			)
		)
	);
break;

case E1_TAG:
	Glue<C2Elem>::push(
		L, 
		new C2Elem(
			MV_TAG,
			new mv(
				gp(mv(*((freeBivector *)elem1->elem)), mv(*((e1_t *)elem2->elem)))
			)
		)
	);
break;

case E2_TAG:
	Glue<C2Elem>::push(
		L, 
		new C2Elem(
			MV_TAG,
			new mv(
				gp(mv(*((freeBivector *)elem1->elem)), mv(*((e2_t *)elem2->elem)))
			)
		)
	);
break;

case NI_TAG:
	Glue<C2Elem>::push(
		L, 
		new C2Elem(
			MV_TAG,
			new mv(
				gp(mv(*((freeBivector *)elem1->elem)), mv(*((ni_t *)elem2->elem)))
			)
		)
	);
break;

case NONI_TAG:
	Glue<C2Elem>::push(
		L, 
		new C2Elem(
			MV_TAG,
			new mv(
				gp(mv(*((freeBivector *)elem1->elem)), mv(*((noni_t *)elem2->elem)))
			)
		)
	);
break;

case I2_TAG:
	Glue<C2Elem>::push(
		L, 
		new C2Elem(
			MV_TAG,
			new mv(
				gp(mv(*((freeBivector *)elem1->elem)), mv(*((I2_t *)elem2->elem)))
			)
		)
	);
break;

case I4_TAG:
	Glue<C2Elem>::push(
		L, 
		new C2Elem(
			MV_TAG,
			new mv(
				gp(mv(*((freeBivector *)elem1->elem)), mv(*((I4_t *)elem2->elem)))
			)
		)
	);
break;

case I4i_TAG:
	Glue<C2Elem>::push(
		L, 
		new C2Elem(
			MV_TAG,
			new mv(
				gp(mv(*((freeBivector *)elem1->elem)), mv(*((I4i_t *)elem2->elem)))
			)
		)
	);
break;

case VECTORE2GA_TAG:
	Glue<C2Elem>::push(
		L, 
		new C2Elem(
			MV_TAG,
			new mv(
				gp(mv(*((freeBivector *)elem1->elem)), mv(*((vectorE2GA *)elem2->elem)))
			)
		)
	);
break;

case BIVECTORE2GA_TAG:
	Glue<C2Elem>::push(
		L, 
		new C2Elem(
			MV_TAG,
			new mv(
				gp(mv(*((freeBivector *)elem1->elem)), mv(*((bivectorE2GA *)elem2->elem)))
			)
		)
	);
break;

case ROTORE2GA_TAG:
	Glue<C2Elem>::push(
		L, 
		new C2Elem(
			MV_TAG,
			new mv(
				gp(mv(*((freeBivector *)elem1->elem)), mv(*((rotorE2GA *)elem2->elem)))
			)
		)
	);
break;

case NORMALIZEDPOINT_TAG:
	Glue<C2Elem>::push(
		L, 
		new C2Elem(
			MV_TAG,
			new mv(
				gp(mv(*((freeBivector *)elem1->elem)), mv(*((normalizedPoint *)elem2->elem)))
			)
		)
	);
break;

case DUALCIRCLE_TAG:
	Glue<C2Elem>::push(
		L, 
		new C2Elem(
			MV_TAG,
			new mv(
				gp(mv(*((freeBivector *)elem1->elem)), mv(*((dualCircle *)elem2->elem)))
			)
		)
	);
break;

case FREEVECTOR_TAG:
	Glue<C2Elem>::push(
		L, 
		new C2Elem(
			MV_TAG,
			new mv(
				gp(mv(*((freeBivector *)elem1->elem)), mv(*((freeVector *)elem2->elem)))
			)
		)
	);
break;

case FREEBIVECTOR_TAG:
	Glue<C2Elem>::push(
		L, 
		new C2Elem(
			MV_TAG,
			new mv(
				gp(mv(*((freeBivector *)elem1->elem)), mv(*((freeBivector *)elem2->elem)))
			)
		)
	);
break;

case CIRCLE_TAG:
	Glue<C2Elem>::push(
		L, 
		new C2Elem(
			MV_TAG,
			new mv(
				gp(mv(*((freeBivector *)elem1->elem)), mv(*((circle *)elem2->elem)))
			)
		)
	);
break;

case LINE_TAG:
	Glue<C2Elem>::push(
		L, 
		new C2Elem(
			MV_TAG,
			new mv(
				gp(mv(*((freeBivector *)elem1->elem)), mv(*((line *)elem2->elem)))
			)
		)
	);
break;

case PSEUDOSCALAR_TAG:
	Glue<C2Elem>::push(
		L, 
		new C2Elem(
			MV_TAG,
			new mv(
				gp(mv(*((freeBivector *)elem1->elem)), mv(*((pseudoscalar *)elem2->elem)))
			)
		)
	);
break;

case NORMALIZEDTRANSLATOR_TAG:
	Glue<C2Elem>::push(
		L, 
		new C2Elem(
			MV_TAG,
			new mv(
				gp(mv(*((freeBivector *)elem1->elem)), mv(*((normalizedTranslator *)elem2->elem)))
			)
		)
	);
break;

case TRANSLATOR_TAG:
	Glue<C2Elem>::push(
		L, 
		new C2Elem(
			MV_TAG,
			new mv(
				gp(mv(*((freeBivector *)elem1->elem)), mv(*((translator *)elem2->elem)))
			)
		)
	);
break;

case EVENVERSOR_TAG:
	Glue<C2Elem>::push(
		L, 
		new C2Elem(
			MV_TAG,
			new mv(
				gp(mv(*((freeBivector *)elem1->elem)), mv(*((evenVersor *)elem2->elem)))
			)
		)
	);
break;

case ODDVERSOR_TAG:
	Glue<C2Elem>::push(
		L, 
		new C2Elem(
			MV_TAG,
			new mv(
				gp(mv(*((freeBivector *)elem1->elem)), mv(*((oddVersor *)elem2->elem)))
			)
		)
	);
break;

case DOUBLE_TAG:
	Glue<C2Elem>::push(
		L, 
		new C2Elem(
			MV_TAG,
			new mv(
				gp(mv(*((freeBivector *)elem1->elem)), *((double *)elem2))
			)
		)
	);
break;
}
break;

case CIRCLE_TAG:
switch(tag2) {
case MV_TAG:
	Glue<C2Elem>::push(
		L, 
		new C2Elem(
			MV_TAG,
			new mv(
				gp(mv(*((circle *)elem1->elem)), mv(*((mv *)elem2->elem)))
			)
		)
	);
break;

case NO_TAG:
	Glue<C2Elem>::push(
		L, 
		new C2Elem(
			MV_TAG,
			new mv(
				gp(mv(*((circle *)elem1->elem)), mv(*((no_t *)elem2->elem)))
			)
		)
	);
break;

case E1_TAG:
	Glue<C2Elem>::push(
		L, 
		new C2Elem(
			MV_TAG,
			new mv(
				gp(mv(*((circle *)elem1->elem)), mv(*((e1_t *)elem2->elem)))
			)
		)
	);
break;

case E2_TAG:
	Glue<C2Elem>::push(
		L, 
		new C2Elem(
			MV_TAG,
			new mv(
				gp(mv(*((circle *)elem1->elem)), mv(*((e2_t *)elem2->elem)))
			)
		)
	);
break;

case NI_TAG:
	Glue<C2Elem>::push(
		L, 
		new C2Elem(
			MV_TAG,
			new mv(
				gp(mv(*((circle *)elem1->elem)), mv(*((ni_t *)elem2->elem)))
			)
		)
	);
break;

case NONI_TAG:
	Glue<C2Elem>::push(
		L, 
		new C2Elem(
			MV_TAG,
			new mv(
				gp(mv(*((circle *)elem1->elem)), mv(*((noni_t *)elem2->elem)))
			)
		)
	);
break;

case I2_TAG:
	Glue<C2Elem>::push(
		L, 
		new C2Elem(
			MV_TAG,
			new mv(
				gp(mv(*((circle *)elem1->elem)), mv(*((I2_t *)elem2->elem)))
			)
		)
	);
break;

case I4_TAG:
	Glue<C2Elem>::push(
		L, 
		new C2Elem(
			MV_TAG,
			new mv(
				gp(mv(*((circle *)elem1->elem)), mv(*((I4_t *)elem2->elem)))
			)
		)
	);
break;

case I4i_TAG:
	Glue<C2Elem>::push(
		L, 
		new C2Elem(
			MV_TAG,
			new mv(
				gp(mv(*((circle *)elem1->elem)), mv(*((I4i_t *)elem2->elem)))
			)
		)
	);
break;

case VECTORE2GA_TAG:
	Glue<C2Elem>::push(
		L, 
		new C2Elem(
			MV_TAG,
			new mv(
				gp(mv(*((circle *)elem1->elem)), mv(*((vectorE2GA *)elem2->elem)))
			)
		)
	);
break;

case BIVECTORE2GA_TAG:
	Glue<C2Elem>::push(
		L, 
		new C2Elem(
			MV_TAG,
			new mv(
				gp(mv(*((circle *)elem1->elem)), mv(*((bivectorE2GA *)elem2->elem)))
			)
		)
	);
break;

case ROTORE2GA_TAG:
	Glue<C2Elem>::push(
		L, 
		new C2Elem(
			MV_TAG,
			new mv(
				gp(mv(*((circle *)elem1->elem)), mv(*((rotorE2GA *)elem2->elem)))
			)
		)
	);
break;

case NORMALIZEDPOINT_TAG:
	Glue<C2Elem>::push(
		L, 
		new C2Elem(
			MV_TAG,
			new mv(
				gp(mv(*((circle *)elem1->elem)), mv(*((normalizedPoint *)elem2->elem)))
			)
		)
	);
break;

case DUALCIRCLE_TAG:
	Glue<C2Elem>::push(
		L, 
		new C2Elem(
			MV_TAG,
			new mv(
				gp(mv(*((circle *)elem1->elem)), mv(*((dualCircle *)elem2->elem)))
			)
		)
	);
break;

case FREEVECTOR_TAG:
	Glue<C2Elem>::push(
		L, 
		new C2Elem(
			MV_TAG,
			new mv(
				gp(mv(*((circle *)elem1->elem)), mv(*((freeVector *)elem2->elem)))
			)
		)
	);
break;

case FREEBIVECTOR_TAG:
	Glue<C2Elem>::push(
		L, 
		new C2Elem(
			MV_TAG,
			new mv(
				gp(mv(*((circle *)elem1->elem)), mv(*((freeBivector *)elem2->elem)))
			)
		)
	);
break;

case CIRCLE_TAG:
	Glue<C2Elem>::push(
		L, 
		new C2Elem(
			MV_TAG,
			new mv(
				gp(mv(*((circle *)elem1->elem)), mv(*((circle *)elem2->elem)))
			)
		)
	);
break;

case LINE_TAG:
	Glue<C2Elem>::push(
		L, 
		new C2Elem(
			EVENVERSOR_TAG,
			new evenVersor(
				gp(*((circle *)elem1->elem), *((line *)elem2->elem))
			)
		)
	);
break;

case PSEUDOSCALAR_TAG:
	Glue<C2Elem>::push(
		L, 
		new C2Elem(
			MV_TAG,
			new mv(
				gp(mv(*((circle *)elem1->elem)), mv(*((pseudoscalar *)elem2->elem)))
			)
		)
	);
break;

case NORMALIZEDTRANSLATOR_TAG:
	Glue<C2Elem>::push(
		L, 
		new C2Elem(
			MV_TAG,
			new mv(
				gp(mv(*((circle *)elem1->elem)), mv(*((normalizedTranslator *)elem2->elem)))
			)
		)
	);
break;

case TRANSLATOR_TAG:
	Glue<C2Elem>::push(
		L, 
		new C2Elem(
			MV_TAG,
			new mv(
				gp(mv(*((circle *)elem1->elem)), mv(*((translator *)elem2->elem)))
			)
		)
	);
break;

case EVENVERSOR_TAG:
	Glue<C2Elem>::push(
		L, 
		new C2Elem(
			MV_TAG,
			new mv(
				gp(mv(*((circle *)elem1->elem)), mv(*((evenVersor *)elem2->elem)))
			)
		)
	);
break;

case ODDVERSOR_TAG:
	Glue<C2Elem>::push(
		L, 
		new C2Elem(
			MV_TAG,
			new mv(
				gp(mv(*((circle *)elem1->elem)), mv(*((oddVersor *)elem2->elem)))
			)
		)
	);
break;

case DOUBLE_TAG:
	Glue<C2Elem>::push(
		L, 
		new C2Elem(
			MV_TAG,
			new mv(
				gp(mv(*((circle *)elem1->elem)), *((double *)elem2))
			)
		)
	);
break;
}
break;

case LINE_TAG:
switch(tag2) {
case MV_TAG:
	Glue<C2Elem>::push(
		L, 
		new C2Elem(
			MV_TAG,
			new mv(
				gp(mv(*((line *)elem1->elem)), mv(*((mv *)elem2->elem)))
			)
		)
	);
break;

case NO_TAG:
	Glue<C2Elem>::push(
		L, 
		new C2Elem(
			MV_TAG,
			new mv(
				gp(mv(*((line *)elem1->elem)), mv(*((no_t *)elem2->elem)))
			)
		)
	);
break;

case E1_TAG:
	Glue<C2Elem>::push(
		L, 
		new C2Elem(
			MV_TAG,
			new mv(
				gp(mv(*((line *)elem1->elem)), mv(*((e1_t *)elem2->elem)))
			)
		)
	);
break;

case E2_TAG:
	Glue<C2Elem>::push(
		L, 
		new C2Elem(
			MV_TAG,
			new mv(
				gp(mv(*((line *)elem1->elem)), mv(*((e2_t *)elem2->elem)))
			)
		)
	);
break;

case NI_TAG:
	Glue<C2Elem>::push(
		L, 
		new C2Elem(
			MV_TAG,
			new mv(
				gp(mv(*((line *)elem1->elem)), mv(*((ni_t *)elem2->elem)))
			)
		)
	);
break;

case NONI_TAG:
	Glue<C2Elem>::push(
		L, 
		new C2Elem(
			MV_TAG,
			new mv(
				gp(mv(*((line *)elem1->elem)), mv(*((noni_t *)elem2->elem)))
			)
		)
	);
break;

case I2_TAG:
	Glue<C2Elem>::push(
		L, 
		new C2Elem(
			MV_TAG,
			new mv(
				gp(mv(*((line *)elem1->elem)), mv(*((I2_t *)elem2->elem)))
			)
		)
	);
break;

case I4_TAG:
	Glue<C2Elem>::push(
		L, 
		new C2Elem(
			MV_TAG,
			new mv(
				gp(mv(*((line *)elem1->elem)), mv(*((I4_t *)elem2->elem)))
			)
		)
	);
break;

case I4i_TAG:
	Glue<C2Elem>::push(
		L, 
		new C2Elem(
			MV_TAG,
			new mv(
				gp(mv(*((line *)elem1->elem)), mv(*((I4i_t *)elem2->elem)))
			)
		)
	);
break;

case VECTORE2GA_TAG:
	Glue<C2Elem>::push(
		L, 
		new C2Elem(
			MV_TAG,
			new mv(
				gp(mv(*((line *)elem1->elem)), mv(*((vectorE2GA *)elem2->elem)))
			)
		)
	);
break;

case BIVECTORE2GA_TAG:
	Glue<C2Elem>::push(
		L, 
		new C2Elem(
			MV_TAG,
			new mv(
				gp(mv(*((line *)elem1->elem)), mv(*((bivectorE2GA *)elem2->elem)))
			)
		)
	);
break;

case ROTORE2GA_TAG:
	Glue<C2Elem>::push(
		L, 
		new C2Elem(
			MV_TAG,
			new mv(
				gp(mv(*((line *)elem1->elem)), mv(*((rotorE2GA *)elem2->elem)))
			)
		)
	);
break;

case NORMALIZEDPOINT_TAG:
	Glue<C2Elem>::push(
		L, 
		new C2Elem(
			MV_TAG,
			new mv(
				gp(mv(*((line *)elem1->elem)), mv(*((normalizedPoint *)elem2->elem)))
			)
		)
	);
break;

case DUALCIRCLE_TAG:
	Glue<C2Elem>::push(
		L, 
		new C2Elem(
			MV_TAG,
			new mv(
				gp(mv(*((line *)elem1->elem)), mv(*((dualCircle *)elem2->elem)))
			)
		)
	);
break;

case FREEVECTOR_TAG:
	Glue<C2Elem>::push(
		L, 
		new C2Elem(
			MV_TAG,
			new mv(
				gp(mv(*((line *)elem1->elem)), mv(*((freeVector *)elem2->elem)))
			)
		)
	);
break;

case FREEBIVECTOR_TAG:
	Glue<C2Elem>::push(
		L, 
		new C2Elem(
			MV_TAG,
			new mv(
				gp(mv(*((line *)elem1->elem)), mv(*((freeBivector *)elem2->elem)))
			)
		)
	);
break;

case CIRCLE_TAG:
	Glue<C2Elem>::push(
		L, 
		new C2Elem(
			MV_TAG,
			new mv(
				gp(mv(*((line *)elem1->elem)), mv(*((circle *)elem2->elem)))
			)
		)
	);
break;

case LINE_TAG:
	Glue<C2Elem>::push(
		L, 
		new C2Elem(
			MV_TAG,
			new mv(
				gp(mv(*((line *)elem1->elem)), mv(*((line *)elem2->elem)))
			)
		)
	);
break;

case PSEUDOSCALAR_TAG:
	Glue<C2Elem>::push(
		L, 
		new C2Elem(
			MV_TAG,
			new mv(
				gp(mv(*((line *)elem1->elem)), mv(*((pseudoscalar *)elem2->elem)))
			)
		)
	);
break;

case NORMALIZEDTRANSLATOR_TAG:
	Glue<C2Elem>::push(
		L, 
		new C2Elem(
			MV_TAG,
			new mv(
				gp(mv(*((line *)elem1->elem)), mv(*((normalizedTranslator *)elem2->elem)))
			)
		)
	);
break;

case TRANSLATOR_TAG:
	Glue<C2Elem>::push(
		L, 
		new C2Elem(
			MV_TAG,
			new mv(
				gp(mv(*((line *)elem1->elem)), mv(*((translator *)elem2->elem)))
			)
		)
	);
break;

case EVENVERSOR_TAG:
	Glue<C2Elem>::push(
		L, 
		new C2Elem(
			MV_TAG,
			new mv(
				gp(mv(*((line *)elem1->elem)), mv(*((evenVersor *)elem2->elem)))
			)
		)
	);
break;

case ODDVERSOR_TAG:
	Glue<C2Elem>::push(
		L, 
		new C2Elem(
			MV_TAG,
			new mv(
				gp(mv(*((line *)elem1->elem)), mv(*((oddVersor *)elem2->elem)))
			)
		)
	);
break;

case DOUBLE_TAG:
	Glue<C2Elem>::push(
		L, 
		new C2Elem(
			MV_TAG,
			new mv(
				gp(mv(*((line *)elem1->elem)), *((double *)elem2))
			)
		)
	);
break;
}
break;

case PSEUDOSCALAR_TAG:
switch(tag2) {
case MV_TAG:
	Glue<C2Elem>::push(
		L, 
		new C2Elem(
			MV_TAG,
			new mv(
				gp(mv(*((pseudoscalar *)elem1->elem)), mv(*((mv *)elem2->elem)))
			)
		)
	);
break;

case NO_TAG:
	Glue<C2Elem>::push(
		L, 
		new C2Elem(
			MV_TAG,
			new mv(
				gp(mv(*((pseudoscalar *)elem1->elem)), mv(*((no_t *)elem2->elem)))
			)
		)
	);
break;

case E1_TAG:
	Glue<C2Elem>::push(
		L, 
		new C2Elem(
			MV_TAG,
			new mv(
				gp(mv(*((pseudoscalar *)elem1->elem)), mv(*((e1_t *)elem2->elem)))
			)
		)
	);
break;

case E2_TAG:
	Glue<C2Elem>::push(
		L, 
		new C2Elem(
			MV_TAG,
			new mv(
				gp(mv(*((pseudoscalar *)elem1->elem)), mv(*((e2_t *)elem2->elem)))
			)
		)
	);
break;

case NI_TAG:
	Glue<C2Elem>::push(
		L, 
		new C2Elem(
			MV_TAG,
			new mv(
				gp(mv(*((pseudoscalar *)elem1->elem)), mv(*((ni_t *)elem2->elem)))
			)
		)
	);
break;

case NONI_TAG:
	Glue<C2Elem>::push(
		L, 
		new C2Elem(
			MV_TAG,
			new mv(
				gp(mv(*((pseudoscalar *)elem1->elem)), mv(*((noni_t *)elem2->elem)))
			)
		)
	);
break;

case I2_TAG:
	Glue<C2Elem>::push(
		L, 
		new C2Elem(
			MV_TAG,
			new mv(
				gp(mv(*((pseudoscalar *)elem1->elem)), mv(*((I2_t *)elem2->elem)))
			)
		)
	);
break;

case I4_TAG:
	Glue<C2Elem>::push(
		L, 
		new C2Elem(
			MV_TAG,
			new mv(
				gp(mv(*((pseudoscalar *)elem1->elem)), mv(*((I4_t *)elem2->elem)))
			)
		)
	);
break;

case I4i_TAG:
	Glue<C2Elem>::push(
		L, 
		new C2Elem(
			MV_TAG,
			new mv(
				gp(mv(*((pseudoscalar *)elem1->elem)), mv(*((I4i_t *)elem2->elem)))
			)
		)
	);
break;

case VECTORE2GA_TAG:
	Glue<C2Elem>::push(
		L, 
		new C2Elem(
			MV_TAG,
			new mv(
				gp(mv(*((pseudoscalar *)elem1->elem)), mv(*((vectorE2GA *)elem2->elem)))
			)
		)
	);
break;

case BIVECTORE2GA_TAG:
	Glue<C2Elem>::push(
		L, 
		new C2Elem(
			MV_TAG,
			new mv(
				gp(mv(*((pseudoscalar *)elem1->elem)), mv(*((bivectorE2GA *)elem2->elem)))
			)
		)
	);
break;

case ROTORE2GA_TAG:
	Glue<C2Elem>::push(
		L, 
		new C2Elem(
			MV_TAG,
			new mv(
				gp(mv(*((pseudoscalar *)elem1->elem)), mv(*((rotorE2GA *)elem2->elem)))
			)
		)
	);
break;

case NORMALIZEDPOINT_TAG:
	Glue<C2Elem>::push(
		L, 
		new C2Elem(
			MV_TAG,
			new mv(
				gp(mv(*((pseudoscalar *)elem1->elem)), mv(*((normalizedPoint *)elem2->elem)))
			)
		)
	);
break;

case DUALCIRCLE_TAG:
	Glue<C2Elem>::push(
		L, 
		new C2Elem(
			MV_TAG,
			new mv(
				gp(mv(*((pseudoscalar *)elem1->elem)), mv(*((dualCircle *)elem2->elem)))
			)
		)
	);
break;

case FREEVECTOR_TAG:
	Glue<C2Elem>::push(
		L, 
		new C2Elem(
			MV_TAG,
			new mv(
				gp(mv(*((pseudoscalar *)elem1->elem)), mv(*((freeVector *)elem2->elem)))
			)
		)
	);
break;

case FREEBIVECTOR_TAG:
	Glue<C2Elem>::push(
		L, 
		new C2Elem(
			MV_TAG,
			new mv(
				gp(mv(*((pseudoscalar *)elem1->elem)), mv(*((freeBivector *)elem2->elem)))
			)
		)
	);
break;

case CIRCLE_TAG:
	Glue<C2Elem>::push(
		L, 
		new C2Elem(
			MV_TAG,
			new mv(
				gp(mv(*((pseudoscalar *)elem1->elem)), mv(*((circle *)elem2->elem)))
			)
		)
	);
break;

case LINE_TAG:
	Glue<C2Elem>::push(
		L, 
		new C2Elem(
			MV_TAG,
			new mv(
				gp(mv(*((pseudoscalar *)elem1->elem)), mv(*((line *)elem2->elem)))
			)
		)
	);
break;

case PSEUDOSCALAR_TAG:
	Glue<C2Elem>::push(
		L, 
		new C2Elem(
			MV_TAG,
			new mv(
				gp(mv(*((pseudoscalar *)elem1->elem)), mv(*((pseudoscalar *)elem2->elem)))
			)
		)
	);
break;

case NORMALIZEDTRANSLATOR_TAG:
	Glue<C2Elem>::push(
		L, 
		new C2Elem(
			MV_TAG,
			new mv(
				gp(mv(*((pseudoscalar *)elem1->elem)), mv(*((normalizedTranslator *)elem2->elem)))
			)
		)
	);
break;

case TRANSLATOR_TAG:
	Glue<C2Elem>::push(
		L, 
		new C2Elem(
			MV_TAG,
			new mv(
				gp(mv(*((pseudoscalar *)elem1->elem)), mv(*((translator *)elem2->elem)))
			)
		)
	);
break;

case EVENVERSOR_TAG:
	Glue<C2Elem>::push(
		L, 
		new C2Elem(
			MV_TAG,
			new mv(
				gp(mv(*((pseudoscalar *)elem1->elem)), mv(*((evenVersor *)elem2->elem)))
			)
		)
	);
break;

case ODDVERSOR_TAG:
	Glue<C2Elem>::push(
		L, 
		new C2Elem(
			MV_TAG,
			new mv(
				gp(mv(*((pseudoscalar *)elem1->elem)), mv(*((oddVersor *)elem2->elem)))
			)
		)
	);
break;

case DOUBLE_TAG:
	Glue<C2Elem>::push(
		L, 
		new C2Elem(
			MV_TAG,
			new mv(
				gp(mv(*((pseudoscalar *)elem1->elem)), *((double *)elem2))
			)
		)
	);
break;
}
break;

case NORMALIZEDTRANSLATOR_TAG:
switch(tag2) {
case MV_TAG:
	Glue<C2Elem>::push(
		L, 
		new C2Elem(
			MV_TAG,
			new mv(
				gp(mv(*((normalizedTranslator *)elem1->elem)), mv(*((mv *)elem2->elem)))
			)
		)
	);
break;

case NO_TAG:
	Glue<C2Elem>::push(
		L, 
		new C2Elem(
			MV_TAG,
			new mv(
				gp(mv(*((normalizedTranslator *)elem1->elem)), mv(*((no_t *)elem2->elem)))
			)
		)
	);
break;

case E1_TAG:
	Glue<C2Elem>::push(
		L, 
		new C2Elem(
			MV_TAG,
			new mv(
				gp(mv(*((normalizedTranslator *)elem1->elem)), mv(*((e1_t *)elem2->elem)))
			)
		)
	);
break;

case E2_TAG:
	Glue<C2Elem>::push(
		L, 
		new C2Elem(
			MV_TAG,
			new mv(
				gp(mv(*((normalizedTranslator *)elem1->elem)), mv(*((e2_t *)elem2->elem)))
			)
		)
	);
break;

case NI_TAG:
	Glue<C2Elem>::push(
		L, 
		new C2Elem(
			MV_TAG,
			new mv(
				gp(mv(*((normalizedTranslator *)elem1->elem)), mv(*((ni_t *)elem2->elem)))
			)
		)
	);
break;

case NONI_TAG:
	Glue<C2Elem>::push(
		L, 
		new C2Elem(
			MV_TAG,
			new mv(
				gp(mv(*((normalizedTranslator *)elem1->elem)), mv(*((noni_t *)elem2->elem)))
			)
		)
	);
break;

case I2_TAG:
	Glue<C2Elem>::push(
		L, 
		new C2Elem(
			MV_TAG,
			new mv(
				gp(mv(*((normalizedTranslator *)elem1->elem)), mv(*((I2_t *)elem2->elem)))
			)
		)
	);
break;

case I4_TAG:
	Glue<C2Elem>::push(
		L, 
		new C2Elem(
			MV_TAG,
			new mv(
				gp(mv(*((normalizedTranslator *)elem1->elem)), mv(*((I4_t *)elem2->elem)))
			)
		)
	);
break;

case I4i_TAG:
	Glue<C2Elem>::push(
		L, 
		new C2Elem(
			MV_TAG,
			new mv(
				gp(mv(*((normalizedTranslator *)elem1->elem)), mv(*((I4i_t *)elem2->elem)))
			)
		)
	);
break;

case VECTORE2GA_TAG:
	Glue<C2Elem>::push(
		L, 
		new C2Elem(
			MV_TAG,
			new mv(
				gp(mv(*((normalizedTranslator *)elem1->elem)), mv(*((vectorE2GA *)elem2->elem)))
			)
		)
	);
break;

case BIVECTORE2GA_TAG:
	Glue<C2Elem>::push(
		L, 
		new C2Elem(
			MV_TAG,
			new mv(
				gp(mv(*((normalizedTranslator *)elem1->elem)), mv(*((bivectorE2GA *)elem2->elem)))
			)
		)
	);
break;

case ROTORE2GA_TAG:
	Glue<C2Elem>::push(
		L, 
		new C2Elem(
			MV_TAG,
			new mv(
				gp(mv(*((normalizedTranslator *)elem1->elem)), mv(*((rotorE2GA *)elem2->elem)))
			)
		)
	);
break;

case NORMALIZEDPOINT_TAG:
	Glue<C2Elem>::push(
		L, 
		new C2Elem(
			MV_TAG,
			new mv(
				gp(mv(*((normalizedTranslator *)elem1->elem)), mv(*((normalizedPoint *)elem2->elem)))
			)
		)
	);
break;

case DUALCIRCLE_TAG:
	Glue<C2Elem>::push(
		L, 
		new C2Elem(
			MV_TAG,
			new mv(
				gp(mv(*((normalizedTranslator *)elem1->elem)), mv(*((dualCircle *)elem2->elem)))
			)
		)
	);
break;

case FREEVECTOR_TAG:
	Glue<C2Elem>::push(
		L, 
		new C2Elem(
			MV_TAG,
			new mv(
				gp(mv(*((normalizedTranslator *)elem1->elem)), mv(*((freeVector *)elem2->elem)))
			)
		)
	);
break;

case FREEBIVECTOR_TAG:
	Glue<C2Elem>::push(
		L, 
		new C2Elem(
			MV_TAG,
			new mv(
				gp(mv(*((normalizedTranslator *)elem1->elem)), mv(*((freeBivector *)elem2->elem)))
			)
		)
	);
break;

case CIRCLE_TAG:
	Glue<C2Elem>::push(
		L, 
		new C2Elem(
			MV_TAG,
			new mv(
				gp(mv(*((normalizedTranslator *)elem1->elem)), mv(*((circle *)elem2->elem)))
			)
		)
	);
break;

case LINE_TAG:
	Glue<C2Elem>::push(
		L, 
		new C2Elem(
			MV_TAG,
			new mv(
				gp(mv(*((normalizedTranslator *)elem1->elem)), mv(*((line *)elem2->elem)))
			)
		)
	);
break;

case PSEUDOSCALAR_TAG:
	Glue<C2Elem>::push(
		L, 
		new C2Elem(
			MV_TAG,
			new mv(
				gp(mv(*((normalizedTranslator *)elem1->elem)), mv(*((pseudoscalar *)elem2->elem)))
			)
		)
	);
break;

case NORMALIZEDTRANSLATOR_TAG:
	Glue<C2Elem>::push(
		L, 
		new C2Elem(
			MV_TAG,
			new mv(
				gp(mv(*((normalizedTranslator *)elem1->elem)), mv(*((normalizedTranslator *)elem2->elem)))
			)
		)
	);
break;

case TRANSLATOR_TAG:
	Glue<C2Elem>::push(
		L, 
		new C2Elem(
			MV_TAG,
			new mv(
				gp(mv(*((normalizedTranslator *)elem1->elem)), mv(*((translator *)elem2->elem)))
			)
		)
	);
break;

case EVENVERSOR_TAG:
	Glue<C2Elem>::push(
		L, 
		new C2Elem(
			MV_TAG,
			new mv(
				gp(mv(*((normalizedTranslator *)elem1->elem)), mv(*((evenVersor *)elem2->elem)))
			)
		)
	);
break;

case ODDVERSOR_TAG:
	Glue<C2Elem>::push(
		L, 
		new C2Elem(
			MV_TAG,
			new mv(
				gp(mv(*((normalizedTranslator *)elem1->elem)), mv(*((oddVersor *)elem2->elem)))
			)
		)
	);
break;

case DOUBLE_TAG:
	Glue<C2Elem>::push(
		L, 
		new C2Elem(
			MV_TAG,
			new mv(
				gp(mv(*((normalizedTranslator *)elem1->elem)), *((double *)elem2))
			)
		)
	);
break;
}
break;

case TRANSLATOR_TAG:
switch(tag2) {
case MV_TAG:
	Glue<C2Elem>::push(
		L, 
		new C2Elem(
			MV_TAG,
			new mv(
				gp(mv(*((translator *)elem1->elem)), mv(*((mv *)elem2->elem)))
			)
		)
	);
break;

case NO_TAG:
	Glue<C2Elem>::push(
		L, 
		new C2Elem(
			MV_TAG,
			new mv(
				gp(mv(*((translator *)elem1->elem)), mv(*((no_t *)elem2->elem)))
			)
		)
	);
break;

case E1_TAG:
	Glue<C2Elem>::push(
		L, 
		new C2Elem(
			MV_TAG,
			new mv(
				gp(mv(*((translator *)elem1->elem)), mv(*((e1_t *)elem2->elem)))
			)
		)
	);
break;

case E2_TAG:
	Glue<C2Elem>::push(
		L, 
		new C2Elem(
			MV_TAG,
			new mv(
				gp(mv(*((translator *)elem1->elem)), mv(*((e2_t *)elem2->elem)))
			)
		)
	);
break;

case NI_TAG:
	Glue<C2Elem>::push(
		L, 
		new C2Elem(
			MV_TAG,
			new mv(
				gp(mv(*((translator *)elem1->elem)), mv(*((ni_t *)elem2->elem)))
			)
		)
	);
break;

case NONI_TAG:
	Glue<C2Elem>::push(
		L, 
		new C2Elem(
			MV_TAG,
			new mv(
				gp(mv(*((translator *)elem1->elem)), mv(*((noni_t *)elem2->elem)))
			)
		)
	);
break;

case I2_TAG:
	Glue<C2Elem>::push(
		L, 
		new C2Elem(
			MV_TAG,
			new mv(
				gp(mv(*((translator *)elem1->elem)), mv(*((I2_t *)elem2->elem)))
			)
		)
	);
break;

case I4_TAG:
	Glue<C2Elem>::push(
		L, 
		new C2Elem(
			MV_TAG,
			new mv(
				gp(mv(*((translator *)elem1->elem)), mv(*((I4_t *)elem2->elem)))
			)
		)
	);
break;

case I4i_TAG:
	Glue<C2Elem>::push(
		L, 
		new C2Elem(
			MV_TAG,
			new mv(
				gp(mv(*((translator *)elem1->elem)), mv(*((I4i_t *)elem2->elem)))
			)
		)
	);
break;

case VECTORE2GA_TAG:
	Glue<C2Elem>::push(
		L, 
		new C2Elem(
			MV_TAG,
			new mv(
				gp(mv(*((translator *)elem1->elem)), mv(*((vectorE2GA *)elem2->elem)))
			)
		)
	);
break;

case BIVECTORE2GA_TAG:
	Glue<C2Elem>::push(
		L, 
		new C2Elem(
			MV_TAG,
			new mv(
				gp(mv(*((translator *)elem1->elem)), mv(*((bivectorE2GA *)elem2->elem)))
			)
		)
	);
break;

case ROTORE2GA_TAG:
	Glue<C2Elem>::push(
		L, 
		new C2Elem(
			MV_TAG,
			new mv(
				gp(mv(*((translator *)elem1->elem)), mv(*((rotorE2GA *)elem2->elem)))
			)
		)
	);
break;

case NORMALIZEDPOINT_TAG:
	Glue<C2Elem>::push(
		L, 
		new C2Elem(
			MV_TAG,
			new mv(
				gp(mv(*((translator *)elem1->elem)), mv(*((normalizedPoint *)elem2->elem)))
			)
		)
	);
break;

case DUALCIRCLE_TAG:
	Glue<C2Elem>::push(
		L, 
		new C2Elem(
			MV_TAG,
			new mv(
				gp(mv(*((translator *)elem1->elem)), mv(*((dualCircle *)elem2->elem)))
			)
		)
	);
break;

case FREEVECTOR_TAG:
	Glue<C2Elem>::push(
		L, 
		new C2Elem(
			MV_TAG,
			new mv(
				gp(mv(*((translator *)elem1->elem)), mv(*((freeVector *)elem2->elem)))
			)
		)
	);
break;

case FREEBIVECTOR_TAG:
	Glue<C2Elem>::push(
		L, 
		new C2Elem(
			MV_TAG,
			new mv(
				gp(mv(*((translator *)elem1->elem)), mv(*((freeBivector *)elem2->elem)))
			)
		)
	);
break;

case CIRCLE_TAG:
	Glue<C2Elem>::push(
		L, 
		new C2Elem(
			MV_TAG,
			new mv(
				gp(mv(*((translator *)elem1->elem)), mv(*((circle *)elem2->elem)))
			)
		)
	);
break;

case LINE_TAG:
	Glue<C2Elem>::push(
		L, 
		new C2Elem(
			MV_TAG,
			new mv(
				gp(mv(*((translator *)elem1->elem)), mv(*((line *)elem2->elem)))
			)
		)
	);
break;

case PSEUDOSCALAR_TAG:
	Glue<C2Elem>::push(
		L, 
		new C2Elem(
			MV_TAG,
			new mv(
				gp(mv(*((translator *)elem1->elem)), mv(*((pseudoscalar *)elem2->elem)))
			)
		)
	);
break;

case NORMALIZEDTRANSLATOR_TAG:
	Glue<C2Elem>::push(
		L, 
		new C2Elem(
			MV_TAG,
			new mv(
				gp(mv(*((translator *)elem1->elem)), mv(*((normalizedTranslator *)elem2->elem)))
			)
		)
	);
break;

case TRANSLATOR_TAG:
	Glue<C2Elem>::push(
		L, 
		new C2Elem(
			MV_TAG,
			new mv(
				gp(mv(*((translator *)elem1->elem)), mv(*((translator *)elem2->elem)))
			)
		)
	);
break;

case EVENVERSOR_TAG:
	Glue<C2Elem>::push(
		L, 
		new C2Elem(
			MV_TAG,
			new mv(
				gp(mv(*((translator *)elem1->elem)), mv(*((evenVersor *)elem2->elem)))
			)
		)
	);
break;

case ODDVERSOR_TAG:
	Glue<C2Elem>::push(
		L, 
		new C2Elem(
			MV_TAG,
			new mv(
				gp(mv(*((translator *)elem1->elem)), mv(*((oddVersor *)elem2->elem)))
			)
		)
	);
break;

case DOUBLE_TAG:
	Glue<C2Elem>::push(
		L, 
		new C2Elem(
			MV_TAG,
			new mv(
				gp(mv(*((translator *)elem1->elem)), *((double *)elem2))
			)
		)
	);
break;
}
break;

case EVENVERSOR_TAG:
switch(tag2) {
case MV_TAG:
	Glue<C2Elem>::push(
		L, 
		new C2Elem(
			MV_TAG,
			new mv(
				gp(mv(*((evenVersor *)elem1->elem)), mv(*((mv *)elem2->elem)))
			)
		)
	);
break;

case NO_TAG:
	Glue<C2Elem>::push(
		L, 
		new C2Elem(
			MV_TAG,
			new mv(
				gp(mv(*((evenVersor *)elem1->elem)), mv(*((no_t *)elem2->elem)))
			)
		)
	);
break;

case E1_TAG:
	Glue<C2Elem>::push(
		L, 
		new C2Elem(
			MV_TAG,
			new mv(
				gp(mv(*((evenVersor *)elem1->elem)), mv(*((e1_t *)elem2->elem)))
			)
		)
	);
break;

case E2_TAG:
	Glue<C2Elem>::push(
		L, 
		new C2Elem(
			MV_TAG,
			new mv(
				gp(mv(*((evenVersor *)elem1->elem)), mv(*((e2_t *)elem2->elem)))
			)
		)
	);
break;

case NI_TAG:
	Glue<C2Elem>::push(
		L, 
		new C2Elem(
			MV_TAG,
			new mv(
				gp(mv(*((evenVersor *)elem1->elem)), mv(*((ni_t *)elem2->elem)))
			)
		)
	);
break;

case NONI_TAG:
	Glue<C2Elem>::push(
		L, 
		new C2Elem(
			MV_TAG,
			new mv(
				gp(mv(*((evenVersor *)elem1->elem)), mv(*((noni_t *)elem2->elem)))
			)
		)
	);
break;

case I2_TAG:
	Glue<C2Elem>::push(
		L, 
		new C2Elem(
			MV_TAG,
			new mv(
				gp(mv(*((evenVersor *)elem1->elem)), mv(*((I2_t *)elem2->elem)))
			)
		)
	);
break;

case I4_TAG:
	Glue<C2Elem>::push(
		L, 
		new C2Elem(
			MV_TAG,
			new mv(
				gp(mv(*((evenVersor *)elem1->elem)), mv(*((I4_t *)elem2->elem)))
			)
		)
	);
break;

case I4i_TAG:
	Glue<C2Elem>::push(
		L, 
		new C2Elem(
			MV_TAG,
			new mv(
				gp(mv(*((evenVersor *)elem1->elem)), mv(*((I4i_t *)elem2->elem)))
			)
		)
	);
break;

case VECTORE2GA_TAG:
	Glue<C2Elem>::push(
		L, 
		new C2Elem(
			MV_TAG,
			new mv(
				gp(mv(*((evenVersor *)elem1->elem)), mv(*((vectorE2GA *)elem2->elem)))
			)
		)
	);
break;

case BIVECTORE2GA_TAG:
	Glue<C2Elem>::push(
		L, 
		new C2Elem(
			MV_TAG,
			new mv(
				gp(mv(*((evenVersor *)elem1->elem)), mv(*((bivectorE2GA *)elem2->elem)))
			)
		)
	);
break;

case ROTORE2GA_TAG:
	Glue<C2Elem>::push(
		L, 
		new C2Elem(
			MV_TAG,
			new mv(
				gp(mv(*((evenVersor *)elem1->elem)), mv(*((rotorE2GA *)elem2->elem)))
			)
		)
	);
break;

case NORMALIZEDPOINT_TAG:
	Glue<C2Elem>::push(
		L, 
		new C2Elem(
			MV_TAG,
			new mv(
				gp(mv(*((evenVersor *)elem1->elem)), mv(*((normalizedPoint *)elem2->elem)))
			)
		)
	);
break;

case DUALCIRCLE_TAG:
	Glue<C2Elem>::push(
		L, 
		new C2Elem(
			MV_TAG,
			new mv(
				gp(mv(*((evenVersor *)elem1->elem)), mv(*((dualCircle *)elem2->elem)))
			)
		)
	);
break;

case FREEVECTOR_TAG:
	Glue<C2Elem>::push(
		L, 
		new C2Elem(
			MV_TAG,
			new mv(
				gp(mv(*((evenVersor *)elem1->elem)), mv(*((freeVector *)elem2->elem)))
			)
		)
	);
break;

case FREEBIVECTOR_TAG:
	Glue<C2Elem>::push(
		L, 
		new C2Elem(
			MV_TAG,
			new mv(
				gp(mv(*((evenVersor *)elem1->elem)), mv(*((freeBivector *)elem2->elem)))
			)
		)
	);
break;

case CIRCLE_TAG:
	Glue<C2Elem>::push(
		L, 
		new C2Elem(
			MV_TAG,
			new mv(
				gp(mv(*((evenVersor *)elem1->elem)), mv(*((circle *)elem2->elem)))
			)
		)
	);
break;

case LINE_TAG:
	Glue<C2Elem>::push(
		L, 
		new C2Elem(
			MV_TAG,
			new mv(
				gp(mv(*((evenVersor *)elem1->elem)), mv(*((line *)elem2->elem)))
			)
		)
	);
break;

case PSEUDOSCALAR_TAG:
	Glue<C2Elem>::push(
		L, 
		new C2Elem(
			MV_TAG,
			new mv(
				gp(mv(*((evenVersor *)elem1->elem)), mv(*((pseudoscalar *)elem2->elem)))
			)
		)
	);
break;

case NORMALIZEDTRANSLATOR_TAG:
	Glue<C2Elem>::push(
		L, 
		new C2Elem(
			MV_TAG,
			new mv(
				gp(mv(*((evenVersor *)elem1->elem)), mv(*((normalizedTranslator *)elem2->elem)))
			)
		)
	);
break;

case TRANSLATOR_TAG:
	Glue<C2Elem>::push(
		L, 
		new C2Elem(
			MV_TAG,
			new mv(
				gp(mv(*((evenVersor *)elem1->elem)), mv(*((translator *)elem2->elem)))
			)
		)
	);
break;

case EVENVERSOR_TAG:
	Glue<C2Elem>::push(
		L, 
		new C2Elem(
			MV_TAG,
			new mv(
				gp(mv(*((evenVersor *)elem1->elem)), mv(*((evenVersor *)elem2->elem)))
			)
		)
	);
break;

case ODDVERSOR_TAG:
	Glue<C2Elem>::push(
		L, 
		new C2Elem(
			MV_TAG,
			new mv(
				gp(mv(*((evenVersor *)elem1->elem)), mv(*((oddVersor *)elem2->elem)))
			)
		)
	);
break;

case DOUBLE_TAG:
	Glue<C2Elem>::push(
		L, 
		new C2Elem(
			MV_TAG,
			new mv(
				gp(mv(*((evenVersor *)elem1->elem)), *((double *)elem2))
			)
		)
	);
break;
}
break;

case ODDVERSOR_TAG:
switch(tag2) {
case MV_TAG:
	Glue<C2Elem>::push(
		L, 
		new C2Elem(
			MV_TAG,
			new mv(
				gp(mv(*((oddVersor *)elem1->elem)), mv(*((mv *)elem2->elem)))
			)
		)
	);
break;

case NO_TAG:
	Glue<C2Elem>::push(
		L, 
		new C2Elem(
			MV_TAG,
			new mv(
				gp(mv(*((oddVersor *)elem1->elem)), mv(*((no_t *)elem2->elem)))
			)
		)
	);
break;

case E1_TAG:
	Glue<C2Elem>::push(
		L, 
		new C2Elem(
			MV_TAG,
			new mv(
				gp(mv(*((oddVersor *)elem1->elem)), mv(*((e1_t *)elem2->elem)))
			)
		)
	);
break;

case E2_TAG:
	Glue<C2Elem>::push(
		L, 
		new C2Elem(
			MV_TAG,
			new mv(
				gp(mv(*((oddVersor *)elem1->elem)), mv(*((e2_t *)elem2->elem)))
			)
		)
	);
break;

case NI_TAG:
	Glue<C2Elem>::push(
		L, 
		new C2Elem(
			MV_TAG,
			new mv(
				gp(mv(*((oddVersor *)elem1->elem)), mv(*((ni_t *)elem2->elem)))
			)
		)
	);
break;

case NONI_TAG:
	Glue<C2Elem>::push(
		L, 
		new C2Elem(
			MV_TAG,
			new mv(
				gp(mv(*((oddVersor *)elem1->elem)), mv(*((noni_t *)elem2->elem)))
			)
		)
	);
break;

case I2_TAG:
	Glue<C2Elem>::push(
		L, 
		new C2Elem(
			MV_TAG,
			new mv(
				gp(mv(*((oddVersor *)elem1->elem)), mv(*((I2_t *)elem2->elem)))
			)
		)
	);
break;

case I4_TAG:
	Glue<C2Elem>::push(
		L, 
		new C2Elem(
			MV_TAG,
			new mv(
				gp(mv(*((oddVersor *)elem1->elem)), mv(*((I4_t *)elem2->elem)))
			)
		)
	);
break;

case I4i_TAG:
	Glue<C2Elem>::push(
		L, 
		new C2Elem(
			MV_TAG,
			new mv(
				gp(mv(*((oddVersor *)elem1->elem)), mv(*((I4i_t *)elem2->elem)))
			)
		)
	);
break;

case VECTORE2GA_TAG:
	Glue<C2Elem>::push(
		L, 
		new C2Elem(
			MV_TAG,
			new mv(
				gp(mv(*((oddVersor *)elem1->elem)), mv(*((vectorE2GA *)elem2->elem)))
			)
		)
	);
break;

case BIVECTORE2GA_TAG:
	Glue<C2Elem>::push(
		L, 
		new C2Elem(
			MV_TAG,
			new mv(
				gp(mv(*((oddVersor *)elem1->elem)), mv(*((bivectorE2GA *)elem2->elem)))
			)
		)
	);
break;

case ROTORE2GA_TAG:
	Glue<C2Elem>::push(
		L, 
		new C2Elem(
			MV_TAG,
			new mv(
				gp(mv(*((oddVersor *)elem1->elem)), mv(*((rotorE2GA *)elem2->elem)))
			)
		)
	);
break;

case NORMALIZEDPOINT_TAG:
	Glue<C2Elem>::push(
		L, 
		new C2Elem(
			MV_TAG,
			new mv(
				gp(mv(*((oddVersor *)elem1->elem)), mv(*((normalizedPoint *)elem2->elem)))
			)
		)
	);
break;

case DUALCIRCLE_TAG:
	Glue<C2Elem>::push(
		L, 
		new C2Elem(
			MV_TAG,
			new mv(
				gp(mv(*((oddVersor *)elem1->elem)), mv(*((dualCircle *)elem2->elem)))
			)
		)
	);
break;

case FREEVECTOR_TAG:
	Glue<C2Elem>::push(
		L, 
		new C2Elem(
			MV_TAG,
			new mv(
				gp(mv(*((oddVersor *)elem1->elem)), mv(*((freeVector *)elem2->elem)))
			)
		)
	);
break;

case FREEBIVECTOR_TAG:
	Glue<C2Elem>::push(
		L, 
		new C2Elem(
			MV_TAG,
			new mv(
				gp(mv(*((oddVersor *)elem1->elem)), mv(*((freeBivector *)elem2->elem)))
			)
		)
	);
break;

case CIRCLE_TAG:
	Glue<C2Elem>::push(
		L, 
		new C2Elem(
			MV_TAG,
			new mv(
				gp(mv(*((oddVersor *)elem1->elem)), mv(*((circle *)elem2->elem)))
			)
		)
	);
break;

case LINE_TAG:
	Glue<C2Elem>::push(
		L, 
		new C2Elem(
			MV_TAG,
			new mv(
				gp(mv(*((oddVersor *)elem1->elem)), mv(*((line *)elem2->elem)))
			)
		)
	);
break;

case PSEUDOSCALAR_TAG:
	Glue<C2Elem>::push(
		L, 
		new C2Elem(
			MV_TAG,
			new mv(
				gp(mv(*((oddVersor *)elem1->elem)), mv(*((pseudoscalar *)elem2->elem)))
			)
		)
	);
break;

case NORMALIZEDTRANSLATOR_TAG:
	Glue<C2Elem>::push(
		L, 
		new C2Elem(
			MV_TAG,
			new mv(
				gp(mv(*((oddVersor *)elem1->elem)), mv(*((normalizedTranslator *)elem2->elem)))
			)
		)
	);
break;

case TRANSLATOR_TAG:
	Glue<C2Elem>::push(
		L, 
		new C2Elem(
			MV_TAG,
			new mv(
				gp(mv(*((oddVersor *)elem1->elem)), mv(*((translator *)elem2->elem)))
			)
		)
	);
break;

case EVENVERSOR_TAG:
	Glue<C2Elem>::push(
		L, 
		new C2Elem(
			MV_TAG,
			new mv(
				gp(mv(*((oddVersor *)elem1->elem)), mv(*((evenVersor *)elem2->elem)))
			)
		)
	);
break;

case ODDVERSOR_TAG:
	Glue<C2Elem>::push(
		L, 
		new C2Elem(
			MV_TAG,
			new mv(
				gp(mv(*((oddVersor *)elem1->elem)), mv(*((oddVersor *)elem2->elem)))
			)
		)
	);
break;

case DOUBLE_TAG:
	Glue<C2Elem>::push(
		L, 
		new C2Elem(
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
	Glue<C2Elem>::push(
		L, 
		new C2Elem(
			MV_TAG,
			new mv(
				gp(*((double *)elem1), *((mv *)elem2->elem))
			)
		)
	);
break;

case NO_TAG:
	Glue<C2Elem>::push(
		L, 
		new C2Elem(
			MV_TAG,
			new mv(
				gp(*((double *)elem1), *((no_t *)elem2->elem))
			)
		)
	);
break;

case E1_TAG:
	Glue<C2Elem>::push(
		L, 
		new C2Elem(
			MV_TAG,
			new mv(
				gp(*((double *)elem1), *((e1_t *)elem2->elem))
			)
		)
	);
break;

case E2_TAG:
	Glue<C2Elem>::push(
		L, 
		new C2Elem(
			MV_TAG,
			new mv(
				gp(*((double *)elem1), *((e2_t *)elem2->elem))
			)
		)
	);
break;

case NI_TAG:
	Glue<C2Elem>::push(
		L, 
		new C2Elem(
			MV_TAG,
			new mv(
				gp(*((double *)elem1), *((ni_t *)elem2->elem))
			)
		)
	);
break;

case NONI_TAG:
	Glue<C2Elem>::push(
		L, 
		new C2Elem(
			MV_TAG,
			new mv(
				gp(*((double *)elem1), *((noni_t *)elem2->elem))
			)
		)
	);
break;

case I2_TAG:
	Glue<C2Elem>::push(
		L, 
		new C2Elem(
			MV_TAG,
			new mv(
				gp(*((double *)elem1), *((I2_t *)elem2->elem))
			)
		)
	);
break;

case I4_TAG:
	Glue<C2Elem>::push(
		L, 
		new C2Elem(
			MV_TAG,
			new mv(
				gp(*((double *)elem1), *((I4_t *)elem2->elem))
			)
		)
	);
break;

case I4i_TAG:
	Glue<C2Elem>::push(
		L, 
		new C2Elem(
			MV_TAG,
			new mv(
				gp(*((double *)elem1), *((I4i_t *)elem2->elem))
			)
		)
	);
break;

case VECTORE2GA_TAG:
	Glue<C2Elem>::push(
		L, 
		new C2Elem(
			MV_TAG,
			new mv(
				gp(*((double *)elem1), *((vectorE2GA *)elem2->elem))
			)
		)
	);
break;

case BIVECTORE2GA_TAG:
	Glue<C2Elem>::push(
		L, 
		new C2Elem(
			MV_TAG,
			new mv(
				gp(*((double *)elem1), *((bivectorE2GA *)elem2->elem))
			)
		)
	);
break;

case ROTORE2GA_TAG:
	Glue<C2Elem>::push(
		L, 
		new C2Elem(
			MV_TAG,
			new mv(
				gp(*((double *)elem1), *((rotorE2GA *)elem2->elem))
			)
		)
	);
break;

case NORMALIZEDPOINT_TAG:
	Glue<C2Elem>::push(
		L, 
		new C2Elem(
			MV_TAG,
			new mv(
				gp(*((double *)elem1), *((normalizedPoint *)elem2->elem))
			)
		)
	);
break;

case DUALCIRCLE_TAG:
	Glue<C2Elem>::push(
		L, 
		new C2Elem(
			MV_TAG,
			new mv(
				gp(*((double *)elem1), *((dualCircle *)elem2->elem))
			)
		)
	);
break;

case FREEVECTOR_TAG:
	Glue<C2Elem>::push(
		L, 
		new C2Elem(
			MV_TAG,
			new mv(
				gp(*((double *)elem1), *((freeVector *)elem2->elem))
			)
		)
	);
break;

case FREEBIVECTOR_TAG:
	Glue<C2Elem>::push(
		L, 
		new C2Elem(
			MV_TAG,
			new mv(
				gp(*((double *)elem1), *((freeBivector *)elem2->elem))
			)
		)
	);
break;

case CIRCLE_TAG:
	Glue<C2Elem>::push(
		L, 
		new C2Elem(
			MV_TAG,
			new mv(
				gp(*((double *)elem1), *((circle *)elem2->elem))
			)
		)
	);
break;

case LINE_TAG:
	Glue<C2Elem>::push(
		L, 
		new C2Elem(
			MV_TAG,
			new mv(
				gp(*((double *)elem1), *((line *)elem2->elem))
			)
		)
	);
break;

case PSEUDOSCALAR_TAG:
	Glue<C2Elem>::push(
		L, 
		new C2Elem(
			MV_TAG,
			new mv(
				gp(*((double *)elem1), *((pseudoscalar *)elem2->elem))
			)
		)
	);
break;

case NORMALIZEDTRANSLATOR_TAG:
	Glue<C2Elem>::push(
		L, 
		new C2Elem(
			MV_TAG,
			new mv(
				gp(*((double *)elem1), *((normalizedTranslator *)elem2->elem))
			)
		)
	);
break;

case TRANSLATOR_TAG:
	Glue<C2Elem>::push(
		L, 
		new C2Elem(
			MV_TAG,
			new mv(
				gp(*((double *)elem1), *((translator *)elem2->elem))
			)
		)
	);
break;

case EVENVERSOR_TAG:
	Glue<C2Elem>::push(
		L, 
		new C2Elem(
			MV_TAG,
			new mv(
				gp(*((double *)elem1), *((evenVersor *)elem2->elem))
			)
		)
	);
break;

case ODDVERSOR_TAG:
	Glue<C2Elem>::push(
		L, 
		new C2Elem(
			MV_TAG,
			new mv(
				gp(*((double *)elem1), *((oddVersor *)elem2->elem))
			)
		)
	);
break;

case DOUBLE_TAG:
	/*
	Glue<C2Elem>::push(
		L, 
		new C2Elem(
			MV_TAG,
			new mv(
				gp(*((double *)elem1), *((double *)elem2))
			)
		)
	);
	*/
break;
}
break;
}
return 1;
}


#ifdef __cplusplus
}
#endif