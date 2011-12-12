#ifndef INC_QUAT_H
#define INC_QUAT_H

#include <math.h>

#define QUAT_ACCURACY_MAX (1.000001)
#define QUAT_ACCURACY_MIN (0.999999)
#define QUAT_EPSILON (0.000001)
#define QUAT_RAD2DEG (57.29577951308)
#define QUAT_DEG2RAD (0.01745329252)
#define QUAT_DEG2RAD_BY2 (0.00872664626)
#define QUAT_PIBY2 (1.57079632679)

class Quat {
public:
	Quat(double w = 1.0, double x = 0.0, double y = 0.0, double z = 0.0);
	Quat(Quat * src);

	double w;
	double x;
	double y;
	double z;
	int normcount;
	static int normrate;
	
	void normcheck();	
	void normalize();
	
	void reset();
	
	void multiply(Quat * q2);	// in-place
	void multiply(Quat * q2, Quat * result);
	void inverse(Quat * result);
	
	void fromQuat(double w, double x, double y, double z);
	void fromQuat(float w, float x, float y, float z);
	void fromAxisAngle(double theta, double x1, double y1, double z1);
	void fromAxisAngle(float theta, float x1, float y1, float z1);
	void fromEuler(double a, double e, double b);
	void fromEuler(float a, float e, float b);
	
	void toMatrix(double * matrix);	// 4x4 matrix as double[16]
	void toMatrix(float * matrix);	// 4x4 matrix as double[16]
	void toAxisAngle(float * aa, float * ax, float * ay, float * az); // axis angle as double[4]
	void toAxisAngle(double * aa, double * ax, double * ay, double * az); // axis angle as double[4]
	void toEuler(double * e);	// euler angles as double[3]
	void toEuler(float * e);	// euler angles as double[3]
	void toVectorX(double * x, double * y, double * z);	// vector as double[3]
	void toVectorX(float * x, float * y, float * z);	// vector as double[3]
	void toVectorY(double * x, double * y, double * z);	// vector as double[3]
	void toVectorY(float * x, float * y, float * z);	// vector as double[3]
	void toVectorZ(double * x, double * y, double * z);	// vector as double[3]
	void toVectorZ(float * x, float * y, float * z);	// vector as double[3]
	
	void rotateVector(double * src, double * dst);
	void rotateVector(float * src, float * dst);
	
	void rotateVectorTransposed(float * src, float * dst);
	
	void rotateby(Quat * q2);	// in-place
};

inline Quat :: Quat(double w, double x, double y, double z)
: w(w), x(x), y(y), z(z), normcount(0)
{}

inline Quat :: Quat(Quat * src) {
	w = src->w; x = src->x; y = src->y; z = src->z;
}

inline void Quat :: normcheck() {
	if (normcount++ > normrate) normalize();
}

inline void Quat :: reset() {
	w = 1.0;		// identity quaternion
	x = y = z = 0.0;
}

inline void Quat :: normalize() {
	normcount = 0;
	double unit = w*w + x*x + y*y + z*z;
	if (unit*unit < QUAT_EPSILON) { 
		// unit too close to epsilon, set to default transform
		w = 1.0; 
		x = y = z = 0.0;
		return;
	}
	if (unit > QUAT_ACCURACY_MAX || unit < QUAT_ACCURACY_MIN) {
		double invmag = 1.0/sqrt(unit);
		w *= invmag; 
		x *= invmag; 
		y *= invmag;
		z *= invmag;
	}
}

// assumes both are already normalized!
inline void Quat :: multiply(Quat * q1, Quat * q2) {
	normcheck();
	w = q1->w*q2->w - q1->x*q2->x - q1->y*q2->y - q1->z*q2->z;
	x = q1->w*q2->x + q1->x*q2->w + q1->y*q2->z - q1->z*q2->y;
	y = q1->w*q2->y + q1->y*q2->w + q1->z*q2->x - q1->x*q2->z;
	z = q1->w*q2->z + q1->z*q2->w + q1->x*q2->y - q1->y*q2->x;
}

inline void Quat :: multiply(Quat * q1) {
	Quat * q = new Quat(this);
	multiply(q1, q);
	delete q;
}

