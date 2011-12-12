#include <stdlib.h>
#include "protocol/al_GraphicsBackend.hpp"
#include "graphics/al_Config.h"
#include "graphics/al_Texture.hpp"
#include "graphics/al_Surface.hpp"

namespace al {
namespace gfx{


Texture::Texture(GraphicsBackend *backend)
:	GPUObject(),
	mBackend(backend),
	mSurface(0),
	mData(0),
	mSubData(0),
	mMode(DATA),
	mRebuild(true),
	mUpdate(false),
	mSubUpdate(false),
	mRect(false),
	mUnit(0),
	mWidth(512),
	mHeight(512),
	mDepth(0),
	mTarget(TEXTURE_2D),
	mFormat(RGBA),
	mSingleChannel(ALPHA),
	mType(UCHAR),
	mWrap(CLAMP_TO_EDGE),
	mMinFilter(LINEAR),
	mMagFilter(LINEAR),
	mBorderColor(0, 0, 0, 1)
{
	//mData = allo_array_wrapper_new();
	//allo_array_wrapper_setup(mData);
	//allo_array_wrapper_retain(mData);
	mData = new al::ArrayWrapper;
	mData->retain();
	
	//mSubData = allo_array_wrapper_new();
	//allo_array_wrapper_setup(mSubData);
	//allo_array_wrapper_retain(mSubData);
	mSubData = new al::ArrayWrapper;
	mSubData->retain();
}

Texture::~Texture() {
	destroy();

	//allo_array_wrapper_release(mData);
	//allo_array_wrapper_release(mSubData);
	mData->release();
	mSubData->release();
}

void Texture::attach(Surface *s) {
	mode(Texture::SURFACE);
	mSurface = s;
}

void Texture::clear(int unit, bool do_bind, bool clear_data) {
	if(do_bind) {
		bind(unit);
	}
	
	if(mMode == Texture::SURFACE) {
		if(mSurface) {
			mSurface->enter();
			mSurface->leave();
		}
	}
	else {
		// clear lattice, upload data
		AlloArrayHeader header;
		header.components = components_for_format(mFormat);
		header.type = array_type_for_type(mType);
		header.dimcount = dimcount_for_target(mTarget);
		header.dim[0] = mWidth;
		header.dim[1] = mHeight;
		header.dim[2] = mDepth;
		
		// alignment
		allo_array_setstride(&header, 1);
		if(! allo_array_equal_headers(
				&(mData->header),
				&(header)
			)
		) {
			allo_array_adapt(mData, &header);
			memset(mData->data.ptr, '\0', allo_array_size(mData));
		}
		else if(clear_data) {
			memset(mData->data.ptr, '\0', allo_array_size(mData));
		}
		
		if(do_bind) {
			mBackend->textureSubmit(this);
		}
	}
	
	if(do_bind) {
		unbind(unit);
	}
}
	
void Texture::bind(int unit) {
	mUnit = unit;

	if(mRebuild) {
		destroy();
	}

	bool did_create = false;
	if(! created()) {
		create();
		mRebuild = false;
		
		if(mMode == SURFACE && mSurface && !mSurface->creating()) {
			mSurface->destroy();
			mSurface->create();
		}
		
		if(!mSurface->creating()) {
			clear(unit);
		}
		
		did_create = true;
		mUpdate = true;
	}

	mBackend->textureBind(this, unit);
	
	if(mSubUpdate) {
		mBackend->textureSubSubmit(mSubOffset, this);
		mSubUpdate = false;
	}

	if(mUpdate) {
		mBackend->textureSubmit(this);

		mUpdate = false;
	}
	
	mBackend->textureEnter(this, unit);
}

void Texture::unbind(int unit) {
	mBackend->textureLeave(this, unit);
	mBackend->textureUnbind(this, unit);
	mUnit = 0;
}

void Texture::setArrayFormat(const AlloArrayHeader &header) {
	mFormat = format_for_array_components(header.components);
	mType = type_for_array_type(header.type);

	mWidth = header.dim[0];
	mHeight = header.dim[1];
	mDepth = header.dim[2];
	
	mTarget = target_for_array_dimcount(header.dimcount);
	
	//allo_array_adapt(&(mData->array), &header);
	allo_array_adapt((mData), &header);
}

void Texture::fromArray(const AlloArray *array) {
	int sz = allo_array_size(array);
	if(sz <= 0) {
		return;
	}

	if(! 
		allo_array_equal_headers(
			&(mData->header), //&(mData->array.header), 
			&(array->header)
		) || 
		mMode != DATA)
	{
		mMode = DATA;
		setArrayFormat(array->header);
		mRebuild = true;
	}
	
	memcpy(mData->data.ptr, array->data.ptr, sz);
	//memcpy(mData->array.data.ptr, array->data.ptr, allo_array_size(array));
	mUpdate = true;
}

void Texture::fromArray(const Vec3i &offset, const AlloArray *array) {
	//AlloArrayHeader &h_internal = mData->array.header;
	AlloArrayHeader &h_internal = mData->header;
	const AlloArrayHeader &h_input = array->header;

	if(h_internal.components != h_input.components) {
		// ERROR
		return;
	}
	
	//if(mData->array.header.type != h_input.type) {
	if(mData->header.type != h_input.type) {
		// ERROR
		return;
	}
	
	if(h_internal.dimcount < h_input.dimcount) {
		//int i = mData->array.header.dimcount;
		int i = mData->header.dimcount;
		for(; i < h_input.dimcount; i++) {
			if(h_input.dim[i] > 1) {
				break;
			}
		}
		
		if(i < h_input.dimcount) {
			// ERROR
			return;
		}
	}
	
	int min_dim = (h_internal.dimcount < h_input.dimcount) ? h_internal.dimcount : h_input.dimcount;
	for(int i=0; i < min_dim; i++) {
		if(h_input.dim[i] > (h_internal.dim[i] - offset[i])) {
			// error, dimension are too big
			return;
		}
	}
	
	mSubOffset[0] = offset[0];
	mSubOffset[1] = offset[1];
	mSubOffset[2] = offset[2];
	
	//allo_array_adapt(&(mSubData->array), &h_input);
	allo_array_adapt(mSubData, &h_input);
	//memcpy(mSubData->array.data.ptr, array->data.ptr, allo_array_size(array));
	memcpy(mSubData->data.ptr, array->data.ptr, allo_array_size(array));
	mSubUpdate = true;
}

void Texture::toArray() {
	mBackend->textureToArray(this);
}

void Texture::allocate(AlloArrayHeader &header) {
	if(! 
		allo_array_equal_headers(
			//&(mData->array.header), 
			&(mData->header), 
			&(header)
		) || 
		mMode != DATA)
	{
		mMode = DATA;
		setArrayFormat(header);
		//memset(mData->array.data.ptr, '\0', allo_array_size(&(mData->array)));
		memset(mData->data.ptr, '\0', allo_array_size(mData));
		mRebuild = true;
	}
	
	mUpdate = true;
}

bool Texture::rect() {
	return mRect;
}

void Texture::rect(bool v) {
	mRect = v;
}
	
char * Texture::getData() {
	//return mData->array.data.ptr;
	return mData->data.ptr;
}

char * Texture::getSubData() {
	//return mSubData->array.data.ptr;
	return mSubData->data.ptr;
}

int Texture::getRowStride() {
	return mData->header.stride[1];
	//return mData->array.header.stride[1];
}

int Texture::width() {
	return mWidth;
}

void Texture::width(int w) {
	if(mWidth != w) {
		mWidth = w;
		mRebuild = true;
	}
}

int Texture::height() {
	return mHeight;
}

void Texture::height(int h) {
	if(mHeight != h) {
		mHeight = h;
		mRebuild = true;
	}
}

int Texture::depth() {
	return mDepth;
}

void Texture::depth(int d) {
	if(mDepth != d) {
		mDepth = d;
		mRebuild = true;
	}
}

void Texture::getDimensions(int &w, int &h) {
	w = mWidth; h = mHeight;;
}

void Texture::getDimensions(int &w, int &h, int &d) {
	w = mWidth; h = mHeight; d = mDepth;
}

void Texture::dimensions(int w, int h) {
	if(mWidth != w || mHeight != h) {
		mWidth = w;
		mHeight = h;
		mRebuild = true;
	}
}

void Texture::dimensions(int w, int h, int d) {
	if(mWidth != w || mHeight != h || mDepth != d) {
		mWidth = w;
		mHeight = h;
		mDepth = d;
		mRebuild = true;
	}
}

Texture::Mode Texture::mode() {
	return mMode;
}

void Texture::mode(Mode v) {
	if(mMode != v) {
		mMode = v;
		mRebuild = true;
		
		if(mMode == SURFACE) {
			mUpdate = false;
		}
	}
}

Texture::Target Texture::target() {
	return mTarget;
}

void Texture::target(Target v) {
	if(mTarget != v) {
		mTarget = v;
		mRebuild = true;
	}
}

Texture::Format Texture::format() {
	return mFormat;
}

void Texture::format(Format v) {
	if(mFormat != v) {
		mFormat = v;
		mRebuild = true;
	}
}

Texture::Type Texture::type() {
	return mType;
}

Texture::Format Texture::singleChannel() {
	return mSingleChannel;
}

void Texture::singleChannel(Format v) {
	if(v == ALPHA || v == LUMINANCE) {
		mSingleChannel = v;
	}
	else {
		mSingleChannel = ALPHA;
	}
}

void Texture::type(Type v) {
	if(mType != v) {
		mType = v;
		mRebuild = true;
	}
}

Texture::Wrap Texture::wrap() {
	return mWrap;
}

void Texture::wrap(Wrap v) {
	if(mWrap != v) {
		mWrap = v;
		mRebuild = true;
	}
}

Texture::Filter Texture::minFilter() {
	return mMinFilter;
}

void Texture::minFilter(Filter v) {
	if(mMinFilter != v) {
		mMinFilter = v;
		mRebuild = true;
	}
}

Texture::Filter Texture::magFilter() {
	return mMagFilter;
}

void Texture::magFilter(Filter v) {
	if(mMagFilter != v) {
		mMagFilter = v;
		mRebuild = true;
	}
}

void Texture::borderColor(const Color& c) {
	mBorderColor = c;
}

void Texture::onCreate() {
	mBackend->textureCreate(this);
}

void Texture::onDestroy() {
	mBackend->textureDestroy(this);
}

Texture::Format Texture::format_for_array_components(int components) {
	switch(components) {
		case 1:	return mSingleChannel;
		case 2:	return LUMALPHA;
		case 3:	return RGB;
		case 4:	
		default:
			return RGBA;
	}
}

int Texture::components_for_format(Format format) {
	switch(format) {
		case ALPHA: return 1;
		case LUMINANCE: return 1;
		case LUMALPHA: return 2;
		case RGB: return 3;
		case RGBA:
		default:
			return 4;
	}
}

Texture::Type Texture::type_for_array_type(AlloTy type) {
	switch(type) {
		case AlloUInt8Ty:		return UCHAR;
		case AlloSInt32Ty:		return INT;
		case AlloUInt32Ty:		return UINT;
		case AlloFloat32Ty:		return FLOAT32;
		default:
			return UCHAR;
	}
}

AlloTy Texture::array_type_for_type(Type type) {
	switch(type) {
		case UCHAR:		return AlloUInt8Ty;
		case INT:		return AlloSInt32Ty;
		case UINT:		return AlloUInt32Ty;
		case FLOAT32:	return AlloFloat32Ty;
		default:
			return AlloUInt8Ty;
	}
}

Texture::Target Texture::target_for_array_dimcount(int dimcount) {
	switch(dimcount) {
		case 1:	return TEXTURE_1D;
		case 3:	return TEXTURE_3D;
		
		case 2:
		default:
			return mRect ? TEXTURE_RECT : TEXTURE_2D;
	}
}

int Texture::dimcount_for_target(Target target) {
	switch(target) {
		case TEXTURE_1D:	return 1;
		case TEXTURE_3D:	return 3;
		case TEXTURE_2D:
		case TEXTURE_RECT:
		default:
			return 2;
	}
}

} // ::al::gfx
} // ::al

