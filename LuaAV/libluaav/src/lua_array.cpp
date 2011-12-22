#ifdef __cplusplus
extern "C" {
#endif

#include "lua.h"
#include "lauxlib.h"
#include <stdlib.h>


#ifdef __cplusplus
}
#endif

#include "lua_utility.h"
#include "lua_array.h"
#include "lua_glue.h"

#ifndef MIN
#define MIN(a, b) (((a) < (b)) ? (a) : (b))
#endif

#ifndef MAX
#define MAX(a, b) (((a) > (b)) ? (a) : (b))
#endif

#ifndef CLAMP
#define CLAMP(v, a, b) MIN(MAX(a, v), b)
#endif


/*
read
write

place
sample
setall

*/

/*! Data buffer containers and management
	The array module provides data structures and functions for manipulating large buffers 
	of data.  Arrays are used as a generic descriptor for blocks of memory that need to be 
	passed around an application.  The array module supports the following data types:
	
	
	<ul>
	<li><b>Floating point types:</b> Float32, Float64</li>
	<li><b>Signed integer types:</b> SInt8, SInt16, SInt32, SInt64</li>
	<li><b>Unsigned integer types:</b> UInt8, UInt16, UInt32, UInt64</li>
	</ul>
	
	The Array data structure constists of a header and a pointer to a block of memory.  The 
	header describes how the memory is fomatted, including the number of components (channels, 
	the data type, the dimensions of the data, and the byte alignment.  The pointer points to 
	the block of memory itself.
	
	Arrays are constructed by setting their header.  When the header is set, the Array's 
	memory will be accordinly formatted.  A header consists of a number of required and optional 
	fields.  For example:
	<luacode>
	Array{
		components = 4,			-- The number of components (samples per-cell)
		type = array.Float32,	-- The data type
		dim = {512, 512},		-- The dimensions
		align = 4,				-- Optional byte-alignment
	}
	</luacode>
	will create a 4-channel float buffer 512x512 pixels in size that is 4-byte aligned.
	
	@module Array
*/

AlloTy supported_types[] = {
	/* floating point numbers */
	AlloFloat32Ty,
	AlloFloat64Ty,
	
	/* signed integers */
	AlloSInt8Ty,
	AlloSInt16Ty,
	AlloSInt32Ty,
	AlloSInt64Ty,
	
	/* unsigned integers */
	AlloUInt8Ty,
	AlloUInt16Ty,
	AlloUInt32Ty,
	AlloUInt64Ty
};

const char * alloty_to_string(AlloTy type) {
	switch(type) {
		/* floating point numbers */
		case AlloFloat32Ty: return "Float32";
		case AlloFloat64Ty: return "Float64";
		
		/* signed integers */
		case AlloSInt8Ty: return "SInt8";
		case AlloSInt16Ty: return "SInt16";
		case AlloSInt32Ty: return "SInt32";
		case AlloSInt64Ty: return "SInt64";
		
		/* unsigned integers */
		case AlloUInt8Ty: return "UInt8";
		case AlloUInt16Ty: return "UInt16";
		case AlloUInt32Ty: return "UInt32";
		case AlloUInt64Ty: return "UInt64";

		default:
			break;
	}
	return "< INVALID TYPE >";
}

/*! Constructor
	@param header Array header
	@ret The Array
	@name Array
*/
int lua_array_dummy() {return 0;}


