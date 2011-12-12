#include "lua_opencv.h"
#include "lua_utility.h"


int opencv_type(int components, AlloTy ty) {
	switch(ty) {
		case AlloSInt8Ty:
			switch(components) {
				case 1: return CV_8SC1;
				case 2: return CV_8SC2;
				case 3: return CV_8SC3;
				case 4:
				default:
					return CV_8SC4;
			}
			
		case AlloSInt16Ty:
			switch(components) {
				case 1: return CV_16SC1;
				case 2: return CV_16SC2;
				case 3: return CV_16SC3;
				case 4:
				default:
					return CV_16SC4;
			}
		
		case AlloSInt32Ty:
			switch(components) {
				case 1: return CV_32SC1;
				case 2: return CV_32SC2;
				case 3: return CV_32SC3;
				case 4:
				default:
					return CV_32SC4;
			}
			
//		case AlloSInt64Ty: // error
			
		
		case AlloUInt8Ty:
			switch(components) {
				case 1: return CV_8UC1;
				case 2: return CV_8UC2;
				case 3: return CV_8UC3;
				case 4:
				default:
					return CV_8UC4;
			}
			
		case AlloUInt16Ty:
			switch(components) {
				case 1: return CV_16UC1;
				case 2: return CV_16UC2;
				case 3: return CV_16UC3;
				case 4:
				default:
					return CV_16UC4;
			}
			
//		case AlloUInt32Ty: // error
//		case AlloUInt64Ty: // error
		
		case AlloFloat32Ty:
			switch(components) {
				case 1: return CV_32FC1;
				case 2: return CV_32FC2;
				case 3: return CV_32FC3;
				case 4:
				default:
					return CV_32FC4;
			}
			
		case AlloFloat64Ty:
			switch(components) {
				case 1: return CV_64FC1;
				case 2: return CV_64FC2;
				case 3: return CV_64FC3;
				case 4:
				default:
					return CV_64FC4;
			}
	}
	return -1;
}


// does shallow 'O(1)' copy
cv::Mat mat_from_array(AlloArray &lat) {
	return cv::Mat(
		lat.header.dim[1],
		lat.header.dim[0],
		opencv_type(lat.header.components, lat.header.type), 
		lat.data.ptr,
		lat.header.stride[1]
	);
}

int opencv_filter_bilateral(lua_State *L) {
	AlloArray *src = lua_array_checkto(L, 1);
	AlloArray *dst = lua_array_checkto(L, 2);
	if(
		allo_array_equal_headers(&(src->header), &(dst->header)) && 
		lua::is<int>(L, 3) &&
		lua::is<double>(L, 4) && 
		lua::is<double>(L, 5)
	) {
		int d = lua::to<int>(L, 3);
		int sigmaColor = lua::to<double>(L, 4);
		int sigmaSpace = lua::to<double>(L, 5);
		int borderType = lua::opt<int>(L, 6, cv::BORDER_DEFAULT);

		cv::Mat src_mat = mat_from_array(*src);
		cv::Mat dst_mat = mat_from_array(*dst);
		cv::bilateralFilter(src_mat, dst_mat, d, sigmaColor, sigmaSpace, borderType);
	}
	else {
		luaL_error(L, "opencv.filter.bilateral: invalid arguments");
	}
	return 0;
}

int opencv_filter_box_filter(lua_State *L) {
	AlloArray *src = lua_array_checkto(L, 1);
	AlloArray *dst = lua_array_checkto(L, 2);
	cv::Size ksize(3, 3);
	cv::Point anchor(-1, -1);
	
	if(
		allo_array_equal_headers(&(src->header), &(dst->header))
	) {
		lua::to_vec_t<int>(L, 3, 2, &(ksize.width));
		lua::to_vec_t<int>(L, 4, 2, &(anchor.x));
		
		bool normalize = lua::opt<bool>(L, 5, true);
		int borderType = lua::opt<int>(L, 6, cv::BORDER_DEFAULT);

		cv::Mat src_mat = mat_from_array(*src);
		cv::Mat dst_mat = mat_from_array(*dst);
		cv::boxFilter(src_mat, dst_mat, -1, ksize, anchor, normalize, borderType);
	}
	else {
		luaL_error(L, "opencv.filter.box_filter: invalid arguments");
	}
	return 0;
}

