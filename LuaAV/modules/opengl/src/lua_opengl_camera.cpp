#include "lua_opengl_camera.h"
#include "lua_glue.h"
#include "lua_utility.h"
#include <stdio.h>
#include "protocol/al_GraphicsBackend.hpp"

#define Deg2Rad	0.017453292519943

Camera :: Camera()
:	mPos(0, 0, 4),
	mLookAbs(0, 1, 0),
	mPosMaster(0, 0, 4),
	mPosTarget(0, 0, 4),
	dirMotion(0, 0, 0),
	dirMotionTarget(0, 0, 0),
	
	mLensAngle(45.),
	mAspect(1.0),
	mNear(0.2),
	mFar(200),
	
	animStep(0),
	animStepIncr(0.005),
	
	mEyeSep(0.1),
	mFocalLength(6),
	mStereo(false),
	mSequential(false),
	mOrtho(false)
{
	mQ.fromAxisAngle(180, 0, 1, 0);
	update();
}

Camera :: ~Camera() {
	// nothing for now
}


void Camera::getRotMatrix(double * m){
	mQ.toMatrix(m);
}


void Camera :: update() {
	mQ.toVectorX(mX);
	mQ.toVectorY(mY);
	mQ.toVectorZ(mZ);
	
	Vec3 zscaled = mZ * mFocalLength;
	
	Vec3::add(mLookat, mPos, zscaled);
	//printf("mLookat direction of cam: %f, %f, %f \n", mLookat.x, mLookat.y, mLookat.z); 
	//mLookat.set(mZ);
	
	if(mStereo) {
		beginStereo();
	}
	
	if(animStep >= 1) {
		if(keys.empty()) {
			animStep = 0;
		}
		else {
			animationIterate();
		}

		mPosMaster = mPos;
	}

	if(animStep > 0) {
		animationStep();
	}
}

void Camera :: animationStep() {
	Vec3::hermite(mPos, mPosMaster, dirMotionMaster, mPosTarget, dirMotionTarget, animStep);
	mQ = mQMaster;
	mQ.slerp(mQTarget, animStep);
	animStep += animStepIncr;
}

void Camera :: animationIterate() {
	animStep = animStepIncr;
	Key first = keys.top();
	Vec3 firstPos = first.pos;
	Vec3 firstLook = first.look;

	keys.pop();

	if(keys.empty()) {
		// last point has reached
		Vec3 zero;
		moveTo(firstPos, zero, firstLook);
	}
	else {
		Key second = keys.top();
		Vec3 t2 = second.pos;
		t2 = t2 - mPos;
		t2*=0.5;
		moveTo(firstPos, t2, firstLook);
	}
	
}
void Camera :: moveTo(Vec3 newPos, Vec3 newDir, Vec3 newLook) {
	// start animation
	animStep = animStepIncr;
	
	// hold the current position as master
	mPosMaster = mPos;
    mPosTarget = newPos;

	dirMotionMaster = dirMotionTarget;
	dirMotionTarget = newDir;
	
	Vec3 zr;
	
	mQMaster = mQ;
	mQTarget.toward_point(zr, mQ, newLook, 1.);
}

void Camera :: animate(int numKeys, Vec3 * keyPos, Vec3 * keyLook) {
	while(!keys.empty()){
		keys.pop();
	}
	
	for(int i = (numKeys-1); i>=0; i--){
		// push the first point on top of the stack
		Key keyPoint(keyPos[i], keyLook[i]);
		keys.push(keyPoint);
	}
	delete [] keyPos;
	delete [] keyLook;
	
	// call first moveTo function, assuming camera's moveDir is (0, 0, 0)
	Key first = keys.top();
	Vec3 firstPos = first.pos;
	Vec3 firstLook = first.look;

	keys.pop();

	Key second = keys.top();
	Vec3 t2 = second.pos;
	t2 = t2 - mPos;
	t2*=0.5;

	dirMotionTarget.set(0, 0, 0); // set t1 to 0, 0, 0
	moveTo(firstPos, t2, firstLook);
}