inline void Quat :: rotateby(Quat * dq) {
	Quat * q = new Quat(this);
//	multiply(q, dq);
	multiply(dq, q);
	delete q;
}

inline void Quat :: inverse(Quat * result) {
	normalize();	
	result->w = w;
	result->x = -x;
	result->y = -y;
	result->z = -z;
}

inline void Quat :: fromQuat(double w, double x, double y, double z)
{
	this->w = w; this->x = x; this->y = y; this->z = z;
}
inline void Quat :: fromQuat(float w, float x, float y, float z) {
	this->w = w; this->x = x; this->y = y; this->z = z;
}

inline void Quat :: fromAxisAngle(double theta, double x1, double y1, double z1) {
	double t2 = theta * 0.00872664626; // * 0.5 * 180/PI
	double sinft2 = sin(t2);
	w = cos(t2);
	x = x1 * sinft2;
	y = y1 * sinft2;
	z = z1 * sinft2;
	normalize();
}

inline void Quat :: fromAxisAngle(float theta, float x1, float y1, float z1) {
	float t2 = theta * 0.00872664626f; // * 0.5 * 180/PI
	float sinft2 = sinf(t2);
	w = cosf(t2);
	x = x1 * sinft2;
	y = y1 * sinft2;
	z = z1 * sinft2;
	normalize();
}

inline void Quat :: fromEuler(double az, double el, double ba) {
	//http://vered.rose.utoronto.ca/people/david_dir/GEMS/GEMS.html
	//Converting from Euler angles to a quaternion is slightly more tricky, as the order of operations 
	//must be correct. Since you can convert the Euler angles to three independent quaternions by 
	//setting the arbitrary axis to the coordinate axes, you can then multiply the three quaternions 
	//together to obtain the final quaternion.

	//So if you have three Euler angles (a, b, c), then you can form three independent quaternions
	//Qx = [ cos(a/2), (sin(a/2), 0, 0)]
    //Qy = [ cos(b/2), (0, sin(b/2), 0)]
    //Qz = [ cos(c/2), (0, 0, sin(c/2))]
	//And the final quaternion is obtained by Qx * Qy * Qz.
	
	double c1 = cos(az * QUAT_DEG2RAD_BY2);
	double c2 = cos(el * QUAT_DEG2RAD_BY2);
	double c3 = cos(ba * QUAT_DEG2RAD_BY2);
	double s1 = sin(az * QUAT_DEG2RAD_BY2);
	double s2 = sin(el * QUAT_DEG2RAD_BY2);
	double s3 = sin(ba * QUAT_DEG2RAD_BY2);

	// equiv quat_multiply(&Qy, &Qx, &Q1); // since many terms are zero
	double tw = c1*c2;
	double tx = c1*s2;
	double ty = s1*c2;
	double tz = - s1*s2;
	
	// equiv quat_multiply(&Q1, &Qz, &Q2); // since many terms are zero
	w = tw*c3 - tz*s3;
	x = tx*c3 + ty*s3;
	y = ty*c3 - tx*s3;
	z = tw*s3 + tz*c3;
	normalize();
}

inline void Quat :: fromEuler(float az, float el, float ba) {
	//http://vered.rose.utoronto.ca/people/david_dir/GEMS/GEMS.html
	//Converting from Euler angles to a quaternion is slightly more tricky, as the order of operations 
	//must be correct. Since you can convert the Euler angles to three independent quaternions by 
	//setting the arbitrary axis to the coordinate axes, you can then multiply the three quaternions 
	//together to obtain the final quaternion.

	//So if you have three Euler angles (a, b, c), then you can form three independent quaternions
	//Qx = [ cos(a/2), (sin(a/2), 0, 0)]
    //Qy = [ cos(b/2), (0, sin(b/2), 0)]
    //Qz = [ cos(c/2), (0, 0, sin(c/2))]
	//And the final quaternion is obtained by Qx * Qy * Qz.
	
	float c1 = cosf(az * QUAT_DEG2RAD_BY2);
	float c2 = cosf(el * QUAT_DEG2RAD_BY2);
	float c3 = cosf(ba * QUAT_DEG2RAD_BY2);
	float s1 = sinf(az * QUAT_DEG2RAD_BY2);
	float s2 = sinf(el * QUAT_DEG2RAD_BY2);
	float s3 = sinf(ba * QUAT_DEG2RAD_BY2);

	// equiv quat_multiply(&Qy, &Qx, &Q1); // since many terms are zero
	float tw = c1*c2;
	float tx = c1*s2;
	float ty = s1*c2;
	float tz = - s1*s2;
	
	// equiv quat_multiply(&Q1, &Qz, &Q2); // since many terms are zero
	w = tw*c3 - tz*s3;
	x = tx*c3 + ty*s3;
	y = ty*c3 - tx*s3;
	z = tw*s3 + tz*c3;
	normalize();
}

