#ifndef LUA_E3GA_H
#define LUA_E3GA_H

#include "lua_utility.h"
#include "lua_glue.h"
#include "e3ga.h"

using namespace e3ga;

enum E3Tag {
	MV_TAG = 0,
	E1_TAG,
	E2_TAG,
	E3_TAG,
	I3_TAG,
	VECTOR_TAG,
	BIVECTOR_TAG,
	TRIVECTOR_TAG,
	ROTOR_TAG,
	ODDVERSOR_TAG,
	DOUBLE_TAG
};


struct E3Elem {
	E3Elem(E3Tag tag, void *elem)
	:	tag(tag),
		elem(elem)
	{}
	
	~E3Elem() {
		switch(tag) {
			case MV_TAG:		delete ((mv *)elem); break;
			case E1_TAG:		delete ((e1_t *)elem); break;
			case E2_TAG:		delete ((e2_t *)elem); break;
			case E3_TAG:		delete ((e3_t *)elem); break;
			case I3_TAG:		delete ((I3_t *)elem); break;
			case VECTOR_TAG:	delete ((vector *)elem); break;
			case BIVECTOR_TAG:	delete ((bivector *)elem); break;
			case TRIVECTOR_TAG:	delete ((trivector *)elem); break;
			case ROTOR_TAG:		delete ((rotor *)elem); break;
			case ODDVERSOR_TAG:	delete ((oddVersor *)elem); break;
		}
	}


	E3Tag tag;
	void *elem;
};


extern const char * name_from_tag(E3Tag tag);

#endif // LUA_E3GA_H