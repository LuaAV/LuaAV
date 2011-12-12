#include"cc.h"
#include<assert.h>
#include<math.h>


// 2 degrees D65 is the default

/*
 *	Tristimulus reference values
 */
const double _ccTristimulusValues[ccNTristimulus][3] = 
{
      /* 2 degrees */
  /*ccTA_2*/{109.850, 100.000, 35.585},		// Incandescent
  /*ccTC_2*/{98.074,  100.000, 118.232},
  /* D50 */ {96.422,  100.000, 82.521},
  /* D55 */ {95.682,  100.000, 92.149},
  /* D65 */ {95.047,  100.000, 108.883},	// Daylight
  /* D75 */ {94.972,  100.000, 122.638},
  /* F2 */  {99.187,  100.000, 67.395},
  /* F7 */  {95.044,  100.000, 108.755},	// Fluorescent
  /* F11 */ {100.966, 100.000, 64.370},

      /* 10 degrees */
  /* A */   {111.144, 100.000, 35.200},		// Incandescent
  /* C */   {97.285,  100.000, 116.145},
  /* D50 */ {96.720,  100.000, 81.427},
  /* D55 */ {95.799,  100.000, 90.926},
  /* D65 */ {94.811,  100.000, 107.304},	// Daylight
  /* D75 */ {94.416,  100.000, 120.641},
  /* F2 */  {103.280, 100.000, 69.026},		// Fluorescent
  /* F7 */  {95.792,  100.000, 107.687},
  /* F11 */ {103.866, 100.000, 65.627}
};



static double ccmin(double val1, double val2, double val3)
{
  if (val1 < val2)
  {
    if(val1 < val3) return val1;
    else return val3;
  }
  else if (val2 < val3)
    return val2;
  return val3;
}

static double ccmax(double val1, double val2, double val3)
{
  if (val1 > val2)
  {
    if(val1 > val3) return val1;
    else return val3;
  }
  else if (val2 > val3)
    return val2;
  return val3;
}

static double Hue_2_RGB( double v1, double v2, double vH )
{
   if ( vH < 0 ) vH += 1;
   if ( vH > 1 ) vH -= 1;
   if ( ( 6 * vH ) < 1 ) return ( v1 + ( v2 - v1 ) * 6 * vH );
   if ( ( 2 * vH ) < 1 ) return ( v2 );
   if ( ( 3 * vH ) < 2 ) return ( v1 + ( v2 - v1 ) * ( ( 2./3 ) - vH ) * 6 );
   return ( v1 );
}

static double degree_2_radian(double val)
{
  return (val*PI)/180;
}

//RGB values = From 0 to 1
int ccRGBtoHSL(double r, double g, double b, double* h, double* s, double* l)
{
  double vmin, vmax, delta;
  double dr,dg,db;

  vmin = ccmin( r, g, b );                // Min. value of RGB
  vmax = ccmax( r, g, b );                // Max. value of RGB
  delta = vmax - vmin;                    // Delta RGB value

  *l = ( vmax + vmin ) / 2;

  if ( delta == 0 )                     // This is a gray, no chroma...
  {
     *h = 0;                            // HSL results = From 0 to 1
     *s = 0;
  }
  else                                  // Chromatic data...
  {
     if ( *l < 0.5 ) *s = delta / ( vmax + vmin );
     else            *s = delta / ( 2 - vmax - vmin );

     dr = ( ( ( vmax - r ) / 6 ) + ( delta / 2 ) ) / delta;
     dg = ( ( ( vmax - g ) / 6 ) + ( delta / 2 ) ) / delta;
     db = ( ( ( vmax - b ) / 6 ) + ( delta / 2 ) ) / delta;

     if      ( r == vmax ) *h = db - dg;
     else if ( g == vmax ) *h = ( 1./3 ) + dr - db;
     else if ( b == vmax ) *h = ( 2./3 ) + dg - dr;

     if ( *h < 0 ) *h += 1;
     if ( *h > 1 ) *h -= 1;
  }

  return 1;
}

