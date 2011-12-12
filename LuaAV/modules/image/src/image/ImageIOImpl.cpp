#include "ImageIOImpl.h"

namespace image {

ImageIOImpl :: ImageIOImpl()
:	ImageImpl(IMAGEIO),
	mImage(0)
{}

ImageIOImpl :: ~ImageIOImpl() {
	if(mImage) {
		CGImageRelease(mImage);
	}
}

void ImageIOImpl :: render_image(AlloArray &mat) {
	size_t width = CGImageGetWidth(mImage);
	size_t height = CGImageGetHeight(mImage);
	
	CGColorSpaceRef color_space = CGColorSpaceCreateWithName(kCGColorSpaceGenericRGB);
	if(color_space == NULL) {
		fprintf(stderr, "Error allocating color space\n");
		return;
	}

	AlloArrayHeader h;
	CGBitmapInfo bitmap_info = CGImageGetBitmapInfo(mImage);	
	CGBitmapInfo context_info = 0;
	CGImageAlphaInfo alpha_info = (CGImageAlphaInfo)(bitmap_info & kCGBitmapAlphaInfoMask);
	
	if(
		(alpha_info == kCGImageAlphaNone) ||
		(alpha_info ==kCGImageAlphaNoneSkipFirst) || 
		(alpha_info == kCGImageAlphaNoneSkipLast)
	) {
		context_info |= kCGImageAlphaNoneSkipLast;
	}
	else {
		context_info |= kCGImageAlphaPremultipliedLast;
	}
	h.components = 4;
	
	if(bitmap_info & kCGBitmapFloatComponents) {
		context_info |= kCGBitmapFloatComponents;
		h.type = AlloFloat32Ty;
	}
	else {
		h.type = AlloUInt8Ty;
	}
	
	h.dimcount = 2;
	h.dim[0] = width;
	h.dim[1] = height;
	allo_array_setstride(&h, 1);
	allo_array_adapt(&mat, &h);
	
    CGContextRef context = CGBitmapContextCreate(
		mat.data.ptr,
		width,
		height,
		8*allo_type_size(mat.header.type),
		mat.header.stride[1],
		color_space,
		context_info
	);
	
	if(context == NULL) {
        fprintf(stderr, "Context not created!");
    }
	
	// render the image
//	CGContextSetFillColorWithColor(c, bgcolor);
	CGRect rect = CGRectMake(0, 0, width, height);
	CGContextDrawImage(context, rect, mImage);
//	CGFloat components[] = {1., 1., 1., 1.};
//	CGColorRef bgcolor = CGColorCreate(color_space, components);
//	CGContextSetFillColorWithColor(context, bgcolor);
//	CGContextFillRect(context, rect);

    CGColorSpaceRelease(color_space);
	CGContextRelease(context);
}

void ImageIOImpl :: get_image_ref(const char *filename) {
	CFStringRef cf_filename = CFStringCreateWithCString(
		kCFAllocatorDefault, 
		filename, 
		kCFStringEncodingASCII
	);
	CFURLRef url = CFURLCreateWithFileSystemPath(
		kCFAllocatorDefault, 
		cf_filename, 
		kCFURLPOSIXPathStyle, 
		false
	);
	
	CFStringRef keys[] = { kCGImageSourceShouldAllowFloat };
	CFBooleanRef values[] = { kCFBooleanTrue };
	CFDictionaryRef options = CFDictionaryCreate(
		kCFAllocatorDefault,
		(const void **)&keys, 
		(const void **)&values, 
		1, 
		NULL, NULL
	);
	
	CGImageSourceRef source = CGImageSourceCreateWithURL(url, options);
	if(source) {
		mImage = CGImageSourceCreateImageAtIndex(source, 0, NULL);
		CFRelease(source);
	}
	else {
		// error
	}
	
	CFRelease(cf_filename);	
	CFRelease(url);	
	CFRelease(options);
}

ImageError ImageIOImpl :: load(const char *filename, AlloArray &mat) {
	ImageError err = IMAGE_ERROR_NONE;
	get_image_ref(filename);
	if(mImage) {
		render_image(mat);
		CGImageRelease(mImage);
		mImage = 0;
	}
	
	return err;
}


CFStringRef uti_for_extension(const char *filename) {
	// kUTTypeJPEG: .jpg, .jpeg
	// kUTTypeJPEG2000: .jp2
	// kUTTypeTIFF: .tif, .tiff
	// kUTTypePICT: .pic, .pct, .pict
	// kUTTypePNG: .png
	// kUTTypeAppleICNS: .icns
	// kUTTypeGIF: .gif
	// kUTTypeBMP: .bmp
	// kUTTypeICO: .ico

	const char *ext = strrchr(filename, '.');
	
	if(strcmp(ext, ".jp2") == 0) {
		return kUTTypeJPEG2000;
	}
	else if(
		strcmp(ext, ".tif") == 0 || 
		strcmp(ext, ".tiff") == 0
	) {
		return kUTTypeTIFF;
	}
	/*
	else if(
		strcmp(ext, ".pic") == 0 || 
		strcmp(ext, ".pct") == 0 || 
		strcmp(ext, ".pict") == 0
	) {
		return kUTTypePICT;
	}
	*/
	else if(strcmp(ext, ".png") == 0) {
		return kUTTypePNG;
	}
	/*
	else if(strcmp(ext, ".icns") == 0) {
		return kUTTypeAppleICNS;
	}
	*/
	else if(strcmp(ext, ".gif") == 0) {
		return kUTTypeGIF;
	}
	else if(strcmp(ext, ".bmp") == 0) {
		return kUTTypeBMP;
	}
	/*
	else if(strcmp(ext, ".ico") == 0) {
		return kUTTypeICO;
	}
	*/
	return kUTTypeJPEG;
}

ImageError ImageIOImpl :: save(const char *filename, AlloArray &mat) {
	ImageError err = IMAGE_ERROR_NONE;

	CGColorSpaceRef color_space = CGColorSpaceCreateWithName(kCGColorSpaceGenericRGB);
	CGDataProviderRef provider = CGDataProviderCreateWithData(
		NULL,
		mat.data.ptr,
		allo_array_size(&mat),
		NULL
	);
	
	CGBitmapInfo bitmap_info = kCGBitmapByteOrderDefault;
	if(mat.header.type == AlloSInt32Ty) {
		bitmap_info |= kCGBitmapFloatComponents;
	}
	if(mat.header.components >= 4) {
		bitmap_info |= kCGImageAlphaNoneSkipLast;
		//bitmap_info |= kCGImageAlphaLast;
	}
	
	CGImageRef srcimg = CGImageCreate(
		mat.header.dim[0],
		mat.header.dim[1],
		8*allo_type_size(mat.header.type),
		8*allo_type_size(mat.header.type)*mat.header.components,
		mat.header.stride[1],
		color_space,
		bitmap_info,
		provider,
		NULL,
		false,
		kCGRenderingIntentDefault
	);
	
	float compression_quality = 1.0;
	CFStringRef dest_keys[] = { kCGImageDestinationLossyCompressionQuality };
	CFNumberRef dest_values[] = { CFNumberCreate(kCFAllocatorDefault, kCFNumberFloat32Type, &compression_quality) };
	CFDictionaryRef dest_options = CFDictionaryCreate(
		kCFAllocatorDefault,
		(const void **)&dest_keys, 
		(const void **)&dest_values, 
		1, 
		NULL, NULL
	);
	
	
	CFStringRef filepath = CFStringCreateWithBytes(
		kCFAllocatorDefault,
		(const UInt8 *)filename,
		strlen(filename),
		kCFStringEncodingASCII,
		false
	);
	CFStringRef uti = uti_for_extension(filename);;
	CFURLRef url = CFURLCreateWithFileSystemPath(
		kCFAllocatorDefault,
		filepath,
		kCFURLPOSIXPathStyle,
		false
	);
	
	CGImageDestinationRef dest = CGImageDestinationCreateWithURL(url, uti, 1, NULL);
	CGImageDestinationAddImage(dest, srcimg, dest_options);
	CGImageDestinationFinalize(dest);
	
	// cleanup
	CFRelease(dest);
	CFRelease(url);
	CFRelease(filepath);
	CFRelease(dest_options);
	CFRelease(dest_values[0]);
	CFRelease(srcimg);
	CFRelease(provider);
	CFRelease(color_space);
	
	return err;
}


} // image::