inline void Quat :: toMatrix(double * m) {
	m[0] = 1.0 - 2.0*y*y - 2.0*z*z;
	m[1] = 2.0*x*y - 2.0*z*w;
	m[2] = 2.0*x*z + 2.0*y*w;
	m[3] = 0.0;
	
	m[4] = 2.0*x*y + 2.0*z*w;
	m[5] = 1.0 - 2.0*x*x - 2.0*z*z;
	m[6] = 2.0*y*z - 2.0*x*w;
	m[7] = 0.0;
	
	m[8] = 2.0*x*z - 2.0*y*w;
	m[9] = 2.0*y*z + 2.0*x*w;
	m[10] = 1.0 - 2.0*x*x - 2.0*y*y;
	m[11] = 0.0;
	
	m[12] = m[13] = m[14] = 0.0;
	m[15] = 1.0;
}

inline void Quat :: toMatrix(float * m) {
	m[0] = 1.f - 2.0f*y*y - 2.0f*z*z;
	m[1] = 2.f*x*y - 2.0f*z*w;
	m[2] = 2.f*x*z + 2.0f*y*w;
	m[3] = 0.f;
	
	m[4] = 2.f*x*y + 2.0f*z*w;
	m[5] = 1.f - 2.0f*x*x - 2.0f*z*z;
	m[6] = 2.f*y*z - 2.0f*x*w;
	m[7] = 0.f;
	
	m[8] = 2.f*x*z - 2.0f*y*w;
	m[9] = 2.f*y*z + 2.0f*x*w;
	m[10] = 1.f - 2.0f*x*x - 2.0f*y*y;
	m[11] = 0.f;
	
	m[12] = m[13] = m[14] = 0.0f;
	m[15] = 1.f;
}


inline void Quat :: toAxisAngle(float * aa, float * ax, float * ay, float * az) {
  float unit = w*w;
  if (unit > QUAT_ACCURACY_MAX || unit < QUAT_ACCURACY_MIN) {
	float invSinAngle = 1.f/sqrt(1.f - unit);
	*aa = acosf(w) * 114.59155902616; // * 2 * 180 / pi
	*ax = x * invSinAngle;
	*ay = y * invSinAngle;
	*az = z * invSinAngle;
  } else {
	*aa = 0.f;
	*ax = x;
	*ay = y;
	*az = z;
  }
}

inline void Quat :: toAxisAngle(double * aa, double * ax, double * ay, double * az) {
  double unit = w*w;
  if (unit > QUAT_ACCURACY_MAX || unit < QUAT_ACCURACY_MIN)  {
	double invSinAngle = 1.0/sqrt(1.0 - unit);
	*aa = acos(w) * 114.59155902616; // * 2 * 180 / pi
	*ax = x * invSinAngle;
	*ay = y * invSinAngle;
	*az = z * invSinAngle;
  } else {
	*aa = 0.0;
	*ax = x;
	*ay = y;
	*az = z;
  }
}

