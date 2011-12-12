#ifndef FLUID_C_H
#define FLUID_C_H

#ifdef __cplusplus
extern "C" {
#endif

// Fluid(unsigned int dim, double viscosity = 0.00001, double diffusion = 0.001, double decay = 0.99) 
void * fluid_create(unsigned int dim, double viscosity, double diffusion, double decay);
void fluid_destroy(void *f);
void fluid_step(void *f);
void fluid_reset(void *f);
void fluid_clear(void *f);

// coordinates
unsigned int fluid_index(void *f, int  x, int y, int z);
unsigned int fluid_indexf(void *f, double x, double y, double z);

// query
unsigned int fluid_dim(void *f);
void fluid_get_velocity(void *f, double *pos, double *vel);

// manipulate
void fluid_add_velocity(void *f, double *pos, double x, double y, double z);

// drawing
//void fluid_draw_velocity(void *f);

#ifdef __cplusplus
}
#endif

#endif	// FLUID_C_H