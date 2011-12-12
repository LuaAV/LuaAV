#ifndef FLUID_H
#define FLUID_H

#include <cmath>
#include <stdlib.h>

/*
#if defined (__APPLE__) || defined (OSX)
	#include <OpenGL/gl.h>
	#include <OpenGL/glext.h>
	#include <OpenGL/glu.h>
#endif

#if defined (__LINUX__) || defined (UNIX)
	#include <GL/gl.h>
	#include <GL/glu.h>
#endif

#ifdef WIN32

	#include <windows.h>

	#include <gl/gl.h>
	#include <gl/glu.h>

	#pragma comment( lib, "winmm.lib")
	#pragma comment( lib, "opengl32.lib" )
	#pragma comment( lib, "glu32.lib" )

#endif
*/

#define MIN(a, b) (((a) < (b)) ? (a) : (b))
#define MAX(a, b) (((a) > (b)) ? (a) : (b))


#define urandom() (((double)rand()) / ((double)RAND_MAX))
#define srandom() (urandom()*2.-1.)

struct vec {
	vec()
	:	x(0), y(0), z(0)
	{}
	
	vec(double x, double y, double z)
	:	x(x), y(y), z(z)
	{}

	double x;
	double y;
	double z;
};


struct Fluid {

	unsigned int bits, dim, dim2, dim3, dimwrap;
	double	*pfield, *pfield0;		// pressure field
	double	*dfield, *dfield0;		// density field
	double	*uxfield, *uxfield0;	// velocity field 
	double	*uyfield, *uyfield0;	// velocity field 
	double	*uzfield, *uzfield0;	// velocity field 
	
	
//	double	*rfield, *rfield0;		// density field
//	double	*gfield, *gfield0;		// density field
//	double	*bfield, *bfield0;		// density field
	
	double viscosity, diffusion, decay;
	
	Fluid(unsigned int dim, double viscosity = 0.00001, double diffusion = 0.001, double decay = 0.99) 
		: /*bits(bits),*/ dim(dim), viscosity(viscosity), diffusion(diffusion), decay(decay)
	{
		dim2 = dim*dim;
		dim3 = dim*dim*dim;
		dimwrap = dim-1;
		
		pfield = new double[dim3];
		dfield = new double[dim3];
		uxfield = new double[dim3];
		uyfield = new double[dim3];
		uzfield = new double[dim3];
		
		pfield0 = new double[dim3];
		dfield0 = new double[dim3];
		uxfield0 = new double[dim3];
		uyfield0 = new double[dim3];
		uzfield0 = new double[dim3];
		
		
//		rfield = new double[dim3]; rfield0 = new double[dim3];
//		gfield = new double[dim3]; gfield0 = new double[dim3];
//		bfield = new double[dim3]; bfield0 = new double[dim3];
		
		reset();
	}
	
	~Fluid() {
		delete[] pfield;
		delete[] dfield;
		delete[] uxfield;
		delete[] uyfield;
		delete[] uzfield;
		delete[] pfield0;
		delete[] dfield0;
		delete[] uxfield0;
		delete[] uyfield0;
		delete[] uzfield0;
		
//		delete[] rfield; delete[] rfield0;
//		delete[] gfield; delete[] gfield0;
//		delete[] bfield; delete[] bfield0;
	}
	
	inline unsigned int index(int  x, int y, int z) {
		return (x & dimwrap)*dim2 + (y & dimwrap)*dim + (z & dimwrap);
	}
	inline unsigned int index(double x, double y, double z) {
		return index((int)floor(x), (int)floor(y), (int)floor(z));
	}
	
	// (linear) interpolate a value at the given coords:
	double interp(double * src, double x, double y, double z) 
	{
		// get the integer components, and normalized 0..1 interp factors, of x,y,z:
		int x0 = (int)x; double xbf = x-(double)x0; double xaf = 1-xbf; 
		int y0 = (int)y; double ybf = y-(double)y0; double yaf = 1-ybf;
		int z0 = (int)z; double zbf = z-(double)z0; double zaf = 1-zbf;
		
		// find the 8 cube corners by getting the corners' array indices:
		int xa = (x0 & (dimwrap))*dim2;	int xb = ((x0+1) & (dimwrap))*dim2;
		int ya = (y0 & (dimwrap))*dim;	int yb = ((y0+1) & (dimwrap))*dim;
		int za = (z0 & (dimwrap));		int zb = ((z0+1) & (dimwrap));
		
		// do the interpolation:
		return  (src[xa + ya + za] * xaf * yaf * zaf) +
				(src[xb + ya + za] * xbf * yaf * zaf) + 
				(src[xa + yb + za] * xaf * ybf * zaf) + 
				(src[xa + ya + zb] * xaf * yaf * zbf) +
				(src[xb + ya + zb] * xbf * yaf * zbf) + 
				(src[xa + yb + zb] * xaf * ybf * zbf) + 
				(src[xb + yb + za] * xbf * ybf * zaf) + 
				(src[xb + yb + zb] * xbf * ybf * zbf);
	}
	