int opencv_filter_blur(lua_State *L) {
	AlloArray *src = lua_array_checkto(L, 1);
	AlloArray *dst = lua_array_checkto(L, 2);
	cv::Size ksize(3, 3);
	cv::Point anchor(-1, -1);
	
	if(
		allo_array_equal_headers(&(src->header), &(dst->header))
	) {
		lua::to_vec_t<int>(L, 3, 2, &(ksize.width));
		lua::to_vec_t<int>(L, 4, 2, &(anchor.x));
		
		int borderType = lua::opt<int>(L, 5, cv::BORDER_DEFAULT);

		cv::Mat src_mat = mat_from_array(*src);
		cv::Mat dst_mat = mat_from_array(*dst);
		cv::blur(src_mat, dst_mat, ksize, anchor, borderType);
	}
	else {
		luaL_error(L, "opencv.filter.blur: invalid arguments");
	}
	return 0;
}

int opencv_filter_dilate(lua_State *L) {
	AlloArray *src = lua_array_checkto(L, 1);
	AlloArray *dst = lua_array_checkto(L, 2);
	cv::Point anchor(-1, -1);
	
	if(
		allo_array_equal_headers(&(src->header), &(dst->header))
	) {
		lua::to_vec_t<int>(L, 3, 2, &(anchor.x));
		int iterations = lua::opt<int>(L, 4, 1);
		int borderType = lua::opt<int>(L, 5, cv::BORDER_DEFAULT);

		cv::Mat src_mat = mat_from_array(*src);
		cv::Mat dst_mat = mat_from_array(*dst);
		cv::dilate(src_mat, dst_mat, cv::Mat(), anchor, iterations, borderType);
	}
	else {
		luaL_error(L, "opencv.filter.dilate: invalid arguments");
	}
	return 0;
}

int opencv_filter_erode(lua_State *L) {
	AlloArray *src = lua_array_checkto(L, 1);
	AlloArray *dst = lua_array_checkto(L, 2);
	cv::Point anchor(-1, -1);
	
	if(
		allo_array_equal_headers(&(src->header), &(dst->header))
	) {
		lua::to_vec_t<int>(L, 3, 2, &(anchor.x));
		int iterations = lua::opt<int>(L, 4, 1);
		int borderType = lua::opt<int>(L, 5, cv::BORDER_DEFAULT);

		cv::Mat src_mat = mat_from_array(*src);
		cv::Mat dst_mat = mat_from_array(*dst);
		cv::erode(src_mat, dst_mat, cv::Mat(), anchor, iterations, borderType);
	}
	else {
		luaL_error(L, "opencv.filter.erode: invalid arguments");
	}
	return 0;
}

int opencv_filter_median_blur(lua_State *L) {
	AlloArray *src = lua_array_checkto(L, 1);
	AlloArray *dst = lua_array_checkto(L, 2);
	
	if(
		allo_array_equal_headers(&(src->header), &(dst->header)) &&
		lua::is<int>(L, 3)
	) {
		int ksize = lua::to<int>(L, 3);

		cv::Mat src_mat = mat_from_array(*src);
		cv::Mat dst_mat = mat_from_array(*dst);
		cv::medianBlur(src_mat, dst_mat, ksize);
	}
	else {
		luaL_error(L, "opencv.filter.median_blur: invalid arguments");
	}
	return 0;
}

int opencv_filter_morphology_ex(lua_State *L) {
	AlloArray *src = lua_array_checkto(L, 1);
	AlloArray *dst = lua_array_checkto(L, 2);
	cv::Point anchor(-1, -1);
	
	if(
		allo_array_equal_headers(&(src->header), &(dst->header)) && 
		lua::is<int>(L, 3)
	) {
		int op = lua::to<int>(L, 3);
		lua::to_vec_t<int>(L, 4, 2, &(anchor.x));
		int iterations = lua::opt<int>(L, 5, 1);
		int borderType = lua::opt<int>(L, 6, cv::BORDER_DEFAULT);

		cv::Mat src_mat = mat_from_array(*src);
		cv::Mat dst_mat = mat_from_array(*dst);
		cv::morphologyEx(src_mat, dst_mat, op, cv::Mat(), anchor, iterations, borderType);
	}
	else {
		luaL_error(L, "opencv.filter.morphology_ex: invalid arguments");
	}
	return 0;
}

