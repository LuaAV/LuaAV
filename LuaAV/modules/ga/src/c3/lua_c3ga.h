#ifndef LUA_C3GA_H
#define LUA_C3GA_H

#include "lua_utility.h"
#include "lua_glue.h"
#include "c3ga.h"

using namespace c3ga;

enum C3Tag {
	MV_TAG = 0,
	NO_TAG,
	E1_TAG,
	E2_TAG,
	E3_TAG,
	NI_TAG,
	NONI_TAG,
	I3_TAG,
	I5_TAG,
	I5i_TAG,
	VECTORE3GA_TAG,
	BIVECTORE3GA_TAG,
	ROTORE3GA_TAG,
	NORMALIZEDPOINT_TAG,
	DUALSPHERE_TAG,
	DUALPLANE_TAG,
	FREEVECTOR_TAG,
	FREEBIVECTOR_TAG,
	FLATPOINT_TAG,
	POINTPAIR_TAG,
	LINE_TAG,
	CIRCLE_TAG,
	SPHERE_TAG,
	PLANE_TAG,
	PSEUDOSCALAR_TAG,
	NORMALIZEDTRANSLATOR_TAG,
	TRANSLATOR_TAG,
	RBM_TAG,
	EVENVERSOR_TAG,
	ODDVERSOR_TAG,
	DOUBLE_TAG
};


struct C3Elem {
	C3Elem(C3Tag tag, void *elem)
	:	tag(tag),
		elem(elem)
	{}
	
	~C3Elem() {
		switch(tag) {
			case MV_TAG:			delete ((mv *)elem); break;
			case NO_TAG:			delete ((no_t *)elem); break;
			case E1_TAG:			delete ((e1_t *)elem); break;
			case E2_TAG:			delete ((e2_t *)elem); break;
			case E3_TAG:			delete ((e3_t *)elem); break;
			case NI_TAG:			delete ((ni_t *)elem); break;
			case NONI_TAG:			delete ((noni_t *)elem); break;
			case I3_TAG:			delete ((I3_t *)elem); break;
			case I5_TAG:			delete ((I5_t *)elem); break;
			case I5i_TAG:			delete ((I5i_t *)elem); break;
			case VECTORE3GA_TAG:	delete ((vectorE3GA *)elem); break;
			case BIVECTORE3GA_TAG:	delete ((bivectorE3GA *)elem); break;
			case ROTORE3GA_TAG:		delete ((rotorE3GA *)elem); break;
			case NORMALIZEDPOINT_TAG:	delete ((normalizedPoint *)elem); break;
			case DUALSPHERE_TAG:	delete ((dualSphere *)elem); break;
			case DUALPLANE_TAG:		delete ((dualPlane *)elem); break;
			case FREEVECTOR_TAG:	delete ((freeVector *)elem); break;
			case FREEBIVECTOR_TAG:	delete ((freeBivector *)elem); break;
			case FLATPOINT_TAG:		delete ((flatPoint *)elem); break;
			case POINTPAIR_TAG:		delete ((pointPair *)elem); break;
			case LINE_TAG:			delete ((line *)elem); break;
			case CIRCLE_TAG:		delete ((circle *)elem); break;
			case SPHERE_TAG:		delete ((sphere *)elem); break;
			case PLANE_TAG:			delete ((plane *)elem); break;
			case PSEUDOSCALAR_TAG:	delete ((pseudoscalar *)elem); break;
			case NORMALIZEDTRANSLATOR_TAG:	delete ((normalizedTranslator *)elem); break;
			case TRANSLATOR_TAG:	delete ((translator *)elem); break;
			case RBM_TAG:	delete	((RBM *)elem); break;
			case EVENVERSOR_TAG:	delete ((evenVersor *)elem); break;
			case ODDVERSOR_TAG:	delete ((oddVersor *)elem); break;
		}
	}


	C3Tag tag;
	void *elem;
};


extern const char * name_from_tag(C3Tag tag);

#endif // LUA_C3GA_H