#ifndef LUA_OPENGL_CAMERA_H
#define LUA_OPENGL_CAMERA_H 1

#include "lua_opengl.h"
#include <math.h>
#include <stack>
#include "vec3.h"
#include "quat.h"
#include "mat4.h"


/*!
	\class Camera

	A virtual camera representation for mono and stereo rendering.

	Stereo pairs create a "virtual" three dimensional image, binocular disparity and
	convergence cues are correct but accommodation cues are inconsistent because each
	eye is looking at a flat image. The visual system will tolerate this conflicting
	accommodation to a certain extent, the classical measure is normally quoted as a
	maximum separation on the display of 1/30 of the distance of the viewer to the
	display.

	The degree of the stereo effect depends on both the distance of the camera to the
	projection plane and the separation of the left and right camera. Too large a
	separation can be hard to resolve and is known as hyperstereo. A good ballpark
	separation of the cameras is 1/20 of the distance to the projection plane, this
	is generally the maximum separation for comfortable viewing. Another constraint
	in general practice is to ensure the negative parallax (projection plane behind
	the object) does not exceed the eye separation.

	A common measure is the parallax angle defined as thea = 2 atan(DX / 2d) where DX
	is the horizontal separation of a projected point between the two eyes and d is
	the distance of the eye from the projection plane. For easy fusing by the majority
	of people, the absolute value of theta should not exceed 1.5 degrees for all points
	in the scene. Note theta is positive for points behind the scene and negative for
	points in front of the screen. It is not uncommon to restrict the negative value of
	theta to some value closer to zero since negative parallax is more difficult to fuse
	especially when objects cut the boundary of the projection plane.

		- Paul Bourke
		http://local.wasp.uwa.edu.au/~pbourke/projection/stereorender/
		
		
	cam.update();
	for(int i=0; i < cam.eyes(); i++){
		cam.enter(i);
			// draw
		cam.leave();
	}
*/
/*
using space::Quat;
using space::Vec3;
using space::Mat4;
*/

class Camera {
public:
	enum Eye {
		RIGHT_EYE = 0,
		LEFT_EYE,
	};

	typedef space::Vec3<double>	Vec3;
	typedef space::Mat4<double>	Mat4;

	struct Key  {
		Vec3 look;
		Vec3 pos;
		
		Key() {}

		Key(Vec3 ps, Vec3 lk) {
			pos.set(ps);
			look.set(lk);
		}
	};

	
	typedef space::Quat<double>		Quat;
	typedef std::stack<Key>     Stack;

public:
	Camera();
	~Camera();

	void update();
	void enter(Eye eye = RIGHT_EYE);
	void leave();

	void setLensAngle(float v) {mLensAngle = v;}
	float getLensAngle() {return mLensAngle;}

	void setAspect(float v) {mAspect = v;}
	float getAspect() {return mAspect;}

	//for some reason windows doesn't like near/far as function names
	void setNear(float v) {mNear = v;}
	float getNear() {return mNear;}

	void setFar(float v) {mFar = v;}
	float getFar() {return mFar;}

	void setEyeSep(float v) {mEyeSep = v;}
	float getEyeSep() {return mEyeSep;}

	void setFocalLength(float v) {mFocalLength = v;}
	float getFocalLength() {return mFocalLength;}

	void setStereo(bool enable) {mStereo = enable;}
	bool getStereo() {return mStereo;}

	void setSequential(bool enable) {mSequential = enable;}
	bool getSequential() {return mSequential;}
	
	void setOrtho(bool enable) {mOrtho = enable;}
	bool getOrtho() {return mOrtho;}

	void eye(Vec3 &v);
	void getEye(Vec3 &v);
	
	void look(Vec3 &v);		// vector input
	void getLook(Vec3 &v);
	
	void getUp(Vec3 &v);
	
	void getQuat(Quat &q);
	void setQuat(Quat &q);
	
	void getRot(double &xang, double &yang, double &zang);
	
	void lookat(Vec3 &v);	// world position input
	void lookAbs(Vec3 &v);
	void lateral(Vec3 &v);

	float width(float dlook);
	float height(float dlook);

	void dolly(float amt);		//in/out movement
	void pedd(float amt);		//pedestal movement (vertical)
	void track(float amt);		//track movement (horizontal)

	void pitch(float amt);		//around X-axis, center at camPos
	void yaw(float amt);		//around Y-axis, center at camPos
	void roll(float amt);		//around Z-axis, center at camPos

	void rotateTowardPoint(Vec3 point);	//around arbitrary axis

	void view(Mat4 &v);
	void getRotMatrix(double * m);

	void moveTo(Vec3 newPos, Vec3 newDir, Vec3 newLook);
	void animate(int numKeys, Vec3 * keyPos, Vec3 * keyLook); //keyLook directions determine the look direction relative to camPos
	void animate(int numKeys, Vec3 * keyPos); //always looks at the same point in world coordinates
	void animationIterate();
	void animationStep();

	void getProjection(double w, double h, Vec3 * corners);
	
	int eyes() {return mStereo ? 2 : 1;}

	//pan
	//tilt
	//zoom
protected:
	void beginStereo();
	void enterMid();
	void enterLeft();
	void enterRight();


protected:
	// local axes and world position
	Vec3		mPos;
	Quat		mQ;


	Vec3		mLookat;
	Vec3		mLookAbs;

	Vec3        dirMotion;

	Vec3		mPosMaster;
	Vec3        mPosTarget;

	Quat		mQMaster;
	Quat		mQTarget;

	Vec3        dirMotionMaster;
	Vec3        dirMotionTarget;

	// camera direction axes
	Vec3		mX;			// eye spread vector //right vector of the camera
	Vec3		mY;			// up vector
	Vec3		mZ;			// lookat vector

	float		mLensAngle;
	float		mAspect;
	float		mNear;
	float		mFar;

	float		animStep;
	float		animStepIncr;

	float		mEyeSep;
	float		mFocalLength;
	float		mTop;		// calculated in beginStereo()
	float		mRight;		// calculated in beginStereo()
	float		mShift;		// calculated in beginStereo()
	Vec3		mEyeShift;	// calculated in beginStereo()

	bool		mOrtho;
	bool		mStereo;
	bool		mSequential;
	Stack		keys;
};

#endif	// LUA_OPENGL_CAMERA_H
