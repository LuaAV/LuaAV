#ifndef INCLUDE_AL_PLANE_HPP
#define INCLUDE_AL_PLANE_HPP

/*
 *  AlloSphere Research Group / Media Arts & Technology, UCSB, 2009
 */

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

#include "allocore/math/al_Vec.hpp"

namespace al {

/// Representation of a Euclidean plane
template <class T>
class Plane{
public:

	typedef al::Vec<3,T> Vec3;

	Plane(){}
	Plane(const Vec3& v1, const Vec3& v2, const Vec3& v3);

	/// Set plane from three points
	void set3Points(const Vec3& v1, const Vec3& v2, const Vec3& v3);
	
	/// Set plane from a normal and point
	void setNormalAndPoint(const Vec3& normal, const Vec3& point);

	/// Set plane from coefficients
	void setCoefficients(T a, T b, T c, T d);

	/// Returns distance from plane to point
	T distance(const Vec3& p) const;

	const Vec3& normal() const { return mNormal; }

protected:
	Vec3 mNormal, mPoint;
	T mD;
};


template <class T>
Plane<T>::Plane(const Vec3& v1, const Vec3& v2, const Vec3& v3){
	set3Points(v1,v2,v3);
}

template <class T>
void Plane<T>::set3Points(const Vec3& v1, const Vec3& v2, const Vec3& v3){

	Vec3 aux1 = v1 - v2;
	Vec3 aux2 = v3 - v2;

	cross(mNormal, aux2, aux1);

	mNormal.normalize();
	mPoint = v2;
	mD = -(mNormal.dot(mPoint));
}

template <class T>
void Plane<T>::setNormalAndPoint(const Vec3& nrm, const Vec3& point){
	mNormal = nrm.sgn();
	mD = -(mNormal.dot(mPoint));
}

template <class T>
void Plane<T>::setCoefficients(T a, T b, T c, T d){
	mNormal(a,b,c);
	T l = mNormal.mag();
	mNormal(a/l,b/l,c/l);
	mD = d/l;
}

template <class T>
T Plane<T>::distance(const Vec3& p) const{
	return (mD + mNormal.dot(p));
}


} // ::al::

#endif
