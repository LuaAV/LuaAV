#ifndef LUA_E2GA_H
#define LUA_E2GA_H

#include "lua_utility.h"
#include "lua_glue.h"
#include "e2ga.h"

using namespace e2ga;

enum E2Tag {
	MV_TAG = 0,
	E1_TAG,
	E2_TAG,
	I2_TAG,
	I2i_TAG,
	VECTOR_TAG,
	BIVECTOR_TAG,
	ROTOR_TAG,
	DOUBLE_TAG
};


struct E2Elem {
	E2Elem(E2Tag tag, void *elem)
	:	tag(tag),
		elem(elem)
	{}
	
	~E2Elem() {
		switch(tag) {
			case MV_TAG:		delete ((mv *)elem); break;
			case E1_TAG:		delete ((e1_t *)elem); break;
			case E2_TAG:		delete ((e2_t *)elem); break;
			case I2_TAG:		delete ((I2_t *)elem); break;
			case I2i_TAG:		delete ((I2i_t *)elem); break;
			case VECTOR_TAG:	delete ((vector *)elem); break;
			case BIVECTOR_TAG:	delete ((bivector *)elem); break;
			case ROTOR_TAG:		delete ((rotor *)elem); break;
		}
	}


	E2Tag tag;
	void *elem;
};


extern const char * name_from_tag(E2Tag tag);

#endif // LUA_E2GA_H