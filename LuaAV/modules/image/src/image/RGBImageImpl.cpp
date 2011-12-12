#include "RGBImageImpl.h"
#include "Image.h"
#include <stdio.h>

namespace image {

RGBImpl :: RGBImpl()
:	ImageImpl(ImageImpl::RGB)
{
	allo_array_clear(&mImage);
}

RGBImpl :: ~RGBImpl() {
	allo_array_destroy(&mImage);
}

ImageError RGBImpl :: load(const char *filename, AlloArray &mat) {
	ImageError err = IMAGE_ERROR_NONE;

	FILE* rgbFile = fopen(filename, "rb");  
	if(!rgbFile) {
		fprintf(stderr, "RGBImpl: couldn't open file %s\n", filename);
		return IMAGE_ERROR_NONE;
	}
	
	int width = 0;
	int height = 0;
	int depth = 0;  
	int type = 0;  
	if(rgb_header_read(rgbFile, 1, &width, &height, &depth, &type)) {
		fprintf(stderr, "RGBImpl: invalid header\n");
		return IMAGE_ERROR_NONE;
	}
	
	if(width*height <= 0) {
		fprintf(stderr, "RGBImpl: image dimensions are 0 or negative\n");
		return IMAGE_ERROR_NONE;
	}

	AlloArray r; allo_array_clear(&r); allo_array_create2d(&r, 1, AlloUInt8Ty, width, height, 1);
	AlloArray g; allo_array_clear(&g); allo_array_create2d(&g, 1, AlloUInt8Ty, width, height, 1);
	AlloArray b; allo_array_clear(&b); allo_array_create2d(&b, 1, AlloUInt8Ty, width, height, 1);
                                    
	switch(type) {       
	// Uncompressed
	case 1: 
		rgb_data_read(
			rgbFile,
			1,
			width,
			height,
			depth,
			type,
			(unsigned char *)r.data.ptr,
			(unsigned char *)g.data.ptr,
			(unsigned char *)b.data.ptr
		);
		break;

	// Run-length encoding
	case 257:
		rgb_rle_data_read(
			rgbFile,
			1,
			width,
			height,
			depth,
			type,
			(unsigned char *)r.data.ptr,
			(unsigned char *)g.data.ptr,
			(unsigned char *)b.data.ptr
		);
		break;

	// ERROR!
	default:
		fprintf(stderr, "RGBImpl: unknown RGB file type.\n");
		return IMAGE_ERROR_GENERIC;
	}

	allo_array_adapt2d(&mat, 3, AlloUInt8Ty, width, height, 1);
	
	int rowsize = mat.header.stride[1];
	int rowsize_channel = r.header.stride[1];
	
	for(int j=0; j < height; j++) {
		unsigned char *data = (unsigned char *)(mat.data.ptr + j*rowsize);
		unsigned char *rr = (unsigned char *)(r.data.ptr + j*rowsize_channel);
		unsigned char *gg = (unsigned char *)(g.data.ptr + j*rowsize_channel);
		unsigned char *bb = (unsigned char *)(b.data.ptr + j*rowsize_channel);
		
		for(int i=0; i < width; i++) {
			*data++ = *rr++;
			*data++ = *gg++;
			*data++ = *bb++;
		}
	}
	
	return err;
}

ImageError RGBImpl :: save(const char *filename, AlloArray &mat) {
	ImageError err = IMAGE_ERROR_NONE;
	
	return err;
}

}	// image::