int ccHSLtoRGB(double h, double s, double l, double* r, double* g, double* b)
{
  double v1, v2;

  if ( s == 0 )                       // HSL values = From 0 to 1
  {
     *r = l;						// RGB results = From 0 to 1
     *g = l;
     *b = l;
  }
  else
  {
     if ( l < 0.5 ) 
       v2 = l * ( 1 + s );
     else           
       v2 = ( l + s ) - ( s * l );

     v1 = 2 * l - v2;

     *r = Hue_2_RGB( v1, v2, h + ( 1./3 ) );
     *g = Hue_2_RGB( v1, v2, h );
     *b = Hue_2_RGB( v1, v2, h - ( 1./3 ) );
  } 

  return 1;

}


// RGB values = From 0 to 1
int ccfRGBtoCMY(double r, double g, double b, double* c, double* m, double* y)
{
  assert(c && m && y);
  *c = 1 - r;
  *m = 1 - g;
  *y = 1 - b;

  return 1;
}

// RGB values = From 0 to 255
int cciRGBtoCMY(int r, int g, int b, double* c, double* m, double* y)
{
  assert(c && m && y);
  assert(r >= 0 && r < 256);
  assert(g >= 0 && g < 256);
  assert(b >= 0 && b < 256);

  *c = 1 - ( (double)r / 255 );
  *m = 1 - ( (double)g / 255 );
  *y = 1 - ( (double)b / 255 );

  return 1;
}

// CMY values = From 0 to 1
int ccCMYtofRGB(double c, double m, double y, double* r, double* g, double* b)
{
  assert(r && g && b);
  assert(c >= 0 && c <= 1);
  assert(m >= 0 && m <= 1);
  assert(y >= 0 && y <= 1);

  *r = ( 1 - c );
  *g = ( 1 - m );
  *b = ( 1 - y );

  return 1;

}
// CMY values = From 0 to 255
int ccCMYtoiRGB(double c, double m, double y, int* r, int* g, int* b)
{
  assert(r && g && b);
  assert(c >= 0 && c <= 1);
  assert(m >= 0 && m <= 1);
  assert(y >= 0 && y <= 1);

  *r = (int)( 1 - c ) * 255;
  *g = (int)( 1 - m ) * 255;
  *b = (int)( 1 - y ) * 255;

  return 1;
}


// CMY values = From 0 to 1
int ccCMYtoCMYK(double c, double m, double y, double* C, double* M, double* Y, double* K)
{
  double var_K = 1;

  assert(C && M && Y && K);
  assert(c >= 0 && c <= 1);
  assert(m >= 0 && m <= 1);
  assert(y >= 0 && y <= 1);

  if ( c < var_K )
    var_K = c;
  if ( m < var_K )
    var_K = m;
  if ( y < var_K )
    var_K = y;

  *C = ( c - var_K ) / ( 1 - var_K );
  *M = ( m - var_K ) / ( 1 - var_K );
  *Y = ( y - var_K ) / ( 1 - var_K );
  *K = var_K;

  return 1;
}

// CMYK values = From 0 to 1
int ccCMYKtoCMY(double c, double m, double y, double k, double* C, double* M, double* Y)
{
  assert(C && M && Y);
  assert(c >= 0 && c <= 1);
  assert(m >= 0 && m <= 1);
  assert(y >= 0 && y <= 1);
  assert(k >= 0 && k <= 1);

  *C = ( c * ( 1 - k ) + k );
  *M = ( m * ( 1 - k ) + k );
  *Y = ( y * ( 1 - k ) + k );
 
  return 1;
}