/*! The array header
	Setting the array header will reformat its memory.
	@LuaMethod GETSET
	@name M.header
*/
int lua_array_header(lua_State *L) {
	AlloArray *s = lua_array_checkto(L, 1);
	if(s) {
		if(lua_istable(L, 2)) {
			AlloArrayHeader header;
			header.components = lua::optfield<uint8_t>(L, 2, s->header.components, "components");
			header.type = lua::optfield<AlloTy>(L, 2, s->header.type, "type");
			
			lua_getfield(L, 2, "dim");
			if(lua_istable(L, -1) && lua_objlen(L, -1) > 0) {
				header.dimcount = (uint8_t)MIN(lua_objlen(L, -1), ALLO_ARRAY_MAX_DIMS);
				for(int i=0; i < header.dimcount; i++) {
					lua_rawgeti(L, -1, i+1);
					header.dim[i] = lua::to<uint32_t>(L, -1);
					lua_pop(L, 1);
				}
			}
			else if(lua_isnumber(L, -1)) {
				header.dimcount = 1;
				header.dim[0] = lua_tonumber(L, -1);
			}
			else {
				header.dimcount = s->header.dimcount;
				for(int i=0; i < header.dimcount; i++) {
					header.dim[i] = s->header.dim[i];
				}
			}
			lua_pop(L, 1);
			
			int align = lua::optfield<int>(L, 2, 4, "align");
			allo_array_setstride(&header, align);
			
			allo_array_destroy(s);
			allo_array_create(s, &header);
			return 0;
		}
		else {
			lua_newtable(L);
			lua::pushfield(L, -1, "components", s->header.components);
			lua::pushfield(L, -1, "type", s->header.type);
			
			lua::push_vec_t(L, (int)s->header.dimcount, s->header.dim);
			lua_setfield(L, -2, "dim");
			return 1;
		}
	}
	else {
		luaL_error(L, "Array.header: invalid arguments");
	}
	return 0;
}

/*! The number of components
	Setting the number of components will reformat its memory.
	@LuaMethod GETSET
	@name M.components
*/
int lua_array_components(lua_State *L) {
	AlloArray *s = lua_array_checkto(L, 1);
	if(s) {
		if(lua::is<uint8_t>(L, 2)) {
			AlloArrayHeader header;
			memcpy(&header, &(s->header), sizeof(AlloArrayHeader));
			header.components = lua::to<uint8_t>(L, 2);
			
			allo_array_setstride(&header, 4);	// TODO: derive from s->header
			
			allo_array_setheader(s, &header);
			allo_array_destroy(s);
			allo_array_create(s, &header);
		}
		else {
			lua::push(L, s->header.components);
			return 1;
		}
	}
	else {
		luaL_error(L, "Array.components: invalid arguments");
	}
	return 0;
}

/*! The data type
	Setting the data type will reformat its memory.
	@LuaMethod GETSET
	@name M.type
*/
int lua_array_type(lua_State *L) {
	AlloArray *s = lua_array_checkto(L, 1);
	if(s) {
		if(lua::is<AlloTy>(L, 2)) {
			AlloArrayHeader header;
			memcpy(&header, &(s->header), sizeof(AlloArrayHeader));
			header.type = lua::to<AlloTy>(L, 2);
			
			allo_array_setstride(&header, 4);	// TODO: derive from s->header
			
			allo_array_setheader(s, &header);
			allo_array_destroy(s);
			allo_array_create(s, &header);
		}
		else {
			lua::push(L, s->header.type);
			return 1;
		}
	}
	else {
		luaL_error(L, "Array.type: invalid arguments");
	}
	return 0;
}

/*! The data dimension
	Setting the data dimension will reformat its memory.
	@LuaMethod GETSET
	@name M.dim
*/
int lua_array_dim(lua_State *L) {
	AlloArray *s = lua_array_checkto(L, 1);
	if(s) {
		uint32_t dim[ALLO_ARRAY_MAX_DIMS];
		int dimcount = ALLO_ARRAY_MAX_DIMS;
		if(lua::to_vec_n<uint32_t>(L, 2, dimcount, dim)) {
			if(dimcount < 1) {
				luaL_error(L, "Array.dim: dim has no values");
			}
			AlloArrayHeader header;
			memcpy(&header, &(s->header), sizeof(AlloArrayHeader));
			header.dimcount = (uint8_t)dimcount;
			for(int i=0; i < dimcount; i++) {
				header.dim[i] = dim[i];
			}
			
			allo_array_setstride(&header, 4);	// TODO: derive from s->header
			
			allo_array_setheader(s, &header);
			allo_array_destroy(s);
			allo_array_create(s, &header);
		}
		else {
			lua::push_vec_t(L, (int)s->header.dimcount, s->header.dim);
			return 1;
		}
	}
	else {
		luaL_error(L, "Array.dim: invalid arguments");
	}
	return 0;
}


