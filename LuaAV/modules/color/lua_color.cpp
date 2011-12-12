#ifdef __cplusplus
extern "C" {
#endif

#include "lua.h"
#include "lauxlib.h"
#include "cc.h"

#ifdef __cplusplus
}
#endif

#include "lua_utility.h"
#include <cmath>



double get_tristimulus(int n, double T) {
	if(n < 4) {
		return (double)D65_2;
	}
	else if(T >= ccNTristimulus) {
		return (double)F11_10;
	}
	else {
		return T;
	}
}

/*!	Shift the hue part of an HSL[A] color by a factor
	@param HSLA A table of HSL and optionally an A value
	@param factor The shifting factor
	@ret The modified HSL[A] value
*/
static int color_hue(lua_State *L) {
	double hsla[] = {0, 0, 0, 1};
	int n = 4;
	if(
		lua::to_vec_n<double>(L, 1, n, hsla) &&
		lua::is<float>(L, 2)
	) {
		float factor = lua::to<float>(L, 2);
		hsla[0] += factor;
		lua::push_vec_t(L, MAX(3, n), hsla);
	}
	return 1;
}

/*!	Scale the saturation part of an HSL[A] color by a factor
	@param HSLA A table of HSL and optionally an A value
	@param factor The scaling factor
	@ret The modified HSL[A] value
*/
static int color_saturate(lua_State *L) {
	double hsla[] = {0, 0, 0, 1};
	int n = 4;
	if(
		lua::to_vec_n<double>(L, 1, n, hsla) &&
		lua::is<float>(L, 2)
	) {
		float factor = lua::to<float>(L, 2);
		hsla[1] *= factor;
		lua::push_vec_t(L, MAX(3, n), hsla);
	}
	return 1;
}


/*!	Scale the lightness part of an HSL[A] color by a factor
	@param HSLA A table of HSL and optionally an A value
	@param factor The scaling factor
	@ret The modified HSL[A] value
*/
static int color_lighten(lua_State *L) {
	double hsla[] = {0, 0, 0, 1};
	int n = 4;
	if(
		lua::to_vec_n<double>(L, 1, n, hsla) &&
		lua::is<float>(L, 2)
	) {
		float factor = lua::to<float>(L, 2);
		hsla[2] *= factor;
		lua::push_vec_t(L, MAX(3, n), hsla);
	}
	return 1;
}

/*! Convert hex color representation into float RGB[A] format
	The hex color representation can have 3, 4, 6, or 8 values.  If 
	it has 4 or 8 values, the last color component is alpha.  The hex 
	representation can optionally be preceded by a '#'.
	@param HEX color string 
	@ret RGB[A] value
*/
static int color_HEXtoRGB(lua_State *L) {
	const char *hex = luaL_checkstring(L, 1);
	double rgba[] = {0, 0, 0, 0};
	int irgba[] = {0, 0, 0, 0};
	if(hex[0] == '#') {
		hex++;
	}
	int percomponent = 1;
	int n = 3;
	int hexlen = strlen(hex);
	switch(hexlen) {
		case 3:
			// use defaults
			break;
			
		case 4:
			n = 4;
			break;
			
		case 6:
			percomponent = 2;
			break;
			
		case 8:
			n = 4;
			percomponent = 2;
			break;
			
		default:
			luaL_error(L, "color.HEXtoRGB: invalid hex format");
	}
	
	for(int i=0; i < hexlen; i++) {
		int component = i/percomponent;
		int exponent = i%percomponent;
		int factor = 1;
		if(exponent == 0) {
			factor = 16;
		}
		
		int v = 0;
		switch(hex[i]) {
			case '0': v = 0; break;
			case '1': v = 1; break;
			case '2': v = 2; break;
			case '3': v = 3; break;
			case '4': v = 4; break;
			case '5': v = 5; break;
			case '6': v = 6; break;
			case '7': v = 7; break;
			case '8': v = 8; break;
			case '9': v = 9; break;
			case 'a': v = 10; break;
			case 'A': v = 10; break;
			case 'b': v = 11; break;
			case 'B': v = 11; break;
			case 'c': v = 12; break;
			case 'C': v = 12; break;
			case 'd': v = 13; break;
			case 'D': v = 13; break;
			case 'e': v = 14; break;
			case 'E': v = 14; break;
			case 'f': v = 15; break;
			case 'F': v = 15; break;
		}
		
		irgba[component] += factor*v;
	}
	
	for(int i=0; i < n; i++) {
		rgba[i] = ((double)irgba[i])/255.;
	}
	
	lua::push_vec_t(L, n, rgba);
	return 1;
}


