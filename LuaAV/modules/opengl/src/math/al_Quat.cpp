#include "math/al_Quat.hpp"

/*
	C-binding implementation:

	al::Quat<double> should be the same as struct al_quat in terms of memory layout.
*/
typedef al::Quat<double> Quatd;

#define TO_QUAT(q) ((Quatd)(q))
#define TO_QUAT_PTR(q) ((Quatd *)(q))

#define FROM_QUAT(q) ((al_quat)(q))
#define FROM_QUAT_PTR(q) ((al_quat *)(q))

al_quat * al_quat_create(double w, double x, double y, double z) {
	Quatd * qd = new Quatd(w, x, y, z);
	return FROM_QUAT_PTR(qd);
}

void al_quat_free(al_quat ** q) {
	if (q && *q) {
		delete (TO_QUAT_PTR(*q));
		*q = 0;
	}
}

void al_quat_normalize(al_quat * q) {
	(TO_QUAT_PTR(q))->normalize();
}

void al_quat_reset(al_quat * q) {
	(TO_QUAT_PTR(q))->identity();
}

void al_quat_conjugate(al_quat * q, al_quat * result) {
	*(TO_QUAT_PTR(result)) = (TO_QUAT_PTR(q))->conjugate();
}

void al_quat_from_quat(al_quat * dst, double w, double x, double y, double z) {
	(TO_QUAT_PTR(dst))->set(w, x, y, z);
}

void al_quat_from_axisangle(al_quat * dst, double theta, double x, double y, double z) {
	(TO_QUAT_PTR(dst))->fromAxisAngle(theta, x, y, z);
}

void al_quat_from_euler(al_quat * dst, double az, double el, double ba) {
	(TO_QUAT_PTR(dst))->fromEuler(az, el, ba);
}

void al_quat_to_vector_x(al_quat * q, double * x, double * y, double * z) {
	(TO_QUAT_PTR(q))->toVectorX(x, y, z);
}

void al_quat_to_vector_y(al_quat * q, double * x, double * y, double * z) {
	(TO_QUAT_PTR(q))->toVectorY(x, y, z);
}

void al_quat_to_vector_z(al_quat * q, double * x, double * y, double * z) {
	(TO_QUAT_PTR(q))->toVectorZ(x, y, z);
}

void al_quat_to_matrix(al_quat * q, double * mat) {
	(TO_QUAT_PTR(q))->toMatrix(mat);
}

void al_quat_rotate_vector(al_quat * q, double * src, double * dst) {
	(TO_QUAT_PTR(q))->rotateVector(src, dst);
}

void al_quat_rotate_vector_transposed(al_quat * q, double * src, double * dst) {
	(TO_QUAT_PTR(q))->rotateVectorTransposed(src, dst);
}