int opencv_filter_laplacian(lua_State *L) {
	AlloArray *src = lua_array_checkto(L, 1);
	AlloArray *dst = lua_array_checkto(L, 2);
	cv::Point anchor(-1, -1);
	
	if(
		allo_array_equal_headers(&(src->header), &(dst->header))
	) {
		int ksize = lua::opt<int>(L, 3, 1);
		double scale = lua::opt<double>(L, 4, 1.);
		double delta = lua::opt<double>(L, 5, 0.);
		int borderType = lua::opt<int>(L, 6, cv::BORDER_DEFAULT);
		
		cv::Mat src_mat = mat_from_array(*src);
		cv::Mat dst_mat = mat_from_array(*dst);
		cv::Laplacian(src_mat, dst_mat, -1, ksize, scale, delta, borderType);
	}
	else {
		luaL_error(L, "opencv.filter.laplacian: invalid arguments");
	}
	return 0;
}

int opencv_filter_sobel(lua_State *L) {
	AlloArray *src = lua_array_checkto(L, 1);
	AlloArray *dst = lua_array_checkto(L, 2);
	cv::Point anchor(-1, -1);
	
	if(
		allo_array_equal_headers(&(src->header), &(dst->header)) && 
		lua::is<int>(L, 3) && 
		lua::is<int>(L, 4)
	) {
		int xorder = lua::to<int>(L, 3);
		int yorder = lua::to<int>(L, 4);
		int ksize = lua::opt<int>(L, 5, 3);
		double scale = lua::opt<double>(L, 6, 1.);
		double delta = lua::opt<double>(L, 7, 0.);
		int borderType = lua::opt<int>(L, 8, cv::BORDER_DEFAULT);
		
		cv::Mat src_mat = mat_from_array(*src);
		cv::Mat dst_mat = mat_from_array(*dst);
		cv::Sobel(src_mat, dst_mat, -1, xorder, yorder, ksize, scale, delta, borderType);
	}
	else {
		luaL_error(L, "opencv.filter.sobel: invalid arguments");
	}
	return 0;
}

int opencv_filter_scharr(lua_State *L) {
	AlloArray *src = lua_array_checkto(L, 1);
	AlloArray *dst = lua_array_checkto(L, 2);
	cv::Point anchor(-1, -1);
	
	if(
		allo_array_equal_headers(&(src->header), &(dst->header)) && 
		lua::is<int>(L, 3) && 
		lua::is<int>(L, 4)
	) {
		int xorder = lua::to<int>(L, 3);
		int yorder = lua::to<int>(L, 4);
		double scale = lua::opt<double>(L, 5, 1.);
		double delta = lua::opt<double>(L, 6, 0.);
		int borderType = lua::opt<int>(L, 7, cv::BORDER_DEFAULT);
		
		cv::Mat src_mat = mat_from_array(*src);
		cv::Mat dst_mat = mat_from_array(*dst);
		cv::Scharr(src_mat, dst_mat, -1, xorder, yorder, scale, delta, borderType);
	}
	else {
		luaL_error(L, "opencv.filter.scharr: invalid arguments");
	}
	return 0;
}

// Geometry
int opencv_geomety_resize(lua_State *L) {
	AlloArray *src = lua_array_checkto(L, 1);
	AlloArray *dst = lua_array_checkto(L, 2);
	
	if(
		src->header.components == dst->header.components && 
		src->header.type == dst->header.type && 
		src->header.dimcount == dst->header.dimcount 
	) {
		int interpolation = lua::opt<int>(L, 3, cv::INTER_LINEAR);
		
		cv::Mat src_mat = mat_from_array(*src);
		cv::Mat dst_mat = mat_from_array(*dst);
		cv::resize(src_mat, dst_mat, dst_mat.size(), 0, 0, interpolation);
	}
	else {
		luaL_error(L, "opencv.geometry.resize: invalid arguments");
	}
	return 0;
}

// Misc.
int opencv_filter_adaptive_threshold(lua_State *L) {
	AlloArray *src = lua_array_checkto(L, 1);
	AlloArray *dst = lua_array_checkto(L, 2);
	
	if(
		allo_array_equal_headers(&(src->header), &(dst->header)) && 
		lua::is<double>(L, 3) &&
		lua::is<int>(L, 4) &&
		lua::is<int>(L, 5) &&
		lua::is<int>(L, 6) &&
		lua::is<double>(L, 7)
	) {
		double maxValue = lua::to<double>(L, 3);
		int adaptiveMethod = lua::to<int>(L, 4);
		int thresholdType = lua::to<int>(L, 5);
		int blockSize = lua::to<int>(L, 6);
		double C = lua::to<double>(L, 7);
		
		cv::Mat src_mat = mat_from_array(*src);
		cv::Mat dst_mat = mat_from_array(*dst);
		cv::adaptiveThreshold(src_mat, dst_mat, maxValue, adaptiveMethod, thresholdType, blockSize, C);
	}
	else {
		luaL_error(L, "opencv.filter.adaptive_threshold: invalid arguments");
	}
	return 0;
}

