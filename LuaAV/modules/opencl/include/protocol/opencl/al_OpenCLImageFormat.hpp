#ifndef AL_OPENCL_IMAGE_FORMAT_H
#define AL_OPENCL_IMAGE_FORMAT_H 1

#include "al_OpenCLInternal.hpp"
#include "allocore/types/al_Array.hpp"
#include <vector>
#include <string>

using std::vector;
using std::list;
using std::string;

namespace al {
namespace cl {


/*
typedef struct _cl_image_format { 
	cl_channel_order image_channel_order;
	cl_channel_type image_channel_data_type;
} cl_image_format;
*/
class OpenCLImageFormat {
public:
	static cl_image_format format_from_array(AlloArray *array, bool normalized = true) {
		cl_image_format format;
		
		switch(array->header.components) {
			case 1:	format.image_channel_order = CL_A; break;
			case 2:	format.image_channel_order = CL_RA; break;
			case 3:	format.image_channel_order = CL_RGB; break;
			case 4:	format.image_channel_order = CL_RGBA; break;
			default:
				opencl_error(USER_OPENCL_ERROR, "format_from_array invalid number of components");
				break;
		}
		
		switch(array->header.type) {
			/* floating point numbers */
			case AlloFloat32Ty:	format.image_channel_data_type = CL_FLOAT; break;

			/* signed integers */
			AlloSInt8Ty:
				if(normalized){	format.image_channel_data_type = CL_SNORM_INT8; break; }
				else {			format.image_channel_data_type = CL_SIGNED_INT8; break; }
				
			AlloSInt16Ty:
				if(normalized){	format.image_channel_data_type = CL_SNORM_INT16; break; }
				else {			format.image_channel_data_type = CL_SIGNED_INT16; break; }
				
			AlloSInt32Ty:
				format.image_channel_data_type = CL_SIGNED_INT32; break;

			/* unsigned integers */
			AlloUInt8Ty:
				if(normalized){	format.image_channel_data_type = CL_UNORM_INT8; break; }
				else {			format.image_channel_data_type = CL_UNSIGNED_INT8; break; }
				
			AlloUInt16Ty:
				if(normalized){	format.image_channel_data_type = CL_UNORM_INT16; break; }
				else {			format.image_channel_data_type = CL_UNSIGNED_INT16; break; }
				
			AlloUInt32Ty:
				format.image_channel_data_type = CL_UNSIGNED_INT32; break;
			
			default:
				opencl_error(USER_OPENCL_ERROR, "format_from_array invalid type");
				break;
		}
		
		return format;
	}

	static cl_image_format get_format(
		cl_channel_order image_channel_order, 
		cl_channel_type image_channel_data_type
	) {
		cl_image_format fmt;
		fmt.image_channel_order = image_channel_order;
		fmt.image_channel_data_type = image_channel_data_type;
		
		return vertify_format(fmt);
	}
	
	/*
		TODO: use:
			cl_int clGetSupportedImageFormats(
				cl_context context, 
				cl_mem_flags flags,
				cl_mem_object_type image_type, 
				cl_uint num_entries, 
				cl_image_format *image_formats, 
				cl_uint *num_image_formats
			)
	*/
	
	static cl_image_format vertify_format(cl_image_format fmt) {
		switch(fmt.image_channel_order) {
			case CL_R:
			case CL_A:
				return fmt;
				
			case CL_INTENSITY:
				switch(fmt.image_channel_data_type) {
					case CL_UNORM_INT8:
					case CL_UNORM_INT16:
					case CL_SNORM_INT8:
					case CL_SNORM_INT16:
					case CL_HALF_FLOAT:
					case CL_FLOAT:
						return fmt;
						
					default:
						opencl_error(
							USER_OPENCL_ERROR, 
							"invalid data type with format CL_INTENSITY"
						);
						return fmt;
				}
			
			case CL_LUMINANCE:
				switch(fmt.image_channel_data_type) {
					case CL_UNORM_INT8:
					case CL_UNORM_INT16:
					case CL_SNORM_INT8:
					case CL_SNORM_INT16:
					case CL_HALF_FLOAT:
					case CL_FLOAT:
						return fmt;
						
					default:
						opencl_error(
							USER_OPENCL_ERROR, 
							"invalid data type with format CL_LUMINANCE"
						);
						return fmt;
				}
				
			case CL_RG:
			case CL_RA:
				return fmt;
				
			case CL_RGB:
				switch(fmt.image_channel_data_type) {
					case CL_UNORM_SHORT_565:
					case CL_UNORM_SHORT_555:
					case CL_UNORM_INT_101010:
						return fmt;
						
					default:
						opencl_error(
							USER_OPENCL_ERROR, 
							"invalid data type with format CL_RGB"
						);
						return fmt;
				}
				
			case CL_RGBA:
				return fmt;
				
			case CL_ARGB:
			case CL_BGRA:
				switch(fmt.image_channel_data_type) {
					case CL_UNORM_INT8:
					case CL_SNORM_INT8:
					case CL_SIGNED_INT8:
					case CL_UNSIGNED_INT8:
						return fmt;
						
					default:
						opencl_error(
							USER_OPENCL_ERROR, 
							"invalid data type with format CL_ARGB or CL_BGRA"
						);
						return fmt;
				}
		}
	}

};

}	// cl::
}	// al::


#endif // AL_OPENCL_IMAGE_FORMAT_H