void Camera :: animate(int numKeys, Vec3 * keyPos) {
	for(int i = (numKeys-1); i>=0; i--){
		// push the first point on top of the stack
		Vec3 look = mLookAbs - keyPos[i];
		Vec3::normalize(look);
		Key keyPoint(keyPos[i], look);
		keys.push(keyPoint);
	}

	delete [] keyPos;
		
	// call first moveTo function, assuming camera's moveDir is (0, 0, 0)
	Key first = keys.top();
	Vec3 firstPos = first.pos;
	Vec3 firstLook = first.look;

	keys.pop();

	Key second = keys.top();
	Vec3 t2 = second.pos;
	t2 = t2 - mPos;
	t2*=0.5;

	dirMotionTarget.set(0, 0, 0); // set t1 to 0, 0, 0
	moveTo(firstPos, t2, firstLook);
}

void Camera :: beginStereo() {
	mTop = mNear*tan(Deg2Rad*mLensAngle*0.5);
	mRight = mAspect*mTop;
	mShift = mEyeSep*0.5*mNear/mFocalLength;
	mEyeShift = mX*(mEyeSep*0.5);
}

void Camera :: enter(Eye eye) {
	if(mStereo) {
		if(eye == LEFT_EYE) { enterLeft(); }
		else				{ enterRight(); }
	}
	else { enterMid(); }
}

void Camera :: enterMid() {
//	glDrawBuffer(GL_BACK);
	
	glMatrixMode(GL_PROJECTION);
	glPushMatrix();
		glLoadIdentity();
		if(mOrtho) {
			glOrtho(-mAspect, mAspect, -1., 1., -100., 100.);
		}
		else {
			gluPerspective(mLensAngle, mAspect, mNear, mFar);
		}
	
		//printf("lookAt X: %f \n", mLookat.x);
		//printf("pos X: %f \n", mPos.x);
	glMatrixMode(GL_MODELVIEW);
	glPushMatrix();
		glLoadIdentity();
		gluLookAt(	mPos.x, mPos.y, mPos.z,
					mLookat.x, mLookat.y, mLookat.z,
					mY.x, mY.y, mY.z);	// can just use the y vector for up

	
	al::gfx::GraphicsBackendOpenGL::gl_error("Entering mono camera view");
}


void Camera :: enterLeft() {
	if(mSequential) {
		glDrawBuffer(GL_BACK);
	}
	else {
		glDrawBuffer(GL_BACK_LEFT);
	}

	glMatrixMode(GL_PROJECTION);
	glPushMatrix();
		glLoadIdentity();
		glFrustum(-mRight+mShift, mRight+mShift, -mTop, mTop, mNear, mFar);

	glMatrixMode(GL_MODELVIEW);
	glPushMatrix();
		glLoadIdentity();
		gluLookAt(	mPos.x-mEyeShift.x, mPos.y-mEyeShift.y, mPos.z-mEyeShift.z,
					mLookat.x, mLookat.y, mLookat.z,
					mY.x, mY.y, mY.z);	// can just use the y vector for up
}

void Camera :: enterRight() {
	if(mSequential) {
		glDrawBuffer(GL_BACK);
	}
	else {
		glDrawBuffer(GL_BACK_RIGHT);
	}

	glMatrixMode(GL_PROJECTION);
	glPushMatrix();
		glLoadIdentity();
		glFrustum(-mRight-mShift, mRight-mShift, -mTop, mTop, mNear, mFar);

	glMatrixMode(GL_MODELVIEW);
	glPushMatrix();
		glLoadIdentity();
		gluLookAt(	mPos.x+mEyeShift.x, mPos.y+mEyeShift.y, mPos.z+mEyeShift.z,
					mLookat.x, mLookat.y, mLookat.z,
					mY.x, mY.y, mY.z);	// can just use the y vector for up
}

