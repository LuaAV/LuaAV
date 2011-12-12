#include "FreeImageImpl.h"
#include <string.h>
#include <stdio.h>

namespace image {

bool FreeImageImpl::c_initialized = false;


FreeImageImpl :: FreeImageImpl()
:	ImageImpl(ImageImpl::FREEIMAGE),
	mImage(NULL)
{
	if(!c_initialized) {
		initialize();
		c_initialized = true;
	}
}

FreeImageImpl :: ~FreeImageImpl() {
	destroy();
}

void FreeImageImpl :: destroy() {
	if(mImage) {
		FreeImage_Unload(mImage);
		mImage = NULL;
	}
}

ImageError FreeImageImpl :: save(const char *filename, AlloArray &mat) {
	ImageError err = IMAGE_ERROR_NONE;

	from_matrix(&mat);

	if(mImage) {
		// check existing image type
		FREE_IMAGE_FORMAT type = FreeImage_GetFileType(filename, 0);
		if(type == FIF_UNKNOWN) {
			// if the image doesn't exist, get it from the name
			type = FreeImage_GetFIFFromFilename(filename);
		}

		if((type != FIF_UNKNOWN) && FreeImage_FIFSupportsWriting(type)) {
			FreeImage_Save(type, mImage, filename, 0);
		}
	}

	/*
	Possible Save Options:

	BMP		BMP_DEFAULT			Save without any compression
			BMP_SAVE_RLE		Compress the bitmap using RLE when saving

	J2K		J2K_DEFAULT			Save with a 16:1 rate
			Integer X in [1..512] Save with a X:1 rate

	JP2		JP2_DEFAULT			Save with a 16:1 rate
			Integer X in [1..512] Save with a X:1 rate

	JPEG	JPEG_DEFAULT		Saves with good quality (75:1)
			JPEG_QUALITYSUPERB	Saves with superb quality (100:1)
			JPEG_QUALITYGOOD	Saves with good quality (75:1)
			JPEG_QUALITYNORMAL	Saves with normal quality (50:1)
			JPEG_QUALITYAVERAGE	Saves with average quality (25:1)
			JPEG_QUALITYBAD		Saves with bad quality (10:1)
			Integer X in [0..100] Save with quality X:1
			JPEG_PROGRESSIVE	Saves as a progressive JPEG file (use | to combine with JPEG quality flags)
			JPEG_SUBSAMPLING_411 Save with high 4x1 chroma subsampling (4:1:1)
			JPEG_SUBSAMPLING_420 Save with medium 2x2 chroma subsampling (4:2:0) - default value
			JPEG_SUBSAMPLING_422 Save with low 2x1 chroma subsampling (4:2:2)
			JPEG_SUBSAMPLING_444 Save with no chroma subsampling (4:4:4)

	PNG		PNG_DEFAULT			Save with ZLib level 6 compression and no interlacing
			PNG_Z_BEST_SPEED	Save using ZLib level 1 compression (default value is 6)
			PNG_Z_DEFAULT_COMPRESSION Save using ZLib level 6 compression (default recommended value)
			PNG_Z_BEST_COMPRESSION Save using ZLib level 9 compression (default value is 6)
			PNG_Z_NO_COMPRESSION Save without ZLib compression
			PNG_INTERLACED		Save using Adam7 interlacing (use | to combine with other save flags)

	PBM/PGM/PPM PNM_DEFAULT		Saves the bitmap as a binary file
			PNM_SAVE_RAW		Saves the bitmap as a binary file
			PNM_SAVE_ASCII		Saves the bitmap as an ASCII file

	TIFF	TIFF_DEFAULT		Save using CCITTFAX4 compression for 1-bit bitmaps and LZW compression for any other bitmaps
			TIFF_CMYK			Stores tags for separated CMYK (use | to combine with TIFF compression flags)
			TIFF_PACKBITS		Save using PACKBITS compression.
			TIFF_DEFLATE		Save using DEFLATE compression (also known as ZLIB compression)
			TIFF_ADOBE_DEFLATE	Save using ADOBE DEFLATE compression
			TIFF_NONE			Save without any compression
			TIFF_CCITTFAX3		Save using CCITT Group 3 fax encoding
			TIFF_CCITTFAX4		Save using CCITT Group 4 fax encoding
			TIFF_LZW			Save using LZW compression
			TIFF_JPEG			Save using JPEG compression (8-bit greyscale and 24-bit  only.  Default  to LZW for other bitdepths).

	*/

	return err;
}

ImageError FreeImageImpl :: load(const char *filename, AlloArray &mat) {
	ImageError err = IMAGE_ERROR_NONE;
	err = load(filename, FIF_UNKNOWN);
	if(err) return err;

	to_matrix(&mat);

	return err;
}

ImageError FreeImageImpl :: load(const char *filename, FREE_IMAGE_FORMAT type) {
	ImageError err = IMAGE_ERROR_NONE;

	if(type == FIF_UNKNOWN) {
		type = FreeImage_GetFIFFromFilename(filename);
	}
	if((type != FIF_UNKNOWN) && FreeImage_FIFSupportsReading(type)) {
		destroy();
		mImage = FreeImage_Load(type, filename, 0);
	}

	if(mImage) {
		FREE_IMAGE_COLOR_TYPE colorType = FreeImage_GetColorType(mImage);

		switch(colorType)
		{
		case FIC_MINISBLACK:
		case FIC_MINISWHITE: {
			FIBITMAP *res = FreeImage_ConvertToGreyscale(mImage);
			FreeImage_Unload(mImage);
			mImage = res;
		}
		break;

		case FIC_PALETTE: {
			if(FreeImage_IsTransparent(mImage)) {
				FIBITMAP *res = FreeImage_ConvertTo32Bits(mImage);
				FreeImage_Unload(mImage);
				mImage = res;
			}
			else {
				FIBITMAP *res = FreeImage_ConvertTo24Bits(mImage);
				FreeImage_Unload(mImage);
				mImage = res;
			}
		}
		break;

		case FIC_CMYK: {
			printf("CMYK images currently not supported\n");
		}
		break;

		default:
		break;
		}

		FreeImage_FlipVertical(mImage);
	}

	return err;
}

void FreeImageImpl :: to_matrix(AlloArray *mat) {
	//Freeimage is not tightly packed, so we use
	//a custom method instead of one of the Matrix
	//utility methods
	if(mImage) {
		AlloArrayHeader h;
		h.components = get_planes();
		h.type = matrix_type();
		h.dimcount = 2;
		get_dim(h.dim[0], h.dim[1]);
		allo_array_setstride(&h, 1);
		
		allo_array_create(mat, &h);

		ArrayFormat format = matrix_format(get_planes());

		switch(format)
		{
		case FORMAT_ALPHA: {
			char *o_pix = (char *)(mat->data.ptr);
			int rowstride = mat->header.stride[1];

			for(int j = 0; j < h.dim[1]; j++) {
				char *pix = (char *)FreeImage_GetScanLine(mImage, j);
				memcpy(o_pix, pix, rowstride);
				o_pix += rowstride;
			}
		}
		break;
		
		case FORMAT_RGB: {
			switch(h.type) {
			case AlloUInt8Ty: {
				char *bp = (char *)(mat->data.ptr);
				int rowstride = mat->header.stride[1];

				for(int j = 0; j < h.dim[1]; j++) {
					RGBTRIPLE *pix = (RGBTRIPLE *)FreeImage_GetScanLine(mImage, j);
					RGBpix<uint8_t> *o_pix = (RGBpix<uint8_t> *)(bp + j*rowstride);
					for(int i=0; i < h.dim[0]; i++) {
						#ifdef IMAGE_LINUX_VERSION
						o_pix->r = pix->rgbtRed;
						o_pix->g = pix->rgbtGreen;
						o_pix->b = pix->rgbtBlue;
						#else
						o_pix->r = pix->rgbtBlue;
						o_pix->g = pix->rgbtGreen;
						o_pix->b = pix->rgbtRed;
						#endif

						++pix;
						++o_pix;
					}
				}
			}
			break;

			case AlloUInt32Ty: {
				char *o_pix = (char *)(mat->data.ptr);
				int rowstride = mat->header.stride[1];

				for(int j = 0; j < h.dim[1]; j++) {
					char *pix = (char *)FreeImage_GetScanLine(mImage, j);
					memcpy(o_pix, pix, rowstride);
					o_pix += rowstride;
				}
			}
			break;

			default: break;

			}
		}
		break;

		case FORMAT_RGBA: {
			switch(h.type) {
			case AlloUInt8Ty: {
				char *bp = (char *)(mat->data.ptr);
				int rowstride = mat->header.stride[1];

				for(int j = 0; j < h.dim[1]; j++) {
					RGBQUAD *pix = (RGBQUAD *)FreeImage_GetScanLine(mImage, j);
					RGBApix<uint8_t> *o_pix = (RGBApix<uint8_t> *)(bp + j*rowstride);
					for(int i=0; i < h.dim[0]; i++) {
						#ifdef IMAGE_LINUX_VERSION
						o_pix->r = pix->rgbRed;
						o_pix->g = pix->rgbGreen;
						o_pix->b = pix->rgbBlue;
						o_pix->a = pix->rgbReserved;
						#else
						o_pix->r = pix->rgbBlue;
						o_pix->g = pix->rgbGreen;
						o_pix->b = pix->rgbRed;
						o_pix->a = pix->rgbReserved;
						#endif

						++pix;
						++o_pix;
					}
				}
			}
			break;

			case AlloUInt32Ty: {
				char *o_pix = (char *)(mat->data.ptr);
				int rowstride = mat->header.stride[1];

				for(int j = 0; j < h.dim[1]; j++) {
					char *pix = (char *)FreeImage_GetScanLine(mImage, j);
					memcpy(o_pix, pix, rowstride);
					o_pix += rowstride;
				}
			}
			break;

            default: break;
			}
		}
		break;

        default: break;
		}
	}
}

void FreeImageImpl :: from_matrix(AlloArray *mat) {
	destroy();

	AlloArrayHeader &header = mat->header;
	int w = header.dim[0];
	int h = (header.dimcount > 1) ? header.dim[1] : 1;

	ArrayFormat format = matrix_format(header.components);

	switch(format) {

	case FORMAT_RGB: {
		switch(header.type) {

			case AlloUInt8Ty: {
				int bpp = mat->header.stride[0];
				mImage = FreeImage_AllocateT(FIT_BITMAP, w, h, bpp*8);

				char *bp = (char *)(mat->data.ptr);
				int rowstride = mat->header.stride[1];

				for(int j = 0; j < header.dim[1]; j++) {
					RGBTRIPLE *pix = (RGBTRIPLE *)FreeImage_GetScanLine(mImage, j);
					RGBpix<uint8_t> *o_pix = (RGBpix<uint8_t> *)(bp + j*rowstride);
					for(int i=0; i < header.dim[0]; i++) {
						pix->rgbtRed = o_pix->r;
						pix->rgbtGreen = o_pix->g;
						pix->rgbtBlue = o_pix->b;

						++pix;
						++o_pix;
					}
				}
			}
			break;

			default:
				set_error(IMAGE_ERROR_GENERIC, "FreeImageImpl::from_matrix(): Input Matrix type not supported");
				break;
		}
	}
	break;

	case FORMAT_RGBA: {
		switch(header.type) {

			case AlloUInt8Ty: {
				int bpp = mat->header.stride[0];
				mImage = FreeImage_AllocateT(FIT_BITMAP, w, h, bpp*8);

				char *bp = (char *)(mat->data.ptr);
				int rowstride = mat->header.stride[1];

				for(int j = 0; j < header.dim[1]; j++) {
					RGBQUAD *pix = (RGBQUAD *)FreeImage_GetScanLine(mImage, j);
					RGBApix<uint8_t> *o_pix = (RGBApix<uint8_t> *)(bp + j*rowstride);
					for(int i=0; i < header.dim[0]; i++) {
						pix->rgbRed = o_pix->b;
						pix->rgbGreen = o_pix->g;
						pix->rgbBlue = o_pix->r;
						pix->rgbReserved = o_pix->a;

						++pix;
						++o_pix;
					}
				}
			}
			break;

			default:
				set_error(IMAGE_ERROR_NONE, "FreeImageImpl::from_matrix(): Input Matrix type not supported");
				break;
		}
	}
	break;

	default:
		set_error(IMAGE_ERROR_NONE, "FreeImageImpl::from_matrix(): Input Matrix format not supported");
		break;
	}
}

void FreeImageImpl :: get_dim(uint32_t &w, uint32_t &h) {
	if(mImage) {
		w = (uint32_t)FreeImage_GetWidth(mImage);
		h = (uint32_t)FreeImage_GetHeight(mImage);
	}
	else {
		w = h = 0;
	}
}

int FreeImageImpl :: get_planes() {
	AlloTy type = matrix_type();
	BITMAPINFOHEADER * hdr = FreeImage_GetInfoHeader(mImage);
	return (hdr->biBitCount)/(8 * allo_type_size(type));
}

char * FreeImageImpl :: get_data() {
	return NULL;
}

AlloTy FreeImageImpl :: matrix_type() {
	FREE_IMAGE_TYPE type = FreeImage_GetImageType(mImage);

	switch(type) {
		case FIT_UINT32: return AlloUInt32Ty;
		case FIT_INT32: return AlloSInt32Ty;

		case FIT_RGBF:
		case FIT_RGBAF:
		case FIT_FLOAT: return AlloFloat32Ty;
		case FIT_DOUBLE: return AlloFloat64Ty;

		case FIT_BITMAP:
		default: return AlloUInt8Ty;
	}
}

ArrayFormat FreeImageImpl :: matrix_format(int planes) {
	switch(planes) {
		case 1:		return FORMAT_ALPHA;
		case 2:		return FORMAT_LUMALPHA;
		case 3:		return FORMAT_RGB;
		case 4:
		default:	return FORMAT_RGBA;
	}
}

void FreeImageImpl :: initialize() {
	FreeImage_Initialise();
}

}	// image::