template<typename T> 
void write_interp(lua_State * L, int top, al::ArrayWrapper * s, int dimcount, int components, double * coords) {
	T vals[4];
	
	if(! lua::to_vec_n<T>(L, top, components, vals)) {
		luaL_error(L, "Array.write: invalid arguments (values to write)");
	}
	
	switch (dimcount) {
		case 3:
			s->write_interp(vals, coords[0], coords[1], coords[2]);
			break;
		case 2:
			s->write_interp(vals, coords[0], coords[1]);
			break;
		default:
			s->write_interp(vals, coords[0]);
			break;
	}
}

/*!	Write values to an interpolated array location
		(values are 'spread' over neighboring cells)
	@param coords The coordinates of the cell to set
	@param values The value to set (as a table)
	@ret The value
	@LuaMethod METHOD
	@name M:write
*/
int lua_array_write(lua_State *L) {
	al::ArrayWrapper * s = lua_array_checkto(L, 1);
	if(s && lua::is<uint32_t>(L, 2)) {
		int dimcount = s->header.dimcount;
		int components = s->header.components;
	
		if (dimcount > 3) 
			luaL_error(L, "Array.write: up to 3 dimensions only");
		if (components > 4) 
			luaL_error(L, "Array.write: up to 4 components only");
		if(!s->data.ptr) 
			luaL_error(L, "Array.write: invalid array data");
		
		double coords[3] = { 0.f, 0.f, 0.f };
		for (int i=0; i<dimcount; i++) {
			coords[i] = luaL_optnumber(L, i+2, 0.);
		}
		
		int top = lua_gettop(L);
		
		switch(s->header.type) {
			case AlloFloat32Ty:
				write_interp<float>(L, top, s, dimcount, components, coords);
				break;
			case AlloFloat64Ty:
				write_interp<double>(L, top, s, dimcount, components, coords);
				break;
				
			case AlloSInt8Ty:
				write_interp<int8_t>(L, top, s, dimcount, components, coords);
				break;
			
			case AlloSInt16Ty:
				write_interp<int16_t>(L, top, s, dimcount, components, coords);
				break;
				
			case AlloSInt32Ty:
				write_interp<int32_t>(L, top, s, dimcount, components, coords);
				break;
				
			case AlloSInt64Ty:
				write_interp<int64_t>(L, top, s, dimcount, components, coords);
				break;

			case AlloUInt8Ty:
				write_interp<uint8_t>(L, top, s, dimcount, components, coords);
				break;
				
			case AlloUInt16Ty:
				write_interp<uint16_t>(L, top, s, dimcount, components, coords);
				break;
				
			case AlloUInt32Ty:
				write_interp<uint32_t>(L, top, s, dimcount, components, coords);
				break;
				
			case AlloUInt64Ty:
				write_interp<uint64_t>(L, top, s, dimcount, components, coords);
				break;
		}
	} else {
		luaL_error(L, "Array.write: invalid arguments");
	}
	return 1;
}

template <typename T>
void fill_vals(T *vals, int nvals, int components) {
	for(int i=nvals; i < components; i++) {
		vals[i] = 0;
	}
}

template <typename T>
void table_to_cell(lua_State *L, int idx, int &nvals, T *vals, int components) {
	if(! lua::to_vec_n<T>(L, idx, nvals, vals)) {
		luaL_error(L, "Array.setcell: invalid arguments");
	}
	fill_vals<T>(vals, nvals, components);
}