void Camera :: leave() {
	glMatrixMode(GL_PROJECTION);
	glPopMatrix();

	glMatrixMode(GL_MODELVIEW);
	glPopMatrix();
}

void Camera :: getEye(Vec3 &v) {
	update();
	v = mPos;
}

void Camera :: getLook(Vec3 &v) {
	update();
	v = mLookat;
}

void Camera :: getUp(Vec3 &v) {
	update();
	v = mY;
}

void Camera :: getQuat(Quat &q) {
	update();
	q = mQ;
}

void Camera :: setQuat(Quat &q) {
	mQ = q;
	update();
}

void Camera :: eye(Vec3 &v) {
	mPos = v;
	update();
}

void Camera :: look(Vec3 &v) {
	Vec3 vv = mPos+v;
	mQ.toward_point(mPos, mQ, vv, 1.);
	update();
}

void Camera :: lookat(Vec3 &v) {
	mQ.set();
	mQ.toward_point(mPos, mQ, v, 1.);
	update();
}

void Camera :: lookAbs(Vec3 &v) {
	mLookAbs = v;
}

void Camera :: lateral(Vec3 &v) {
	update();
	mQ.toVectorX(v);
}

float Camera :: width(float dlook) {
	return mAspect*height(dlook);
}

float Camera :: height(float dlook) {
	return tan(mLensAngle*0.5*Deg2Rad)*dlook*2.;
}

void Camera :: dolly(float amt) {
	mPos += mZ*amt;
}

void Camera :: pedd(float amt) {
	mPos += mY*amt;
}

void Camera :: track(float amt) {
	mPos += mX*amt;
}

// quat = cos(theta/2) + sin(theta/2)*axis
// quat = cos(amt/2) + sin(amt/2)(1, 0, 0)
void Camera :: pitch(float amt) {
	double rad = Deg2Rad*amt;

	// rotate about the camera's current X-axis
	Vec3 v;
	mQ.toVectorX(v);
	
	v *= sin(rad*0.5);

	Quat rot_q(cos(rad*0.5), v.x, v.y, v.z);
	mQ.revMul(rot_q);
	
	update();
}

// question of whether to rotate about the camera's Y axis
// or the absolute Y axis.  In the former case, the camera 
// can easily get off the vertical axis.  The latter case is more like 
// how people rotate around the Y axis.
void Camera :: yaw(float amt) {
	double rad = Deg2Rad*amt;

	// rotate about the absolute Y-axis
	Vec3 v(0, 1, 0);
    mQ.toVectorY(v);
	v *= sin(rad*0.5);

	Quat rot_q(cos(rad*0.5), v.x, v.y, v.z);
	mQ.revMul(rot_q);
	
	update();
}

void Camera :: roll(float amt) {
	double rad = Deg2Rad*amt;

	// rotate about the camera's current Z-axis
	Vec3 v;
	mQ.toVectorZ(v);
	v *= sin(rad*0.5);

	Quat rot_q(cos(rad*0.5), v.x, v.y, v.z);
	mQ.revMul(rot_q);
	
	update();
}


void Camera :: rotateTowardPoint(Vec3 point) {
	animStep = animStepIncr;
	Vec3 zr;
	mQMaster = mQ;
	mQTarget.toward_point(zr, mQ, point, 1.);
	mPosTarget = mPos;

}

void Camera :: view(Mat4 &v) {
	Vec3 eye, look, up;
	getEye(eye);
	getLook(look);
	getUp(up);
	v.lookat(eye, look, up);
	
	eye *= -1.;
	v.translate(eye);
}

void Camera::getRot(double &xang, double &yang, double &zang){
	mQ.toEuler(xang, yang, zang);
}

