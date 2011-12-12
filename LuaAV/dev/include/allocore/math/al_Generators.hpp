#ifndef INCLUDE_AL_MATH_GENERATORS_HPP
#define INCLUDE_AL_MATH_GENERATORS_HPP

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

/*
	Generator function objects

	A generator is a lightweight object that generates a sequence of elements.
	Generators have a standard interface specified by the Val class. 
	A const qualified generator only means that its generating function 
	parameters are held constant; its current value can change.
	
	When a generator is iterated (through its nullary function operator) it
	will compute its next value, store the result in its value member variable,
	and return its previously stored value. The reason the previous value is 
	returned is so that its value can be set directly and on its next iteration
	it will return that value.
*/

namespace al {
namespace gen{

// This is needed since templates are not always smart about inheriting super members.
#define INHERIT\
	using Val<T>::val; using Val<T>::operator=;\
	T   operator[](int i) const { return (*this)(); }


/// Single value generator
template <class T>
struct Val{
	mutable T val;										///< Value
	// Since this is a generator, we will allow its value to be modified if 
	// it's a const.

	Val(): val(T(0)){}									///< Constructor
	Val(const T& v): val(v){}							///< Constructor
	Val& operator= (const T& v){val=v; return *this;}	///< Set value
	T operator()() const { return val; }				///< Generate next value
};

/// Recursive add generator
template <class T>
struct RAdd: public Val<T>{ INHERIT;
	T add;												///< Addition amount

	RAdd(const T& add=T(1), const T& ival=T(0))
	: Val<T>(ival), add(add){}							///< Constructor
	T operator()() const {T r=val; val+=add; return r;}	///< Generate next value
};

/// Recursive multiply generator
template <class T>
struct RMul: public Val<T>{ INHERIT;
	T mul;												///< Multiplication amount

	RMul(const T& mul=T(1), const T& ival=T(1))
	:	Val<T>(ival), mul(mul){}						///< Constructor
	T operator()() const {T r=val; val*=mul; return r;}	///< Generate next value
};

/// Recursive multiply-add generator
template <class T>
struct RMulAdd: public Val<T>{ INHERIT;
	T mul;												///< Multiplication amount
	T add;												///< Addition amount

	RMulAdd(const T& mul=T(1), const T& add=T(0), const T& ival=T(0))
	:	Val<T>(ival), mul(mul), add(add){}				///< Constructor
	T operator()() const {T r=val; val=r*mul+add; return r; }	///< Generate next value
};


/// Sinusoidal generator based on recursive formula x0 = c x1 - x2
template <class T>
struct RSin : public Val<T>{ INHERIT;

	/// Constructor
	RSin(const T& frq=T(0), const T& phs=T(0), const T& amp=T(1))
	:	val2(0), mul(0){ set(frq,phs,amp); }

	/// Generate next value.
	T operator()() const {
		T v0 = mul * val - val2;
		val2 = val;
		return val = v0;
	}

	void next3(T& o2, T& o1, T& o0) const {
		T v0 = o0 = mul * val  - val2;
		o2 = val2 = mul * v0   - val;
		o1 = val  = mul * val2 - v0;
	}
	
	T freq() const { return acos(mul*0.5) * M_1_2PI; }
	
	/// Set parameters from unit freq, phase, and amplitude.
	RSin& set(const T& frq, const T& phs, const T& amp=T(1)){
		T f=frq*M_2PI, p=phs*M_2PI;
		mul  = (T)2 * (T)cos(f);
		val2 = (T)sin(p - f * T(2))*amp;
		val  = (T)sin(p - f       )*amp;
		return *this;
	}

	mutable T val2;
	T mul;			///< Multiplication factor. [-2, 2] range gives stable sinusoids.
};


#undef INHERIT

} // ::al::gen::
} // ::al::

#endif