/*! Convert RGB[A] [0, 1] to HSL[A] [0, 1]
	If the input has an alpha channel, it will be appended to output
	@param ... A table or unpacked list of RGB[A] values
	@ret Table of HSL[A] values
*/
static int color_RGBtoHSL(lua_State *L) {
	double rgba[] = {0, 0, 0, 1};
	int n = 4;
	if(lua::to_vec_n<double>(L, 1, n, rgba)) {
		double hsla[] = {0, 0, 0, 1};
		ccRGBtoHSL(
			rgba[0], rgba[1], rgba[2], 
			hsla, hsla+1, hsla+2
		);
		hsla[3] = rgba[3];
		lua::push_vec_t(L, MAX(3, n), hsla);
	}
	return 1;
}

/*! Convert HSL[A] [0, 1] to RGB[A] [0, 1]
	If the input has an alpha channel, it will be appended to the output
	@param ... A table or unpacked list of HSL[A] values
	@ret Table of RGB[A] values
*/
static int color_HSLtoRGB(lua_State *L) {
	double hsla[] = {0, 0, 0, 1};
	int n = 4;
	if(lua::to_vec_n<double>(L, 1, n, hsla)) {
		double rgba[] = {0, 0, 0, 1};
		hsla[0] = fmodf(hsla[0], 1.);
		ccHSLtoRGB(
			hsla[0], hsla[1], hsla[2], 
			rgba, rgba+1, rgba+2
		);
		rgba[3] = hsla[3];
		lua::push_vec_t(L, MAX(3, n), rgba);
	}
	return 1;
}


/*! Convert RGB[A] [0, 1] to CMY[A] [0, 1]
	If the input has an alpha channel, it will be appended to output
	@param ... A table or unpacked list of RGB[A] values
	@ret Table of CMY[A] values
*/
static int color_RGBtoCMY(lua_State *L) {
	double rgba[] = {0, 0, 0, 1};
	int n = 4;
	if(lua::to_vec_n<double>(L, 1, n, rgba)) {
		double cmya[] = {0, 0, 0, 1};
		ccfRGBtoCMY(
			rgba[0], rgba[1], rgba[2], 
			cmya, cmya+1, cmya+2
		);
		cmya[3] = rgba[3];
		lua::push_vec_t(L, MAX(3, n), cmya);
	}
	return 1;
}

/*! Convert CMY[A] [0, 1] to RGB[A] [0, 1]
	If the input has an alpha channel, it will be appended to the output
	@param ... A table or unpacked list of CMY[A] values
	@ret Table of RGB[A] values
*/
static int color_CMYtoRGB(lua_State *L) {
	double cmya[] = {0, 0, 0, 1};
	int n = 4;
	if(lua::to_vec_n<double>(L, 1, n, cmya)) {
		double rgba[] = {0, 0, 0, 1};
		ccHSLtoRGB(
			cmya[0], cmya[1], cmya[2], 
			rgba, rgba+1, rgba+2
		);
		rgba[3] = cmya[3];
		lua::push_vec_t(L, MAX(3, n), rgba);
	}
	return 1;
}

/*! Convert CMY [0, 1] to CMYK [0, 1]
	@param ... table or unpacked list of CMY values
	@ret Table of CMYK values
*/
static int color_CMYtoCMYK(lua_State *L) {
	double cmy[] = {0, 0, 0};
	if(lua::to_vec<double>(L, 1, 3, cmy)) {
		double cmyk[] = {0, 0, 0, 0};
		ccCMYtoCMYK(
			cmy[0], cmy[1], cmy[2],
			cmyk, cmyk+1, cmyk+2, cmyk+3
		);
		lua::push_vec_t(L, 4, cmyk);
	}
	return 1;
}