/*!	Set a cell of the array with a value
	@param coords The coordinates of the cell to set
	@param ... The value to set as a table or unpacked values
	@LuaMethod METHOD
	@name M:setcell
*/
int lua_array_setcell(lua_State *L) {
	AlloArray *s = lua_array_checkto(L, 1);
	int top = lua_gettop(L);
	if(lua::is<uint32_t>(L, 2) && top >= 3) {
		uint32_t coords[ALLO_ARRAY_MAX_DIMS];
		int ncoords = MIN(top-2, s->header.dimcount);
		for(int i=2; i < top; i++) {
			coords[i-2] = CLAMP(lua::to<uint32_t>(L, i), 0, s->header.dim[i-2]-1);
		}
		
		char vals[64]; // sizeof(double)*4
		int nvals = 4;
		
		switch(s->header.type) {
			case AlloFloat32Ty:
				table_to_cell<float>(L, top, nvals, (float *)vals, s->header.components);
				break;
				
			case AlloFloat64Ty:
				table_to_cell<double>(L, top, nvals, (double *)vals, s->header.components);
				break;
				
			case AlloSInt8Ty:
				table_to_cell<int8_t>(L, top, nvals, (int8_t *)vals, s->header.components);
				break;
			
			case AlloSInt16Ty:
				table_to_cell<int16_t>(L, top, nvals, (int16_t *)vals, s->header.components);
				break;
				
			case AlloSInt32Ty:
				table_to_cell<int32_t>(L, top, nvals, (int32_t *)vals, s->header.components);
				break;
				
			case AlloSInt64Ty:
				table_to_cell<int64_t>(L, top, nvals, (int64_t *)vals, s->header.components);
				break;

			case AlloUInt8Ty:
				table_to_cell<uint8_t>(L, top, nvals, (uint8_t *)vals, s->header.components);
				break;
				
			case AlloUInt16Ty:
				table_to_cell<uint16_t>(L, top, nvals, (uint16_t *)vals, s->header.components);
				break;
				
			case AlloUInt32Ty:
				table_to_cell<uint32_t>(L, top, nvals, (uint32_t *)vals, s->header.components);
				break;
				
			case AlloUInt64Ty:
				table_to_cell<uint64_t>(L, top, nvals, (uint64_t *)vals, s->header.components);
				break;
		}
		
		nvals = s->header.components;
		
		char *ptr = s->data.ptr;
		if(!ptr) {
			luaL_error(L, "Array.setcell: invalid array data");
		}
		
		for(int i=0; i < ncoords; i++) {
			ptr += coords[i] * s->header.stride[i];
		}
		memcpy(ptr, (char *)vals, allo_type_size(s->header.type)*nvals);
	}
	else {
		luaL_error(L, "Array.setcell: invalid arguments");
	}
	return 0;
}

template<typename T> 
void read_interp(lua_State * L, al::ArrayWrapper * s, int dimcount, int components, double * coords) {
	T vals[4];
	switch (dimcount) {
		case 3:
			s->read_interp(vals, coords[0], coords[1], coords[2]);
			break;
		case 2:
			s->read_interp(vals, coords[0], coords[1]);
			break;
		default:
			s->read_interp(vals, coords[0]);
			break;
	}
	lua::push_vec_t<T>(L, components, vals);
}

/*!	Read values of an interpolated array location
	@param coords The coordinates of the cell to get
	@ret The value
	@LuaMethod METHOD
	@name M:read
*/
int lua_array_read(lua_State *L) {
	al::ArrayWrapper * s = lua_array_checkto(L, 1);
	if(s && lua::is<uint32_t>(L, 2)) {
		int dimcount = s->header.dimcount;
		int components = s->header.components;
	
		if (dimcount > 3) 
			luaL_error(L, "Array.read: up to 3 dimensions only");
		if (components > 4) 
			luaL_error(L, "Array.read: up to 4 components only");
		if(!s->data.ptr) 
			luaL_error(L, "Array.read: invalid array data");
		
		double coords[3] = { 0.f, 0.f, 0.f };
		for (int i=0; i<dimcount; i++) {
			coords[i] = luaL_optnumber(L, i+2, 0.);
		}
		
		switch(s->header.type) {
			case AlloFloat32Ty:
				read_interp<float>(L, s, dimcount, components, coords);
				break;
			case AlloFloat64Ty:
				read_interp<double>(L, s, dimcount, components, coords);
				break;
				
			case AlloSInt8Ty:
				read_interp<int8_t>(L, s, dimcount, components, coords);
				break;
			
			case AlloSInt16Ty:
				read_interp<int16_t>(L, s, dimcount, components, coords);
				break;
				
			case AlloSInt32Ty:
				read_interp<int32_t>(L, s, dimcount, components, coords);
				break;
				
			case AlloSInt64Ty:
				read_interp<int64_t>(L, s, dimcount, components, coords);
				break;

			case AlloUInt8Ty:
				read_interp<uint8_t>(L, s, dimcount, components, coords);
				break;
				
			case AlloUInt16Ty:
				read_interp<uint16_t>(L, s, dimcount, components, coords);
				break;
				
			case AlloUInt32Ty:
				read_interp<uint32_t>(L, s, dimcount, components, coords);
				break;
				
			case AlloUInt64Ty:
				read_interp<uint64_t>(L, s, dimcount, components, coords);
				break;
		}
	} else {
		luaL_error(L, "Array.read: invalid arguments");
	}
	return 1;
}