inline void Quat :: toEuler(double * e) {
	// http://www.mathworks.com/access/helpdesk/help/toolbox/aeroblks/quaternionstoeulerangles.html
	double sqw = w*w;
	double sqx = x*x;
	double sqy = y*y;
	double sqz = z*z;
	e[0] = QUAT_RAD2DEG * asin(-2.0 * (x*z - w*y));
	e[1] = QUAT_RAD2DEG * atan2(2.0 * (y*z + w*x),(sqw - sqx - sqy + sqz));
	e[3] = QUAT_RAD2DEG * atan2(2.0 * (x*y + w*z), (sqw + sqx - sqy - sqz));
}

inline void Quat :: toEuler(float * e) {
	// http://www.mathworks.com/access/helpdesk/help/toolbox/aeroblks/quaternionstoeulerangles.html
	float sqw = w*w;
	float sqx = x*x;
	float sqy = y*y;
	float sqz = z*z;
	e[0] = QUAT_RAD2DEG * asinf(-2.0f * (x*z - w*y));
	e[1] = QUAT_RAD2DEG * atan2f(2.0f * (y*z + w*x),(sqw - sqx - sqy + sqz));
	e[3] = QUAT_RAD2DEG * atan2f(2.0f * (x*y + w*z), (sqw + sqx - sqy - sqz));
}

inline void Quat :: toVectorX(double * vx, double * vy, double * vz) {
	*vx = 1.0 - 2.0*y*y - 2.0*z*z;
	*vy = 2.0*x*y + 2.0*z*w;
	*vz = 2.0*x*z - 2.0*y*w;	
}

inline void Quat :: toVectorY(double * vx, double * vy, double * vz) {
	*vx = 2.0*x*y - 2.0*z*w;
	*vy = 1.0 - 2.0*x*x - 2.0*z*z;
	*vz = 2.0*y*z + 2.0*x*w;
}

inline void Quat :: toVectorZ(double * vx, double * vy, double * vz) {
	*vx = 2.0*x*z + 2.0*y*w;
	*vy = 2.0*y*z - 2.0*x*w;
	*vz = 1.0 - 2.0*x*x - 2.0*y*y;
}

inline void Quat :: toVectorX(float * vx, float * vy, float * vz) {
	*vx = 1.0f - 2.0f*y*y - 2.0f*z*z;
	*vy = 2.0f*x*y + 2.0f*z*w;
	*vz = 2.0f*x*z - 2.0f*y*w;	
}

inline void Quat :: toVectorY(float * vx, float * vy, float * vz) {
	*vx = 2.0f*x*y - 2.0f*z*w;
	*vy = 1.0f - 2.0f*x*x - 2.0f*z*z;
	*vz = 2.0f*y*z + 2.0f*x*w;
}

inline void Quat :: toVectorZ(float * vx, float * vy, float * vz) {
	*vx = 2.0f*x*z + 2.0f*y*w;
	*vy = 2.0f*y*z - 2.0f*x*w;
	*vz = 1.0f - 2.0f*x*x - 2.0f*y*y;
}

inline void Quat :: rotateVector(double * src, double * dst) {
	double matrix[16];
	toMatrix(matrix);
	dst[0] = src[0] * matrix[0] + src[1] * matrix[1] + src[2] * matrix[2];
	dst[1] = src[0] * matrix[4] + src[1] * matrix[5] + src[2] * matrix[6];
	dst[2] = src[0] * matrix[8] + src[1] * matrix[9] + src[2] * matrix[10];
}

inline void Quat :: rotateVectorTransposed(float * src, float * dst) {
	float matrix[16];
	toMatrix(matrix);
	dst[0] = src[0] * matrix[0] + src[1] * matrix[4] + src[2] * matrix[8];
	dst[1] = src[0] * matrix[1] + src[1] * matrix[5] + src[2] * matrix[9];
	dst[2] = src[0] * matrix[2] + src[1] * matrix[6] + src[2] * matrix[10];
}

inline void Quat :: rotateVector(float * src, float * dst) {
	float matrix[16];
	toMatrix(matrix);
	dst[0] = src[0] * matrix[0] + src[1] * matrix[1] + src[2] * matrix[2];
	dst[1] = src[0] * matrix[4] + src[1] * matrix[5] + src[2] * matrix[6];
	dst[2] = src[0] * matrix[8] + src[1] * matrix[9] + src[2] * matrix[10];
}

#endif