/*! Convert CMYK [0, 1] to CMY [0, 1]
	@param ... table or unpacked list of CMYK values
	@ret Table of CMY values
*/
static int color_CMYKtoCMY(lua_State *L) {
	double cmyk[] = {0, 0, 0, 0};
	if(lua::to_vec<double>(L, 1, 4, cmyk)) {
		double cmy[] = {0, 0, 0};
		ccCMYKtoCMY(
			cmyk[0], cmyk[1], cmyk[2], cmyk[3],
			cmy, cmy+1, cmy+2
		);
		lua::push_vec_t(L, 3, cmy);
	}
	return 1;
}

/*! Convert RGB[A] [0, 1] to HSV[A] [0, 1]
	If the input has an alpha channel, it will be appended to output
	@param ... A table or unpacked list of RGB[A] values
	@ret Table of HSV[A] values
*/
static int color_RGBtoHSV(lua_State *L) {
	double rgba[] = {0, 0, 0, 1};
	int n = 4;
	if(lua::to_vec_n<double>(L, 1, n, rgba)) {
		double hsva[] = {0, 0, 0, 1};
		ccRGBtoHSV(
			rgba[0], rgba[1], rgba[2], 
			hsva, hsva+1, hsva+2
		);
		hsva[3] = rgba[3];
		lua::push_vec_t(L, MAX(3, n), hsva);
	}
	return 1;
}

/*! Convert HSV[A] [0, 1] to RGB[A] [0, 1]
	If the input has an alpha channel, it will be appended to the output
	@param ... A table or unpacked list of HSV[A] values
	@ret Table of RGB[A] values
*/
static int color_HSVtoRGB(lua_State *L) {
	double hsva[] = {0, 0, 0, 1};
	int n = 4;
	if(lua::to_vec_n<double>(L, 1, n, hsva)) {
		double rgba[] = {0, 0, 0, 1};
		ccHSLtoRGB(
			hsva[0], hsva[1], hsva[2], 
			rgba, rgba+1, rgba+2
		);
		rgba[3] = hsva[3];
		lua::push_vec_t(L, MAX(3, n), rgba);
	}
	return 1;
}

/*! Convert XYZ [0, 1] to Hunter Lab [0, 1]
	@param ... table or unpacked list of XYZ values
	@ret Table of Hunter Lab values
*/
static int color_XYZtoHunterLab(lua_State *L) {
	double xyz[] = {0, 0, 0};
	if(lua::to_vec<double>(L, 1, 3, xyz)) {
		double Lab[] = {0, 0, 0, 0};
		ccXYZtoHunterLab(
			xyz[0], xyz[1], xyz[2],
			Lab, Lab+1, Lab+2
		);
		lua::push_vec_t(L, 3, Lab);
	}
	return 1;
}


/*! Convert Hunter Lab [0, 1] to XYZ [0, 1]
	@param ... table or unpacked list of Hunter Lab values
	@ret Table of XYZ values
*/
static int color_HunterLabtoXYZ(lua_State *L) {
	double Lab[] = {0, 0, 0};
	if(lua::to_vec<double>(L, 1, 3, Lab)) {
		double xyz[] = {0, 0, 0};
		ccHunterLabtoXYZ(
			Lab[0], Lab[1], Lab[2],
			xyz, xyz+1, xyz+2
		);
		lua::push_vec_t(L, 3, xyz);
	}
	return 1;
}


/*! Convert XYZ [0, 1] to Yxy [0, 1]
	@param ... table or unpacked list of XYZ values
	@ret Table of Yxy values
*/
static int color_XYZtoYxy(lua_State *L) {
	double xyz[] = {0, 0, 0};
	if(lua::to_vec<double>(L, 1, 3, xyz)) {
		double Yxy[] = {0, 0, 0};
		ccXYZtoYxy(
			xyz[0], xyz[1], xyz[2],
			Yxy, Yxy+1, Yxy+2
		);
		lua::push_vec_t(L, 3, Yxy);
	}
	return 1;
}