	inline void reset() {
		for (int i=0; i<dim3; i++) {
			pfield[i] = urandom();
			dfield[i] = srandom();
			uxfield[i] = srandom();
			uyfield[i] = srandom();
			uzfield[i] = srandom();
			
			pfield0[i] = pfield[i];
			dfield0[i] = dfield[i];
			uxfield0[i] = uxfield[i];
			uyfield0[i] = uyfield[i];
			uzfield0[i] = uzfield[i];
			
			
//			rfield[i] = urandom(); rfield0[i] = rfield[i];
//			gfield[i] = urandom(); gfield0[i] = gfield[i];
//			bfield[i] = urandom(); bfield0[i] = bfield[i];
		}
	}
	
	inline void clear() {
		for (int i=0; i<dim3; i++) {
			pfield[i] = 0.;
			dfield[i] = 0.;
			uxfield[i] = 0.;
			uyfield[i] = 0.;
			uzfield[i] = 0.;
			
			pfield0[i] = pfield[i];
			dfield0[i] = dfield[i];
			uxfield0[i] = uxfield[i];
			uyfield0[i] = uyfield[i];
			uzfield0[i] = uzfield[i];
			
			
//			rfield[i] = urandom(); rfield0[i] = rfield[i];
//			gfield[i] = urandom(); gfield0[i] = gfield[i];
//			bfield[i] = urandom(); bfield0[i] = bfield[i];
		}
	}
	
	/*
	inline void drawPressure() {
		glBegin(GL_POINTS);
		int xi, yi;
		for (int x=0; x<dim; x++) {
			xi = x * dim2;
			for (int y=0; y<dim; y++) {
				yi = y * dim;
				for (int z=0; z<dim; z++) {
					double c = pfield[xi+yi+z];
					glColor3d(c, 0.5, 1-c);
					glVertex3d(x+0.5, y+0.5, z+0.5);
				}
			}
		}
		glEnd();
	}
	
	inline void drawDensity() {
		glBegin(GL_POINTS);
		int xi, yi;
		for (int x=0; x<dim; x++) {
			xi = x * dim2;
			for (int y=0; y<dim; y++) {
				yi = y * dim;
				for (int z=0; z<dim; z++) {
//					int i = xi+yi+z;
					
//					double c = dfield[xi+yi+z];
//					double r, g, b;
//					HSLtoRGB(c, 1, 0.5, r, g, b);
					
//					double r = rfield[i];
//					double g = gfield[i];
//					double b = bfield[i];

					double r = 0.8;
					double g = 0.8;
					double b = 0.8;
					glColor3d(r*r+0.2, g*g+0.2, b*b+0.2);
					glVertex3d(x+0.5, y+0.5, z+0.5);
				}
			}
		}
		glEnd();
	}
	
	
	inline void drawVelocity() {
		glBegin(GL_LINES);
		int xi, yi;
		for (int x=0; x<dim; x++) {
			xi = x * dim2;
			for (int y=0; y<dim; y++) {
				yi = y * dim;
				for (int z=0; z<dim; z++) {
					int i = xi+yi+z;
					
//					double c = dfield[xi+yi+z];
//					double r, g, b;
//					HSLtoRGB(c, 1, 0.5, r, g, b);
					
//					double r = rfield[i];
//					double g = gfield[i];
//					double b = bfield[i];

					double r = 0.8;
					double g = 0.8;
					double b = 0.8;
					
					glColor3d(r*r+0.2, g*g+0.2, b*b+0.2);
					glVertex3d(x+0.5, y+0.5, z+0.5);
					glColor4d(0., 0., 0., 0.);
					glVertex3d(x+0.5+uxfield[i], y+0.5+uyfield[i], z+0.5+uzfield[i]);
				}
			}
		}
		glEnd();
	}
	*/
	
	inline void addVelocity(vec & v, vec & d) {
		addVelocity(v, d.x, d.y, d.z);
	}
	
