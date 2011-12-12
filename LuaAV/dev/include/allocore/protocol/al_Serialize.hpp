#ifndef INCLUDE_AL_SERIALIZE_HPP
#define INCLUDE_AL_SERIALIZE_HPP

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


#include "al_Serialize.h"
#include <vector>
#include <string>

namespace al{

namespace ser{

template<class T> inline uint32_t encode(char * b, const T * v, uint32_t n){ return 0; }

#define DEF(t,T,c)\
template<> inline uint32_t encode(char * b, const t * v, uint32_t n){ return serEncode##T(b, c v,n); }
DEF(float,   Float32,)
DEF(double,  Float64,)
DEF(char,    Int8, (const int8_t *))
DEF(int8_t,  Int8,)
DEF(int16_t, Int16,)
DEF(int32_t, Int32,)
DEF(int64_t, Int64,)
DEF(uint8_t, UInt8,)
DEF(bool,    UInt8, (const uint8_t *))
DEF(uint16_t,UInt16,)
DEF(uint32_t,UInt32,)
DEF(uint64_t,UInt64,)
#undef DEF

template<class T> uint8_t getType();
template<> inline uint8_t getType<float   >(){ return 'f'; }
template<> inline uint8_t getType<double  >(){ return 'd'; }
template<> inline uint8_t getType<bool    >(){ return 't'; }
template<> inline uint8_t getType<uint8_t >(){ return 't'; }
template<> inline uint8_t getType<uint16_t>(){ return 'T'; }
template<> inline uint8_t getType<uint32_t>(){ return 'u'; }
template<> inline uint8_t getType<uint64_t>(){ return 'U'; }
template<> inline uint8_t getType<int8_t  >(){ return 'h'; }
template<> inline uint8_t getType<int16_t >(){ return 'H'; }
template<> inline uint8_t getType<int32_t >(){ return 'i'; }
template<> inline uint8_t getType<int64_t >(){ return 'I'; }

} // ser::


///
struct Serializer{

	template <class T>
	Serializer& operator<< (T v);

	Serializer& operator<< (const char * v);
	Serializer& operator<< (const std::string& v);
	
	template <class T>
	Serializer& add(const T * v, uint32_t num);

	const std::vector<char>& buf() const;

private:
	std::vector<char> mBuf, mTemp;
	
	template <class T> void checkSize(uint32_t n=1);
};


///
struct Deserializer{
	
	Deserializer(const std::vector<char>& b);
	
	Deserializer(const char * b, uint32_t n);
	
	template <class T>
	Deserializer& operator>> (T& v);
	Deserializer& operator>> (char * v);
	Deserializer& operator>> (std::string& v);

	const std::vector<char>& buf() const;

private:
	int mStart;
	std::vector<char> mBuf;
	char * bufDec();
};




// =============================================================================
// Implementation
// =============================================================================


template <class T> Serializer& Serializer::operator<< (T v){
	return add(&v, 1);
}

template <class T> Serializer& Serializer::add(const T * v, uint32_t num){
	checkSize<T>(num);
	uint32_t n = ser::encode(&mTemp[0], v, num);
	mBuf.insert(mBuf.end(), mTemp.begin(), mTemp.begin()+n);
	return *this;		
}

template <class T> void Serializer::checkSize(uint32_t n){
	uint32_t need = n*sizeof(T) + serHeaderSize();
	if(mTemp.size() < need) mTemp.resize(need);
}


template <class T> Deserializer& Deserializer::operator>> (T& v){
	uint32_t n = serDecode(bufDec(), &v);
	mStart += n;
	return *this;
}

//template <class T>
//Deserializer& decode(const T * v, uint32_t num){
//	uint32_t n = decode(bufDec(), v);
//	return *this;		
//}

} // al::


#endif
