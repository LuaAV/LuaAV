#ifndef INCLUDE_AL_MEMORY_HPP
#define INCLUDE_AL_MEMORY_HPP

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
#include <string.h>

namespace al {

/*
	An Arena is a block from which to allocate items that will share a lifetime.
	When the lifetime is expired, there is no need to free the items allocated;
	just deleting the Arena (or letting it go out of scope if stack allocated) 
	will free all associated memory.
	
	AKA AutoReleasePool. 
	
	E.g.
	
	void test() {
		Arena arena;
		double * x = (double *)arena.calloc(sizeof(double) * 10);
		Foo * foo = arena.alloc<Foo>();
		Foo * foo1 = arena.New<Foo>();
		
		... // instructions using x and foo
		
	}	// stack variable arena goes out of scope, and frees memory of x and foo
*/
class Arena {
public:
	Arena();
	~Arena();
	
	void * alloc(size_t size) { return impl->alloc(size); }
	void * calloc(size_t size) { return impl->calloc(size); }
	
	template<typename T>
	T * alloc() { return alloc(sizeof(T)); }
	template<typename T>
	T * calloc() { return calloc(sizeof(T)); }
	
	template<typename T>
	T * New() { return new (alloc(sizeof(T))) T(); }
	
	
	class Impl {
	public:
		virtual ~Impl() {};
		virtual void * alloc(size_t size) = 0;
		virtual void * calloc(size_t size) { return memset(alloc(size), 0, size); };
	};
protected:
	Impl * impl;
};

} // al::

#endif /* include guard */