	inline void addVelocity(vec & v, double x, double y, double z) {
		int i = index(v.x, v.y, v.z);
		uxfield[i] = x;
		uyfield[i] = y;
		uzfield[i] = z;
		uxfield0[i] = x;
		uyfield0[i] = y;
		uzfield0[i] = z;
	}
	
	inline void addColor(vec & v, vec & d) {
/*		int i = index(v.x, v.y, v.z);
		rfield[i] = d.x;
		gfield[i] = d.y;
		bfield[i] = d.z;*/
	}
	
	inline void addDensity(vec & v, double d = 1) {
		dfield[index(v.x, v.y, v.z)] = d;
	}
	
	inline void addPressure(vec & v, double d = 1) {
		pfield[index(v.x, v.y, v.z)] = d;
	}
	
	inline void getFlow(vec & pos, vec & flow) {
		int i = index(pos.x, pos.y, pos.z);
		flow.x = uxfield[i];
		flow.y = uyfield[i];
		flow.z = uzfield[i];
	}
	
	inline void getPressure(vec & pos, double & p) {
		int i = index(pos.x, pos.y, pos.z);
		p = pfield[i];
	}
	
	inline void getColor(vec & pos, vec & col) {
/*		int i = index(pos.x, pos.y, pos.z);
		col.x = rfield[i]+0.5;
		col.y = gfield[i]+0.5;
		col.z = bfield[i]+0.5;*/
	}
	
	inline void diffuse (double * f, double * f0, double a = 0.1, unsigned int loops = 8) { 
		for ( int k=0 ; k<loops ; k++ ) { 
			for ( int x=0 ; x<dim ; x++ ) { 
				for ( int y=0 ; y<dim ; y++ ) { 
					for ( int z=0 ; z<dim ; z++ ) { 
						unsigned int i = index(x, y, z);
						unsigned int ip__ = index(x+1, y, z);
						unsigned int im__ = index(x-1, y, z);
						unsigned int i_p_ = index(x, y+1, z);
						unsigned int i_m_ = index(x, y-1, z);
						unsigned int i__p = index(x, y, z+1);
						unsigned int i__m = index(x, y, z-1);
						
						// reverse solver:
						f[i] = (f0[i] + a * (f[ip__]+f[im__]+f[i_p_]+f[i_m_]+f[i__p]+f[i__m])) / (1+6*a);
					}
				} 
			} 
		} 
	}
	
	inline void diffuse(vec * f, vec * f0, double a = 0.1, unsigned int loops = 8) { 
		for ( int k=0 ; k<loops ; k++ ) { 
			for ( int x=0 ; x<dim ; x++ ) { 
				for ( int y=0 ; y<dim ; y++ ) { 
					for ( int z=0 ; z<dim ; z++ ) { 
						unsigned int i = index(x, y, z);
						unsigned int ip__ = index(x+1, y, z);
						unsigned int im__ = index(x-1, y, z);
						unsigned int i_p_ = index(x, y+1, z);
						unsigned int i_m_ = index(x, y-1, z);
						unsigned int i__p = index(x, y, z+1);
						unsigned int i__m = index(x, y, z-1);
						
						// reverse solver:
						f[i].x = (f0[i].x + a * (f[ip__].x+f[im__].x+f[i_p_].x+f[i_m_].x+f[i__p].x+f[i__m].x)) / (1+6*a);
						f[i].y = (f0[i].y + a * (f[ip__].y+f[im__].y+f[i_p_].y+f[i_m_].y+f[i__p].y+f[i__m].y)) / (1+6*a);
						f[i].z = (f0[i].z + a * (f[ip__].z+f[im__].z+f[i_p_].z+f[i_m_].z+f[i__p].z+f[i__m].z)) / (1+6*a);
					}
				} 
			} 
		} 
	} 
	
	inline void advect(double * f, double * f0, double * ux, double * uy, double * uz, double rate = 1) {
		for ( int x=0 ; x<dim ; x++ ) { 
			for ( int y=0 ; y<dim ; y++ ) { 
				for ( int z=0 ; z<dim ; z++ ) {		
					int i = index(x, y, z);
					
					// get coordinates back in time from current cell center:
					double x0 = x - rate * ux[i];
					double y0 = y - rate * uy[i];
					double z0 = z - rate * uz[i];
					
					// get interpolated value at x0, y0, z0:
					f[i] = interp(f0, x0, y0, z0);
					//printf("%f: %f %f %f\n", f[i], x, y, z);
				}
			} 
		} 
	}	

	template<typename T>
	inline void swapPtrs(T ** a, T ** b) { T * tmp = *a; *a = *b; *b = tmp; }
	
