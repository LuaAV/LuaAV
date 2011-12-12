#ifndef _CC_H_
#define _CC_H_

enum {
  ccTA_2, ccTC_2, D50_2, D55_2, D65_2, D75_2, F2_2, F7_2, F11_2,
  ccTA_10, ccTC_10, D50_10, D55_10, D65_10, D75_10, F2_10, F7_10, F11_10,
  ccNTristimulus
};

enum { ccX, ccY, ccZ};

#define PI 3.14159265

/*
 *	Function Headers
 */

int ccXYZtoRGB(double x, double y, double z, double* r, double* g, double* b, char ObsIll); //

int ccRGBtoXYZ(double r, double g, double b, double* x, double* y, double* z, char ObsIll); //

int ccXYZtoYxy(double x, double y, double z, double* rY, double* rx, double* ry);//

int ccYxytoXYZ(double Y, double x, double y, double* rx, double* ry, double* rz);//

int ccXYZtoHunterLab(double x, double y, double z, double* L, double* a, double* b); //

int ccHunterLabtoXYZ(double L, double a, double b, double* x, double* y, double* z); //

int ccXYZtoCIE_Lab(double x, double y, double z, double* L, double* a, double* b, char ObsIll); //

int ccCIE_LabtoXYZ(double L, double a, double b, double* x, double* y, double* z, char ObsIll); //

int ccCIE_LabtoCIE_LCH(double L, double a, double b, double* rL, double* rC, double* rH);

int ccCIE_LCHtoCIE_Lab(double L, double C, double H, double* rL, double* ra, double* rb);

int ccXYZtoCIE_Luv(double x, double y, double z, double* L, double* u, double* v, char ObsIll);

int ccCIE_LuvtoXYZ(double L, double u, double v, double* x, double* y, double* z, char ObsIll);

int ccRGBtoHSL(double L, double a, double b, double* h, double* s, double* l); //

int ccHSLtoRGB(double h, double s, double l, double* r, double* g, double* b); //

int ccRGBtoHSV(double r, double g, double b, double* h, double* s, double* v); //

int ccHSVtoRGB(double h, double s, double v, double* r, double* g, double* b); //

int ccfRGBtoCMY(double r, double g, double b, double* c, double* m, double* y); //

int cciRGBtoCMY(int r, int g, int b, double* c, double* m, double* y); //

int ccCMYtofRGB(double c, double m, double y, double* r, double* g, double* b); //

int ccCMYtoiRGB(double c, double m, double y, int* r, int* g, int* b); //

int ccCMYtoCMYK(double c, double m, double y, double* C, double* M, double* Y, double* K); //

int ccCMYKtoCMY(double c, double m, double y, double k, double* C, double* M, double* Y); //

int ccFixRGB(double* r, double* g, double* b);


#endif
