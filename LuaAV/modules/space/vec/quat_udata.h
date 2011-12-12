#ifndef SPACE_QUAT_UDATA_H
#define SPACE_QUAT_UDATA_H 1

#include "quat.h"
#include "vec3.h"
#include "lua_utility.h"

#define TEM template <typename T>

namespace space {

typedef double LuaVecTy;
typedef Vec3<LuaVecTy> LuaVec3;
typedef Quat<LuaVecTy> LuaQuat;

/*! 3D orientation and rotation operations.
	3D orientation and rotation operations.  Quat methods operate on array quadruplets: {w, x, y, z}.

	@module space.quat
*/



/*! Normalization
	Normalizes a quaternion
	@param q Quaternion
	@ret Result quaternion
	@LuaMethod METHOD
	@name normalize
*/
int lua_quat_normalize(lua_State *L) {
	LuaQuat q;
	if(lua::to_vec_t<LuaVecTy>(L, 1, 4, &(q.w))) {
		q.normalize();
		lua::push_vec_t<LuaVecTy>(L, 4, &(q.w));
	}
	else {
		luaL_error(L, "quat.normalize invalid arguments");
	}

	return 1;
}

/*! Inversion
	Inverts a quaternion
	@param q Quaternion
	@ret Result quaternion
	@LuaMethod METHOD
	@name inverse
*/
int lua_quat_inverse(lua_State *L) {
	LuaQuat q;
	if(lua::to_vec_t<LuaVecTy>(L, 1, 4, &(q.w))) {
		LuaQuat qi;
		q.inverse(qi);
		lua::push_vec_t<LuaVecTy>(L, 4, &(qi.w));
	}
	else {
		luaL_error(L, "quat.inverse invalid arguments");
	}

	return 1;
}

/*! Multiplication
	Multiplies 2 quaternions
	@param q1 First quaternion
	@param q2 Second quaternion
	@ret result quaternion
	@LuaMethod METHOD
	@name mul
*/
int lua_quat_mul(lua_State *L) {
	LuaQuat q1;
	LuaQuat q2;
	if(
		lua::to_vec_t<LuaVecTy>(L, 1, 4, &(q1.w)) && 
		lua::to_vec_t<LuaVecTy>(L, 2, 4, &(q2.w))
	) {
		q1.mul(q2);
		lua::push_vec_t<LuaVecTy>(L, 4, &(q1.w));
	}
	else {
		luaL_error(L, "quat.mul invalid arguments");
	}

	return 1;
}

int lua_quat_revmul(lua_State *L) {
	LuaQuat q1;
	LuaQuat q2;
	if(
		lua::to_vec_t<LuaVecTy>(L, 1, 4, &(q1.w)) && 
		lua::to_vec_t<LuaVecTy>(L, 2, 4, &(q2.w))
	) {
		q1.revMul(q2);
		lua::push_vec_t<LuaVecTy>(L, 4, &(q1.w));
	}
	else {
		luaL_error(L, "quat.revmul invalid arguments");
	}

	return 1;
}

/*! From Axis-Angle
	Derives a quaternion from axis-angle format
	@param theta Angle of rotation
	@param x x-component of axis
	@param y y-component of axis
	@param z z-component of axis
	@ret Result quaternion
	@LuaMethod METHOD
	@name from_axis_angle
*/
int lua_quat_from_axis_angle(lua_State *L) {
	LuaVecTy aa[] = {0, 0, 0, 0};
	if(lua::to_vec<LuaVecTy>(L, 1, 4, aa)) {
		LuaQuat q;
		q.fromAxisAngle(aa[0], aa[1], aa[2], aa[3]);
		lua::push_vec_t<LuaVecTy>(L, 4, &(q.w));
	}
	else {
		luaL_error(L, "quat.from_axis_angle invalid arguments");
	}

	return 1;
}

int lua_quat_from_euler(lua_State *L) {
	LuaVecTy ea[] = {0, 0, 0};
	if(lua::to_vec<LuaVecTy>(L, 1, 3, ea)) {
		LuaQuat q;
		q.fromEuler(ea[0], ea[1], ea[2]);
		lua::push_vec_t<LuaVecTy>(L, 4, &(q.w));
	}
	else {
		luaL_error(L, "quat.fromEuler invalid arguments");
	}

	return 1;
}


/*! To Axis-Angle
	Derives the axis-angle format from a quaternion
	@param q Quaternion
	@ret theta Angle of rotation
	@ret x-component of axis
	@ret y-component of axis
	@ret z-component of axis
	@LuaMethod METHOD
	@name to_axis_angle
*/
int lua_quat_to_axis_angle(lua_State *L) {
	LuaQuat q;
	if(lua::to_vec_t<LuaVecTy>(L, 1, 4, &(q.w))) {
		LuaVecTy aa[4];
		q.toAxisAngle(aa[0], aa[1], aa[2], aa[3]);
		lua::push_vec_t<LuaVecTy>(L, 4, aa);
	}
	else {
		luaL_error(L, "quat.to_axis_angle invalid arguments");
	}

	return 1;
}

int lua_quat_to_euler(lua_State *L) {
	LuaQuat q;
	if(lua::to_vec_t<LuaVecTy>(L, 1, 4, &(q.w))) {
		LuaVecTy ea[3];
		q.toEuler(ea[0], ea[1], ea[2]);
		lua::push_vec_t<LuaVecTy>(L, 3, ea);
	}
	else {
		luaL_error(L, "quat.to_euler invalid arguments");
	}

	return 1;
}

/*! To x-vector
	Derives the x-axis of the orientation described
	by the quaternion
	@param q Quaternion
	@ret x-axis vector
	@LuaMethod METHOD
	@name to_vector_x
*/
int lua_quat_to_vector_x(lua_State *L) {
	LuaQuat q;
	if(lua::to_vec_t<LuaVecTy>(L, 1, 4, &(q.w))) {
		LuaVec3 v;
		q.toVectorX(v);
		lua::push_vec_t<LuaVecTy>(L, 3, &(v.x));
	}
	else {
		luaL_error(L, "quat.to_vector_x invalid arguments");
	}

	return 1;
}

/*! To y-vector
	Derives the y-axis of the orientation described
	by the quaternion
	@param q Quaternion
	@ret y-axis vector
	@LuaMethod METHOD
	@name to_vector_y
*/
int lua_quat_to_vector_y(lua_State *L) {
	LuaQuat q;
	if(lua::to_vec_t<LuaVecTy>(L, 1, 4, &(q.w))) {
		LuaVec3 v;
		q.toVectorY(v);
		lua::push_vec_t<LuaVecTy>(L, 3, &(v.x));
	}
	else {
		luaL_error(L, "quat.to_vector_y invalid arguments");
	}

	return 1;
}

/*! To z-vector
	Derives the z-axis of the orientation described
	by the quaternion
	@param q Quaternion
	@ret z-axis vector
	@LuaMethod METHOD
	@name to_vector_z
*/
int lua_quat_to_vector_z(lua_State *L) {
	LuaQuat q;
	if(lua::to_vec_t<LuaVecTy>(L, 1, 4, &(q.w))) {
		LuaVec3 v;
		q.toVectorZ(v);
		lua::push_vec_t<LuaVecTy>(L, 3, &(v.x));
	}
	else {
		luaL_error(L, "quat.to_vector_z invalid arguments");
	}

	return 1;
}

int lua_quat_rotate_vector(lua_State *L) {
	LuaQuat q;
	LuaVec3 v;
	if(
		lua::to_vec_t<LuaVecTy>(L, 1, 4, &(q.w)) && 
		lua::to_vec_t<LuaVecTy>(L, 2, 3, &(v.x))
	) {
		q.rotateVector(v);
		lua::push_vec_t<LuaVecTy>(L, 3, &(v.x));
	}
	else {
		luaL_error(L, "quat.rotate_vector invalid arguments");
	}

	return 1;
}

/*! Spherical Linear Interpolation
	Executes spherical linear interpolation on 2 quaternions
	by the quaternion
	@param q1 First quaternion (t=0)
	@param q2 Second quaternion (t=1)
	@param t Interpolation factor [0, 1]
	@ret Interpolated quaternion
	@LuaMethod METHOD
	@name slerp
*/
int lua_quat_slerp(lua_State *L) {
	LuaQuat q1;
	LuaQuat q2;
	if(
		lua::to_vec_t<LuaVecTy>(L, 1, 4, &(q1.w)) && 
		lua::to_vec_t<LuaVecTy>(L, 2, 4, &(q2.w)) &&
		lua::is<LuaVecTy>(L, 3)
	) {
		LuaVecTy amt = lua::to<LuaVecTy>(L, 3);
		q1.slerp(q2, amt);
		lua::push_vec_t<LuaVecTy>(L, 4, &(q1.w));
	}
	else {
		luaL_error(L, "quat.slerp invalid arguments");
	}

	return 1;
}

int lua_quat_toward_point(lua_State *L) {
	LuaVec3 v1;
	LuaQuat q1;
	LuaVec3 v2;
	if(
		lua::to_vec_t<LuaVecTy>(L, 1, 3, &(v1.x)) && 
		lua::to_vec_t<LuaVecTy>(L, 2, 4, &(q1.w)) &&
		lua::to_vec_t<LuaVecTy>(L, 3, 3, &(v2.x))
	) {
		LuaVecTy amt = lua::opt<LuaVecTy>(L, 4, 1.);
		LuaQuat qt;
		qt.toward_point(v1, q1, v2, amt);
		lua::push_vec_t<LuaVecTy>(L, 4, &(qt.w));
	}
	else {
		luaL_error(L, "quat.toward_point invalid arguments");
	}

	return 1;
}

int lua_quat_rotor(lua_State *L) {
	LuaVec3 v1;
	LuaVec3 v2;
	if(
		lua::to_vec_t<LuaVecTy>(L, 1, 3, &(v1.x)) && 
		lua::to_vec_t<LuaVecTy>(L, 2, 3, &(v2.x))
	) {
		if(lua::is<float>(L, 3)) {
			float amt = lua::to<float>(L, 3);
			LuaQuat q = LuaQuat::rotor(v1, v2, amt);
			lua::push_vec_t<LuaVecTy>(L, 4, &(q.w));
		}
		else {
			LuaQuat q = LuaQuat::rotor(v1, v2);
			lua::push_vec_t<LuaVecTy>(L, 4, &(q.w));
		}
	}
	else {
		luaL_error(L, "quat.rotor invalid arguments");
	}

	return 1;
}

}	// space::

#endif	// SPACE_QUAT_UDATA_H
