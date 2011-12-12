#ifndef INCLUDE_AL_FRUSTUM_HPP
#define INCLUDE_AL_FRUSTUM_HPP

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

#include "allocore/math/al_Plane.hpp"
#include "allocore/math/al_Vec.hpp"


namespace al {


template <class T> class Frustum;

typedef Frustum<double> Frustumd;

/*
The vertices of the far (back) plane can be simply calculated by the ratio of 
similar triangles.

ltf = ((f/n)l, (f/n)t, f)
lbf = ((f/n)l, (f/n)b, f)
rtf = ((f/n)r, (f/n)t, f)
rbf = ((f/n)r, (f/n)b, f)
*/

/// Rectangular frustum

/// A frustum has the shape of a four-sided pyramid truncated at the top.
/// For graphics, this specifies the viewing frustum of a camera.
template <class T>
class Frustum{
public:

	enum{ TOP=0, BOTTOM, LEFT, RIGHT, NEARP, FARP };
	enum{ OUTSIDE, INTERSECT, INSIDE };

	Vec<3,T> ntl, ntr, nbl, nbr, ftl, ftr, fbl, fbr;	// corners
	Plane<T> pl[6];									// faces

	/// Test whether point is in frustum
	int pointInFrustum(const Vec<3,T>& p) const;
	
	/// Test whether sphere is in frustum
	int sphereInFrustum(const Vec<3,T>& p, float raio) const;
	
	/// Test whether axis-aligned box is in frustum
	int boxInFrustum(const Vec<3,T>& xyz, const Vec<3,T>& dim) const;

	void computePlanes();

//	T mNear, mFar;			// clipping z distances
//	T mRatio;				// aspect ratio
//	T mAngle, mTanFOV;		// lens angle, tan(field of view)
//	T nw, nh, fw, fh;		// near and far plane dimensions

//	/// Set from camera parameters
//	void setCamInternals(T angle, T ratio, T nearDist, T farDist);
//	
//	/// Set from camera position, look, and up vectors
//	void setCamDef(const Vec<3,T>& pos, const Vec<3,T>& look, const Vec<3,T>& up);
		
};

template <class T>
void Frustum<T>::computePlanes(){
	pl[TOP].set3Points(ntr,ntl,ftl);
	pl[BOTTOM].set3Points(nbl,nbr,fbr);
	pl[LEFT].set3Points(ntl,nbl,fbl);
	pl[RIGHT].set3Points(nbr,ntr,fbr);
	pl[NEARP].set3Points(ntl,ntr,nbr);
	pl[FARP].set3Points(ftr,ftl,fbl);
}

//
//template <class T>
//void Frustum<T>::setCamInternals(T angle, T ratio, T nearD, T farD){
//	mRatio = ratio;
//	mAngle = angle;
//	mNear = nearD;
//	mFar = farD;
//
//	static double const tanCoef = 0.01745329252*0.5;	// degree-to-radian over /2
//	mTanFOV = (T)tan(mAngle * tanCoef);
//	nh = mNear * mTanFOV;
//	nw = nh * mRatio; 
//	fh = mFar  * mTanFOV;
//	fw = fh * mRatio;
//}
//
//template <class T>
//void Frustum<T>::setCamDef(const Vec<3,T>& p, const Vec<3,T>& l, const Vec<3,T>& u){
//
//	Vec<3,T> Z = (p-l).normalize();
//	Vec<3,T> X = cross(u,Z).normalize();
//	Vec<3,T> Y = cross(Z,X);
//
//	Vec<3,T> nc = p - Z * mNear;
//	Vec<3,T> fc = p - Z * mFar;
//
//	ntl = nc + Y * nh - X * nw;
//	ntr = nc + Y * nh + X * nw;
//	nbl = nc - Y * nh - X * nw;
//	nbr = nc - Y * nh + X * nw;
//
//	ftl = fc + Y * fh - X * fw;
//	ftr = fc + Y * fh + X * fw;
//	fbl = fc - Y * fh - X * fw;
//	fbr = fc - Y * fh + X * fw;
//
//	pl[TOP].set3Points(ntr,ntl,ftl);
//	pl[BOTTOM].set3Points(nbl,nbr,fbr);
//	pl[LEFT].set3Points(ntl,nbl,fbl);
//	pl[RIGHT].set3Points(nbr,ntr,fbr);
//	pl[NEARP].set3Points(ntl,ntr,nbr);
//	pl[FARP].set3Points(ftr,ftl,fbl);
//}

template <class T>
int Frustum<T>::pointInFrustum(const Vec<3,T>& p) const {
	int result = INSIDE;
	for(int i=0; i<6; ++i){
		if(pl[i].distance(p) < 0)
			return OUTSIDE;
	}
	return result;
}

template <class T>
int Frustum<T>::sphereInFrustum(const Vec<3,T>& p, float raio) const {
	int result = INSIDE;
	for(int i=0; i<6; ++i){
		float distance = pl[i].distance(p);
		if(distance < -raio)
			return OUTSIDE;
		else if(distance < raio)
			result = INTERSECT;
	}
	return result;
}

template <class T>
int Frustum<T>::boxInFrustum(const Vec<3,T>& xyz, const Vec<3,T>& dim) const {
	int result = INSIDE;
	for(int i=0; i<6; i++){
		const Vec3d& plNrm = pl[i].normal();
		Vec<3,T> vp = xyz;
		
		if(plNrm[0] > 0) vp[0] += dim[0];
		if(plNrm[1] > 0) vp[1] += dim[1];
		if(plNrm[2] > 0) vp[2] += dim[2];
		if(pl[i].distance(vp) < 0) return OUTSIDE;	

		Vec<3,T> vn = xyz;
		if(plNrm[0] < 0) vn[0] += dim[0];
		if(plNrm[1] < 0) vn[1] += dim[1];
		if(plNrm[2] < 0) vn[2] += dim[2];
		if(pl[i].distance(vn) < 0) result = INTERSECT;
	}
	return result;
}





} // ::al::

#endif