	inline void normalize(double * f) {
		double min = 0., max = 0.;
		for (int i=0; i<dim3; i++) {
			min = MIN(min, f[i]);
			max = MAX(max, f[i]);
		}
		double scalar = 1.0/(max-min);
		//printf("%f %f \n", min, max);
		for (int i=0; i<dim3; i++) {
			f[i] = f[i]*scalar - min;
		}
	}
	
	inline void step();
};


namespace stable {

	#define INDEX(x, y, z) ((z & (N-1)) + (y & (N-1))*N + (x & (N-1))*N*N)
	#define BEGIN_PER_CELL	for (int x=0 ; x<N ; x++ ) { \
							for (int y=0 ; y<N ; y++ ) { \
							for (int z=0 ; z<N ; z++ ) { \
								int i = INDEX(x, y, z);
	#define END_PER_CELL	}}}
	#define SWAP_PTR(x0,x) {double * tmp=x0;x0=x;x=tmp;}

	/*
		Stable diffusion solver
			for dimension N, diffuses field p (previous p0)
		
		This method avoids oscillation and divergence in diffusion, 
			granting a *stable* result instead:
			an iterative technique to solve a sparse linear system
			(the Gauss-Seidel relaxation method)	
	*/
	inline void stable_solve(int N, double * p, double * p0, double diffusion, double divisor ) {
		static int iterations = 20; ///<<< seems like this can be reduced - big cpu savings... but...?
		double div = 1.0/divisor;
		
		for (int n=0 ; n<iterations ; n++) {	
			BEGIN_PER_CELL			
				p[i] = (
							p0[i] + 
							diffusion*(
								p[INDEX(x-1, y, z)] + p[INDEX(x+1, y, z)] +
								p[INDEX(x, y-1, z)] + p[INDEX(x, y+1, z)] +
								p[INDEX(x, y, z-1)] + p[INDEX(x, y, z+1)]
							)
						) * div;
			END_PER_CELL
		}
	}

	inline void diffuse(int N, double * p, double * p0, double diffusion)
	{
		double a = diffusion; // * (N * N * N);
		stable_solve(N, p, p0, a, (1+6*a) );
	}

	// (linear) interpolate a value at the given coords:
	inline double interp(int N, double * p0, double x, double y, double z) 
	{
		int dimwrap = N-1; // assumes N is a power of 2!!
		
		// get the integer components, and normalized 0..1 interp factors, of x,y,z:
		int x0 = (int)x; double xbf = x-(double)x0; double xaf = 1-xbf; 
		int y0 = (int)y; double ybf = y-(double)y0; double yaf = 1-ybf;
		int z0 = (int)z; double zbf = z-(double)z0; double zaf = 1-zbf;
		
		// find the 8 cube corners by getting the corners' array indices:
		int xa = (x0 & (dimwrap))*N*N;	int xb = ((x0+1) & (dimwrap))*N*N;
		int ya = (y0 & (dimwrap))*N;	int yb = ((y0+1) & (dimwrap))*N;
		int za = (z0 & (dimwrap));		int zb = ((z0+1) & (dimwrap));
		
		// do the interpolation:
		return  (p0[xa + ya + za] * xaf * yaf * zaf) +
				(p0[xb + ya + za] * xbf * yaf * zaf) + 
				(p0[xa + yb + za] * xaf * ybf * zaf) + 
				(p0[xa + ya + zb] * xaf * yaf * zbf) +
				(p0[xb + ya + zb] * xbf * yaf * zbf) + 
				(p0[xa + yb + zb] * xaf * ybf * zbf) + 
				(p0[xb + yb + za] * xbf * ybf * zaf) + 
				(p0[xb + yb + zb] * xbf * ybf * zbf);
	}

	/*
		Translate densities etc. (p, p0 = previous) through the vector field (vx, vy, vz) over dim N
			Does a linear back-trace of center point through vector field, 
			using linear interpolation to gather p0 at this source and place in new p
	*/
	inline void advect(int N, double * p, double * p0, double * vx, double * vy, double * vz)
	{
		static double rate = 1.0;
		BEGIN_PER_CELL
			// back trace:
			double x0 = x - rate * vx[i]; 
			double y0 = y - rate * vy[i]; 
			double z0 = z - rate * vz[i]; 
			// trilinearinterp p0 at (x0, y0, z0):
			p[i] = interp(N, p0, x0, y0, z0);
		END_PER_CELL
	}

	/*
		Clever part of Jos Stam's work. 
			A velocity field can become divergent (have regions that are purely emanating or aggregating)
				violating the definition of an incompressible fluid
			But, since a velocity field can be seen as an incompressible velocity field + a gradient field,
				we can subtract a gradient field from our bad velocity field to get an incompressible one
			To calculate this gradient field and then subtract it, we use this function:
	*/
	inline void project(int N, double * vx, double * vy, double * vz, double * g, double * g0 )
	{
		double h = 1.0/N;
		
		// g and g0 represent the velocity gradient
		BEGIN_PER_CELL
			// previous instantaneous magnitude of velocity gradient 
			//		= (sum of velocity gradients per axis)/2N:
			g0[i] = -0.5 * h * (
									vx[INDEX(x+1, y, z)]-vx[INDEX(x-1, y, z)]+	// velocity gradients
									vy[INDEX(x, y+1, z)]-vy[INDEX(x, y-1, z)]+	// velocity gradients
									vz[INDEX(x, y, z+1)]-vz[INDEX(x, y, z-1)]	// velocity gradients
								);
			// zero out the present velocity gradient:
			g[i] = 0;
		END_PER_CELL
		
		// reuse the Gauss-Seidel relaxation solver to safely diffuse the velocity gradients from g0 to g:
		stable_solve(N, g, g0, 1, 6);
		
		// now subtract this gradient from our current velocity field:
		BEGIN_PER_CELL
			vx[i] -= 0.5 * N * (g[INDEX(x+1, y, z)]-g[INDEX(x-1, y, z)]); // gradient calculated by neighbors
			vy[i] -= 0.5 * N * (g[INDEX(x, y+1, z)]-g[INDEX(x, y-1, z)]);
			vz[i] -= 0.5 * N * (g[INDEX(x, y, z+1)]-g[INDEX(x, y, z-1)]);
		END_PER_CELL
	}

	void dens_step (int N, double * p, double * p0, double * vx, double * vy, double * vz, double diffusion )
	{
		SWAP_PTR( p0, p ); 
		diffuse(N, p, p0, diffusion);
		SWAP_PTR( p0, p ); 
		advect(N, p, p0, vx, vy, vz);
	}

	void vel_step (int N, double * vx, double * vy, double * vz, double * vx0, double * vy0, double * vz0, double viscosity)
	{
		// diffuse the velocity field (per axis):
		SWAP_PTR ( vx0, vx ); 
		diffuse ( N, vx, vx0, viscosity );
		SWAP_PTR ( vy0, vy ); 
		diffuse ( N, vy, vy0, viscosity );
		SWAP_PTR ( vz0, vz ); 
		diffuse ( N, vz, vz0, viscosity );
		// stabilize it: (vx0, vy0 are whatever, being used as temporaries to store gradient field)
		project ( N, vx, vy, vz, vx0, vy0 );
		
		// advect the velocity field (per axis):
		SWAP_PTR (vx0, vx); 
		SWAP_PTR (vy0, vy);
		SWAP_PTR (vz0, vz);
		advect ( N, vx, vx0, vx0, vy0, vz0 ); 
		advect ( N, vy, vy0, vx0, vy0, vz0 );
		advect ( N, vz, vz0, vx0, vy0, vz0 );
//		// stabilize it: (vx0, vy0 are whatever, being used as temporaries to store gradient field)
		project ( N, vx, vy, vz, vx0, vy0 );
	}

	#undef BEGIN_PER_CELL
	#undef END_PER_CELL
	#undef INDEX
	#undef SWAP_PTR

} // namespace

inline void Fluid :: step() {
//
//	// diffuse pressure:
//	stable::dens_step(dim, 
//		pfield, pfield0, 
//		uxfield, uyfield, uzfield, 
//		diffusion );
	
//	// diffuse color
//	stable::dens_step(dim, 
//		rfield, rfield0, 
//		uxfield, uyfield, uzfield, 
//		diffusion );
//	stable::dens_step(dim, 
//		gfield, gfield0, 
//		uxfield, uyfield, uzfield, 
//		diffusion );
//	stable::dens_step(dim, 
//		bfield, bfield0, 
//		uxfield, uyfield, uzfield, 
//		diffusion );
		
	// diffuse velocity:
	stable::vel_step(dim, 
		uxfield, uyfield, uzfield, 
		uxfield0, uyfield0, uzfield0, 
		viscosity);
	
//	// decay :
//	for (int i=0; i<dim3; i++) {
//		pfield[i] *= decay;
//		rfield[i] *= decay;
//		gfield[i] *= decay;
//		bfield[i] *= decay;
//	}
	


 }

#endif