int ccRGBtoHSV(double r, double g, double b, double* h, double* s, double* v)
{
  double var_Min;
  double var_Max;
  double del_Max;

  var_Min  = ccmin( r, g, b );    //Min. value of RGB
  var_Max  = ccmax( r, g, g );    //Max. value of RGB
  del_Max = var_Max - var_Min;    //Delta RGB value 

  *v = var_Max;

  if ( del_Max == 0 )                      // This is a gray, no chroma...
  {
     *h = 0;                               // HSV results = From 0 to 1
     *s = 0;
  }
  else                                     // Chromatic data...
  {
     double del_R = ( ( ( var_Max - r ) / 6 ) + ( del_Max / 2 ) ) / del_Max;
     double del_G = ( ( ( var_Max - g ) / 6 ) + ( del_Max / 2 ) ) / del_Max;
     double del_B = ( ( ( var_Max - b ) / 6 ) + ( del_Max / 2 ) ) / del_Max;

     *s = del_Max / var_Max;


     if      ( r == var_Max ) *h = del_B - del_G;
     else if ( g == var_Max ) *h = ( 1./3 ) + del_R - del_B;
     else if ( g == var_Max ) *h = ( 2./3 ) + del_G - del_R;

     if ( *h < 0 ) ; *h += 1;
     if ( *h > 1 ) ; *h -= 1;
  }

  return 1;
}

int ccHSVtoRGB(double h, double s, double v, double* r, double* g, double* b)
{
  if ( s == 0 )                       // HSV values = From 0 to 1
  {
     *r = v;
     *g = v;
     *b = v;
  }
  else
  {
     double var_h = h * 6;
     double var_i = floor( var_h );
     double var_1 = v * ( 1 - s );
     double var_2 = v * ( 1 - s * ( var_h - var_i ) );
     double var_3 = v * ( 1 - s * ( 1 - ( var_h - var_i ) ) );

     if      ( var_i == 0 ) { *r = v     ; *g = var_3 ; *b = var_1; }
     else if ( var_i == 1 ) { *r = var_2 ; *g = v     ; *b = var_1; }
     else if ( var_i == 2 ) { *r = var_1 ; *g = v     ; *b = var_3; }
     else if ( var_i == 3 ) { *r = var_1 ; *g = var_2 ; *b = v;     }
     else if ( var_i == 4 ) { *r = var_3 ; *g = var_1 ; *b = v;     }
     else                   { *r = v     ; *g = var_1 ; *b = var_2; }

  }

  return 1;
}

int ccXYZtoHunterLab(double x, double y, double z, double* L, double* a, double* b)
{
  *L = 10 * sqrt( y );
  *a = 17.5 * ( ( ( 1.02 * x ) - y ) / sqrt( y ) );
  *b = 7 * ( ( y - ( 0.847 * z ) ) / sqrt( y ) );

  return 1;
}

int ccHunterLabtoXYZ(double L, double a, double b, double* x, double* y, double* z)
{
  double var_Y = L / 10;
  double var_X = a / 17.5 * L / 10;
  double var_Z = b / 7 * L / 10;

  *y = pow(var_Y,2);
  *x = ( var_X + *y ) / 1.02;
  *z = -( var_Z - *y ) / 0.847;

  return 1;
}

int ccXYZtoYxy(double x, double y, double z, double* rY, double* rx, double* ry)
{
  *rY = y;
  *rx = x / ( x + y + z );
  *ry = y / ( x + y + z );

  return 1;
}

int ccYxytoXYZ(double Y, double x, double y, double* rx, double* ry, double* rz)
{
  //Y = From 0 to 100
  //x = From 0 to 1
  //y = From 0 to 1

  *rx = x * ( Y / y );
  *ry = Y;
  *rz = ( 1 - x - y ) * ( Y / y );

  return 1;
}

int ccFixRGB(double* r, double* g, double* b)
{
    double min,max;
    int mod=0;
    min=(*r>*g)?*g:*r;
    min=(min>*b)?*b:min;
    if (min<0) {*r+=-min; *g+=-min; *b+=-min; mod=1;}
    max=(*r>*g)?*r:*g;
    max=(max>*b)?max:*b;
    if (max>1){*r/=max; *g/=max; *b/=max; mod=1;}
    return mod;
}

