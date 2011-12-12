#ifndef SPACE_VEC3_UDATA_H
#define SPACE_VEC3_UDATA_H 1

#include <vec3.h>
#include "lua_space.h"
#include "lua_utility.h"

#define TEM template <typename T>

namespace space {

typedef double LuaVecTy;
typedef Vec3<LuaVecTy> LuaVec3;

/*! 3-Component vector operations.
	3-Component vector operations.  Vec3 methods operate on array triplets: {x, y, z}.
	
	@module space.vec3
*/

/*! Addition
	Adds 2 vectors
	@param v1 First vector
	@param v2 Second vector
	@ret Result vector
	@LuaMethod METHOD
	@name add
*/
static int lua_vec3_add(lua_State *L) {
	LuaVec3 v1;
	LuaVec3 v2;
	if(
		lua::to_vec_t<LuaVecTy>(L, 1, 3, &(v1.x)) && 
		lua::to_vec_t<LuaVecTy>(L, 2, 3, &(v2.x))
	) {
		LuaVec3 v3;
		LuaVec3::add(v3, v1, v2);
		lua::push_vec_t<LuaVecTy>(L, 3, &(v3.x));
	}
	else {
		luaL_error(L, "Error: Vec3.add invalid arguments");
	}

	return 1;
}

/*! Subtraction
	Subtracts 2 vectors
	@param v1 First vector
	@param v2 Second vector
	@ret Result vector
	@LuaMethod METHOD
	@name sub
*/
static int lua_vec3_sub(lua_State *L) {
	LuaVec3 v1;
	LuaVec3 v2;
	if(
		lua::to_vec_t<LuaVecTy>(L, 1, 3, &(v1.x)) && 
		lua::to_vec_t<LuaVecTy>(L, 2, 3, &(v2.x))
	) {
		LuaVec3 v3;
		LuaVec3::sub(v3, v1, v2);
		lua::push_vec_t<LuaVecTy>(L, 3, &(v3.x));
	}
	else {
		luaL_error(L, "Error: Vec3.sub invalid arguments");
	}

	return 1;
}

/*! Multiply
	Component-wise multiply of 2 vectors
	@param v1 First vector
	@param v2 Second vector
	@ret Result vector
	@LuaMethod METHOD
	@name mul
*/
static int lua_vec3_mul(lua_State *L) {
	LuaVec3 v1;
	if(lua::to_vec_t<LuaVecTy>(L, 1, 3, &(v1.x))) {
		LuaVec3 v2;
		LuaVec3 v3;

		if(lua::to_vec_t<LuaVecTy>(L, 2, 3, &(v2.x))) {
			LuaVec3::mul(v3, v1, v2);
		}
		else if(lua::is<LuaVecTy>(L, 2)) {
			LuaVecTy scale = lua::to<LuaVecTy>(L, 2);
			LuaVec3::scale(v3, v1, scale);
		}
		else {
			luaL_error(L, "Error: Vec3.mul invalid arguments");
		}
		
		lua::push_vec_t<LuaVecTy>(L, 3, &(v3.x));
	}
	else {
		luaL_error(L, "Error: Vec3.mul invalid arguments");
	}

	return 1;
}

/*! Division
	Component-wise division of 2 vectors
	@param v1 First vector
	@param v2 Second vector
	@ret Result vector
	@LuaMethod METHOD
	@name div
*/
static int lua_vec3_div(lua_State *L) {
	LuaVec3 v1;
	LuaVec3 v2;
	if(
		lua::to_vec_t<LuaVecTy>(L, 1, 3, &(v1.x)) && 
		lua::to_vec_t<LuaVecTy>(L, 2, 3, &(v2.x))
	) {
		LuaVec3 v3;
		LuaVec3::div(v3, v1, v2);
		lua::push_vec_t<LuaVecTy>(L, 3, &(v3.x));
	}
	else {
		luaL_error(L, "Error: Vec3.div invalid arguments");
	}

	return 1;
}

/*! Scale
	Scale a vector by a factor
	@param v Vector
	@param s Scale factor
	@ret Result vector
	@LuaMethod METHOD
	@name scale
*/
static int lua_vec3_scale(lua_State *L) {
	LuaVec3 v1;
	if(
		lua::to_vec_t<LuaVecTy>(L, 1, 3, &(v1.x)) && 
		lua::is<LuaVecTy>(L, 2)
	) {
		LuaVecTy scale = lua::to<LuaVecTy>(L, 2);
		LuaVec3 v2;
		LuaVec3::scale(v2, v1, scale);
		lua::push_vec_t<LuaVecTy>(L, 3, &(v2.x));
	}
	else {
		luaL_error(L, "Error: Vec3.scale invalid arguments");
	}

	return 1;
}

/*! Dot Product
	Dot product of 2 vectors
	@param v1 First vector
	@param v2 Second vector
	@ret Dot product
	@LuaMethod METHOD
	@name dot
*/
static int lua_vec3_dot(lua_State *L) {
	LuaVec3 v1;
	LuaVec3 v2;
	if(
		lua::to_vec_t<LuaVecTy>(L, 1, 3, &(v1.x)) && 
		lua::to_vec_t<LuaVecTy>(L, 2, 3, &(v2.x))
	) {
		LuaVecTy dot = LuaVec3::dot(v1, v2);
		lua::push<LuaVecTy>(L, dot);
	}
	else {
		luaL_error(L, "Error: Vec3.dot invalid arguments");
	}

	return 1;
}

/*! Squared Magnitude
	Gets the squared magnitude of a vector
	@param v Vector
	@ret Squared magnitude
	@LuaMethod METHOD
	@name mag_sqr
*/
static int lua_vec3_mag_sqr(lua_State *L) {
	LuaVec3 v1;
	if(lua::to_vec_t<LuaVecTy>(L, 1, 3, &(v1.x))) {
		LuaVecTy msq = LuaVec3::mag_sqr(v1);
		lua::push<LuaVecTy>(L, msq);
	}
	else {
		luaL_error(L, "Error: Vec3.mag_sqr invalid arguments");
	}

	return 1;
}

/*! Magnitude
	Gets the magnitude of a vector
	@param v Vector
	@ret Magnitude
	@LuaMethod METHOD
	@name mag
*/
static int lua_vec3_mag(lua_State *L) {
	LuaVec3 v1;
	if(lua::to_vec_t<LuaVecTy>(L, 1, 3, &(v1.x))) {
		LuaVecTy mag = LuaVec3::mag(v1);
		lua::push<LuaVecTy>(L, mag);
	}
	else {
		luaL_error(L, "Error: Vec3.mag invalid arguments");
	}

	return 1;
}

/*! Cross Product
	Caluclates the cross product of 2 vectors
	@param v1 First vector
	@param v2 Second vector
	@ret Result vector
	@LuaMethod METHOD
	@name cross
*/
static int lua_vec3_cross(lua_State *L) {
	LuaVec3 v1;
	LuaVec3 v2;
	if(
		lua::to_vec_t<LuaVecTy>(L, 1, 3, &(v1.x)) && 
		lua::to_vec_t<LuaVecTy>(L, 2, 3, &(v2.x))
	) {
		LuaVec3 v3;
		LuaVec3::cross(v3, v1, v2);
		lua::push_vec_t<LuaVecTy>(L, 3, &(v3.x));
	}
	else {
		luaL_error(L, "Error: Vec3.cross invalid arguments");
	}

	return 1;
}

/*! Normalize
	Normalizes a vector
	@param v Vector
	@ret Result vector
	@LuaMethod METHOD
	@name normalize
*/
static int lua_vec3_normalize(lua_State *L) {
	LuaVec3 v1;
	if(lua::to_vec_t<LuaVecTy>(L, 1, 3, &(v1.x))) {
		LuaVec3::normalize(v1);
		lua::push_vec_t<LuaVecTy>(L, 3, &(v1.x));
	}
	else {
		luaL_error(L, "Error: Vec3.normalize argments must a table");
	}

	return 1;
}

/*! Linear Interpolation
	Executes a linear interpolation between 2 vectors
	@param v1 First vector
	@param v2 Second vector
	@param t Interpolation amount [0, 1]
	@ret Result vector
	@LuaMethod METHOD
	@name lerp
*/
static int lua_vec3_lerp(lua_State *L) {
	LuaVec3 v1;
	LuaVec3 v2;
	if(
		lua::to_vec_t<LuaVecTy>(L, 1, 3, &(v1.x)) && 
		lua::to_vec_t<LuaVecTy>(L, 2, 3, &(v2.x)) && 
		lua::is<LuaVecTy>(L, 3)
	) {
		LuaVecTy amt = lua::to<LuaVecTy>(L, 3);
		LuaVec3 v3;
		LuaVec3::lerp(v3, v1, v2, amt);
		lua::push_vec_t<LuaVecTy>(L, 3, &(v3.x));
	}

	else {
		luaL_error(L, "Error: Vec3.lerp argments must be 2 tables and a float");
	}

	return 1;
}

/*! Quadtratic ease in
	Executes a quadratic ease in between 2 vectors
	@param v1 First vector
	@param v2 Second vector
	@param t Ease amount [0, 1]
	@ret Result vector
	@LuaMethod METHOD
	@name easein2
*/
static int lua_vec3_easein2(lua_State *L) {
	LuaVec3 v1;
	LuaVec3 v2;
	if(
		lua::to_vec_t<LuaVecTy>(L, 1, 3, &(v1.x)) && 
		lua::to_vec_t<LuaVecTy>(L, 2, 3, &(v2.x)) && 
		lua::is<LuaVecTy>(L, 3)
	) {
		LuaVecTy amt = lua::to<LuaVecTy>(L, 3);
		LuaVec3 v3 = LuaVec3::easein2(v1, v2, amt);
		lua::push_vec_t<LuaVecTy>(L, 3, &(v3.x));
	}

	else {
		luaL_error(L, "Error: Vec3.easein2 argments must be 2 tables and a float");
	}

	return 1;
}

/*! Quadtratic ease out
	Executes a quadratic ease out between 2 vectors
	@param v1 First vector
	@param v2 Second vector
	@param t Ease amount [0, 1]
	@ret Result vector
	@LuaMethod METHOD
	@name easeout2
*/
static int lua_vec3_easeout2(lua_State *L) {
	LuaVec3 v1;
	LuaVec3 v2;
	if(
		lua::to_vec_t<LuaVecTy>(L, 1, 3, &(v1.x)) && 
		lua::to_vec_t<LuaVecTy>(L, 2, 3, &(v2.x)) && 
		lua::is<LuaVecTy>(L, 3)
	) {
		LuaVecTy amt = lua::to<LuaVecTy>(L, 3);
		LuaVec3 v3 = LuaVec3::easeout2(v1, v2, amt);
		lua::push_vec_t<LuaVecTy>(L, 3, &(v3.x));
	}

	else {
		luaL_error(L, "Error: Vec3.easeout2 argments must be 2 tables and a float");
	}

	return 1;
}

/*! Quadtratic ease in/out
	Executes a quadratic ease in/out between 2 vectors
	@param v1 First vector
	@param v2 Second vector
	@param t Ease amount [0, 1]
	@ret Result vector
	@LuaMethod METHOD
	@name easeinout2
*/
static int lua_vec3_easeinout2(lua_State *L) {
	LuaVec3 v1;
	LuaVec3 v2;
	if(
		lua::to_vec_t<LuaVecTy>(L, 1, 3, &(v1.x)) && 
		lua::to_vec_t<LuaVecTy>(L, 2, 3, &(v2.x)) && 
		lua::is<LuaVecTy>(L, 3)
	) {
		LuaVecTy amt = lua::to<LuaVecTy>(L, 3);
		LuaVec3 v3 = LuaVec3::easeinout2(v1, v2, amt);
		lua::push_vec_t<LuaVecTy>(L, 3, &(v3.x));
	}

	else {
		luaL_error(L, "Error: Vec3.easeinout2 argments must be 2 tables and a float");
	}

	return 1;
}

/*! Casteljau Spline
	Finds a point on a Casteljau spline
	@param v1 First vector
	@param v2 Second vector
	@param v3 Third vector
	@param v4 Fourth vector
	@param t Amount along spline [0, 1]
	@ret Result vector
	@LuaMethod METHOD
	@name casteljau
*/
static int lua_vec3_casteljau(lua_State *L) {
	LuaVec3 v1;
	LuaVec3 v2;
	LuaVec3 v3;
	LuaVec3 v4;
	if(
		lua::to_vec_t<LuaVecTy>(L, 1, 3, &(v1.x)) && 
		lua::to_vec_t<LuaVecTy>(L, 2, 3, &(v2.x)) && 
		lua::to_vec_t<LuaVecTy>(L, 3, 3, &(v3.x)) && 
		lua::to_vec_t<LuaVecTy>(L, 4, 3, &(v4.x)) && 
		lua::is<LuaVecTy>(L, 5)
	) {
		LuaVecTy amt = lua::to<LuaVecTy>(L, 5);
		LuaVec3 v;
		LuaVec3::casteljau(v, v1, v2, v3, v4, amt);
		lua::push_vec_t<LuaVecTy>(L, 3, &(v.x));
	}

	else {
		luaL_error(L, "Error: Vec3.casteljau arguments must be 4 tables and a float");
	}

	return 1;
}

/*! Hermite Tangent Spline
	Finds a point on a Hermite spline specified by points and tangents.
	@param v1 First point
	@param t1 Tangent at first point
	@param v2 Second point
	@param t2 Tangent at second point
	@param t Amount along spline [0, 1]
	@ret Result vector
	@LuaMethod METHOD
	@name hermite
*/
static int lua_vec3_hermite(lua_State *L) {
	LuaVec3 v1;
	LuaVec3 v2;
	LuaVec3 v3;
	LuaVec3 v4;
	if(
		lua::to_vec_t<LuaVecTy>(L, 1, 3, &(v1.x)) && 
		lua::to_vec_t<LuaVecTy>(L, 2, 3, &(v2.x)) && 
		lua::to_vec_t<LuaVecTy>(L, 3, 3, &(v3.x)) && 
		lua::to_vec_t<LuaVecTy>(L, 4, 3, &(v4.x)) && 
		lua::is<LuaVecTy>(L, 5)
	) {
		LuaVecTy amt = lua::to<LuaVecTy>(L, 5);
		LuaVec3 v;
		LuaVec3::hermite(v, v1, v2, v3, v4, amt);
		lua::push_vec_t<LuaVecTy>(L, 3, &(v.x));
	}
	else {
		luaL_error(L, "Error: Vec3.hermite arguments must be 4 tables and a float");
	}

	return 1;
}

/*! Hermite Spline
	Finds a point on a Hermite spline specified by points.
	@param v1 First point
	@param v2 Second point
	@param v3 Third point
	@param v4 Fourth point
	@param t Amount along spline [0, 1]
	@ret Result vector
	@LuaMethod METHOD
	@name hermite2
*/
static int lua_vec3_hermite2(lua_State *L) {
	LuaVec3 v1;
	LuaVec3 v2;
	LuaVec3 v3;
	LuaVec3 v4;
	if(
		lua::to_vec_t<LuaVecTy>(L, 1, 3, &(v1.x)) && 
		lua::to_vec_t<LuaVecTy>(L, 2, 3, &(v2.x)) && 
		lua::to_vec_t<LuaVecTy>(L, 3, 3, &(v3.x)) && 
		lua::to_vec_t<LuaVecTy>(L, 4, 3, &(v4.x)) && 
		lua::is<LuaVecTy>(L, 5)
	) {
		LuaVecTy amt = lua::to<LuaVecTy>(L, 5);
		LuaVec3 v;
		LuaVec3::hermite2(v, v1, v2, v3, v4, amt);
		lua::push_vec_t<LuaVecTy>(L, 3, &(v.x));
	}
	else {
		luaL_error(L, "Error: Vec3.hermite2 arguments must be 4 tables and a float");
	}

	return 1;
}

/*! Normal
	Finds a normal to a triangle
	@param v1 First point
	@param v2 Second point
	@param v3 Third point
	@ret Result vector
	@LuaMethod METHOD
	@name normal
*/
static int lua_vec3_normal(lua_State *L) {
	LuaVec3 v1;
	LuaVec3 v2;
	LuaVec3 v3;
	if(
		lua::to_vec_t<LuaVecTy>(L, 1, 3, &(v1.x)) && 
		lua::to_vec_t<LuaVecTy>(L, 2, 3, &(v2.x)) && 
		lua::to_vec_t<LuaVecTy>(L, 3, 3, &(v3.x))
	) {
		LuaVec3 v4;
		LuaVec3::normal(v4, v1, v2, v3);
		lua::push_vec_t<LuaVecTy>(L, 3, &(v4.x));
	}
	else {
		luaL_error(L, "Error: Vec3.normal argments must be 3 tables");
	}

	return 1;
}

static int lua_vec3_centroid3(lua_State *L) {
	LuaVec3 v1;
	LuaVec3 v2;
	LuaVec3 v3;
	if(
		lua::to_vec_t<LuaVecTy>(L, 1, 3, &(v1.x)) && 
		lua::to_vec_t<LuaVecTy>(L, 2, 3, &(v2.x)) && 
		lua::to_vec_t<LuaVecTy>(L, 3, 3, &(v3.x))
	) {
		LuaVec3 v4;
		LuaVec3::centroid3(v4, v1, v2, v3);
		lua::push_vec_t<LuaVecTy>(L, 3, &(v4.x));
	}
	else {
		luaL_error(L, "Error: Vec3.centroid3 argments must be 3 tables");
	}

	return 1;
}

/*! Tangent
	Finds an arbitrary tanget in the vector's tangent plane.
	@param v Vector
	@ret Result vector
	@LuaMethod METHOD
	@name tangent
*/
static int lua_vec3_tangent(lua_State *L) {
	LuaVec3 v1;
	if(lua::to_vec_t<LuaVecTy>(L, 1, 3, &(v1.x))) {
		LuaVec3 v2 = LuaVec3::tangent(v1);
		lua::push_vec_t<LuaVecTy>(L, 3, &(v2.x));
	}
	else {
		luaL_error(L, "Error: Vec3.tangent argments must be 3 tables");
	}

	return 1;
}

/*! Rotation axis
	Finds the rotation axis between 2 vectors
	@param v1 First vector
	@param v2 Second vector
	@ret Result vector
	@LuaMethod METHOD
	@name rotaxis
*/
static int lua_vec3_rotaxis(lua_State *L) {
	LuaVec3 v1;
	LuaVec3 v2;
	if(
		lua::to_vec_t<LuaVecTy>(L, 1, 3, &(v1.x)) && 
		lua::to_vec_t<LuaVecTy>(L, 2, 3, &(v2.x))
	) {
		LuaVec3 v3 = LuaVec3::rotaxis(v1, v2);
		lua::push_vec_t<LuaVecTy>(L, 3, &(v3.x));
	}
	else {
		luaL_error(L, "Error: Vec3.rotaxis argments must be 3 tables");
	}

	return 1;
}

/*! Minimum
	Component-wise minimum between 2 vectors
	@param v1 First vector
	@param v2 Second vector
	@ret Result vector
	@LuaMethod METHOD
	@name min
*/
static int lua_vec3_min(lua_State *L) {
	LuaVec3 v1;
	LuaVec3 v2;
	if(
		lua::to_vec_t<LuaVecTy>(L, 1, 3, &(v1.x)) && 
		lua::to_vec_t<LuaVecTy>(L, 2, 3, &(v2.x))
	) {
		LuaVec3 v3 = LuaVec3::vmin(v1, v2);
		lua::push_vec_t<LuaVecTy>(L, 3, &(v3.x));
	}
	else {
		luaL_error(L, "Error: Vec3.min argments must be 2 tables");
	}
	
	return 1;
}

/*! Maximum
	Component-wise maximum between 2 vectors
	@param v1 First vector
	@param v2 Second vector
	@ret Result vector
	@LuaMethod METHOD
	@name max
*/
static int lua_vec3_max(lua_State *L) {
	LuaVec3 v1;
	LuaVec3 v2;
	if(
		lua::to_vec_t<LuaVecTy>(L, 1, 3, &(v1.x)) && 
		lua::to_vec_t<LuaVecTy>(L, 2, 3, &(v2.x))
	) {
		LuaVec3 v3 = LuaVec3::vmax(v1, v2);
		lua::push_vec_t<LuaVecTy>(L, 3, &(v3.x));
	}
	else {
		luaL_error(L, "Error: Vec3.max argments must be 2 tables");
	}
	
	return 1;
}

/*! Modulo
	Component-wise modulo of a vector
	@param v Vector
	@param m Module factor
	@ret Result vector
	@LuaMethod METHOD
	@name mod
*/
static int lua_vec3_mod(lua_State *L) {
	LuaVec3 v1;
	if(
		lua::to_vec_t<LuaVecTy>(L, 1, 3, &(v1.x)) && 
		lua::is<LuaVecTy>(L, 2)
	) {
		LuaVecTy m = lua::to<LuaVecTy>(L, 2);
		LuaVec3 v2 = LuaVec3::mod(v1, m);
		lua::push_vec_t<LuaVecTy>(L, 3, &(v2.x));
	}
	else {
		luaL_error(L, "Error: Vec3.mod argments must be 1 table and a number");
	}
	
	return 1;
}

static int lua_vec3_quantize(lua_State *L) {
	LuaVec3 v1;
	if(
		lua::to_vec_t<LuaVecTy>(L, 1, 3, &(v1.x)) && 
		lua::is<LuaVecTy>(L, 2)
	) {
		LuaVecTy m = lua::to<LuaVecTy>(L, 2);
		LuaVec3 v2 = LuaVec3::quantize(v1, m);
		lua::push_vec_t<LuaVecTy>(L, 3, &(v2.x));
	}
	else {
		luaL_error(L, "Error: Vec3.quantize argments must be 1 table and a number");
	}
	
	return 1;
}

/*! Angle
	Finds the angle between 2 vectors
	@param v1 First vector
	@param v2 Second vector
	@ret Result vector
	@LuaMethod METHOD
	@name angle
*/
static int lua_vec3_angle(lua_State *L) {
	LuaVec3 v1;
	LuaVec3 v2;
	if(
		lua::to_vec_t<LuaVecTy>(L, 1, 3, &(v1.x)) && 
		lua::to_vec_t<LuaVecTy>(L, 2, 3, &(v2.x))
	) {
		LuaVecTy a = LuaVec3::angle(v1, v2);
		lua::push<LuaVecTy>(L, a);
	}
	else {
		luaL_error(L, "Error: Vec3.angle invalid arguments");
	}

	return 1;
}

/*! Spherical Linear Interpolation
	Executes a spherical linear interpolation between 2 vectors
	@param v1 First vector
	@param v2 Second vector
	@ret Result vector
	@LuaMethod METHOD
	@name slerp
*/
static int lua_vec3_slerp(lua_State *L) {
	LuaVec3 v1;
	LuaVec3 v2;
	if(
		lua::to_vec_t<LuaVecTy>(L, 1, 3, &(v1.x)) && 
		lua::to_vec_t<LuaVecTy>(L, 2, 3, &(v2.x)) && 
		lua::is<LuaVecTy>(L, 3)
	) {
		LuaVecTy amt = lua::to<LuaVecTy>(L, 3);
		LuaVec3 v3 = LuaVec3::slerp(v1, v2, amt);
		lua::push_vec_t<LuaVecTy>(L, 3, &(v3.x));
	}
	else {
		luaL_error(L, "Error: Vec3.slerp argments must be 2 tables and a number");
	}
	
	return 1;
}

static int lua_vec3_axis_angle(lua_State *L) {
	LuaVec3 v1;
	LuaVec3 v2;
	if(
		lua::to_vec_t<LuaVecTy>(L, 1, 3, &(v1.x)) && 
		lua::to_vec_t<LuaVecTy>(L, 2, 3, &(v2.x))
	) {
		LuaVecTy angle;
		LuaVec3 axis;
		LuaVec3::axis_angle(angle, axis, v1, v2);
		
		LuaVecTy vals[] = {angle, axis.x, axis.y, axis.z};
		lua::push_vec_t<LuaVecTy>(L, 4, vals);
	}
	else {
		luaL_error(L, "Error: Vec3.axis_angle argments must be 2 tables");
	}
	
	return 2;
}

#undef LUA_METHOD
#undef TEM

}	// space::

#endif	// SPACE_VEC3_UDATA_H