/*! Convert Yxy [0, 1] to XYZ [0, 1]
	@param ... table or unpacked list of Yxy values
	@ret Table of XYZ values
*/
static int color_YxytoXYZ(lua_State *L) {
	double Yxy[] = {0, 0, 0};
	if(lua::to_vec<double>(L, 1, 3, Yxy)) {
		double xyz[] = {0, 0, 0};
		ccYxytoXYZ(
			Yxy[0], Yxy[1], Yxy[2],
			xyz, xyz+1, xyz+2
		);
		lua::push_vec_t(L, 3, xyz);
	}
	return 1;
}

/*! Convert XYZ [0, 1] to RGB [0, 1]
	@param ... table or unpacked list of RGB values
	@ret Table of RGB values
*/
static int color_XYZtoRGB(lua_State *L) {
	double xyzT[] = {0, 0, 0, 0};
	int n = 4;
	if(lua::to_vec_n<double>(L, 1, n, xyzT)) {
		xyzT[3] = get_tristimulus(n, xyzT[3]);
	
		double rgb[] = {0, 0, 0};
		ccXYZtoRGB(
			xyzT[0], xyzT[1], xyzT[2],
			rgb, rgb+1, rgb+2, 
			(char)xyzT[3]
		);
		lua::push_vec_t(L, 3, rgb);
	}
	return 1;
}

/*! Convert RGB [0, 1] to XYZ [0, 1]
	@param ... table or unpacked list of RGB values
	@ret Table of XYZ values
*/
static int color_RGBtoXYZ(lua_State *L) {
	double rgbT[] = {0, 0, 0, 0.};
	int n = 4;
	if(lua::to_vec_n<double>(L, 1, n, rgbT)) {
		rgbT[3] = get_tristimulus(n, rgbT[3]);
		double xyz[] = {0, 0, 0};
		ccRGBtoXYZ(
			rgbT[0], rgbT[1], rgbT[2],
			xyz, xyz+1, xyz+2, 
			(char)rgbT[3]
		);
		lua::push_vec_t(L, 3, xyz);
	}
	return 1;
}

/*! Convert XYZ [0, 1] to CIE Lab [0, 1]
	@param ... table or unpacked list of XYZ values
	@ret Table of CIE Lab values
*/
static int color_XYZtoCIE_Lab(lua_State *L) {
	double xyzT[] = {0, 0, 0, 0};
	int n = 4;
	if(lua::to_vec_n<double>(L, 1, n, xyzT)) {
		xyzT[3] = get_tristimulus(n, xyzT[3]);
	
		double Lab[] = {0, 0, 0};
		ccXYZtoCIE_Lab(
			xyzT[0], xyzT[1], xyzT[2],
			Lab, Lab+1, Lab+2, 
			(char)xyzT[3]
		);
		lua::push_vec_t(L, 3, Lab);
	}
	return 1;
}

/*! Convert CIE Lab [0, 1] to XYZ [0, 1]
	@param ... table or unpacked list of CIE Lab values
	@ret Table of XYZ values
*/
static int color_CIE_LabtoXYZ(lua_State *L) {
	double LabT[] = {0, 0, 0, 0.};
	int n = 4;
	if(lua::to_vec_n<double>(L, 1, n, LabT)) {
		LabT[3] = get_tristimulus(n, LabT[3]);
		double xyz[] = {0, 0, 0};
		ccCIE_LabtoXYZ(
			LabT[0], LabT[1], LabT[2],
			xyz, xyz+1, xyz+2, 
			(char)LabT[3]
		);
		lua::push_vec_t(L, 3, xyz);
	}
	return 1;
}

/*! Convert CIE Lab [0, 1] to CIE LCH [0, 1]
	@param ... A table or unpacked list of CIE Lab values
	@ret Table of CIE LCH values
*/
static int color_CIE_LabtoCIE_LCH(lua_State *L) {
	double Lab[] = {0, 0, 0};
	if(lua::to_vec<double>(L, 1, 3, Lab)) {
		double lch[] = {0, 0, 0};
		ccCIE_LabtoCIE_LCH(
			Lab[0], Lab[1], Lab[2], 
			lch, lch+1, lch+2
		);
		lua::push_vec_t(L, 3, lch);
	}
	return 1;
}

