#include "fluid.h"
#include "fluid_c.h"

#define FLUID(x)	((Fluid *)x)

#ifdef __cplusplus
extern "C" {
#endif

// Fluid(unsigned int dim, double viscosity = 0.00001, double diffusion = 0.001, double decay = 0.99) 
void * fluid_create(unsigned int dim, double viscosity, double diffusion, double decay) {
	return new Fluid(dim, viscosity, diffusion, decay);
}

void fluid_destroy(void *f) {
	delete FLUID(f);
}

void fluid_step(void *f) {
	FLUID(f)->step();
}

void fluid_reset(void *f) {
	FLUID(f)->reset();
}

void fluid_clear(void *f) {
	FLUID(f)->clear();
}


// coordinates
unsigned int fluid_index(void *f, int x, int y, int z) {
	return FLUID(f)->index(x, y, z);
}

unsigned int fluid_indexf(void *f, double x, double y, double z) {
	return FLUID(f)->index(x, y, z);
}


// query
unsigned int fluid_dim(void *f) {
	return FLUID(f)->dim;
}


// manipulate
void fluid_add_velocity(void *f, double *pos, double x, double y, double z) {
	vec p(pos[0], pos[1], pos[2]);
	FLUID(f)->addVelocity(p, x, y, z);
}

void fluid_get_velocity(void *f, double *pos, double *vel) {
	vec p(pos[0], pos[1], pos[2]);
	vec flow;
	FLUID(f)->getFlow(p, flow);
	
	vel[0] = flow.x;
	vel[1] = flow.y;
	vel[2] = flow.z;
}

/*
// drawing
void fluid_draw_velocity(void *f) {
	FLUID(f)->drawVelocity();
}
*/

#ifdef __cplusplus
}
#endif