int opencv_filter_cvt_color(lua_State *L) {
	AlloArray *src = lua_array_checkto(L, 1);
	AlloArray *dst = lua_array_checkto(L, 2);
	
	bool equal_dim = src->header.dimcount == dst->header.dimcount;
	if(equal_dim) {
		for(int i=0; i < src->header.dimcount; i++) {
			equal_dim = equal_dim && (src->header.dim[i] == dst->header.dim[i]);
		}
	}
	if(
		src->header.type == dst->header.type && 
		equal_dim &&
		lua::is<int>(L, 3)
	) {
		int code = lua::to<int>(L, 3);
		
		cv::Mat src_mat = mat_from_array(*src);
		cv::Mat dst_mat = mat_from_array(*dst);
		cv::cvtColor(src_mat, dst_mat, code, dst->header.components);
	}
	else {
		luaL_error(L, "opencv.filter.cvt_color: invalid arguments");
	}
	return 0;
}

int opencv_filter_distance_transform(lua_State *L) {
	AlloArray *src = lua_array_checkto(L, 1);
	AlloArray *dst = lua_array_checkto(L, 2);
	
	bool equal_dim = src->header.dimcount == dst->header.dimcount;
	if(equal_dim) {
		for(int i=0; i < src->header.dimcount; i++) {
			equal_dim = equal_dim && (src->header.dim[i] == dst->header.dim[i]);
		}
	}
	if(
		src->header.components == dst->header.components && 
		equal_dim && 
		lua::is<int>(L, 3) &&
		lua::is<int>(L, 4)
	) {
		int distanceType = lua::to<int>(L, 3);
		int maskSize = lua::to<int>(L, 4);
		
		cv::Mat src_mat = mat_from_array(*src);
		cv::Mat dst_mat = mat_from_array(*dst);
		cv::distanceTransform(src_mat, dst_mat, distanceType, maskSize);
	}
	else {
		luaL_error(L, "opencv.filter.distance_transform: invalid arguments");
	}
	return 0;
}

int opencv_filter_threshold(lua_State *L) {
	AlloArray *src = lua_array_checkto(L, 1);
	AlloArray *dst = lua_array_checkto(L, 2);
	
	if(
		src->header.type == dst->header.type && 
		allo_array_equal_headers(&(src->header), &(dst->header)) && 
		lua::is<double>(L, 3) &&
		lua::is<double>(L, 4) &&
		lua::is<int>(L, 5)
	) {
		double thresh = lua::to<double>(L, 3);
		double maxVal = lua::to<double>(L, 4);
		int thresholdType = lua::to<int>(L, 5);
		
		cv::Mat src_mat = mat_from_array(*src);
		cv::Mat dst_mat = mat_from_array(*dst);
		cv::threshold(src_mat, dst_mat, thresh, maxVal, thresholdType);
	}
	else {
		luaL_error(L, "opencv.filter.threshold: invalid arguments");
	}
	return 0;
}