/*! Convert CIE LCH [0, 1] to CIE Lab [0, 1]
	@param ... A table or unpacked list of CIE LCH values
	@ret Table of CIE Lab values
*/
static int color_CIE_LCHtoCIE_Lab(lua_State *L) {
	double lch[] = {0, 0, 0};
	if(lua::to_vec<double>(L, 1, 3, lch)) {
		double Lab[] = {0, 0, 0};
		ccCIE_LCHtoCIE_Lab(
			lch[0], lch[1], lch[2], 
			Lab, Lab+1, Lab+2
		);
		lua::push_vec_t(L, 3, Lab);
	}
	return 1;
}

/*! Convert XYZ [0, 1] to CIE Luv [0, 1]
	@param ... table or unpacked list of XYZ values
	@ret Table of CIE Luv values
*/
static int color_XYZtoCIE_Luv(lua_State *L) {
	double xyzT[] = {0, 0, 0, 0};
	int n = 4;
	if(lua::to_vec_n<double>(L, 1, n, xyzT)) {
		xyzT[3] = get_tristimulus(n, xyzT[3]);
	
		double Luv[] = {0, 0, 0};
		ccXYZtoCIE_Luv(
			xyzT[0], xyzT[1], xyzT[2],
			Luv, Luv+1, Luv+2, 
			(char)xyzT[3]
		);
		lua::push_vec_t(L, 3, Luv);
	}
	return 1;
}

/*! Convert CIE Luv [0, 1] to XYZ [0, 1]
	@param ... table or unpacked list of CIE Luv values
	@ret Table of XYZ values
*/
static int color_CIE_LuvtoXYZ(lua_State *L) {
	double LuvT[] = {0, 0, 0, 0.};
	int n = 4;
	if(lua::to_vec_n<double>(L, 1, n, LuvT)) {
		LuvT[3] = get_tristimulus(n, LuvT[3]);
		double xyz[] = {0, 0, 0};
		ccCIE_LuvtoXYZ(
			LuvT[0], LuvT[1], LuvT[2],
			xyz, xyz+1, xyz+2, 
			(char)LuvT[3]
		);
		lua::push_vec_t(L, 3, xyz);
	}
	return 1;
}

typedef struct _color_name {
	const char *name;
	double vals[3];
} t_color_name;


