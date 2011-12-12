#ifdef __cplusplus
extern "C" {
#endif

#include "lua.h"
#include "lauxlib.h"

#ifdef __cplusplus
}
#endif

//#include "fluid.h"
#include "lua_utility.h"
#include "lua_lattice.h"

#define LOW_WRAP(v, lim)	(((v) >= 0) ? (v) : (lim-1))

template <typename T>
void fluid_diffuse_2d(AlloLattice *src, AlloLattice *dst, T a, int nloops) {
	int components = src->header.components;
	int h = src->header.dim[1];
	int w = src->header.dim[0];
	int rowstride = src->header.stride[1];
	char *src_ptr = src->data.ptr;
	char *dst_ptr = dst->data.ptr;
	
	for(int loop = 0; loop < nloops; loop++) {
		for(int j=0; j < h; j++) {
			T *f_src_p = (T *)(src_ptr + LOW_WRAP((j-1), h)*rowstride);
			T *f_src = (T *)(src_ptr + j*rowstride);
			T *f_src_n = (T *)(src_ptr + ((j+1) % h)*rowstride);
			T *f_dst = (T *)(dst_ptr + j*rowstride);
			
			for(int k=0; k < components; k++) {
				*f_dst++ = ((*f_src) + a * (
					f_src[(w-1)*components] + f_src[components] + 
					(*f_src_p) + (*f_src_n)
				)) / (1 + 4*a);
				
				f_src++;
				f_src_p++;
				f_src_n++;
			}
			
			for(int i=1; i < w-1; i++) {
				for(int k=0; k < components; k++) {
					*f_dst++ = ((*f_src) + a * (
						f_src[-components] + f_src[components] + 
						(*f_src_p) + (*f_src_n)
					)) / (1 + 4*a);
					
					f_src++;
					f_src_p++;
					f_src_n++;
				}
			}
			
			for(int k=0; k < components; k++) {
				*f_dst++ = ((*f_src) + a * (
					f_src[-components] + f_src[components*(1-w)] + 
					(*f_src_p) + (*f_src_n)
				)) / (1 + 4*a);
				
				f_src++;
				f_src_p++;
				f_src_n++;
			}
		}
	}
}

template <typename T>
void fluid_decay_2d(AlloLattice *src, T v) {
	int components = src->header.components;
	int h = src->header.dim[1];
	int w = src->header.dim[0];
	int rowstride = src->header.stride[1];
	char *src_ptr = src->data.ptr;
	
	for(int j=0; j < h; j++) {
		T *f_src = (T *)(src_ptr + j*rowstride);
		
		for(int i=0; i < w; i++) {
			for(int k=0; k < components; k++) {
				*f_src++ = (*f_src) * v;
			}
		}
	}
}


int lua_fluid_diffuse(lua_State *L) {
	AlloLattice *src = lua_lattice_checkto(L, 1);
	AlloLattice *dst = lua_lattice_checkto(L, 2);
	if(allo_lattice_equal_headers(&(src->header), &(dst->header))) {
		double a = lua::opt<double>(L, 3, 0.1);
		int nloops = lua::opt<int>(L, 4, 8);

		if(src->header.dimcount == 2) {
			switch(src->header.type) {
				case AlloFloat32Ty: fluid_diffuse_2d<float>(src, dst, a, nloops); break;
				case AlloFloat64Ty: fluid_diffuse_2d<double>(src, dst, a, nloops); break;
				default:
					luaL_error(L, "fluid.diffuse: input must be Float32 or Float64");
					break;
			}
		}
		else if(src->header.dimcount == 3) {
			switch(src->header.type) {
//				case AlloFloat32Ty: fluid_diffuse_3d<float>(src, dst, a, nloops); break;
//				case AlloFloat64Ty: fluid_diffuse_3d<double>(src, dst, a, nloops); break;
				default:
					luaL_error(L, "fluid.diffuse: input must be Float32 or Float64");
					break;
			}
		}
		else {
			luaL_error(L, "fluid.diffuse: input must be 2D or 3D");
		}
	}
	else {
		luaL_error(L, "fluid.diffuse: inputs have different lattice headers");
	}
	return 0;
}

int lua_fluid_decay(lua_State *L) {
	AlloLattice *src = lua_lattice_checkto(L, 1);
	if(lua::is<double>(L, 2)) {
		double v = lua::to<double>(L, 2);

		if(src->header.dimcount == 2) {
			switch(src->header.type) {
				case AlloFloat32Ty: fluid_decay_2d<float>(src, v); break;
				case AlloFloat64Ty: fluid_decay_2d<double>(src, v); break;
				default:
					luaL_error(L, "fluid.diffuse: input must be Float32 or Float64");
					break;
			}
		}
		else if(src->header.dimcount == 3) {
			switch(src->header.type) {
//				case AlloFloat32Ty: fluid_diffuse_3d<float>(src, dst, a, nloops); break;
//				case AlloFloat64Ty: fluid_diffuse_3d<double>(src, dst, a, nloops); break;
				default:
					luaL_error(L, "fluid.diffuse: input must be Float32 or Float64");
					break;
			}
		}
		else {
			luaL_error(L, "fluid.diffuse: input must be 2D or 3D");
		}
	}
	else {
		luaL_error(L, "fluid.diffuse: inputs have different lattice headers");
	}
	return 0;
}


#ifdef __cplusplus
extern "C" {
#endif

int luaopen_fluid(lua_State *L) {
	const char * libname = lua_tostring(L, 1);
	
	struct luaL_reg lib[] = {
		{ "diffuse", lua_fluid_diffuse },
		{ "decay", lua_fluid_decay },
		{NULL, NULL},
	};
	
	luaL_register(L, libname, lib);
	
	return 1;
}

#ifdef __cplusplus
}
#endif