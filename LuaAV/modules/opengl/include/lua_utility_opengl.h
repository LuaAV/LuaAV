#ifndef LUA_UTILITY_OPENGL_H
#define LUA_UTILITY_OPENGL_H 1

#include "lua_utility.h"

namespace lua {

	template <class T, class F>
	class Iterator {
	private:
		Iterator() {};
		~Iterator() {};

	public:
		static int iter(lua_State *L);
		static int push(lua_State *L, T iterator, T end);
	};

	template <class T, class F>
	int Iterator<T, F> :: iter(lua_State *L) {
		T *iterator = (T *)lua_touserdata(L, lua_upvalueindex(1));
		T *end = (T *)lua_touserdata(L, lua_upvalueindex(2));
		
		if( (*iterator) != *end ) {
			F::push(L, *iterator);
			(*iterator)++;
			return 1;
		}
		
		return 0;
	}

	template <class T, class F>
	int Iterator<T, F> :: push(lua_State *L, T iterator, T end) {
		// create userdata
		T *p_iterator = (T *)lua_newuserdata(L, sizeof(T));
		*p_iterator = iterator;
		
		T *p_end = (T *)lua_newuserdata(L, sizeof(T));
		*p_end = end;
		
		// create the iteration closure
		lua_pushcclosure(L, iter, 2);
		
		return 1;
	}
		
} // lua::

#endif // LUA_UTILITY_OPENGL_H