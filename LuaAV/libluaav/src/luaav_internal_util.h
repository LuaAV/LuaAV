#ifndef INCLUDE_LUAAV_INTERNAL_UTIL_H
#define INCLUDE_LUAAV_INTERNAL_UTIL_H 1

/*
 *  AlloSphere Research Group / Media Arts & Technology, UCSB, 2009
 */

/*
	Copyright (C) 2006-2008. The Regents of the University of California (REGENTS). 
	All Rights Reserved.

	Permission to use, copy, modify, distribute, and distribute modified versions
	of this software and its documentation without fee and without a signed
	licensing agreement, is hereby granted, provided that the above copyright
	notice, the list of contributors, this paragraph and the following two paragraphs 
	appear in all copies, modifications, and distributions.

	IN NO EVENT SHALL REGENTS BE LIABLE TO ANY PARTY FOR DIRECT, INDIRECT,
	SPECIAL, INCIDENTAL, OR CONSEQUENTIAL DAMAGES, INCLUDING LOST PROFITS, ARISING
	OUT OF THE USE OF THIS SOFTWARE AND ITS DOCUMENTATION, EVEN IF REGENTS HAS
	BEEN ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.

	REGENTS SPECIFICALLY DISCLAIMS ANY WARRANTIES, INCLUDING, BUT NOT LIMITED TO,
	THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR
	PURPOSE. THE SOFTWARE AND ACCOMPANYING DOCUMENTATION, IF ANY, PROVIDED
	HEREUNDER IS PROVIDED "AS IS". REGENTS HAS  NO OBLIGATION TO PROVIDE
	MAINTENANCE, SUPPORT, UPDATES, ENHANCEMENTS, OR MODIFICATIONS.
*/

#include "luaav.h"
#include "lua_utility.h"

/*
	Very simple singly linked list implementation; 
	T must have a T * next member.
*/
template<typename T>
struct SimpleList {
	T * first, * last;
	
	inline void prepend(T * u);
	inline void append(T * u);
	inline void remove(T * u);
	
	SimpleList() : first(NULL), last(NULL) {}
};


/* 
	INLINE IMPLEMENTATION
*/
#pragma mark Inline Implementation

template<typename T>
inline void SimpleList<T> :: append(T * u) {
	if (last) {
		u->next = NULL;
		last->next = u;
		last = u;
	} else {
		u->next = NULL;
		first = last = u;
	}	
}

template<typename T>
inline void SimpleList<T> :: prepend(T * u) {
	if (first) {
		u->next = first;
		first = u;
	} else {
		u->next = NULL;
		first = last = u;
	}
}

template<typename T>
inline void SimpleList<T> :: remove(T * u) {
	T * n = first;
	
	if (first == u) {
		first = u->next;
		if (last == u) last = NULL;
		u->next = NULL; 
		return;
	}
	
	int i=0;
	while (n) {
		if (n->next == u) {
			n->next = u->next;
			u->next = NULL; 
			if (last == u) {
				last = n;
			}
			return;
		}
		i++;
		n = n->next;
	}
}


#endif /* include guard */
