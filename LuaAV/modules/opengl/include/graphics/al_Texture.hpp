#ifndef INCLUDE_AL_GRAPHICS_TEXTURE_HPP
#define INCLUDE_AL_GRAPHICS_TEXTURE_HPP

#include "graphics/al_Common.hpp"
#include "graphics/al_GPUObject.hpp"
#include "allocore/types/al_Array.hpp"
#include "types/al_Color.hpp"

namespace al {
namespace gfx{

class Surface;
class GraphicsBackend;

class Texture : public GPUObject {
public:

	enum Mode {
		DATA = 0,
		SURFACE
	};

	enum Target {
		TEXTURE_1D = 0,
		TEXTURE_2D,
		TEXTURE_RECT,
		TEXTURE_3D
	};

	enum Format {
		ALPHA = 0,
		LUMINANCE,
		LUMALPHA,
		RGB,
		BGR,
		RGBA,
		BGRA
	};

	enum Type {
		UCHAR = 0,
		INT,
		UINT,
		FLOAT32
	};

	enum Wrap {
		CLAMP = 0,
		CLAMP_TO_BORDER,
		CLAMP_TO_EDGE,
		MIRRORED_REPEAT,
		REPEAT,
	};

	enum Filter {
		NEAREST = 0,
		LINEAR,
		NEAREST_MIPMAP_NEAREST,
		LINEAR_MIPMAP_NEAREST,
		NEAREST_MIPMAP_LINEAR,
		LINEAR_MIPMAP_LINEAR
	};

	Texture(GraphicsBackend *backend);
	virtual ~Texture();
	
	void attach(Surface *s);
	
	void clear(int unit=0, bool do_bind=true, bool clear_data=false);

	void bind(int unit = 0);
	void unbind(int unit = 0);
	void setArrayFormat(const AlloArrayHeader &header);
	void fromArray(const AlloArray *array);
	void fromArray(const Vec3i &offset, const AlloArray *array);
	void toArray();

	void allocate(AlloArrayHeader &header);

	// trigger textureSubmit:
	void update() { mUpdate = true; }

	// retrieve internal array:
	AlloArray * array() {return mData;} //&(mData->array);}
	al::ArrayWrapper * wrapper() {return mData;}

	bool rect();
	void rect(bool v);

	char * getData();
	char * getSubData();
	int getRowStride();

	int width();
	void width(int w);
	int height();
	void height(int h);
	int depth();
	void depth(int d);

	void getDimensions(int &w, int &h);
	void getDimensions(int &w, int &h, int &d);

	void dimensions(int w, int h);
	void dimensions(int w, int h, int d);

	Mode mode();
	void mode(Mode v);

	Target target();
	void target(Target v);

	Format format();
	void format(Format v);
	
	Format singleChannel();
	void singleChannel(Format v);

	Type type();
	void type(Type v);

	Wrap wrap();
	void wrap(Wrap v);

	Filter minFilter();
	void minFilter(Filter v);

	Filter magFilter();
	void magFilter(Filter v);


	void borderColor(const Color& c);
	Color& borderColor() {return mBorderColor;}
	
	GraphicsBackend * backend() {return mBackend;}
	Surface * surface() {return mSurface;}
	
	int unit() {return mUnit;}
	
	bool needsRebuild() {return mRebuild || !created();}


	virtual void onCreate();
	virtual void onDestroy();
protected:

	Format format_for_array_components(int components);
	int components_for_format(Format format);
	Type type_for_array_type(AlloTy type);
	AlloTy array_type_for_type(Type type);
	Target target_for_array_dimcount(int dimcount);
	int dimcount_for_target(Target target);


	GraphicsBackend	* mBackend;			///< Library backend
	Surface * mSurface;					///< Surface object
	al::ArrayWrapper * mData;			///< Pixel data
	al::ArrayWrapper * mSubData;		///< Sub-array Pixel data
	Mode			mMode;				///< Texture mode
	bool			mRebuild;			///< Rebuild flag
	bool			mUpdate;			///< Update flag
	Vec3i			mSubOffset;			///< Offset for sub-update
	bool			mSubUpdate;			///< Update subarray flag
	bool			mRect;				///< Use rectangular textures flags
	int				mUnit;				///< Texture unit bound to
	int				mWidth;				///< Width of texture in pixels
	int				mHeight;			///< Height of texture in pixels
	int				mDepth;				///< Depth of texture in pixels
	Target			mTarget;			///< Texture target (2D, RECT, 3D, etc.)
	Format			mFormat;			///< Texture format (RGB, RGBA, etc.)
	Format			mSingleChannel;		///< Preferred single channel texture format (ALPHA or LUMINANCE)
	Type			mType;				///< Texture type (UCHAR, FLOAT, etc.)
	Wrap			mWrap;				///< Wrap mode
	Filter			mMinFilter;			///< Minification filter
	Filter			mMagFilter;			///< Magnification filter (NEAREST or LINEAR)
	Color			mBorderColor;		///< Border color

}; // Texture


} // ::al::gfx
} // ::al

#endif