#ifdef __cplusplus
extern "C" {
#endif

struct enum_entry {
	const char *name;
	int val;
};

#define ENUM_ENTRY(v)	{#v, (int)cv::v}
#define DEFINE_ENTRY(v) {#v, v}

enum_entry opencv_enums[] = {
	ENUM_ENTRY(BORDER_REPLICATE),
	ENUM_ENTRY(BORDER_CONSTANT),
	ENUM_ENTRY(BORDER_REFLECT),
	ENUM_ENTRY(BORDER_REFLECT_101),
	ENUM_ENTRY(BORDER_REFLECT101),
	ENUM_ENTRY(BORDER_WRAP),
	ENUM_ENTRY(BORDER_TRANSPARENT),
	ENUM_ENTRY(BORDER_DEFAULT),
	ENUM_ENTRY(BORDER_ISOLATED),
	
	ENUM_ENTRY(MORPH_ERODE),
	ENUM_ENTRY(MORPH_DILATE),
	ENUM_ENTRY(MORPH_OPEN),
	ENUM_ENTRY(MORPH_CLOSE),
	ENUM_ENTRY(MORPH_GRADIENT),
	ENUM_ENTRY(MORPH_TOPHAT),
	ENUM_ENTRY(MORPH_BLACKHAT),
	
	ENUM_ENTRY(INTER_NEAREST),
	ENUM_ENTRY(INTER_LINEAR),
	ENUM_ENTRY(INTER_AREA),
	ENUM_ENTRY(INTER_CUBIC),
	ENUM_ENTRY(INTER_LANCZOS4),
	
	ENUM_ENTRY(ADAPTIVE_THRESH_MEAN_C),
	ENUM_ENTRY(ADAPTIVE_THRESH_GAUSSIAN_C),
	
	ENUM_ENTRY(THRESH_BINARY),
	ENUM_ENTRY(THRESH_BINARY_INV),
	ENUM_ENTRY(THRESH_TRUNC),
	ENUM_ENTRY(THRESH_TOZERO),
	ENUM_ENTRY(THRESH_TOZERO_INV),
	
	DEFINE_ENTRY(CV_BGR2BGRA),
	DEFINE_ENTRY(CV_RGB2RGBA),
	DEFINE_ENTRY(CV_BGRA2BGR),
	DEFINE_ENTRY(CV_RGBA2RGB),
	DEFINE_ENTRY(CV_BGR2RGBA),
	DEFINE_ENTRY(CV_RGB2BGRA),
	DEFINE_ENTRY(CV_RGBA2BGR),
	DEFINE_ENTRY(CV_BGRA2RGB),
	DEFINE_ENTRY(CV_BGR2RGB),
	DEFINE_ENTRY(CV_RGB2BGR),
	DEFINE_ENTRY(CV_BGRA2RGBA),
	DEFINE_ENTRY(CV_RGBA2BGRA),
	DEFINE_ENTRY(CV_BGR2GRAY),
	DEFINE_ENTRY(CV_RGB2GRAY),
	DEFINE_ENTRY(CV_GRAY2BGR),
	DEFINE_ENTRY(CV_GRAY2RGB),
	DEFINE_ENTRY(CV_GRAY2BGRA),
	DEFINE_ENTRY(CV_GRAY2RGBA),
	DEFINE_ENTRY(CV_BGRA2GRAY),
	DEFINE_ENTRY(CV_RGBA2GRAY),
	DEFINE_ENTRY(CV_BGR2HSV),
	DEFINE_ENTRY(CV_RGB2HSV),
	DEFINE_ENTRY(CV_BGR2Lab),
	DEFINE_ENTRY(CV_RGB2Lab),
	DEFINE_ENTRY(CV_BGR2Luv),
	DEFINE_ENTRY(CV_RGB2Luv),
	DEFINE_ENTRY(CV_BGR2HLS),
	DEFINE_ENTRY(CV_RGB2HLS),
	DEFINE_ENTRY(CV_HSV2BGR),
	DEFINE_ENTRY(CV_HSV2RGB),
	DEFINE_ENTRY(CV_Lab2BGR),
	DEFINE_ENTRY(CV_Lab2RGB),
	DEFINE_ENTRY(CV_Luv2BGR),
	DEFINE_ENTRY(CV_Luv2RGB),
	DEFINE_ENTRY(CV_HLS2BGR),
	DEFINE_ENTRY(CV_HLS2RGB),
	
	DEFINE_ENTRY(CV_DIST_L1),
	DEFINE_ENTRY(CV_DIST_L2),
	DEFINE_ENTRY(CV_DIST_C),
	DEFINE_ENTRY(CV_DIST_MASK_PRECISE),
};



int luaopen_opencv(lua_State *L) {
	const char *libname = lua_tostring(L, -1);
	luaL_Reg opencv_lib[] = {
		{ "bilateral", opencv_filter_bilateral },
		{ "box_filter", opencv_filter_box_filter },
		{ "blur", opencv_filter_blur },
		{ "dilate", opencv_filter_dilate },
		{ "erode", opencv_filter_erode },
		{ "median_blur", opencv_filter_median_blur },
		{ "morphology_ex", opencv_filter_morphology_ex },
		{ "laplacian", opencv_filter_laplacian },
		{ "sobel", opencv_filter_sobel },
		{ "scharr", opencv_filter_scharr },
		
		{ "resize", opencv_geomety_resize },
		
		{ "adaptive_threshold", opencv_filter_adaptive_threshold },
		{ "cvt_color", opencv_filter_cvt_color },
		{ "distance_transform", opencv_filter_distance_transform },
		{ "threshold", opencv_filter_threshold },
		{NULL, NULL}
	};
	luaL_register(L, libname, opencv_lib);
	
	for(int i=0; i < sizeof(opencv_enums)/sizeof(enum_entry); i++) {
		lua::pushfield<int>(L, -1, opencv_enums[i].name, opencv_enums[i].val);
	}

	return 1;
}

#ifdef __cplusplus
}
#endif