//r,g,b from 0 to 1
int ccXYZtoRGB(double x, double y, double z, double* r, double* g, double* b, char ObsIll)
{
  double ref_X = _ccTristimulusValues[ObsIll][ccX];
  double ref_Y = _ccTristimulusValues[ObsIll][ccY];
  double ref_Z = _ccTristimulusValues[ObsIll][ccZ];

  double var_X = x / ref_X;        //X = From 0 to ref_X
  double var_Y = y / ref_Y;        //Y = From 0 to ref_Y
  double var_Z = z / ref_Z;        //Z = From 0 to ref_Y

  double var_R = var_X *  3.2406 + var_Y * -1.5372 + var_Z * -0.4986;
  double var_G = var_X * -0.9689 + var_Y *  1.8758 + var_Z *  0.0415;
  double var_B = var_X *  0.0557 + var_Y * -0.2040 + var_Z *  1.0570;

  if ( var_R > 0.0031308 ) var_R = 1.055 * ( pow(var_R,( 1 / 2.4 )) ) - 0.055;
  else                     var_R = 12.92 * var_R;
  if ( var_G > 0.0031308 ) var_G = 1.055 * ( pow(var_G,( 1 / 2.4 )) ) - 0.055;
  else                     var_G = 12.92 * var_G;
  if ( var_B > 0.0031308 ) var_B = 1.055 * ( pow(var_B,( 1 / 2.4 )) ) - 0.055;
  else                     var_B = 12.92 * var_B;

  *r = var_R;
  *g = var_G;
  *b = var_B;

  return 1;
}

int ccRGBtoXYZ(double r, double g, double b, double* x, double* y, double* z, char ObsIll)
{
  double var_R = r;        //R = From 0 to 255
  double var_G = g;        //G = From 0 to 255
  double var_B = b;        //B = From 0 to 255

  if ( var_R > 0.04045 ) var_R = pow(( ( var_R + 0.055 ) / 1.055 ),2.4);
  else                   var_R = var_R / 12.92;
  if ( var_G > 0.04045 ) var_G = pow(( ( var_G + 0.055 ) / 1.055 ),2.4);
  else                   var_G = var_G / 12.92;
  if ( var_B > 0.04045 ) var_B = pow(( ( var_B + 0.055 ) / 1.055 ),2.4);
  else                   var_B = var_B / 12.92;

  var_R = var_R * 100;
  var_G = var_G * 100;
  var_B = var_B * 100;

  *x = var_R * 0.4124 + var_G * 0.3576 + var_B * 0.1805;
  *y = var_R * 0.2126 + var_G * 0.7152 + var_B * 0.0722;
  *z = var_R * 0.0193 + var_G * 0.1192 + var_B * 0.9505;

  return 1;
}

int ccXYZtoCIE_Lab(double x, double y, double z, double* L, double* a, double* b, char ObsIll)
{
  double var_X = x/_ccTristimulusValues[ObsIll][ccX];
  double var_Y = y/_ccTristimulusValues[ObsIll][ccY];
  double var_Z = z/_ccTristimulusValues[ObsIll][ccZ];

  if ( var_X > 0.008856 ) var_X = pow(var_X,( 1./3 ));
  else                    var_X = ( 7.787 * var_X ) + ( 16./ 116 );
  if ( var_Y > 0.008856 ) var_Y = pow(var_Y,( 1./3 ));
  else                    var_Y = ( 7.787 * var_Y ) + ( 16./ 116 );
  if ( var_Z > 0.008856 ) var_Z = pow(var_Z,( 1./3 ));
  else                    var_Z = ( 7.787 * var_Z ) + ( 16./ 116 );

  *L = ( 116 * var_Y ) - 16;
  *a = 500 * ( var_X - var_Y );
  *b = 200 * ( var_Y - var_Z );

  return 1;
}