void Camera :: getProjection(double w, double h, Vec3 * corners) {
	// get the un-projected values at the corners of the viewport:
	GLint viewport[4]; // Where The Viewport Values Will Be Stored
	GLdouble modelview[16]; // Where The 16 Doubles Of The Modelview Matrix Are To Be Stored
	GLdouble projection[16]; // Where The 16 Doubles Of The Projection Matrix Are To Be Stored
	glGetIntegerv(GL_VIEWPORT, viewport);
	glGetDoublev(GL_MODELVIEW_MATRIX, modelview); // Retrieve The Modelview Matrix
	glGetDoublev(GL_PROJECTION_MATRIX, projection); // Retrieve The Projection Matrix

	gluUnProject( 0, 0, mNear, modelview, projection, viewport, &corners[0].x, &corners[0].y, &corners[0].z); //NBL
	gluUnProject( w, 0, mNear, modelview, projection, viewport, &corners[1].x, &corners[1].y, &corners[1].z); //NBR
	gluUnProject( w, h, mNear, modelview, projection, viewport, &corners[2].x, &corners[2].y, &corners[2].z); //NTR
	gluUnProject( 0, h, mNear, modelview, projection, viewport, &corners[3].x, &corners[3].y, &corners[3].z); //NTL

}

/*! Camera for in a 3D environment
	Camera exposes a number of settings that determine the viewing transformation.  On each frame, 
	further internal paramers need to be calculated before actually entering the viewing transformation.
	
	Typical usage:
	<pre>
	cam:update()
	cam:enter()
		-- draw scene
	cam:leave()
	</pre>
	
	@module opengl.Camera
*/

/*! Update the internal camera parameters
	Update should be called once per-frame before calling enter.  Update 
	takes the current settings and calculates internal parameters required 
	to build the viewing transformation.
	
	@name M:update
*/
int lua_camera_update(lua_State *L) {
	Camera *s = Glue<Camera>::checkto(L, 1);
	s->update();
	return 0;
}

/*! Enter the camera viewing transformation 
	@name M:enter
*/
int lua_camera_enter(lua_State *L) {
	Camera *s = Glue<Camera>::checkto(L, 1);
	s->enter(luaL_optinteger(L, 2, 0) ? Camera::LEFT_EYE : Camera::RIGHT_EYE);
	return 0;
}

/*! Leave the current viewing transformation
	@name M:leave
*/
int lua_camera_leave(lua_State *L) {
	Camera *s = Glue<Camera>::checkto(L, 1);
	s->leave();
	return 0;
}

/*! The lens angle
	@LuaMethod GETSET
	@name M.lens_angle
*/
int lua_camera_lens_angle(lua_State *L) {
	Camera *s = Glue<Camera>::checkto(L, 1);
	if(lua::is<float>(L, 2)) {
		s->setLensAngle(lua::to<float>(L, 2));
		return 0;
	}
	else {
		lua::push<float>(L, s->getLensAngle());
		return 1;
	}
	return 0;
}

/*! The aspect ratio
	@LuaMethod GETSET
	@name M.aspect
*/
int lua_camera_aspect(lua_State *L) {
	Camera *s = Glue<Camera>::checkto(L, 1);
	if(lua::is<float>(L, 2)) {
		s->setAspect(lua::to<float>(L, 2));
		return 0;
	}
	else {
		lua::push<float>(L, s->getAspect());
		return 1;
	}
	return 0;
}

/*! The near clipping plane distance
	@LuaMethod GETSET
	@name M.near
*/
int lua_camera_near(lua_State *L) {
	Camera *s = Glue<Camera>::checkto(L, 1);
	if(lua::is<float>(L, 2)) {
		s->setNear(lua::to<float>(L, 2));
		return 0;
	}
	else {
		lua::push<float>(L, s->getNear());
		return 1;
	}
	return 0;
}

/*! The far clipping plane distance
	@LuaMethod GETSET
	@name M.far
*/
int lua_camera_far(lua_State *L) {
	Camera *s = Glue<Camera>::checkto(L, 1);
	if(lua::is<float>(L, 2)) {
		s->setFar(lua::to<float>(L, 2));
		return 0;
	}
	else {
		lua::push<float>(L, s->getFar());
		return 1;
	}
	return 0;
}