/*!	Get the values of a array cell
	@param coords The coordinates of the cell to get
	@ret The value
	@LuaMethod METHOD
	@name M:getcell
*/
int lua_array_getcell(lua_State *L) {
	AlloArray *s = lua_array_checkto(L, 1);
	if(s && lua::is<uint32_t>(L, 2)) {
		uint32_t coords[ALLO_ARRAY_MAX_DIMS];
		int top = lua_gettop(L);
		int ncoords = MIN(top-1, s->header.dimcount);
		for(int i=2; i <= top; i++) {
			coords[i-2] = CLAMP(lua::to<uint32_t>(L, i), 0, s->header.dim[i-2]);
		}
		
		char *ptr = s->data.ptr;
		if(!ptr) {
			luaL_error(L, "Array.getcell: invalid array data");
		}
		
		for(int i=0; i < ncoords; i++) {
			ptr += coords[i] * s->header.stride[i];
		}		
		
		
		switch(s->header.type) {
			case AlloFloat32Ty:
				lua::push_vec_t<float>(L, (int)s->header.components, (float *)ptr);
				break;
				
			case AlloFloat64Ty:
				lua::push_vec_t<double>(L, (int)s->header.components, (double *)ptr);
				break;
				
			case AlloSInt8Ty:
				lua::push_vec_t<int8_t>(L, (int)s->header.components, (int8_t *)ptr);
				break;
			
			case AlloSInt16Ty:
				lua::push_vec_t<int16_t>(L, (int)s->header.components, (int16_t *)ptr);
				break;
				
			case AlloSInt32Ty:
				lua::push_vec_t<int32_t>(L, (int)s->header.components, (int32_t *)ptr);
				break;
				
			case AlloSInt64Ty:
				lua::push_vec_t<int64_t>(L, (int)s->header.components, (int64_t *)ptr);
				break;

			case AlloUInt8Ty:
				lua::push_vec_t<uint8_t>(L, (int)s->header.components, (uint8_t *)ptr);
				break;
				
			case AlloUInt16Ty:
				lua::push_vec_t<uint16_t>(L, (int)s->header.components, (uint16_t *)ptr);
				break;
				
			case AlloUInt32Ty:
				lua::push_vec_t<uint32_t>(L, (int)s->header.components, (uint32_t *)ptr);
				break;
				
			case AlloUInt64Ty:
				lua::push_vec_t<uint64_t>(L, (int)s->header.components, (uint64_t *)ptr);
				break;
		}
	}
	else {
		luaL_error(L, "Array.getcell: invalid arguments");
	}
	return 1;
}




/*!	Clear the array
	Clearing the array sets all values to 0
	@LuaMethod METHOD
	@name M:clear
*/
int lua_array_clear(lua_State *L) {
	AlloArray *s = lua_array_checkto(L, 1);
	if(s->data.ptr) {
		memset(s->data.ptr, '\0', allo_array_size(s));
	}
	return 1;
}

