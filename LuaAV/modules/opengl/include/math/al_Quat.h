#ifndef INCLUDE_AL_QUAT_H
#define INCLUDE_AL_QUAT_H

#include <math.h>
#include "math/al_Constants.hpp"

/*!
	C binding for al::Quat<double> (@see al_Quat.hpp)
*/

#define QUAT_ACCURACY_MAX (1.000001)
#define QUAT_ACCURACY_MIN (0.999999)
#define QUAT_EPSILON (0.0000001)
#define QUAT_DEG2RAD_BY2 (M_DEG2RAD)
#define QUAT_PIBY2 (1.57079632679)

typedef struct al_quat al_quat;

al_quat * al_quat_create(double w, double x, double y, double z);
void al_quat_free(al_quat ** q);

void al_quat_from_quat(al_quat * dst, double w, double x, double y, double z);
void al_quat_from_axisangle(al_quat * dst, double theta, double x, double y, double z);
void al_quat_from_euler(al_quat * dst, double az, double el, double ba);

void al_quat_to_vector_x(al_quat * q, double * x, double * y, double * z);
void al_quat_to_vector_y(al_quat * q, double * x, double * y, double * z);
void al_quat_to_vector_z(al_quat * q, double * x, double * y, double * z);
void al_quat_to_matrix(al_quat * q, double * mat);

void al_quat_normalize(al_quat * q);
void al_quat_reset(al_quat * q);

void al_quat_multiply_inplace(al_quat * q, al_quat * q2);
void al_quat_multiply(al_quat * q, al_quat * q2, al_quat * result);
void al_quat_conjugate(al_quat * q, al_quat * result);
void al_quat_rotate(al_quat * q, al_quat * q2);

void al_quat_rotate_vector(al_quat * q, double * src, double * dst);
void al_quat_rotate_vector_transposed(al_quat * q, double * src, double * dst);


#endif /* include guard */