int lua_camera_eye_sep(lua_State *L) {
	Camera *s = Glue<Camera>::checkto(L, 1);
	if(lua::is<float>(L, 2)) {
		s->setEyeSep(lua::to<float>(L, 2));
		return 0;
	}
	else {
		lua::push<float>(L, s->getEyeSep());
		return 1;
	}
	return 0;
}
int lua_camera_stereo(lua_State *L) {
	Camera *s = Glue<Camera>::checkto(L, 1);
	if(lua::is<bool>(L, 2)) {
		s->setStereo(lua::to<bool>(L, 2));
		return 0;
	}
	else {
		lua::push<bool>(L, s->getEyeSep());
		return 1;
	}
	return 0;
}


int lua_camera_focal_length(lua_State *L) {
	Camera *s = Glue<Camera>::checkto(L, 1);
	if(lua::is<float>(L, 2)) {
		s->setFocalLength(lua::to<float>(L, 2));
		return 0;
	}
	else {
		lua::push<float>(L, s->getFocalLength());
		return 1;
	}
	return 0;
}

/*! The eye position
	@LuaMethod GETSET
	@name M.eye
*/
int lua_camera_eye(lua_State *L) {
	Camera *s = Glue<Camera>::checkto(L, 1);
	Camera::Vec3 v(0, 0, 0);
	if(lua::to_vec_t<double>(L, 2, 3, &v.x)) {
		s->eye(v);
		return 0;
	}
	else {
		s->getEye(v);
		lua::push_vec_t<double>(L, 3, &v.x);
		return 1;
	}
	return 0;
}

/*! The look vector
	@LuaMethod GETSET
	@name M.look
*/
int lua_camera_look(lua_State *L) {
	Camera *s = Glue<Camera>::checkto(L, 1);
	Camera::Vec3 v(0, 0, 0);
	if(lua::to_vec_t<double>(L, 2, 3, &v.x)) {
		s->look(v);
		return 0;
	}
	else {
		s->getLook(v);
		lua::push_vec_t<double>(L, 3, &v.x);
		return 1;
	}
	return 0;
}

/*! The look at position
	@LuaMethod SET
	@name M.lookat
*/
int lua_camera_lookat(lua_State *L) {
	Camera *s = Glue<Camera>::checkto(L, 1);
	Camera::Vec3 v(0, 0, 0);
	if(lua::to_vec_t<double>(L, 2, 3, &v.x)) {
		s->lookat(v);
	}
	return 0;
}

/*! The up vector
	@LuaMethod GET
	@name M.up
*/
int lua_camera_up(lua_State *L) {
	Camera *s = Glue<Camera>::checkto(L, 1);
	Camera::Vec3 v(0, 0, 0);
	s->getUp(v);
	lua::push_vec_t<double>(L, 3, &v.x);
	return 1;
}

/*! Rotate around the z-axis
	@param angle Angle in degress
	@name M:roll
*/
int lua_camera_roll(lua_State *L) {
	Camera *s = Glue<Camera>::checkto(L, 1);
	if(lua::is<float>(L, 2)) {
		s->roll(lua::to<float>(L, 2));
	}
	return 0;
}

/*! Rotate around the x-axis
	@param angle Angle in degress
	@name M:pitch
*/
int lua_camera_pitch(lua_State *L) {
	Camera *s = Glue<Camera>::checkto(L, 1);
	if(lua::is<float>(L, 2)) {
		s->pitch(lua::to<float>(L, 2));
	}
	return 0;
}

/*! Rotate around the y-axis
	@param angle Angle in degress
	@name M:yaw
*/
int lua_camera_yaw(lua_State *L) {
	Camera *s = Glue<Camera>::checkto(L, 1);
	if(lua::is<float>(L, 2)) {
		s->yaw(lua::to<float>(L, 2));
	}
	return 0;
}

/*! Move in the direction of the local x-axis
	@param amt Amount to move
	@name M:movex
*/
int lua_camera_movex(lua_State *L) {
	Camera *s = Glue<Camera>::checkto(L, 1);
	if(lua::is<float>(L, 2)) {
		s->track(lua::to<float>(L, 2));
	}
	return 0;
}

