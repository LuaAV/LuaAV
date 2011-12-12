#ifndef LUA_C2GA_H
#define LUA_C2GA_H

#include "lua_utility.h"
#include "lua_glue.h"
#include "c2ga.h"

using namespace c2ga;

enum C2Tag {
	MV_TAG = 0,
	NO_TAG,
	E1_TAG,
	E2_TAG,
	NI_TAG,
	NONI_TAG,
	I2_TAG,
	I4_TAG,
	I4i_TAG,
	VECTORE2GA_TAG,
	BIVECTORE2GA_TAG,
	ROTORE2GA_TAG,
	NORMALIZEDPOINT_TAG,
	DUALCIRCLE_TAG,
	FREEVECTOR_TAG,
	FREEBIVECTOR_TAG,
	CIRCLE_TAG,
	LINE_TAG,
	PSEUDOSCALAR_TAG,
	NORMALIZEDTRANSLATOR_TAG,
	TRANSLATOR_TAG,
	EVENVERSOR_TAG,
	ODDVERSOR_TAG,
	DOUBLE_TAG
};


struct C2Elem {
	C2Elem(C2Tag tag, void *elem)
	:	tag(tag),
		elem(elem)
	{}
	
	~C2Elem() {
		switch(tag) {
			case MV_TAG:			delete ((mv *)elem); break;
			case NO_TAG:			delete ((no_t *)elem); break;
			case E1_TAG:			delete ((e1_t *)elem); break;
			case E2_TAG:			delete ((e2_t *)elem); break;
			case NI_TAG:			delete ((ni_t *)elem); break;
			case NONI_TAG:			delete ((noni_t *)elem); break;
			case I2_TAG:			delete ((I2_t *)elem); break;
			case VECTORE2GA_TAG:	delete ((vectorE2GA *)elem); break;
			case BIVECTORE2GA_TAG:	delete ((bivectorE2GA *)elem); break;
			case ROTORE2GA_TAG:		delete ((rotorE2GA *)elem); break;
			case NORMALIZEDPOINT_TAG:	delete ((normalizedPoint *)elem); break;
			case DUALCIRCLE_TAG:	delete ((dualCircle *)elem); break;
			case FREEVECTOR_TAG:	delete ((freeVector *)elem); break;
			case FREEBIVECTOR_TAG:	delete ((freeBivector *)elem); break;
			case CIRCLE_TAG:		delete ((circle *)elem); break;
			case LINE_TAG:			delete ((line *)elem); break;
			case PSEUDOSCALAR_TAG:	delete ((pseudoscalar *)elem); break;
			case NORMALIZEDTRANSLATOR_TAG:	delete ((normalizedTranslator *)elem); break;
			case TRANSLATOR_TAG:	delete ((translator *)elem); break;
			case EVENVERSOR_TAG:	delete ((evenVersor *)elem); break;
			case ODDVERSOR_TAG:	delete ((oddVersor *)elem); break;
		}
	}


	C2Tag tag;
	void *elem;
};


extern const char * name_from_tag(C2Tag tag);

#endif // LUA_C2GA_H