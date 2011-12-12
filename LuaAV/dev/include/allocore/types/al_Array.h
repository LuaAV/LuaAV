/*
 *  allo.h
 *  A set of definitions for data interchange amenable to a C ABI.
 *
 *  AlloSphere, Media Arts & Technology, UCSB
 *
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
	HEREUNDER IS PROVIDED "AS IS". REGENTS HAS NO OBLIGATION TO PROVIDE
	MAINTENANCE, SUPPORT, UPDATES, ENHANCEMENTS, OR MODIFICATIONS.
*/

#ifndef INCLUDE_ALLO_ARRAY_H
#define INCLUDE_ALLO_ARRAY_H 1

/*
 AlloArray is a multidimensional array.
 It is a pointer to data followed by meta-data to describe its type and layout.
 
 Separating the data from the header meta-data allows logic to performed on layouts without data 
 - a specific layout can be defined and checked against
 
 
	 0			1			2			3			4
	 Specifier	row			column		pillar		file
	 Tensor		scalar		vector		matrix		
	 Sound		sample		time		channel		pattern
	 Polytope	point		line		polygon		polyhedron	polychoron
	 n-Hypercube	point		line		square		cube		tesseract
	 Boundary	none		vertex		edge		face		cell
	 Movement	position	velocity	accel.		jerk		snap
 */

#include "allocore/system/al_Config.h"
#include <string.h>
#include <stdlib.h>


/*
 Maximum number of dimensions a array may represent
 To model higher dimensional spaces, use a nested array descriptor
 */
#define ALLO_ARRAY_MAX_DIMS (4)