int ccCIE_LabtoXYZ(double L, double a, double b, double* x, double* y, double* z, char ObsIll)
{
  double var_Y = ( L + 16 ) / 116;
  double var_X = a / 500 + var_Y;
  double var_Z = var_Y - b / 200;

  if ( pow(var_Y,3) > 0.008856 ) var_Y = pow(var_Y,3);
  else                      var_Y = ( var_Y - 16./ 116 ) / 7.787;
  if ( pow(var_X,3) > 0.008856 ) var_X = pow(var_X,3);
  else                      var_X = ( var_X - 16./ 116 ) / 7.787;
  if ( pow(var_Z,3) > 0.008856 ) var_Z = pow(var_Z,3);
  else                      var_Z = ( var_Z - 16./ 116 ) / 7.787;

  *x = _ccTristimulusValues[ObsIll][ccX] * var_X;
  *y = _ccTristimulusValues[ObsIll][ccY] * var_Y;
  *z = _ccTristimulusValues[ObsIll][ccZ] * var_Z;

  return 1;
}

int ccCIE_LabtoCIE_LCH(double L, double a, double b, double* rL, double* rC, double* rH)
{
  double var_H = atan2( b, a );  //Quadrant by signs

  if ( var_H > 0 ) var_H = ( var_H / PI ) * 180;
  else             var_H = 360 - ( (int)var_H / PI ) * 180;

  *rL = L;
  *rC = sqrt( pow(a,2) + pow(b,2) );
  *rH = var_H;

  return 1;
}

int ccCIE_LCHtoCIE_Lab(double L, double C, double H, double* rL, double* ra, double* rb)
{
  //CIE-H° = From 0 to 360°

  *rL = L;
  *ra = cos( degree_2_radian(H) ) * C;
  *rb = sin( degree_2_radian(H) ) * C;

  return 1;
}

int ccXYZtoCIE_Luv(double x, double y, double z, double* L, double* u, double* v, char ObsIll)
{
  double ref_X = _ccTristimulusValues[ObsIll][ccX];
  double ref_Y = _ccTristimulusValues[ObsIll][ccY];
  double ref_Z = _ccTristimulusValues[ObsIll][ccZ];
  
  double ref_U = ( 4 * ref_X ) / ( ref_X + ( 15 * ref_Y ) + ( 3 * ref_Z ) );
  double ref_V = ( 9 * ref_Y ) / ( ref_X + ( 15 * ref_Y ) + ( 3 * ref_Z ) );

  double var_U = ( 4 * x ) / ( x + ( 15 * y ) + ( 3 * z ) );
  double var_V = ( 9 * y ) / ( x + ( 15 * y ) + ( 3 * z ) );
  double var_Y = y / 100;

  
  if ( var_Y > 0.008856 ) var_Y = pow(var_Y,( 1./3 ));
  else                    var_Y = ( 7.787 * var_Y ) + ( 16./ 116 );


  *L = ( 116 * var_Y ) - 16;
  *u = 13 * *L * ( var_U - ref_U );
  *v = 13 * *L * ( var_V - ref_V );

  return 1;
}

int ccCIE_LuvtoXYZ(double L, double u, double v, double* x, double* y, double* z, char ObsIll)
{
  double ref_X = _ccTristimulusValues[ObsIll][ccX];
  double ref_Y = _ccTristimulusValues[ObsIll][ccY];
  double ref_Z = _ccTristimulusValues[ObsIll][ccZ];

  double ref_U = ( 4 * ref_X ) / ( ref_X + ( 15 * ref_Y ) + ( 3 * ref_Z ) );
  double ref_V = ( 9 * ref_Y ) / ( ref_X + ( 15 * ref_Y ) + ( 3 * ref_Z ) );

  double var_U = u / ( 13 * L ) + ref_U;
  double var_V = v / ( 13 * L ) + ref_V;

  double var_Y = ( L + 16 ) / 116;
  
  if ( pow(var_Y,3) > 0.008856 ) var_Y = pow(var_Y,3);
  else                      var_Y = ( var_Y - 16./ 116 ) / 7.787;

  (*y) = var_Y * 100;
  (*x) =  - ( 9 * *y * var_U ) / ( ( var_U - 4 ) * var_V  - var_U * var_V );
  (*z) = ( 9 * *y - ( 15 * var_V * *y ) - ( var_V * *x ) ) / ( 3 * var_V );

  return 1;
}