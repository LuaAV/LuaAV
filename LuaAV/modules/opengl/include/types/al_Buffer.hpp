#ifndef INCLUDE_AL_BUFFER_HPP
#define INCLUDE_AL_BUFFER_HPP

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

#include <vector>

namespace al{

/// Buffer

///
///
template <class T, class Alloc=std::allocator<T> >
class Buffer : protected Alloc{
	typedef Alloc super;
public:

	/// @param[in] size			Initial size
	explicit Buffer(int size=0)
	:	mSize(size), mFill(0), mPos(size-1), mElems(size)
	{}

	/// @param[in] size			Initial size
	/// @param[in] capacity		Initial capacity
	Buffer(int size, int capacity)
	:	mSize(size), mFill(0), mPos(size-1), mElems(capacity)
	{}

	~Buffer(){}


	int capacity() const { return mElems.capacity(); }	///< Returns total capacity
	int fill() const { return mFill; }					///< Returns buffer fill amount
	int pos() const { return mPos; }					///< Returns write position
	int size() const { return mSize; }					///< Returns size
	T * elems(){ return &mElems[0]; }					///< Returns C-style pointer to elements
	const T * elems() const { return &mElems[0]; }		///< Returns C-style pointer to elements


	/// Set element at absolute index
	T& operator[](int i){ return atAbs(i); }
	
	/// Get element at absolute index
	const T& operator[](int i) const { return atAbs(i); }

	/// Assign value to elements

	/// This function fills a Buffer with n copies of the given value. Note that
	/// the assignment completely changes the buffer and that the resulting size
	/// is the same as the number of elements assigned.  Old data may be lost.
	void assign(int n, const T& v){ mElems.assign(n,v); }

	/// Set element at absolute index
	T& atAbs(int i){ return mElems[i]; }
	
	/// Get element at absolute index
	const T& atAbs(int i) const { return mElems[i]; }

	/// Set element at relative index
	T& atRel(int i){ return mElems[wrapOnce(pos()-i, size())]; }
	
	/// Get element at relative index
	const T& atRel(int i) const { return mElems[wrapOnce(pos()-i, size())]; }

	/// Get last element
	const T& last() const { return mElems[mPos-1]; }

	/// Clear
	void clear(){ mSize=mFill=0; mPos=-1; }

	/// Resize buffer
	
	/// This will set both the size and capacity of the buffer to the requested 
	/// size. If the number is smaller than the current size the buffer is 
	/// truncated, otherwise the buffer is extended and new elements are
	/// default-constructed.
	void resize(int n){
		mElems.resize(n);
		setSize(n);
	}
	
	/// Set size of buffer
	
	/// If the requested size is larger than the current capacity, then the 
	/// buffer will be resized.
	void size(int n){
		if(capacity() < n) resize(n);
		else setSize(n);
	}

	/// Appends element to end of buffer growing its size if necessary
	void append(const T &v, double growFactor=2){
		if(size() >= capacity()){	// double array size if too small
			mElems.resize((size() ? size() : 4)*growFactor);
		}
		super::construct(elems()+size(), v);
		mPos=mSize;
		++mSize;
	}
	/// synonym for append():
	void push_back(const T &v, double growFactor=2) { append(v, growFactor); }	

	/// Write new element to ring buffer
	void write(const T& v){
		++mPos; if(pos() == size()){ mPos=0; }
		Alloc::construct(elems()+pos(), v);
		if(fill() < size()) ++mFill; 
	}

private:
	int mSize;		// number of elements in array
	int	mFill;		// number of elements written to buffer (up to size())
	int mPos;		// circular buffer write position
	std::vector<T, Alloc> mElems;
	
	void setSize(int n){
		mSize=n;
		if(mFill>=n) mFill = n-1;
		if(mPos >=n) mPos  = n-1;	
	}
	
	// Moves value one period closer to interval [0, max)
	static int wrapOnce(int v, int max){
		if(v <  0)   return v+max;
		if(v >= max) return v-max;
		return v;
	}
};




template <int N, class T>
struct Multi{
	typedef Multi M;
//	Multi(){}
//	Multi(const T& e ){ mem::set(elems, N, e); }
//	Multi(const T* es){ mem::copy(elems, es, N); }

	T elems[N];
	
	/// Set element at index with no bounds checking
	T& operator[](int i){ return elems[i];}
	
	/// Get element at index with no bounds checking
	const T& operator[](int i) const { return elems[i]; }

	#define DO for(int i=0; i<N; ++i)

	bool operator !=(const M& v){ DO{ if((*this)[i] == v[i]) return false; } return true; }
	bool operator !=(const T& v){ DO{ if((*this)[i] == v   ) return false; } return true; }
	M& operator   = (const M& v){ DO{ (*this)[i] = v[i]; } return *this; }
	M& operator   = (const T& v){ DO{ (*this)[i] = v;    } return *this; }
	bool operator ==(const M& v){ DO{ if((*this)[i] != v[i]) return false; } return true; }
	bool operator ==(const T& v){ DO{ if((*this)[i] != v   ) return false; } return true; }

	#undef DO

	/// Returns size of array
	static int size(){ return N; }

	/// Zeros all elements.
	void zero(){ memset(elems, 0, N * sizeof(T)); }
};


/// Fixed size shift buffer
template <int N, class T>
struct ShiftBuffer : public Multi<N,T>{

	typedef Multi<N,T> Base;
	using Base::elems;
	using Base::operator=;

	ShiftBuffer(const T& v=T()){ *this = v; }

	/// Push new element onto buffer. Newest element is at index 0.
	void operator()(const T& v){
		for(int i=N-1; i>0; --i) elems[i] = elems[i-1];
		elems[0]=v;
	}
};




} // al::

#endif
