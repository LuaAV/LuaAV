local clang = require("clang")
compiler = clang.Compiler()

local header_path = app.resourcepath.."/Headers"
compiler:include(header_path)

local code = [[
	extern "C" {
	
	#include "lua.h"
	#include "lauxlib.h"
	
	}
	
	#include <stdio.h>
	
	template<typename T>
	struct Vec3{
	public:
		T x;
		T y;
		T z;
		
		Vec3(T x=0, T y=0, T z=0)
		:	x(x), y(y), z(z)
		{}
		
		const Vec3 operator* (const Vec3 v) const {
			return Vec3(x*v.x, y*v.y, z*v.z);
		}
		
		const Vec3 operator* (T s) const {
			return Vec3(x*s, y*s, z*s);
		}
	};
	
	
	typedef unsigned int UINT;
	typedef int Fixed;

	#define fixed1		(1<<16L) 
	#define fixedhalf	(1<<15L)
	#define IntToFixed(x) ((Fixed) ((x)<<16L))
	#define CharToFixed(x)	IntToFixed(((int)(x)))
	#define FloatToFixed(x) ((Fixed)((x) * (float)fixed1))
	#define FixedToInt(x) ((x)>>16L)
	#define FixMul(x,y) (((y)>>8L) * ((x)>>8L))   //both are large(>1)         	
	#define FixMul2(x,y) (((y) * ((x)>>8L))>>8L)  //y is small          
	#define FixMul3(x,y) ((((y)>>8L) * (x))>>8L)  //x is small

	template<>
	struct Vec3<char>{
	public:
		char x;
		char y;
		char z;
		
		Vec3(char x=0, char y=0, char z=0)
		:	x(x), y(y), z(z)
		{}
		
		const Vec3 operator* (const Vec3 v) const {
			return Vec3(x*v.x, y*v.y, z*v.z);
		}
		
		const Vec3 operator* (char s) const {
			return Vec3(x*s, y*s, z*s);
		}
		
		/*
		const Vec3 operator* (float s) const {
			Fixed fs = FloatToFixed(s);
			return Vec3(
				FixedToInt(FixMul(CharToFixed(x), fs)), 
				FixedToInt(FixMul(CharToFixed(y), fs)), 
				FixedToInt(FixMul(CharToFixed(z), fs))
			);
		}
		*/
		const Vec3 operator* (float s) const {
			return Vec3(x*s, y*s, z*s);
		}
		
	};
	
	
	
	
	extern "C" {
	
	int test(lua_State *L) {
		Vec3<float> v1(1, 2, 3);
		Vec3<float> v2(0.5, 0.25, 2.);
		Vec3<float> v = v1*v2*2.;
		printf("x: %f %f %f\n", v.x, v.y, v.z);
		
		
		Vec3<char> v1c(64, 123, 5);
		v1c = v1c*0.0125f;
		printf("t: %d %d %d\n", v1c.x, v1c.y, v1c.z);
		return 0;
	}
	
	}
]]

if(compiler:compile(code)) then
	compiler:dump()
	local jit = assert(compiler:jit())
	local f = jit:pushcfunction("test")
	print(f)
	
	go(1, function()
		print("f(): ")
		f()
	end)
end