int lua_array_setall(lua_State *L) {
	AlloArray *s = lua_array_checkto(L, 1);
	if(s->data.ptr) {
	
		char vals[64]; // sizeof(double)*4
		int nvals = 4;
		memset(vals, '\0', 64);
		
		if(lua_istable(L, 2)) {
			switch(s->header.type) {
				case AlloFloat32Ty:
					table_to_cell<float>(L, 2, nvals, (float *)vals, s->header.components);
					break;
					
				case AlloFloat64Ty:
					table_to_cell<double>(L, 2, nvals, (double *)vals, s->header.components);
					break;
					
				case AlloSInt8Ty:
					table_to_cell<int8_t>(L, 2, nvals, (int8_t *)vals, s->header.components);
					break;
				
				case AlloSInt16Ty:
					table_to_cell<int16_t>(L, 2, nvals, (int16_t *)vals, s->header.components);
					break;
					
				case AlloSInt32Ty:
					table_to_cell<int32_t>(L, 2, nvals, (int32_t *)vals, s->header.components);
					break;
					
				case AlloSInt64Ty:
					table_to_cell<int64_t>(L, 2, nvals, (int64_t *)vals, s->header.components);
					break;

				case AlloUInt8Ty:
					table_to_cell<uint8_t>(L, 2, nvals, (uint8_t *)vals, s->header.components);
					break;
					
				case AlloUInt16Ty:
					table_to_cell<uint16_t>(L, 2, nvals, (uint16_t *)vals, s->header.components);
					break;
					
				case AlloUInt32Ty:
					table_to_cell<uint32_t>(L, 2, nvals, (uint32_t *)vals, s->header.components);
					break;
					
				case AlloUInt64Ty:
					table_to_cell<uint64_t>(L, 2, nvals, (uint64_t *)vals, s->header.components);
					break;
			}
		}
	
		int height = (s->header.dimcount > 1) ? s->header.dim[1] : 1;
		int width = s->header.dim[0];
		int rowstride = s->header.stride[1];
		int cellstride = s->header.stride[0];
		
		for(int j=0; j < height; j++) {
			char *ptr = s->data.ptr + j*rowstride;
			for(int i=0; i < width; i++) {
				for(int k=0; k < cellstride; k++) {
					*ptr++ = vals[k];
				}
			}
		}
	}
	return 0;
}

int lua_array_adapt(lua_State *L) {
	AlloArray *dst = lua_array_checkto(L, 1);
	AlloArray *src = lua_array_checkto(L, 2);
	allo_array_adapt(dst, &src->header);
	return 0;
}

int lua_array_copy(lua_State *L) {
	AlloArray *dst = lua_array_checkto(L, 1);
	AlloArray *src = lua_array_checkto(L, 2);
	allo_array_adapt(dst, &src->header);
	if(src->data.ptr) {
		memcpy(dst->data.ptr, src->data.ptr, allo_array_size(src));
	}
	return 0;
}

int lua_array_debug(lua_State *L) {
	al::Array *s = lua_array_checkto(L, 1);
	al::ArrayWrapper * w = (al::ArrayWrapper *)s;
	printf("Array %p ptr %p refs %d\n", s, s->data.ptr, w->refs());
	return 1;
}

template<> void Glue<al::ArrayWrapper>::usr_mt(lua_State * L) {
	static luaL_reg methods[] = {
		{ "setcell", lua_array_setcell },
		{ "getcell", lua_array_getcell },
		{ "write", lua_array_write },
		{ "read", lua_array_read },
		{ "clear", lua_array_clear },
		{ "setall", lua_array_setall },
		{ "adapt", lua_array_adapt },
		{ "copy", lua_array_copy },
		{ "debug", lua_array_debug },
		{ NULL, NULL }
	};

	static luaL_reg getters[] = {
		{ "header", lua_array_header },
		{ "components", lua_array_components },
		{ "type", lua_array_type },
		{ "dim", lua_array_dim },
		{ NULL, NULL }
	};

	static luaL_reg setters[] = {
		{ "header", lua_array_header },
		{ "components", lua_array_components },
		{ "type", lua_array_type },
		{ "dim", lua_array_dim },
		{ NULL, NULL }
	};
	Glue<al::ArrayWrapper>::usr_attr_mt(
		L, 
		methods, 
		getters, 
		setters
	);

	lua_pushnumber(L, ARRAY_MAGIC_VALUE);
	lua_setfield(L, -2, "__magic");
}


#ifdef __cplusplus
extern "C" {
#endif

int luaopen_Array(lua_State *L) {
	const char * libname = lua_tostring(L, 1);

	Glue<al::ArrayWrapper>::define(L);
	Glue<al::ArrayWrapper>::register_class(L);
	Glue<al::ArrayWrapper>::register_module(L, libname);
	
	int ntypes = sizeof(supported_types)/sizeof(AlloTy);
	for(int i=0; i < ntypes; i++) {
		lua::push(L, supported_types[i]);
		lua_setfield(L, -2, alloty_to_string(supported_types[i]));
	}

	return 1;
}


#ifdef __cplusplus
}
#endif
