#include "graphics/al_ShaderAttribute.hpp"
#include "graphics/al_ShaderBackend.hpp"

namespace al {
namespace gfx {

ShaderAttribute :: ShaderAttribute(ShaderBackend *shader, const char *name, Type type)
:	mShader(shader),
	mName(name),
	mUpdate(false),
	mLocation(INVALID_LOCATION),
	mType(type)
{
	mBaseType = ShaderParam::data_type_for_type(type);
	mDataSize = ShaderParam::size_for_type(type);
}

ShaderAttribute :: ShaderAttribute(ShaderBackend *shader, const char *name, int location, Type type)
:	mShader(shader),
	mName(name),
	mUpdate(false),
	mLocation(location),
	mType(type)
{
	mBaseType = ShaderParam::data_type_for_type(type);
	mDataSize = ShaderParam::size_for_type(type);
}

ShaderAttribute :: ~ShaderAttribute() {
	// nothing for now
}

void ShaderAttribute :: set_location(int location) {
	if(mShader->get_id() <= INVALID_SHADER) {
		mUpdate = true;
		mLocation = location;
	}
}

void ShaderAttribute :: realize_location(int location) {
	mLocation = location;
}

void ShaderAttribute :: set_type(Type type) {
	mType = type;
	mBaseType = ShaderParam::data_type_for_type(type);
	mDataSize = ShaderParam::size_for_type(type);
}

} // gfx::
} // al::