static t_color_name color_names[] = {
	{"aliceblue", {0.941176, 0.972549, 1.000000}},
	{"antiquewhite", {0.980392, 0.921569, 0.843137}},
	{"aquamarine", {0.498039, 1.000000, 0.831373}},
	{"azure", {0.941176, 1.000000, 1.000000}},
	{"beige", {0.960784, 0.960784, 0.862745}},
	{"bisque", {1.000000, 0.894118, 0.768627}},
	{"black", {0., 0., 0.}},
	{"blanchedalmond", {1.000000, 0.921569, 0.803922}},
	{"blueviolet", {0.541176, 0.168627, 0.886275}},
	{"brown", {0.647059, 0.164706, 0.164706}},
	{"burlywood", {0.870588, 0.721569, 0.529412}},
	{"chartreuse", {0.498039, 1.000000, 0.000000}},
	{"chocolate", {0.823529, 0.411765, 0.117647}},
	{"coral", {1.000000, 0.498039, 0.313725}},
	{"cornflowerblue", {0.392157, 0.584314, 0.929412}},
	{"cornsilk", {1.000000, 0.972549, 0.862745}},
	{"crimson", {0.862745, 0.078431, 0.235294}},
	{"darkgoldenrod", {0.721569, 0.525490, 0.043137}},
	{"darkgray", {0.662745, 0.662745, 0.662745}},
	{"darkgrey", {0.662745, 0.662745, 0.662745}},
	{"darkkhaki", {0.741176, 0.717647, 0.419608}},
	{"darkmagenta", {0.545098, 0.000000, 0.545098}},
	{"darkorange", {1.000000, 0.549020, 0.000000}},
	{"darkorchid", {0.600000, 0.196078, 0.800000}},
	{"darkred", {0.545098, 0.000000, 0.000000}},
	{"darksalmon", {0.913725, 0.588235, 0.478431}},
	{"darkseagreen", {0.560784, 0.737255, 0.560784}},
	{"darkviolet", {0.580392, 0.000000, 0.827451}},
	{"deeppink", {1.000000, 0.078431, 0.576471}},
	{"dimgray", {0.411765, 0.411765, 0.411765}},
	{"dimgrey", {0.411765, 0.411765, 0.411765}},
	{"firebrick", {0.698039, 0.133333, 0.133333}},
	{"floralwhite", {1.000000, 0.980392, 0.941176}},
	{"fuchsia", {1.000000, 0.000000, 1.000000}},
	{"gainsboro", {0.862745, 0.862745, 0.862745}},
	{"ghostwhite", {0.972549, 0.972549, 1.000000}},
	{"gold", {1.000000, 0.843137, 0.000000}},
	{"goldenrod", {0.854902, 0.647059, 0.125490}},
	{"gray", {0.501961, 0.501961, 0.501961}},
	{"grey", {0.501961, 0.501961, 0.501961}},
	{"greenyellow", {0.678431, 1.000000, 0.184314}},
	{"honeydew", {0.941176, 1.000000, 0.941176}},
	{"hotpink", {1.000000, 0.411765, 0.705882}},
	{"indianred", {0.803922, 0.360784, 0.360784}},
	{"ivory", {1.000000, 1.000000, 0.941176}},
	{"khaki", {0.941176, 0.901961, 0.549020}},
	{"lavender", {0.901961, 0.901961, 0.980392}},
	{"lavenderblush", {1.000000, 0.941176, 0.960784}},
	{"lawngreen", {0.486275, 0.988235, 0.000000}},
	{"lemonchiffon", {1.000000, 0.980392, 0.803922}},
	{"lightblue", {0.678431, 0.847059, 0.901961}},
	{"lightcoral", {0.941176, 0.501961, 0.501961}},
	{"lightcyan", {0.878431, 1.000000, 1.000000}},
	{"lightgoldenrodyellow", {0.980392, 0.980392, 0.823529}},
	{"lightgray", {0.827451, 0.827451, 0.827451}},
	{"lightgreen", {0.564706, 0.933333, 0.564706}},
	{"lightgrey", {0.827451, 0.827451, 0.827451}},
	{"lightpink", {1.000000, 0.713725, 0.756863}},
	{"lightsalmon", {1.000000, 0.627451, 0.478431}},
	{"lightskyblue", {0.529412, 0.807843, 0.980392}},
	{"lightslategray", {0.466667, 0.533333, 0.600000}},
	{"lightslategrey", {0.466667, 0.533333, 0.600000}},
	{"lightsteelblue", {0.690196, 0.768627, 0.870588}},
	{"lightyellow", {1.000000, 1.000000, 0.878431}},
	{"linen", {0.980392, 0.941176, 0.901961}},
	{"magenta", {1.000000, 0.000000, 1.000000}},
	{"maroon", {0.501961, 0.000000, 0.000000}},
	{"mediumaquamarine", {0.400000, 0.803922, 0.666667}},
	{"mediumorchid", {0.729412, 0.333333, 0.827451}},
	{"mediumpurple", {0.576471, 0.439216, 0.858824}},
	{"mediumslateblue", {0.482353, 0.407843, 0.933333}},
	{"mediumvioletred", {0.780392, 0.082353, 0.521569}},
	{"mintcream", {0.960784, 1.000000, 0.980392}},
	{"mistyrose", {1.000000, 0.894118, 0.882353}},
	{"moccasin", {1.000000, 0.894118, 0.709804}},
	{"navajowhite", {1.000000, 0.870588, 0.678431}},
	{"oldlace", {0.992157, 0.960784, 0.901961}},
	{"olive", {0.501961, 0.501961, 0.000000}},
	{"olivedrab", {0.419608, 0.556863, 0.137255}},
	{"orange", {1.000000, 0.647059, 0.000000}},
	{"orangered", {1.000000, 0.270588, 0.000000}},
	{"orchid", {0.854902, 0.439216, 0.839216}},
	{"palegoldenrod", {0.933333, 0.909804, 0.666667}},
	{"palegreen", {0.596078, 0.984314, 0.596078}},
	{"paleturquoise", {0.686275, 0.933333, 0.933333}},
	{"palevioletred", {0.858824, 0.439216, 0.576471}},
	{"papayawhip", {1.000000, 0.937255, 0.835294}},
	{"peachpuff", {1.000000, 0.854902, 0.725490}},
	{"peru", {0.803922, 0.521569, 0.247059}},
	{"pink", {1.000000, 0.752941, 0.796078}},
	{"plum", {0.866667, 0.627451, 0.866667}},
	{"powderblue", {0.690196, 0.878431, 0.901961}},
	{"purple", {0.501961, 0.000000, 0.501961}},
	{"red", {1.000000, 0.000000, 0.000000}},
	{"rosybrown", {0.737255, 0.560784, 0.560784}},
	{"saddlebrown", {0.545098, 0.270588, 0.074510}},
	{"salmon", {0.980392, 0.501961, 0.447059}},
	{"sandybrown", {0.956863, 0.643137, 0.376471}},
	{"seashell", {1.000000, 0.960784, 0.933333}},
	{"sienna", {0.627451, 0.321569, 0.176471}},
	{"silver", {0.752941, 0.752941, 0.752941}},
	{"skyblue", {0.529412, 0.807843, 0.921569}},
	{"slateblue", {0.415686, 0.352941, 0.803922}},
	{"slategray", {0.439216, 0.501961, 0.564706}},
	{"slategrey", {0.439216, 0.501961, 0.564706}},
	{"snow", {1.000000, 0.980392, 0.980392}},
	{"tan", {0.823529, 0.705882, 0.549020}},
	{"thistle", {0.847059, 0.749020, 0.847059}},
	{"tomato", {1.000000, 0.388235, 0.278431}},
	{"violet", {0.933333, 0.509804, 0.933333}},
	{"wheat", {0.960784, 0.870588, 0.701961}},
	{"white", {1.000000, 1.000000, 1.000000}},
	{"whitesmoke", {0.960784, 0.960784, 0.960784}},
	{"yellow", {1.000000, 1.000000, 0.000000}},
	{"yellowgreen", {0.603922, 0.803922, 0.196078}},
	{NULL, {0, 0, 0}}
};