#ifdef __cplusplus
extern "C" {
#endif


#pragma mark AlloTy
/*
	Unique identifiers for principal types 
		(inspired by SDIF; higher bits represent semantics, lower bits represent size)
		(unlike SDIF, assumption is little-endian; and 'byte' is represented as AlloUInt8Ty) 
*/
enum {	
	/* type with no size */
	AlloVoidTy			= 0x0000,
	
	/* floating point numbers */
	AlloFloat32Ty		= 0x0004,
	AlloFloat64Ty		= 0x0008,
	
	/* signed integers */
	AlloSInt8Ty			= 0x0101, 
	AlloSInt16Ty		= 0x0102,
	AlloSInt32Ty		= 0x0104,
	AlloSInt64Ty		= 0x0108,
	
	/* unsigned integers */
	AlloUInt8Ty			= 0x0201,
	AlloUInt16Ty		= 0x0202,
	AlloUInt32Ty		= 0x0204,
	AlloUInt64Ty		= 0x0208,
	
	/* structural types */
	AlloArrayTy			= 0x1A2C,	/* 2C == 44 bytes, sizeof AlloArray */
	
	/* pointer types */
	AlloPointer32Ty		= 0x2F04,
	AlloPointer64Ty		= 0x2F08
};

typedef uint16_t AlloTy;
	
	
static size_t allo_type_size(const AlloTy ty);
typedef struct {
	
	/* The type of data stored (see enums above) */
	AlloTy type;
	
	/*
		The number of values per cell
		Aka components, planecount, parts, rank, order...
	*/
	uint8_t components;
	
	/* The number of dimensions (actually should not be > ALLO_ARRAY_MAX_DIMS!) */
	uint8_t dimcount;
	
	/* The size of each dimension */
	uint32_t dim[ALLO_ARRAY_MAX_DIMS];
	
	/* 
		# of bytes between elements of that dimension
	*/
	uint32_t stride[ALLO_ARRAY_MAX_DIMS];
	
} AlloArrayHeader;
	
	

typedef struct {
	/*
		The data encapsulated.
		First member of the struct to permit easy dereferencing without pointer adjustment.
		Anonymous union with 64-bit padding for portability
		Serialized data should be little-endian, but in-memory data should match to the native endianness
		Stride is necessary for arrays that are 4 or 8 byte aligned
	*/
	union{
		char * ptr;
		uint64_t pad;
	} data;
	
	/*
		The description of this data
	*/
	AlloArrayHeader header;
		
} AlloArray;


	
	

	
/*
 *
 ********* INLINE IMPLEMENTATION BELOW ***********
 *
 */
#pragma mark -------------------------------------
	
	
	

/*
	Return the size for a given type
*/
static inline size_t allo_type_size(const AlloTy ty) {
	switch(ty) {
		case AlloUInt8Ty:		return sizeof(uint8_t);	
		case AlloUInt16Ty:		return sizeof(uint16_t);
		case AlloUInt32Ty:		return sizeof(uint32_t);
		case AlloUInt64Ty:		return sizeof(uint64_t);
		case AlloSInt8Ty:		return sizeof(int8_t);
		case AlloSInt16Ty:		return sizeof(int16_t);
		case AlloSInt32Ty:		return sizeof(int32_t);
		case AlloSInt64Ty:		return sizeof(int64_t);
		case AlloFloat32Ty:		return sizeof(float);
		case AlloFloat64Ty:		return sizeof(double);
		case AlloArrayTy:		return sizeof(AlloArray);
		case AlloPointer32Ty:	return sizeof(int32_t);
		case AlloPointer64Ty:	return sizeof(int64_t);
		default:				return 0;
	}
}

/*
	Return the number of elements (cells) in a array
*/
static inline uint32_t allo_array_elements(const AlloArray * lat) {
	uint32_t i, elements = 1;
	for (i=0; i<lat->header.dimcount; i++) 
		elements *= lat->header.dim[i];
	return elements;
}

/*
	Return the memory footprint of a array
*/
static inline size_t allo_array_size(const AlloArray * lat) {
	int idx = lat->header.dimcount-1;
	return lat->header.stride[idx] * lat->header.dim[idx];
}

/*
	Set a array header, e.g. just after allocating
*/
static inline void allo_array_setheader(AlloArray * lat, const AlloArrayHeader * header) {
	memcpy(&lat->header, header, sizeof(AlloArrayHeader));
}

/*
	Set stride factors based on a specific byte alignment
*/
static inline void allo_array_setstride(AlloArrayHeader * h, unsigned alignSize){
	unsigned typeSize = allo_type_size(h->type);
	unsigned numDims = h->dimcount;
	h->stride[0] = h->components * typeSize;
	
	if(numDims>1){
		h->stride[1] = h->stride[0] * h->dim[0];		// compute ideal row stride amount
		unsigned remain = h->stride[1] % alignSize;		// compute pad bytes
		if(remain){ h->stride[1] += alignSize - remain;}// add pad bytes (if any)
		
		unsigned i=2;
		for(; i<numDims; ++i){ h->stride[i] = h->stride[i-1] * h->dim[i-1]; }
	}
}

static inline int allo_array_equal_headers(AlloArrayHeader *h1, const AlloArrayHeader *h2) {
	int equiv =	h1->components == h2->components && 
				h1->type == h2->type && 
				h1->dimcount == h2->dimcount;
					
	for(int i=0; i < h1->dimcount; i++) {
		equiv = equiv && h1->dim[i] == h2->dim[i];
		equiv = equiv && h1->stride[i] == h2->stride[i];
	}

	return equiv;
}


static inline void allo_array_header_clear(AlloArrayHeader *h) {
	memset(h, '\0', sizeof(AlloArrayHeader));
}

static inline void allo_array_clear(AlloArray *lat) {
	allo_array_header_clear( &(lat->header) );
	lat->data.ptr = 0;
}

static inline void allo_array_destroy(AlloArray *lat) {
	if(lat->data.ptr) {
		if(lat->data.ptr) {
			free(lat->data.ptr);
			allo_array_clear(lat);
		}
	}
}

static inline void allo_array_create(AlloArray *lat, const AlloArrayHeader *h) {
	allo_array_destroy(lat);
	allo_array_setheader(lat, h);
	lat->data.ptr = (char *)calloc(1, allo_array_size(lat));
}

static inline void allo_array_create1d(
	AlloArray *lat, 
	uint8_t components, 
	AlloTy type, 
	uint32_t dimx, 
	size_t align
) {
	AlloArrayHeader header;
	header.type = type;
	header.components = components;
	header.dimcount = 1;
	header.dim[0] = dimx;
	allo_array_setstride(&header, align);
	allo_array_create(lat, &header);
}

static inline void allo_array_create2d(
	AlloArray *lat, 
	uint8_t components, 
	AlloTy type, 
	uint32_t dimx, 
	uint32_t dimy, 
	size_t align
) {
	AlloArrayHeader header;
	header.type = type;
	header.components = components;
	header.dimcount = 2;
	header.dim[0] = dimx;
	header.dim[1] = dimy;
	allo_array_setstride(&header, align);
	allo_array_create(lat, &header);
}


/*
	Adapt a latticle to another size
*/
static inline void allo_array_adapt(AlloArray *lat, const AlloArrayHeader *h) {
	if(! allo_array_equal_headers( &(lat->header), h)) {
		allo_array_create(lat, h);
	}
}

static inline void allo_array_adapt2d(
	AlloArray *lat, 
	uint8_t components, 
	AlloTy type, 
	uint32_t dimx, 
	uint32_t dimy, 
	size_t align
) {
	AlloArrayHeader header;
	header.type = type;
	header.components = components;
	header.dimcount = 2;
	header.dim[0] = dimx;
	header.dim[1] = dimy;
	allo_array_setstride(&header, align);
	allo_array_adapt(lat, &header);
}

/*
	Copy a array into another array
*/
static inline void allo_array_copy(AlloArray *dst, AlloArray *src){
	allo_array_adapt(dst, &(src->header));
	memcpy(dst->data.ptr, src->data.ptr, allo_array_size(src));
}



#ifdef __cplusplus
}
#endif

#endif /* INCLUDE_ALLO_H */
