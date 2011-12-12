#ifndef INCLUDE_AL_INTERVAL_HPP
#define INCLUDE_AL_INTERVAL_HPP

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

namespace al {

/// A closed interval [min, max]

/// An interval is a connected region of the real line. Geometrically, it
/// describes a 0-sphere. Order is strongly enforced so that the endpoints will
/// always satisfy min <= max.
template <class T>
class Interval{
public:

	Interval()
	:	mMin(0), mMax(1){}

	Interval(const T& min, const T& max)
	{ endpoints(min,max); }

	T center() const { return (max()+min())/T(2); }	///< Returns center point

	/// Test is point is contained exclusively within interval
	bool contains(const T& v) const { return v>=min() && v<=max(); }

	bool degenerate() const { return min()==max(); }///< Returns true if diameter is zero
	T diameter() const { return max()-min(); }		///< Returns absolute difference of endpoints
	const T& max() const { return mMax; }			///< Get maximum endpoint
	const T& min() const { return mMin; }			///< Get minimum endpoint
	bool proper() const { return min()!=max(); }	///< Returns true if diameter is non-zero
	T radius() const { return diameter()/T(2); }	///< Returns one-half the diameter

	/// Linearly map point in interval to point in the unit interval
	T toUnit(const T& v) const { return (v-min())/diameter(); }
	
	template <class U>
	bool operator == (const Interval<U>& v){ return min()==v.min() && max()==v.max(); }

	template <class U>
	bool operator != (const Interval<U>& v){ return !(*this == v); }
	
	template <class U>
	Interval& operator +=(const Interval<U>& v){ endpoints(min()+v.min(), max()+v.max()); return *this; }

	template <class U>
	Interval& operator -=(const Interval<U>& v){ endpoints(min()-v.max(), max()-v.min()); return *this; }
	
	template <class U>
	Interval& operator *=(const Interval<U>& v){
		T a=min()*v.min(), b=min()*v.max(), c=max()*v.min(), d=max()*v.max();
		mMin = min(min(a,b),min(c,d));
		mMax = max(max(a,b),max(c,d));
		return *this;
	}

	template <class U>
	Interval& operator /=(const Interval<U>& v){
		T a=min()/v.min(), b=min()/v.max(), c=max()/v.min(), d=max()/v.max();
		mMin = min(min(a,b),min(c,d));
		mMax = max(max(a,b),max(c,d));
		return *this;
	}

	/// Set center point preserving diameter
	Interval& center(const T& v){ return centerDiameter(v, diameter()); }

	/// Set diameter (width) preserving center
	Interval& diameter(const T& v){ return centerDiameter(center(), v); }

	/// Set center and diameter
	Interval& centerDiameter(const T& c, const T& d){
		mMin = c - d*T(0.5);
		mMax = mMin + d;
		return *this;
	}

	/// Set the endpoints
	Interval& endpoints(const T& min, const T& max){
		mMax=max; mMin=min;
		if(mMin > mMax){ T t=mMin; mMin=mMax; mMax=t; }
		return *this;
	}

	/// Translate interval by fixed amount
	Interval& translate(const T& v){ mMin+=v; mMax+=v; return *this; }

	/// Set maximum endpoint
	Interval& max(const T& v){ return endpoints(min(), v); }
	
	/// Set minimum endpoint
	Interval& min(const T& v){ return endpoints(v, max()); }

private:
	T mMin, mMax;

	const T& min(const T& a, const T& b){ return a<b?a:b; }
	const T& max(const T& a, const T& b){ return a>b?a:b; }
};

} // ::al::

#endif