#define COLOR_METHOD(name)	{#name, color_##name}

luaL_reg color_lib[] = {
	COLOR_METHOD(hue),
	COLOR_METHOD(saturate),
	COLOR_METHOD(lighten),

	COLOR_METHOD(HEXtoRGB),
	COLOR_METHOD(RGBtoHSL),
	COLOR_METHOD(HSLtoRGB),
	COLOR_METHOD(RGBtoCMY),
	COLOR_METHOD(CMYtoRGB),
	COLOR_METHOD(CMYtoCMYK),
	COLOR_METHOD(CMYKtoCMY),
	COLOR_METHOD(RGBtoHSV),
	COLOR_METHOD(HSVtoRGB),
	COLOR_METHOD(XYZtoHunterLab),
	COLOR_METHOD(HunterLabtoXYZ),
	COLOR_METHOD(XYZtoYxy),
	COLOR_METHOD(YxytoXYZ),
	COLOR_METHOD(XYZtoRGB),
	COLOR_METHOD(RGBtoXYZ),
	COLOR_METHOD(XYZtoCIE_Lab),
	COLOR_METHOD(CIE_LabtoXYZ),
	COLOR_METHOD(CIE_LabtoCIE_LCH),
	COLOR_METHOD(CIE_LCHtoCIE_Lab),
	COLOR_METHOD(XYZtoCIE_Luv),
	COLOR_METHOD(CIE_LuvtoXYZ),
	{0, 0}
};

#ifdef __cplusplus
extern "C" {
#endif

/*!
	Color module entry point
*/
int luaopen_color(lua_State *L) {
	const char * libname = lua_tostring(L, 1);
	luaL_register(L, libname, color_lib);
	
	for(int i=0; color_names[i].name; i++) {
		lua::push_vec_t<double>(L, 3, color_names[i].vals);
		lua_setfield(L, -2, color_names[i].name);
	}
	
	return 1;
}

#ifdef __cplusplus
}
#endif