/*! Move in the direction of the local y-axis
	@param amt Amount to move
	@name M:movey
*/
int lua_camera_movey(lua_State *L) {
	Camera *s = Glue<Camera>::checkto(L, 1);
	if(lua::is<float>(L, 2)) {
		s->pedd(lua::to<float>(L, 2));
	}
	return 0;
}

/*! Move in the direction of the local z-axis
	@param amt Amount to move
	@name M:movez
*/
int lua_camera_movez(lua_State *L) {
	Camera *s = Glue<Camera>::checkto(L, 1);
	if(lua::is<float>(L, 2)) {
		s->dolly(lua::to<float>(L, 2));
	}
	return 0;
}

template<> const char * Glue<Camera>::usr_name() { return "Camera"; }
template<> bool Glue<Camera>::usr_has_index() { return true; }

template<> void Glue<Camera>::usr_push(lua_State * L, Camera * u) {
	Glue<Camera>::usr_attr_push(L);
	Glue<Camera>::usr_attr_prototype(L);
}

template<> Camera * Glue<Camera>::usr_new(lua_State * L) {
	Camera *cam = new Camera();
	return cam;
}

template<> void Glue<Camera>::usr_index(lua_State * L, Camera * u) {
	Glue<Camera>::usr_attr_index(L);
}

template<> void Glue<Camera>::usr_newindex(lua_State * L, Camera * u) {
	Glue<Camera>::usr_attr_newindex(L);
}

template<> int Glue<Camera>::usr_tostring(lua_State * L, Camera * u) {
	lua_pushfstring(L, "%s: %p", Glue<Camera>::usr_name(), u); 
	return 1;
}

template<> void Glue<Camera>::usr_gc(lua_State * L, Camera * u) { 
	delete u;
}

template<> void Glue<Camera>::usr_mt(lua_State * L) {
	static luaL_reg methods[] = {
		{ "update", lua_camera_update },
		{ "enter", lua_camera_enter },
		{ "leave", lua_camera_leave },
		{ "roll", lua_camera_roll },
		{ "pitch", lua_camera_pitch },
		{ "yaw", lua_camera_yaw },
		{ "movex", lua_camera_movex },
		{ "movey", lua_camera_movey },
		{ "movez", lua_camera_movez },
		{ NULL, NULL}
	};
	
//	LUA_METHOD(lateral, GET),
//	LUA_METHOD(quat, GETSET),
	
	
	static luaL_reg getters[] = {
		{ "lens_angle", lua_camera_lens_angle },
		{ "aspect", lua_camera_aspect },
		{ "near", lua_camera_near },
		{ "far", lua_camera_far },
		{ "eye_sep", lua_camera_eye_sep },
		{ "focal_length", lua_camera_focal_length },
		{ "eye", lua_camera_eye},
		{ "look", lua_camera_look },
		{ "up", lua_camera_up },
		{ NULL, NULL}
	};
	static luaL_reg setters[] = {
		{ "lens_angle", lua_camera_lens_angle },
		{ "aspect", lua_camera_aspect },
		{ "near", lua_camera_near },
		{ "far", lua_camera_far },
		{ "eye_sep", lua_camera_eye_sep },
		{ "stereo", lua_camera_stereo },
		{ "focal_length", lua_camera_focal_length },
		{ "eye", lua_camera_eye},
		{ "look", lua_camera_look },
		{ "lookat", lua_camera_lookat },
		{ NULL, NULL}
	};
	Glue<Camera>::usr_attr_mt(L, methods, getters, setters);
}

#ifdef __cplusplus
extern "C" {
#endif

int luaopen_opengl_Camera(lua_State *L) {
	Glue<Camera>::define(L);
	Glue<Camera>::register_class(L);
	luaL_getmetatable(L, Glue<Camera>::mt_name(L));

	return 1;
}

#ifdef __cplusplus
}
#endif
