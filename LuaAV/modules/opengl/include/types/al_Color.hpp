#ifndef INCLUDE_AL_COLOR_HPP
#define INCLUDE_AL_COLOR_HPP

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

namespace al{

struct Color;
struct HSV;

/// An RGBA color.
struct Color{

	union{
		struct{
			float r;	///< Red component in [0, 1]
			float g;	///< Green component in [0, 1]
			float b;	///< Blue component in [0, 1]
			float a;	///< Alpha component in [0, 1]
		};
		float components[4];
	};


	/// @param[in] r			red component
	/// @param[in] g			green component
	/// @param[in] b			blue component
	/// @param[in] a			alpha component
	Color(float r, float g, float b, float a=1.f)
	//:	r(r), g(g), b(b), a(a)	<< Clang can't handle it
	{ this->r=r;  this->g=g; this->b=b; this->a=a; }

	/// @param[in] gray			red/green/blue components
	/// @param[in] a			alpha component
	Color(float gray=1.f, float a=1.f)
	//:	r(gray), g(gray), b(gray), a(a)	<< Clang can't handle it
	{ this->r=gray;  this->g=gray; this->b=gray; this->a=a; }

	/// @param[in] hsv			HSV value
	/// @param[in] a			alpha component
	Color(const HSV& hsv, float a=1.f)
	:	a(a)
	{	*this = hsv; }


	/// Set color component at index with no bounds checking
	float& operator[](int i){ return components[i]; }
	
	/// Get color component at index with no bounds checking
	const float& operator[](int i) const { return components[i]; }

	const float * to_ptr() const {return components;}

	/// Set RGBA components
	Color& set(float re, float gr, float bl, float al){ a=al; return set(re,gr,bl); }

	/// Set RGB components
	Color& set(float re, float gr, float bl){ r=re; g=gr; b=bl; return *this; }

	/// Set from gray value
	Color& set(float v){ return set(v,v,v); }

	/// Set from gray value and alpha
	Color& set(float v, float al){ return set(v,al); }

	/// Set components from tightly packed RGBA array
	template <class Array4>
	Color& operator=(const Array4& v){ return set(v[0], v[1], v[2], v[3]); }

	/// Set from gray value
	Color& operator= (float v){ return set(v); }

	/// Set RGB components from HSV
	Color& operator= (const HSV& v);

	Color& operator+= (const Color& v){ return set(r+v.r, g+v.g, b+v.b, a+v.a); }
	Color& operator-= (const Color& v){ return set(r-v.r, g-v.g, b-v.b, a-v.a); }
	Color& operator*= (const Color& v){ return set(r*v.r, g*v.g, b*v.b, a*v.a); }
	Color& operator/= (const Color& v){ return set(r/v.r, g/v.g, b/v.b, a/v.a); }
	Color& operator+= (float v){ return set(r+v, g+v, b+v, a+v); }
	Color& operator-= (float v){ return set(r-v, g-v, b-v, a-v); }
	Color& operator*= (float v){ return set(r*v, g*v, b*v, a*v); }
	Color& operator/= (float v){ return set(r/v, g/v, b/v, a/v); }

	Color operator- () const { return Color(-r,-g,-b,-a); }
	Color operator+ (const Color& v) const { return Color(*this)+=v; }
	Color operator- (const Color& v) const { return Color(*this)-=v; }
	Color operator* (const Color& v) const { return Color(*this)*=v; }
	Color operator/ (const Color& v) const { return Color(*this)/=v; }
	Color operator+ (float v) const { return Color(*this)+=v; }
	Color operator- (float v) const { return Color(*this)-=v; }
	Color operator* (float v) const { return Color(*this)*=v; }
	Color operator/ (float v) const { return Color(*this)/=v; }

	/// Returns nearest black or white color
	Color blackAndWhite() const { return Color(luminance()>0.5f?1.f:0.f); }

	/// Clamp all components into [0,1] range
	void clamp(){
		r<0.f ? r=0.f : (r>1.f ? r=1.f : 0);
		g<0.f ? g=0.f : (g>1.f ? g=1.f : 0);
		b<0.f ? b=0.f : (b>1.f ? b=1.f : 0);
		a<0.f ? a=0.f : (a>1.f ? a=1.f : 0);
	}

	/// Returns inverted color
	Color inverse() const { return Color(1.f-r, 1.f-g, 1.f-b, a); }
	
	/// Invert RGB components
	Color& invert(){ return set(1.f-r, 1.f-g, 1.f-b); }
	
	/// Returns luminance value
	float luminance() const { return r*0.3f+g*0.59f+b*0.11f; }
};

inline Color operator + (float s, const Color& c){ return  c+s; }
inline Color operator - (float s, const Color& c){ return -c+s; }
inline Color operator * (float s, const Color& c){ return  c*s; }
inline Color operator / (float s, const Color& c){ return Color(s/c.r, s/c.g, s/c.b, s/c.a); }


struct HSV{

	float h, s, v;

	template<class T>
	HSV(T * hsv): h(hsv[0]), s(hsv[1]), v(hsv[2]){}

	HSV(float h=0, float s=1, float v=1): h(h), s(s), v(v){}	
	HSV(const Color& c){ *this = c; }

	HSV& operator()(float hue, float sat, float val){ h=hue; s=sat; v=val; return *this; }

	HSV& operator=(const Color& c);
};


} // al::

#endif
