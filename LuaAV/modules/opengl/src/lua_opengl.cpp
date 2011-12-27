#include "lua_opengl.h"
#include "lua_opengl_enums.h"
#include "lua_utility_opengl.h"
#include "lua_opengl_extensions.h"
#include <string>
#include <map>
#include <set>

// glue userdata
#include "lua_glue.h"

using std::string;
using std::map;
using std::set;

/*! OpenGL bindings
	OpenGL bindings are low-level functions mapping directly to the OpenGL API.  Nearly all functions 
	have a binding with the exception of functions that deal with large buffers of data.  These functions 
	(e.g. glTexImage2D, glReadPixels, ...) are more approriately handled with a higher-level interface, 
	and so are left out.
	
	In addition to the basic OpenGL functions, the opengl module has the <b>initctx</b> and <b>closectx</b> 
	functions.  These functions are for performing resource initialization and teardown on context creation and 
	destruction.  In particular, the OpenGL module uses GLEW for managing OpenGL extensions.  initctx is used to
	signal GLEW to load the extensions when the context is ready.  initctx and closectx are also used to 
	signal to Texture, Mesh, and other high-level objects that the context their resources are attached to 
	are being created and destroyed.
	
	initctx and closectx are automatically called when using a app.Window.  When app.Window creates a context, 
	it will look for an opengl module and call opengl.initctx.  Similarly, when the window gets destroyed, it will 
	look for opengl.closectx.
	
	@see app.Window
	@module opengl
*/

typedef set<al::gfx::GPUObject *> ResourceSet;
typedef map<string, ResourceSet> ContextMap;
typedef map<al::gfx::GPUObject *, string> ResourceMap;

// Global data structures for managing context resources
Extensions g_extensions;
ContextMap g_context_resources;
ResourceMap g_resources;
al::gfx::GraphicsBackendOpenGL g_opengl_backend;


#ifdef __cplusplus
extern "C" {
#endif

void lua_opengl_error(const char *msg) {
	GLenum err = glGetError();

	switch(err) {
		case GL_INVALID_ENUM:
			printf("%s:\n %s\n", msg, "An unacceptable value is specified for an enumerated argument. The offending command is ignored and has no other side effect than to set the error flag.");
			break;

		case GL_INVALID_VALUE:
			printf("%s:\n %s\n", msg, "A numeric argument is out of range. The offending command is ignored and has no other side effect than to set the error flag.");
			break;

		case GL_INVALID_OPERATION:
			printf("%s:\n %s\n", msg, "The specified operation is not allowed in the current state. The offending command is ignored and has no other side effect than to set the error flag.");
			break;

		case GL_STACK_OVERFLOW:
			printf("%s:\n %s\n", msg, "This command would cause a stack overflow. The offending command is ignored and has no other side effect than to set the error flag.");
			break;

		case GL_STACK_UNDERFLOW:
			printf("%s:\n %s\n", msg, "This command would cause a stack underflow. The offending command is ignored and has no other side effect than to set the error flag.");
			break;

		case GL_OUT_OF_MEMORY:
			printf("%s:\n %s\n", msg, "There is not enough memory left to execute the command.  The state of the GL is undefined, except for the state of the error flags, after this error is recorded.");
			break;

		case GL_TABLE_TOO_LARGE:
			printf("%s:\n %s\n", msg, "The specified table exceeds the implementation's maximum supported table size.  The offending command is ignored and has no other side effect than to set the error flag.");
			break;

		case GL_NO_ERROR:
			break;

		default:
			break;
	}
}

void lua_opengl_register(const char *ctx, al::gfx::GPUObject *o) {
	ContextMap::iterator it = g_context_resources.find(string(ctx));
	if(it == g_context_resources.end()) {
		g_context_resources.insert(std::pair<string, ResourceSet>(string(ctx), ResourceSet()));
		it = g_context_resources.find(string(ctx));
	}
	
	it->second.insert(o);
	g_resources.insert(std::pair<al::gfx::GPUObject *, string>(o, string(ctx)));
}

void lua_opengl_unregister(al::gfx::GPUObject *o) {
	ResourceMap::iterator rit = g_resources.find(o);
	if(rit != g_resources.end()) {
		
		ContextMap::iterator it = g_context_resources.find( rit->second );
		if(it != g_context_resources.end()) {
			ResourceSet &ctx_set = it->second;
			ResourceSet::iterator sit = ctx_set.find(o);
			if(sit != ctx_set.end()) {
				ctx_set.erase(sit);
			}
		}
		
		g_resources.erase(rit);
	}
}

al::gfx::GraphicsBackendOpenGL * lua_opengl_get_backend(const char *ctx) {
	return &g_opengl_backend;
}

#ifdef __cplusplus
}
#endif

/*! Signal the module to initialize OpenGL resources
	initctx should only be called when a valid OpenGL context is present.  Otherwise, it will 
	likely cause an invalid access error and crash.  Typically, it is called before any frames 
	are drawn and the context has just been initialized.  It should only be called once on 
	context creation.
	
	@param ctx The name of the context.
		
	@see app.Window
	@name initctx
*/
int gl_initctx(lua_State *L) {
	if(lua_type(L, 1) == LUA_TSTRING) {
//		const char *ctx = lua_tostring(L, 1);
		g_extensions.vertify_extensions();
	}
	else {
		luaL_error(L, "opengl.initctx: no context name provided");
	}
	return 0;
}

/*! Signal the module to destroy OpenGL resources
	closectx should only be called when a valid OpenGL context is present.  Typically, it is 
	called right before a context is destroyed. It should only be called once on 
	context destruction.
	
	@param ctx The name of the context.
				
	@see app.Window
	@name closectx
*/
int gl_closectx(lua_State *L) {
	if(lua_type(L, 1) == LUA_TSTRING) {
		const char *ctx = lua_tostring(L, 1);
		ContextMap::iterator it = g_context_resources.find(string(ctx));
		if(it != g_context_resources.end()) {
			ResourceSet &ctx_set = it->second;
			ResourceSet::iterator sit = ctx_set.begin();
			ResourceSet::iterator site = ctx_set.end();
			for(; sit != site; ++sit) {
				(*sit)->destroy();
			}
		}
	}
	else {
		luaL_error(L, "opengl.closectx: no context name provided");
	}
	return 0;
}


/*! Operate on the accumulation buffer
	OpenGL Documentation: <a target="_blank" href="http://www.opengl.org/sdk/docs/man/xhtml/glAccum.xml">glAccum</a>
	
	@param op Specifies the accumulation buffer operation. Symbolic constants GL_ACCUM, GL_LOAD, 
				GL_ADD, GL_MULT, and GL_RETURN are accepted.
	@param value Specifies a floating-point value used in the accumulation buffer operation. 
				op determines how value is used.
				
	@name Accum
*/
int gl_Accum(lua_State *L) {
	if(lua::is<GLenum>(L, 1) && lua::is<GLfloat>(L, 2)) {
		GLenum op = lua::to<GLenum>(L, 1);
		GLfloat value = lua::to<GLfloat>(L, 2);
		glAccum(op, value);
	}
	else {
		luaL_error(L, "gl.Accum: invalid arguments");
	}
	return 0;
}

/*! Specify the alpha test function
	OpenGL Documentation: <a target="_blank" href="http://www.opengl.org/sdk/docs/man/xhtml/glAlphaFunc.xml">glAlphaFunc</a>
	
	@param func Specifies the alpha comparison function. 
				Symbolic constants GL_NEVER, GL_LESS, GL_EQUAL, GL_LEQUAL, 
				GL_GREATER, GL_NOTEQUAL, GL_GEQUAL, and GL_ALWAYS are accepted. 
				The initial value is GL_ALWAYS
	@param ref Specifies the reference value that incoming alpha values are compared to. 
				This value is clamped to the range 0 1 , where 0 represents the lowest 
				possible alpha value and 1 the highest possible value. The initial 
				reference value is 0
				
	@name AlphaFunc
*/
int gl_AlphaFunc(lua_State *L) {
	if(lua::is<GLenum>(L, 1) && lua::is<GLclampf>(L, 2)) {
		GLenum func = lua::to<GLenum>(L, 1);
		GLclampf ref = lua::to<GLclampf>(L, 2);
		glAlphaFunc(func, ref);
	}
	else {
		luaL_error(L, "gl.AlphaFunc: invalid arguments");
	}
	return 0;
}

/*! Delimit the vertices of a primitive or a group of like primitives
	OpenGL Documentation: <a target="_blank" href="http://www.opengl.org/sdk/docs/man/xhtml/glBegin.xml">glBegin</a>
	
	@param mode Specifies the primitive or primitives that will be created from vertices 
				presented between glBegin and the subsequent glEnd. Ten symbolic constants 
				are accepted: GL_POINTS, GL_LINES, GL_LINE_STRIP, GL_LINE_LOOP, 
				GL_TRIANGLES, GL_TRIANGLE_STRIP, GL_TRIANGLE_FAN, GL_QUADS, GL_QUAD_STRIP, 
				and GL_POLYGON.
				
	@name Begin
*/
int gl_Begin(lua_State *L) {
	if(lua::is<GLenum>(L, 1)) {
		GLenum mode = lua::to<GLenum>(L, 1);
		glBegin(mode);
	}
	else {
		luaL_error(L, "gl.Begin: invalid arguments");
	}
	return 0;
}


/*! Specify pixel arithmetic
	OpenGL Documentation: <a target="_blank" href="http://www.opengl.org/sdk/docs/man/xhtml/glBlendFunc.xml">glBlendFunc</a>
	
	@param sfactor Specifies how the red, green, blue, and alpha source blending factors 
					are computed. The following symbolic constants are accepted: GL_ZERO, 
					GL_ONE, GL_SRC_COLOR, GL_ONE_MINUS_SRC_COLOR, GL_DST_COLOR, 
					GL_ONE_MINUS_DST_COLOR, GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA, 
					GL_DST_ALPHA, GL_ONE_MINUS_DST_ALPHA, GL_CONSTANT_COLOR, 
					GL_ONE_MINUS_CONSTANT_COLOR, GL_CONSTANT_ALPHA, 
					GL_ONE_MINUS_CONSTANT_ALPHA, and GL_SRC_ALPHA_SATURATE. 
					The initial value is GL_ONE.
					
	@param dfactor Specifies how the red, green, blue, and alpha destination blending 
					factors are computed.  The same constants are accepted as for sfactor.
					
	@name BlendFunc
*/
int gl_BlendFunc(lua_State *L) {
	if(lua::is<GLenum>(L, 1) && lua::is<GLenum>(L, 2)) {
		GLenum sfactor = lua::to<GLenum>(L, 1);
		GLenum dfactor = lua::to<GLenum>(L, 2);
		glBlendFunc(sfactor, dfactor);
	}
	else {
		luaL_error(L, "gl.BlendFunc: invalid arguments");
	}
	return 0;
}


/*! Specify the equation used for both the RGB blend equation and the Alpha blend equation
	OpenGL Documentation: <a target="_blank" href="http://www.opengl.org/sdk/docs/man/xhtml/glBlendEquation.xml">glBlendEquation</a>
	
	@param mode Specifies how source and destination colors are combined. It must be GL_FUNC_ADD, 
				GL_FUNC_SUBTRACT, GL_FUNC_REVERSE_SUBTRACT, GL_MIN, GL_MAX.
					
	@name BlendEquation
*/
int gl_BlendEquation(lua_State *L) {
	if(lua::is<GLenum>(L, 1)) {
		GLenum mode = lua::to<GLenum>(L, 1);
		glBlendEquation(mode);
	}
	else {
		luaL_error(L, "gl.BlendEquation: invalid arguments");
	}
	return 0;
}

/*! Execute a display list
	OpenGL Documentation: <a target="_blank" href="http://www.opengl.org/sdk/docs/man/xhtml/glCallList.xml">glCallList</a>
	
	@param list Specifies the integer name of the display list to be executed.
					
	@name CallList
*/
int gl_CallList(lua_State *L) {
	if(lua::is<GLuint>(L, 1)) {
		GLuint listv = lua::to<GLuint>(L, 1);
		glCallList(listv);
	}
	else {
		luaL_error(L, "gl.CallList: invalid arguments");
	}
	return 0;
}


/*! Clear buffers to preset values
	OpenGL Documentation: <a target="_blank" href="http://www.opengl.org/sdk/docs/man/xhtml/glClear.xml">glClear</a>
	
	@param mask Bitwise OR of masks that indicate the buffers to be cleared. 
				The four masks are GL_COLOR_BUFFER_BIT, GL_DEPTH_BUFFER_BIT, 
				GL_ACCUM_BUFFER_BIT, and GL_STENCIL_BUFFER_BIT.
					
	@name Clear
*/
int gl_Clear(lua_State *L) {
	if(lua::is<GLbitfield>(L, 1)) {
		GLuint mask = lua::to<GLbitfield>(L, 1);
		glClear(mask);
	}
	else {
		luaL_error(L, "gl.Clear: invalid arguments");
	}
	return 0;
}

/*! Specify clear values for the accumulation buffer
	OpenGL Documentation: <a target="_blank" href="http://www.opengl.org/sdk/docs/man/xhtml/glClearAccum.xml">glClearAccum</a>
	
	@param ... Either a 4 numbers or a table specifying the red, green, blue, and 
				alpha values used when the accumulation buffer is cleared. 
				The initial values are all 0.
					
	@name ClearAccum
*/
int gl_ClearAccum(lua_State *L) {
	GLfloat v[] = {0, 0, 0, 1};
	if(lua::to_vec<GLfloat>(L, 1, 4, v)) {
		glClearAccum(v[0], v[1], v[2], v[3]);
	}
	else {
		luaL_error(L, "gl.ClearAccum: invalid arguments");
	}
	return 0;
}

/*! Specify clear values for the color buffers
	OpenGL Documentation: <a target="_blank" href="http://www.opengl.org/sdk/docs/man/xhtml/glClearColor.xml">glClearColor</a>
	
	@param ... Either a 4 numbers or a table specifying the red, green, blue, and 
				alpha values used when the color buffers are cleared. 
				The initial values are all 0.
					
	@name ClearColor
*/
int gl_ClearColor(lua_State *L) {
	GLfloat v[] = {0, 0, 0, 1};
	if(lua::to_vec<GLfloat>(L, 1, 4, v)) {
		glClearColor(v[0], v[1], v[2], v[3]);
	}
	else {
		luaL_error(L, "gl.ClearColor: invalid arguments");
	}
	return 0;
}

/*! Specify the clear value for the depth buffer
	OpenGL Documentation: <a target="_blank" href="http://www.opengl.org/sdk/docs/man/xhtml/glClearDepth.xml">glClearDepth</a>
	
	@param depth Specifies the depth value used when the depth buffer is cleared. 
				The initial value is 1.
					
	@name ClearDepth
*/
int gl_ClearDepth(lua_State *L) {
	if(lua::is<GLclampd>(L, 1)) {
		GLclampd depth = lua::to<GLclampd>(L, 1);
		glClearDepth(depth);
	}
	else {
		luaL_error(L, "gl.ClearDepth: invalid arguments");
	}
	return 0;
}

/*! Specify the clear value for the color index buffers
	OpenGL Documentation: <a target="_blank" href="http://www.opengl.org/sdk/docs/man/xhtml/glClearIndex.xml">glClearIndex</a>
	
	@param c Specifies the index used when the color index buffers are cleared. 
			The initial value is 0.
					
	@name ClearIndex
*/
int gl_ClearIndex(lua_State *L) {
	if(lua::is<GLfloat>(L, 1)) {
		GLfloat c = lua::to<GLfloat>(L, 1);
		glClearIndex(c);
	}
	else {
		luaL_error(L, "gl.ClearIndex: invalid arguments");
	}
	return 0;
}

/*! Specify the clear value for the stencil buffer
	OpenGL Documentation: <a target="_blank" href="http://www.opengl.org/sdk/docs/man/xhtml/glClearStencil.xml">glClearStencil</a>
	
	@param c Specifies the index used when the stencil buffer is cleared. 
			The initial value is 0.
					
	@name ClearStencil
*/
int gl_ClearStencil(lua_State *L) {
	if(lua::is<GLint>(L, 1)) {
		GLint s = lua::to<GLint>(L, 1);
		glClearStencil(s);
	}
	else {
		luaL_error(L, "gl.ClearStencil: invalid arguments");
	}
	return 0;
}

/*! Specify a plane against which all geometry is clipped
	OpenGL Documentation: <a target="_blank" href="http://www.opengl.org/sdk/docs/man/xhtml/glClipPlane.xml">glClipPlane</a>
	
	@param plane Specifies which clipping plane is being positioned. Symbolic 
					names of the form GL_CLIP_PLANEi, where i is an integer 
					between 0 and GL_MAX_CLIP_PLANES -1 , are accepted.
	@param equation Specifies the address of an array of four double-precision 
					floating-point values. These values are interpreted as a 
					plane equation.
					
	@name ClipPlane
*/
int gl_ClipPlane(lua_State *L) {
	GLdouble equation[] = {0, 0, 0, 1};
	if(
		lua::is<GLenum>(L, 1) &&
		lua::to_vec_t<GLdouble>(L, 2, 4, equation)
	) {
		GLenum plane = lua::to<GLenum>(L, 1);
		glClipPlane(plane, equation);
	}
	else {
		luaL_error(L, "gl.ClipPlane: invalid arguments");
	}
	return 0;
}

/*! Set the current color
	OpenGL Documentation: <a target="_blank" href="http://www.opengl.org/sdk/docs/man/xhtml/glColor.xml">glColor</a>
	
	@param ... Specify either 3 or 4 numbers or a table for new red, green, 
				blue and [alpha] values for the current color.  The default 
				value is (0, 0, 0, 1).
					
	@name Color
*/
int gl_Color(lua_State *L) {
	GLdouble v[] = {0, 0, 0, 1};
	int n = 4;
	if(lua::to_vec_n<GLdouble>(L, 1, n, v)) {
		if(n == 3 && lua_istable(L, 1) && lua_isnumber(L, 2)) {
			v[3] = lua_tonumber(L, 2);
		}
		glColor4dv(v);
	}
	else {
		luaL_error(L, "gl.Color: invalid arguments");
	}
	return 0;
}

/*! Enable and disable writing of frame buffer color components
	OpenGL Documentation: <a target="_blank" href="http://www.opengl.org/sdk/docs/man/xhtml/glColorMask.xml">glColorMask</a>
	
	@param ... Either 4 numbers or a table specifying whether red, green, blue, 
				and alpha can or cannot be written into the frame buffer. The 
				initial values are all GL_TRUE, indicating that the color 
				components can be written.
					
	@name ColorMask
*/
int gl_ColorMask(lua_State *L) {
	GLboolean v[] = {GL_TRUE, GL_TRUE, GL_TRUE, GL_TRUE};
	if(lua::to_vec<GLboolean>(L, 1, 4, v)) {
		glColorMask(v[0], v[1], v[2], v[3]);
	}
	else {
		luaL_error(L, "gl.ColorMask: invalid arguments");
	}
	return 0;
}

/*! Cause a material color to track the current color
	OpenGL Documentation: <a target="_blank" href="http://www.opengl.org/sdk/docs/man/xhtml/glColorMaterial.xml">glColorMaterial</a>
	
	@param face Specifies whether front, back, or both front and back material parameters 
				should track the current color. Accepted values are GL_FRONT, GL_BACK, and 
				GL_FRONT_AND_BACK. The initial value is GL_FRONT_AND_BACK.

	@param mode Specifies which of several material parameters track the current color. 
				Accepted values are GL_EMISSION, GL_AMBIENT, GL_DIFFUSE, GL_SPECULAR, and 
				GL_AMBIENT_AND_DIFFUSE. The initial value is GL_AMBIENT_AND_DIFFUSE.
					
	@name ColorMaterial
*/
int gl_ColorMaterial(lua_State *L) {
	if(lua::is<GLenum>(L, 1) && lua::is<GLenum>(L, 2)) {
		GLenum face = lua::to<GLenum>(L, 1);
		GLenum mode = lua::to<GLenum>(L, 2);
		glColorMaterial(face, mode);
	}
	else {
		luaL_error(L, "gl.ColorMaterial: invalid arguments");
	}
	return 0;
}

/*! Copy pixels in the frame buffer
	OpenGL Documentation: <a target="_blank" href="http://www.opengl.org/sdk/docs/man/xhtml/glCopyPixels.xml">glCopyPixels</a>
	
	@param x Specify the x window coordinate of the lower left 
				corner of the rectangular region of pixels to be copied.
	@param y Specify the y window coordinate of the lower left 
				corner of the rectangular region of pixels to be copied.
				
	@param width Specify the horizontal dimensions of the rectangular region of pixels 
					to be copied. Both must be nonnegative.
	@param height Specify the vertical dimensions of the rectangular region of pixels 
					to be copied. Both must be nonnegative.

	@param type Specifies whether color values, depth values, or stencil values 
				are to be copied. Symbolic constants GL_COLOR, GL_DEPTH, and 
				GL_STENCIL are accepted.
					
	@name CopyPixels
*/
int gl_CopyPixels(lua_State *L) {
	if(
		lua::is<GLint>(L, 1) && 
		lua::is<GLint>(L, 2) && 
		lua::is<GLsizei>(L, 3) && 
		lua::is<GLsizei>(L, 4) && 
		lua::is<GLenum>(L, 5)
	) {
		GLint x = lua::to<GLint>(L, 1);
		GLint y = lua::to<GLint>(L, 2);
		GLsizei width = lua::to<GLsizei>(L, 3);
		GLsizei height = lua::to<GLsizei>(L, 4);
		GLenum type = lua::to<GLenum>(L, 5);
		glCopyPixels(x, y, width, height, type);
	}
	else {
		luaL_error(L, "gl.CopyPixels: invalid arguments");
	}
	return 0;
}

/*! Copy pixels into a 1D or 2D texture image
	If no height argument is given, glCopyTexImage1D will be used, otherwise, glCopyTexImage2D 
	be used. Example usage:
	
	<luacode>
	1D case: gl.CopyTexImage(GL.TEXTURE_1D, 0, GL.RGBA, 0, 0, 128, 0)
	2D case: gl.CopyTexImage(GL.TEXTURE_2D, 0, GL.RGBA, 0, 0, 128, 128, 0)
	</luacode>
	
	OpenGL Documentation: <a target="_blank" href="http://www.opengl.org/sdk/docs/man/xhtml/glCopyTexImage1D.xml">glCopyTexImage1D</a>, 
	<a target="_blank" href="http://www.opengl.org/sdk/docs/man/xhtml/glCopyTexImage2D.xml">glCopyTexImage2D</a>
	
	@param target Only relevant in the 2D case.  Specifies the target texture. 
					Must be GL_TEXTURE_2D, GL_TEXTURE_CUBE_MAP_POSITIVE_X, 
					GL_TEXTURE_CUBE_MAP_NEGATIVE_X, GL_TEXTURE_CUBE_MAP_POSITIVE_Y, 
					GL_TEXTURE_CUBE_MAP_NEGATIVE_Y, GL_TEXTURE_CUBE_MAP_POSITIVE_Z, or 
					GL_TEXTURE_CUBE_MAP_NEGATIVE_Z.
	@param level Specifies the level-of-detail number. Level 0 is the base image level. 
				Level n is the nth mipmap reduction image.
	@param internalformat Specifies the internal format of the texture. See OpenGL documentation for 
							accepted values.
	@param x Specify the x window coordinate of the left corner of the row of pixels to be copied.
	@param y Specify the y window coordinate of the left corner of the row of pixels to be copied.
	@param width Specifies the width of the texture image. Must be 0 or 2 n + 2 border for some 
					integer n.
	@param [height] Specifies the height of the texture image. Must be 0 or 2 m + 2 border for some 
					integer m. The height of the texture image in the 1D case is 1.
	@param border Specifies the width of the border. Must be either 0 or 1.
					
	@name CopyTexImage
*/
int gl_CopyTexImage(lua_State *L) {
	if(
		lua::is<GLenum>(L, 1) && 
		lua::is<GLint>(L, 2) && 
		lua::is<GLenum>(L, 3) && 
		lua::is<GLint>(L, 4) && 
		lua::is<GLint>(L, 5) && 
		lua::is<GLsizei>(L, 6)
	) {
		GLenum target = lua::to<GLenum>(L, 1);
		GLint level = lua::to<GLint>(L, 2);
		GLenum internalformat = lua::to<GLenum>(L, 3);
		GLint x = lua::to<GLint>(L, 4);
		GLint y = lua::to<GLint>(L, 5);
		GLsizei width = lua::to<GLsizei>(L, 6);
		
		int top = lua_gettop(L);
		if(
			top == 7 && 
			lua::is<GLint>(L, 7)
		) {
			GLint border = lua::to<GLint>(L, 7);
			glCopyTexImage1D(target, level, internalformat, x, y, width, border);
		}
		else if(top == 8) {
			GLsizei height = lua::to<GLsizei>(L, 7);
			GLint border = lua::to<GLint>(L, 8);
			glCopyTexImage2D(target, level, internalformat, x, y, width, height, border);
		}
		else {
			luaL_error(L, "gl.CopyTexImage: invalid arguments");
		}
	}
	else {
		luaL_error(L, "gl.CopyTexImage: invalid arguments");
	}
	return 0;
}

/*! Copy a 1D or 2D texture subimage
	If no yoffset or height argument is given, glCopyTexSubImage1D will be used, otherwise, glCopyTexSubImage2D 
	be used. Example usage:
	
	<luacode>
	1D case: gl.glCopyTexSubImage(GL.TEXTURE_1D, 0, 32, 0, 0, 64)
	2D case: gl.glCopyTexSubImage(GL.TEXTURE_2D, 0, 16, 24, 0, 0, 128, 128)
	</luacode>
	
	OpenGL Documentation: <a target="_blank" href="http://www.opengl.org/sdk/docs/man/xhtml/glCopyTexSubImage1D.xml">glCopyTexSubImage1D</a>, 
	<a target="_blank" href="http://www.opengl.org/sdk/docs/man/xhtml/glCopyTexSubImage2D.xml">glCopyTexSubImage2D</a>
	
	@param target Only relevant in the 2D case.  Specifies the target texture. 
					Must be GL_TEXTURE_2D, GL_TEXTURE_CUBE_MAP_POSITIVE_X, 
					GL_TEXTURE_CUBE_MAP_NEGATIVE_X, GL_TEXTURE_CUBE_MAP_POSITIVE_Y, 
					GL_TEXTURE_CUBE_MAP_NEGATIVE_Y, GL_TEXTURE_CUBE_MAP_POSITIVE_Z, or 
					GL_TEXTURE_CUBE_MAP_NEGATIVE_Z.
	@param level Specifies the level-of-detail number. Level 0 is the base image level. 
				Level n is the nth mipmap reduction image.
	@param xoffset Specifies a texel offset in the x direction within the texture array..
	@param [yoffset] Specifies a texel offset in the y direction within the texture array..
	@param x Specify the x window coordinate of the left corner of the row of pixels to be copied.
	@param y Specify the y window coordinate of the left corner of the row of pixels to be copied.
	@param width Specifies the width of the texture subimage.
	@param [height] Specifies the height of the texture subimage.
					
	@name CopyTexSubImage
*/
int gl_CopyTexSubImage(lua_State *L) {
	if(
		lua::is<GLenum>(L, 1) && 
		lua::is<GLint>(L, 2)
	) {
		GLenum target = lua::to<GLenum>(L, 1);
		GLint level = lua::to<GLint>(L, 2);

		int top = lua_gettop(L);
		if(
			top == 6 && 
			lua::is<GLint>(L, 3) &&
			lua::is<GLint>(L, 4) &&
			lua::is<GLint>(L, 5) &&
			lua::is<GLsizei>(L, 6)
		) {
			GLint xoffset = lua::to<GLint>(L, 3);
			GLint x = lua::to<GLint>(L, 4);
			GLint y = lua::to<GLint>(L, 5);
			GLsizei width = lua::to<GLsizei>(L, 6);
			glCopyTexSubImage1D(
				target,
				level,
				xoffset,
				x, y, 
				width
			);
		}
		else if(
			top == 8 && 
			lua::is<GLint>(L, 3) &&
			lua::is<GLint>(L, 4) &&
			lua::is<GLint>(L, 5) &&
			lua::is<GLint>(L, 6) &&
			lua::is<GLsizei>(L, 7) &&
			lua::is<GLsizei>(L, 8)
		) {
			GLint xoffset = lua::to<GLint>(L, 3);
			GLint yoffset = lua::to<GLint>(L, 4);
			GLint x = lua::to<GLint>(L, 5);
			GLint y = lua::to<GLint>(L, 6);
			GLsizei width = lua::to<GLsizei>(L, 7);
			GLsizei height = lua::to<GLsizei>(L, 8);
			glCopyTexSubImage2D(
				target,
				level,
				xoffset,
				yoffset, 
				x, y, 
				width,
				height
			);
		}
		else {
			luaL_error(L, "gl.CopyTexSubImage: invalid arguments");
		}
	}
	else {
		luaL_error(L, "gl.CopyTexSubImage: invalid arguments");
	}
	return 0;
}

/*! Specify whether front- or back-facing facets can be culled
	OpenGL Documentation: <a target="_blank" href="http://www.opengl.org/sdk/docs/man/xhtml/glCullFace.xml">glCullFace</a>
	
	@param mode Specifies whether front- or back-facing facets are candidates for culling. 
				Symbolic constants GL_FRONT, GL_BACK, and GL_FRONT_AND_BACK are accepted. 
				The initial value is GL_BACK.
					
	@name CullFace
*/
int gl_CullFace(lua_State *L) {
	if(lua::is<GLenum>(L, 1)) {
		GLenum mode = lua::to<GLenum>(L, 1);
		glCullFace(mode);
	}
	else {
		luaL_error(L, "gl.CullFace: invalid arguments");
	}
	return 0;
}

/*! Delete a contiguous group of display lists
	OpenGL Documentation: <a target="_blank" href="http://www.opengl.org/sdk/docs/man/xhtml/glDeleteLists.xml">glDeleteLists</a>
	
	@param list Specifies the integer name of the first display list to delete.
	@param range Specifies the number of display lists to delete.
					
	@name DeleteLists
*/
int gl_DeleteLists(lua_State *L) {
	if(lua::is<GLuint>(L, 1) && lua::is<GLsizei>(L, 2)) {
		GLuint listv = lua::to<GLuint>(L, 1);
		GLsizei range = lua::to<GLsizei>(L, 2);
		glDeleteLists(listv, range);
	}
	else {
		luaL_error(L, "gl.DeleteLists: invalid arguments");
	}
	return 0;
}

/*! Specify the value used for depth buffer comparisons
	OpenGL Documentation: <a target="_blank" href="http://www.opengl.org/sdk/docs/man/xhtml/glDepthFunc.xml">glDepthFunc</a>
	
	@param func Specifies the depth comparison function. Symbolic constants GL_NEVER, 
				GL_LESS, GL_EQUAL, GL_LEQUAL, GL_GREATER, GL_NOTEQUAL, GL_GEQUAL, 
				and GL_ALWAYS are accepted. The initial value is GL_LESS.
					
	@name DepthFunc
*/
int gl_DepthFunc(lua_State *L) {
	if(lua::is<GLenum>(L, 1)) {
		GLenum func = lua::to<GLenum>(L, 1);
		glDepthFunc(func);
	}
	else {
		luaL_error(L, "gl.DepthFunc: invalid arguments");
	}
	return 0;
}

/*! Enable or disable writing into the depth buffer
	OpenGL Documentation: <a target="_blank" href="http://www.opengl.org/sdk/docs/man/xhtml/glDepthMask.xml">glDepthMask</a>
	
	@param flag Specifies whether the depth buffer is enabled for writing. If flag is 
				GL_FALSE, depth buffer writing is disabled. Otherwise, it is enabled. 
				Initially, depth buffer writing is enabled.
					
	@name DepthMask
*/
int gl_DepthMask(lua_State *L) {
	if(lua::is<GLboolean>(L, 1)) {
		GLboolean flag = lua::to<GLboolean>(L, 1);
		glDepthMask(flag);
	}
	else {
		luaL_error(L, "gl.DepthMask: invalid arguments");
	}
	return 0;
}

/*! Specify mapping of depth values from normalized device coordinates to window coordinates
	OpenGL Documentation: <a target="_blank" href="http://www.opengl.org/sdk/docs/man/xhtml/glDepthRange.xml">glDepthRange</a>
	
	@param near Specifies the mapping of the near clipping plane to window coordinates. 
				The initial value is 0.
	@param far Specifies the mapping of the far clipping plane to window coordinates. 
				The initial value is 1.
					
	@name DepthRange
*/
int gl_DepthRange(lua_State *L) {
	if(lua::is<GLclampd>(L, 1) && lua::is<GLclampd>(L, 2)) {
		GLclampd near = lua::to<GLclampd>(L, 1);
		GLclampd far = lua::to<GLclampd>(L, 2);
		glDepthRange(near, far);
	}
	else {
		luaL_error(L, "gl.DepthRange: invalid arguments");
	}
	return 0;
}

/*! Disable server-side GL capabilities
	OpenGL Documentation: <a target="_blank" href="http://www.opengl.org/sdk/docs/man/xhtml/glEnable.xml">glDisable</a>
	
	@param cap Specifies a symbolic constant indicating a GL capability.
					
	@name Disable
*/
int gl_Disable(lua_State *L) {
	if(lua::is<GLenum>(L, 1)) {
		GLenum cap = lua::to<GLenum>(L, 1);
		glDisable(cap);
	}
	else {
		luaL_error(L, "gl.Disable: invalid arguments");
	}
	return 0;
}

/*! Specify which color buffers are to be drawn into
	OpenGL Documentation: <a target="_blank" href="http://www.opengl.org/sdk/docs/man/xhtml/glDrawBuffer.xml">glDrawBuffer</a>
	
	@param mode Specifies up to four color buffers to be drawn into. Symbolic constants 
				GL_NONE, GL_FRONT_LEFT, GL_FRONT_RIGHT, GL_BACK_LEFT, GL_BACK_RIGHT, 
				GL_FRONT, GL_BACK, GL_LEFT, GL_RIGHT, GL_FRONT_AND_BACK, and GL_AUXi, 
				where i is between 0 and the value of GL_AUX_BUFFERS minus 1, are 
				accepted. (GL_AUX_BUFFERS is not the upper limit; use glGet to query 
				the number of available aux buffers.) The initial value is GL_FRONT 
				for single-buffered contexts, and GL_BACK for double-buffered contexts
					
	@name DrawBuffer
*/
int gl_DrawBuffer(lua_State *L) {
	if(lua::is<GLenum>(L, 1)) {
		GLenum mode = lua::to<GLenum>(L, 1);
		glDrawBuffer(mode);
	}
	else {
		luaL_error(L, "gl.DrawBuffer: invalid arguments");
	}
	return 0;
}

/*! Flag edges as either boundary or nonboundary
	OpenGL Documentation: <a target="_blank" href="http://www.opengl.org/sdk/docs/man/xhtml/glEdgeFlag.xml">glEdgeFlag</a>
	
	@param flag Specifies the current edge flag value, either GL_TRUE or GL_FALSE. 
				The initial value is GL_TRUE.
					
	@name EdgeFlag
*/
int gl_EdgeFlag(lua_State *L) {
	if(lua::is<GLboolean>(L, 1)) {
		GLboolean flag = lua::to<GLboolean>(L, 1);
		glEdgeFlag(flag);
	}
	else {
		luaL_error(L, "gl.EdgeFlag: invalid arguments");
	}
	return 0;
}

/*! Enable server-side GL capabilities
	OpenGL Documentation: <a target="_blank" href="http://www.opengl.org/sdk/docs/man/xhtml/glEnable.xml">glEnable</a>
	
	@param cap Specifies a symbolic constant indicating a GL capability.
					
	@name Enable
*/
int gl_Enable(lua_State *L) {
	if(lua::is<GLenum>(L, 1)) {
		GLenum cap = lua::to<GLenum>(L, 1);
		glEnable(cap);
	}
	else {
		luaL_error(L, "gl.Enable: invalid arguments");
	}
	return 0;
}

/*! Delimit the vertices of a primitive or a group of like primitives
	OpenGL Documentation: <a target="_blank" href="http://www.opengl.org/sdk/docs/man/xhtml/glBegin.xml">glEnd</a>
					
	@name End
*/
int gl_End(lua_State *L) {
	glEnd();
	return 0;
}

/*! End the creation or replacement of a display list
	OpenGL Documentation: <a target="_blank" href="http://www.opengl.org/sdk/docs/man/xhtml/glNewList.xml">glEndList</a>
					
	@name EndList
*/
int gl_EndList(lua_State *L) {
	glEndList();
	return 0;
}

/*! Block until all GL execution is complete
	OpenGL Documentation: <a target="_blank" href="http://www.opengl.org/sdk/docs/man/xhtml/glFinish.xml">glFinish</a>
					
	@name Finish
*/
int gl_Finish(lua_State *L) {
	glFinish();
	return 0;
}

/*! Force execution of GL commands in finite time
	OpenGL Documentation: <a target="_blank" href="http://www.opengl.org/sdk/docs/man/xhtml/glFlush.xml">glFlush</a>
					
	@name Flush
*/
int gl_Flush(lua_State *L) {
	glFlush();
	return 0;
}

/*! Specify fog parameters
	OpenGL Documentation: <a target="_blank" href="http://www.opengl.org/sdk/docs/man/xhtml/glFog.xml">glFog</a>
	
	@param pname Specifies a single-valued fog parameter. GL_FOG_MODE, GL_FOG_DENSITY, 
					GL_FOG_START, GL_FOG_END, GL_FOG_INDEX, and GL_FOG_COORD_SRC are 
					accepted.
	
	@param ... Specifies the value that pname will be set to.
					
	@name Fog
*/
int gl_Fog(lua_State *L) {
	GLfloat params[] = {1, 1, 1, 1};
	if(lua::is<GLenum>(L, 1) && lua::to_vec<GLfloat>(L, 2, 4, params)) {
		GLenum pname = lua::to<GLenum>(L, 1);
		glFogfv(pname, params);
	}
	else {
		luaL_error(L, "gl.Fog: invalid arguments");
	}
	return 0;
}

/*! Define front- and back-facing polygons
	OpenGL Documentation: <a target="_blank" href="http://www.opengl.org/sdk/docs/man/xhtml/glFrontFace.xml">glFrontFace</a>
	
	@param mode Specifies the orientation of front-facing polygons. GL_CW and GL_CCW 
				are accepted. The initial value is GL_CCW.
					
	@name FrontFace
*/
int gl_FrontFace(lua_State *L) {
	if(lua::is<GLenum>(L, 1)) {
		GLenum mode = lua::to<GLenum>(L, 1);
		glFrontFace(mode);
	}
	else {
		luaL_error(L, "gl.FrontFace: invalid arguments");
	}
	return 0;
}

/*! Multiply the current matrix by a perspective matrix
	OpenGL Documentation: <a target="_blank" href="http://www.opengl.org/sdk/docs/man/xhtml/glFrustum.xml">glFrustum</a>
	
	@param left Specify the coordinates for the left vertical clipping plane.
	@param right Specify the coordinates for the right vertical clipping plane.
	@param bottom Specify the coordinates for the top horizontal clipping plane.
	@param top Specify the coordinates for the bottom horizontal clipping plane.
	@param near Specify the distance to the near depth clipping plane. Must be positive.
	@param far Specify the distance to the far depth clipping plane. Must be positive.
					
	@name Frustum
*/
int gl_Frustum(lua_State *L) {
	if(
		lua::is<GLdouble>(L, 1) &&
		lua::is<GLdouble>(L, 2) &&
		lua::is<GLdouble>(L, 3) &&
		lua::is<GLdouble>(L, 4) &&
		lua::is<GLdouble>(L, 5) &&
		lua::is<GLdouble>(L, 6)
	) {
		GLdouble left = lua::to<GLdouble>(L, 1);
		GLdouble right = lua::to<GLdouble>(L, 2);
		GLdouble bottom = lua::to<GLdouble>(L, 3);
		GLdouble top = lua::to<GLdouble>(L, 4);
		GLdouble near = lua::to<GLdouble>(L, 5);
		GLdouble far = lua::to<GLdouble>(L, 6);
		glFrustum(left, right, bottom, top, near, far);
	}
	else {
		luaL_error(L, "gl.Frustum: invalid arguments");
	}
	return 0;
}

/*! Generate a contiguous set of empty display lists
	OpenGL Documentation: <a target="_blank" href="http://www.opengl.org/sdk/docs/man/xhtml/glGenLists.xml">glGenLists</a>
	
	@param range Specifies the number of contiguous empty display lists to be generated.
					
	@name GenLists
*/
int gl_GenLists(lua_State *L) {
	if(lua::is<GLsizei>(L, 1)) {
		GLsizei range = lua::to<GLsizei>(L, 1);
		glGenLists(range);
	}
	else {
		luaL_error(L, "gl.GenLists: invalid arguments");
	}
	return 0;
}

/*! Return the value or values of a selected parameter
	OpenGL Documentation: <a target="_blank" href="http://www.opengl.org/sdk/docs/man/xhtml/glGet.xml">glGet</a>
	
	@param pname Specifies the parameter value to be returned. 
				See documentation for the list of accepted values.
	
	@ret The parameter value. Values are returned unpacked.
	@name Get
*/
int gl_Get(lua_State *L) {
	if(lua::is<GLenum>(L, 1)) {
		GLenum pname = lua::to<GLenum>(L, 1);
		
		switch(pname) {
			// 1 boolean
			case GL_AUTO_NORMAL:
			case GL_BLEND:
			case GL_CLIP_PLANE0:
			case GL_CLIP_PLANE1:
			case GL_CLIP_PLANE2:
			case GL_CLIP_PLANE3:
			case GL_CLIP_PLANE4:
			case GL_CLIP_PLANE5:
			case GL_COLOR_ARRAY:
			case GL_COLOR_LOGIC_OP:
			case GL_COLOR_MATERIAL:
			case GL_COLOR_SUM:
			case GL_COLOR_TABLE:
			case GL_CONVOLUTION_1D:
			case GL_CONVOLUTION_2D:
			case GL_CULL_FACE:
			case GL_CURRENT_RASTER_POSITION_VALID:
			case GL_DEPTH_WRITEMASK:
			case GL_DITHER:
			case GL_DOUBLEBUFFER:
			case GL_FOG:
			case GL_FOG_COORD_ARRAY:
			case GL_HISTOGRAM:
			case GL_INDEX_ARRAY:
			case GL_INDEX_LOGIC_OP:
			case GL_INDEX_MODE:
			case GL_LIGHT0:
			case GL_LIGHT1:
			case GL_LIGHT2:
			case GL_LIGHT3:
			case GL_LIGHT4:
			case GL_LIGHT5:
			case GL_LIGHT6:
			case GL_LIGHT7:
			case GL_LIGHTING:
			case GL_LIGHT_MODEL_LOCAL_VIEWER:
			case GL_LIGHT_MODEL_TWO_SIDE:
			case GL_LINE_SMOOTH:
			case GL_LINE_STIPPLE:
			case GL_MAP1_COLOR_4:
			case GL_MAP1_INDEX:
			case GL_MAP1_NORMAL:
			case GL_MAP1_TEXTURE_COORD_1:
			case GL_MAP1_TEXTURE_COORD_2:
			case GL_MAP1_TEXTURE_COORD_3:
			case GL_MAP1_TEXTURE_COORD_4:
			case GL_MAP1_VERTEX_3:
			case GL_MAP1_VERTEX_4:
			case GL_MAP2_COLOR_4:
			case GL_MAP2_INDEX:
			case GL_MAP2_NORMAL:
			case GL_MAP2_TEXTURE_COORD_1:
			case GL_MAP2_TEXTURE_COORD_2:
			case GL_MAP2_TEXTURE_COORD_3:
			case GL_MAP2_TEXTURE_COORD_4:
			case GL_MAP2_VERTEX_3:
			case GL_MAP2_VERTEX_4:
			case GL_MAP_COLOR:
			case GL_MAP_STENCIL:
			case GL_MINMAX:
			case GL_NORMAL_ARRAY:
			case GL_NORMALIZE:
			case GL_PACK_LSB_FIRST:
			case GL_PACK_SWAP_BYTES:
			case GL_POINT_SMOOTH:
			case GL_POINT_SPRITE:
			case GL_POLYGON_OFFSET_FILL:
			case GL_POLYGON_OFFSET_LINE:
			case GL_POLYGON_OFFSET_POINT:
			case GL_POLYGON_SMOOTH:
			case GL_POLYGON_STIPPLE:
			case GL_POST_COLOR_MATRIX_COLOR_TABLE:
			case GL_POST_CONVOLUTION_COLOR_TABLE:
			case GL_RESCALE_NORMAL:
			case GL_RGBA_MODE:
			case GL_SAMPLE_COVERAGE_INVERT:
			case GL_SCISSOR_TEST:
			case GL_SECONDARY_COLOR_ARRAY:
			case GL_SEPARABLE_2D:
			case GL_STENCIL_TEST:
			case GL_STEREO:
			case GL_TEXTURE_1D:
			case GL_TEXTURE_2D:
			case GL_TEXTURE_3D:
			case GL_TEXTURE_COORD_ARRAY:
			case GL_TEXTURE_CUBE_MAP:
			case GL_TEXTURE_GEN_Q:
			case GL_TEXTURE_GEN_R:
			case GL_TEXTURE_GEN_S:
			case GL_TEXTURE_GEN_T:
			case GL_UNPACK_LSB_FIRST:
			case GL_UNPACK_SWAP_BYTES:
			case GL_VERTEX_ARRAY:
			case GL_VERTEX_PROGRAM_POINT_SIZE:
			case GL_VERTEX_PROGRAM_TWO_SIDE: {
				GLboolean params[1];
				glGetBooleanv(pname, params);
				lua::push(L, params[0]); 
				return 1;
			} break;
			
			
			
			// 4 boolean
			case GL_COLOR_WRITEMASK: {
				GLboolean params[4];
				glGetBooleanv(pname, params);
				for(int i=0; i < 4; i++) {
					lua::push(L, params[i]); 
				}
				return 4;
			} break;
			
		
			// 1 int
			case GL_ACCUM_ALPHA_BITS:
			case GL_ACCUM_BLUE_BITS:
			case GL_ACCUM_GREEN_BITS:
			case GL_ACCUM_RED_BITS:
			case GL_ACTIVE_TEXTURE:
			case GL_ALPHA_BITS:
			case GL_ALPHA_TEST:
			case GL_ALPHA_TEST_FUNC:
			case GL_ARRAY_BUFFER_BINDING:
			case GL_ATTRIB_STACK_DEPTH:
			case GL_AUX_BUFFERS:
			case GL_BLEND_DST_ALPHA:
			case GL_BLEND_DST_RGB:
			case GL_BLEND_EQUATION_RGB:
			case GL_BLEND_EQUATION_ALPHA:
			case GL_BLEND_SRC_ALPHA:
			case GL_BLEND_SRC_RGB:
			case GL_BLUE_BITS:
			case GL_CLIENT_ACTIVE_TEXTURE:
			case GL_CLIENT_ATTRIB_STACK_DEPTH:
			case GL_COLOR_ARRAY_BUFFER_BINDING:
			case GL_COLOR_ARRAY_SIZE:
			case GL_COLOR_ARRAY_STRIDE:
			case GL_COLOR_ARRAY_TYPE:
			case GL_COLOR_MATERIAL_FACE:
			case GL_COLOR_MATERIAL_PARAMETER:
			case GL_COLOR_MATRIX_STACK_DEPTH:
			case GL_CULL_FACE_MODE:
			case GL_CURRENT_INDEX:
			case GL_CURRENT_PROGRAM:
			case GL_CURRENT_RASTER_INDEX:
			case GL_DEPTH_BITS:
			case GL_DEPTH_FUNC:
			case GL_DEPTH_TEST:
			case GL_DRAW_BUFFER:
			case GL_DRAW_BUFFER0:
			case GL_DRAW_BUFFER1:
			case GL_DRAW_BUFFER2:
			case GL_DRAW_BUFFER3:
			case GL_DRAW_BUFFER4:
			case GL_DRAW_BUFFER5:
			case GL_DRAW_BUFFER6:
			case GL_DRAW_BUFFER7:
			case GL_DRAW_BUFFER8:
			case GL_DRAW_BUFFER9:
			case GL_DRAW_BUFFER10:
			case GL_DRAW_BUFFER11:
			case GL_DRAW_BUFFER12:
			case GL_DRAW_BUFFER13:
			case GL_DRAW_BUFFER14:
			case GL_DRAW_BUFFER15:
			case GL_EDGE_FLAG:
			case GL_EDGE_FLAG_ARRAY:
			case GL_EDGE_FLAG_ARRAY_STRIDE:
			case GL_ELEMENT_ARRAY_BUFFER_BINDING:
			case GL_FEEDBACK_BUFFER_SIZE:
			case GL_FEEDBACK_BUFFER_TYPE:
			case GL_FOG_COORD_ARRAY_BUFFER_BINDING:
			case GL_FOG_COORD_ARRAY_STRIDE:
			case GL_FOG_COORD_ARRAY_TYPE:
			case GL_FOG_COORD_SRC:
			case GL_FOG_HINT:
			case GL_FOG_INDEX:
			case GL_FOG_MODE:
			case GL_FRAGMENT_SHADER_DERIVATIVE_HINT:
			case GL_FRONT_FACE:
			case GL_GENERATE_MIPMAP_HINT:
			case GL_GREEN_BITS:
			case GL_INDEX_ARRAY_BUFFER_BINDING:
			case GL_INDEX_ARRAY_STRIDE:
			case GL_INDEX_ARRAY_TYPE:
			case GL_INDEX_BITS:
			case GL_INDEX_OFFSET:
			case GL_INDEX_SHIFT:
			case GL_INDEX_WRITEMASK:
			case GL_LIGHT_MODEL_COLOR_CONTROL:
			case GL_LINE_SMOOTH_HINT:
			case GL_LINE_STIPPLE_PATTERN:
			case GL_LINE_STIPPLE_REPEAT:
			case GL_LIST_BASE:
			case GL_LIST_INDEX:
			case GL_LIST_MODE:
			case GL_LOGIC_OP_MODE:
			case GL_MAP1_GRID_SEGMENTS:
			case GL_MATRIX_MODE:
			case GL_MAX_3D_TEXTURE_SIZE:
			case GL_MAX_CLIENT_ATTRIB_STACK_DEPTH:
			case GL_MAX_ATTRIB_STACK_DEPTH:
			case GL_MAX_CLIP_PLANES:
			case GL_MAX_COLOR_MATRIX_STACK_DEPTH:
			case GL_MAX_COMBINED_TEXTURE_IMAGE_UNITS:
			case GL_MAX_CUBE_MAP_TEXTURE_SIZE:
			case GL_MAX_DRAW_BUFFERS:
			case GL_MAX_ELEMENTS_INDICES:
			case GL_MAX_ELEMENTS_VERTICES:
			case GL_MAX_EVAL_ORDER:
			case GL_MAX_FRAGMENT_UNIFORM_COMPONENTS:
			case GL_MAX_LIGHTS:
			case GL_MAX_LIST_NESTING:
			case GL_MAX_MODELVIEW_STACK_DEPTH:
			case GL_MAX_NAME_STACK_DEPTH:
			case GL_MAX_PIXEL_MAP_TABLE:
			case GL_MAX_PROJECTION_STACK_DEPTH:
			case GL_MAX_TEXTURE_COORDS:
			case GL_MAX_TEXTURE_IMAGE_UNITS:
			case GL_MAX_TEXTURE_LOD_BIAS:
			case GL_MAX_TEXTURE_SIZE:
			case GL_MAX_TEXTURE_STACK_DEPTH:
			case GL_MAX_TEXTURE_UNITS:
			case GL_MAX_VARYING_FLOATS:
			case GL_MAX_VERTEX_ATTRIBS:
			case GL_MAX_VERTEX_TEXTURE_IMAGE_UNITS:
			case GL_MAX_VERTEX_UNIFORM_COMPONENTS:
			case GL_MODELVIEW_STACK_DEPTH:
			case GL_NAME_STACK_DEPTH:
			case GL_NORMAL_ARRAY_BUFFER_BINDING:
			case GL_NORMAL_ARRAY_STRIDE:
			case GL_NORMAL_ARRAY_TYPE:
			case GL_NUM_COMPRESSED_TEXTURE_FORMATS:
			case GL_PACK_ALIGNMENT:
			case GL_PACK_ROW_LENGTH:
			case GL_PACK_SKIP_IMAGES:
			case GL_PACK_SKIP_PIXELS:
			case GL_PACK_SKIP_ROWS:
			case GL_PERSPECTIVE_CORRECTION_HINT:
			case GL_PIXEL_MAP_A_TO_A_SIZE:
			case GL_PIXEL_MAP_B_TO_B_SIZE:
			case GL_PIXEL_MAP_G_TO_G_SIZE:
			case GL_PIXEL_MAP_I_TO_A_SIZE:
			case GL_PIXEL_MAP_I_TO_B_SIZE:
			case GL_PIXEL_MAP_I_TO_G_SIZE:
			case GL_PIXEL_MAP_I_TO_I_SIZE:
			case GL_PIXEL_MAP_I_TO_R_SIZE:
			case GL_PIXEL_MAP_R_TO_R_SIZE:
			case GL_PIXEL_MAP_S_TO_S_SIZE:
			case GL_PIXEL_PACK_BUFFER_BINDING:
			case GL_PIXEL_UNPACK_BUFFER_BINDING:
			case GL_PACK_IMAGE_HEIGHT:
			case GL_POINT_SMOOTH_HINT:
			case GL_POLYGON_SMOOTH_HINT:
			case GL_PROJECTION_STACK_DEPTH:
			case GL_READ_BUFFER:
			case GL_RED_BITS:
			case GL_RENDER_MODE:
			case GL_SAMPLE_BUFFERS:
			case GL_SAMPLES:
			case GL_SECONDARY_COLOR_ARRAY_BUFFER_BINDING:
			case GL_SECONDARY_COLOR_ARRAY_SIZE:
			case GL_SECONDARY_COLOR_ARRAY_STRIDE:
			case GL_SECONDARY_COLOR_ARRAY_TYPE:
			case GL_SELECTION_BUFFER_SIZE:
			case GL_SHADE_MODEL:
			case GL_STENCIL_BACK_FAIL:
			case GL_STENCIL_BACK_FUNC:
			case GL_STENCIL_BACK_PASS_DEPTH_FAIL:
			case GL_STENCIL_BACK_PASS_DEPTH_PASS:
			case GL_STENCIL_BACK_REF:
			case GL_STENCIL_BACK_VALUE_MASK:
			case GL_STENCIL_BACK_WRITEMASK:
			case GL_STENCIL_BITS:
			case GL_STENCIL_CLEAR_VALUE:
			case GL_STENCIL_FAIL:
			case GL_STENCIL_FUNC:
			case GL_STENCIL_PASS_DEPTH_FAIL:
			case GL_STENCIL_PASS_DEPTH_PASS:
			case GL_STENCIL_REF:
			case GL_STENCIL_VALUE_MASK:
			case GL_STENCIL_WRITEMASK:
			case GL_SUBPIXEL_BITS:
			case GL_TEXTURE_BINDING_1D:
			case GL_TEXTURE_BINDING_2D:
			case GL_TEXTURE_BINDING_3D:
			case GL_TEXTURE_BINDING_CUBE_MAP:
			case GL_TEXTURE_COMPRESSION_HINT:
			case GL_TEXTURE_COORD_ARRAY_BUFFER_BINDING:
			case GL_TEXTURE_COORD_ARRAY_SIZE:
			case GL_TEXTURE_COORD_ARRAY_STRIDE:
			case GL_TEXTURE_COORD_ARRAY_TYPE:
			case GL_TEXTURE_STACK_DEPTH:
			case GL_UNPACK_ALIGNMENT:
			case GL_UNPACK_IMAGE_HEIGHT:
			case GL_UNPACK_ROW_LENGTH:
			case GL_UNPACK_SKIP_IMAGES:
			case GL_UNPACK_SKIP_PIXELS:
			case GL_UNPACK_SKIP_ROWS:
			case GL_VERTEX_ARRAY_BUFFER_BINDING:
			case GL_VERTEX_ARRAY_SIZE:
			case GL_VERTEX_ARRAY_STRIDE:
			case GL_VERTEX_ARRAY_TYPE: {
				GLint params[1];
				glGetIntegerv(pname, params);
				lua::push(L, params[0]);
				return 1;
			} break;
			
			
			
			
			// 2 int
			case GL_MAX_VIEWPORT_DIMS:
			case GL_MAP2_GRID_SEGMENTS:
			case GL_POLYGON_MODE: {
				GLint params[2];
				glGetIntegerv(pname, params);
				for(int i=0; i < 2; i++) {
					lua::push(L, params[i]); 
				}
				return 2;
			} break;
			
			// 4 int
			case GL_SCISSOR_BOX:
			case GL_VIEWPORT: {
				GLint params[4];
				glGetIntegerv(pname, params);
				for(int i=0; i < 4; i++) {
					lua::push(L, params[i]); 
				}
				return 4;
			} break;
			
					
			
			
			// GL_NUM_COMPRESSED_TEXTURE_FORMATS int
			case GL_COMPRESSED_TEXTURE_FORMATS: {
				GLint params[32];
				GLint n = 0;
				glGetIntegerv(GL_NUM_COMPRESSED_TEXTURE_FORMATS, &n);
				glGetIntegerv(pname, params);
				for(int i=0; i < n; i++) {
					lua::push(L, params[i]);
				}
				return n;
			} break;
			
			
			// 1 float
			case GL_ALPHA_BIAS:
			case GL_ALPHA_SCALE:
			case GL_ALPHA_TEST_REF:
			case GL_BLUE_BIAS:
			case GL_BLUE_SCALE:
			case GL_CURRENT_FOG_COORD:
			case GL_CURRENT_RASTER_DISTANCE:
			case GL_DEPTH_BIAS:
			case GL_DEPTH_CLEAR_VALUE:
			case GL_DEPTH_SCALE:
			case GL_EDGE_FLAG_ARRAY_BUFFER_BINDING:
			case GL_FOG_DENSITY:
			case GL_FOG_END:
			case GL_FOG_START:
			case GL_GREEN_BIAS:
			case GL_GREEN_SCALE:
			case GL_INDEX_CLEAR_VALUE:
			case GL_LINE_WIDTH:
			//case GL_LINE_WIDTH_GRANULARITY:
			case GL_POINT_FADE_THRESHOLD_SIZE:
			case GL_POINT_SIZE:
			//case GL_POINT_SIZE_GRANULARITY:
			case GL_POINT_SIZE_MAX:
			case GL_POINT_SIZE_MIN:
			case GL_POLYGON_OFFSET_FACTOR:
			case GL_POLYGON_OFFSET_UNITS:
			case GL_POST_COLOR_MATRIX_RED_BIAS:
			case GL_POST_COLOR_MATRIX_GREEN_BIAS:
			case GL_POST_COLOR_MATRIX_BLUE_BIAS:
			case GL_POST_COLOR_MATRIX_ALPHA_BIAS:
			case GL_POST_COLOR_MATRIX_RED_SCALE:
			case GL_POST_COLOR_MATRIX_GREEN_SCALE:
			case GL_POST_COLOR_MATRIX_BLUE_SCALE:
			case GL_POST_COLOR_MATRIX_ALPHA_SCALE:
			case GL_POST_CONVOLUTION_RED_BIAS:
			case GL_POST_CONVOLUTION_GREEN_BIAS:
			case GL_POST_CONVOLUTION_BLUE_BIAS:
			case GL_POST_CONVOLUTION_ALPHA_BIAS:
			case GL_POST_CONVOLUTION_RED_SCALE:
			case GL_POST_CONVOLUTION_GREEN_SCALE:
			case GL_POST_CONVOLUTION_BLUE_SCALE:
			case GL_POST_CONVOLUTION_ALPHA_SCALE:
			case GL_RED_BIAS:
			case GL_RED_SCALE:
			case GL_SAMPLE_COVERAGE_VALUE:
			case GL_SMOOTH_LINE_WIDTH_GRANULARITY:
			case GL_SMOOTH_POINT_SIZE_GRANULARITY:
			case GL_ZOOM_X:
			case GL_ZOOM_Y: {
				GLdouble params[1];
				glGetDoublev(pname, params);
				lua::push(L, params[0]);
				return 1;
			} break;
			
			
			
			// 2 float
			case GL_ALIASED_POINT_SIZE_RANGE:
			case GL_ALIASED_LINE_WIDTH_RANGE:
			case GL_DEPTH_RANGE:
//			case GL_LINE_WIDTH_RANGE:
			case GL_MAP1_GRID_DOMAIN:
			case GL_SMOOTH_LINE_WIDTH_RANGE:
			case GL_SMOOTH_POINT_SIZE_RANGE: {
				GLdouble params[2];
				glGetDoublev(pname, params);
				for(int i=0; i < 2; i++) {
					lua::push(L, params[i]); 
				}
				return 2;
			} break;
			
			
			// 3 float
			case GL_CURRENT_NORMAL:
			case GL_POINT_DISTANCE_ATTENUATION: {
				GLdouble params[3];
				glGetDoublev(pname, params);
				for(int i=0; i < 3; i++) {
					lua::push(L, params[i]); 
				}
				return 3;
			} break;
			
			
			// 4 float
			case GL_ACCUM_CLEAR_VALUE:
			case GL_BLEND_COLOR:
			case GL_COLOR_CLEAR_VALUE:
			case GL_CURRENT_COLOR:
			case GL_CURRENT_RASTER_COLOR:
			case GL_CURRENT_RASTER_POSITION:
			case GL_CURRENT_RASTER_SECONDARY_COLOR:
			case GL_CURRENT_RASTER_TEXTURE_COORDS:
			case GL_CURRENT_SECONDARY_COLOR:
			case GL_CURRENT_TEXTURE_COORDS:
			case GL_FOG_COLOR:
			case GL_LIGHT_MODEL_AMBIENT:
			case GL_MAP2_GRID_DOMAIN: {
				GLdouble params[4];
				glGetDoublev(pname, params);
				for(int i=0; i < 4; i++) {
					lua::push(L, params[i]); 
				}
				return 4;
			} break;
			
			
			// 16 double
			case GL_COLOR_MATRIX:
			case GL_MODELVIEW_MATRIX:
			case GL_PROJECTION_MATRIX:
			case GL_TEXTURE_MATRIX:
			case GL_TRANSPOSE_COLOR_MATRIX:
			case GL_TRANSPOSE_MODELVIEW_MATRIX:
			case GL_TRANSPOSE_PROJECTION_MATRIX:
			case GL_TRANSPOSE_TEXTURE_MATRIX: {
				GLdouble params[16];
				glGetDoublev(pname, params);
				for(int i=0; i < 16; i++) {
					lua::push(L, params[i]); 
				}
				return 16;
			} break;
			
			default:
				break;
			
		}
	}
	else {
		luaL_error(L, "gl.Get: invalid arguments");
	}
	return 1;
}

/*! Return the coefficients of the specified clipping plane
	OpenGL Documentation: <a target="_blank" href="http://www.opengl.org/sdk/docs/man/xhtml/glGetClipPlane.xml">glGetClipPlane</a>
	
	@param plane Specifies a clipping plane. The number of clipping planes depends on the 
					implementation, but at least six clipping planes are supported. They are 
					identified by symbolic names of the form GL_CLIP_PLANE i where i ranges 
					from 0 to the value of GL_MAX_CLIP_PLANES - 1.
	
	@ret The four coefficients of the plane equation for plane.
					
	@name GetClipPlane
*/
int gl_GetClipPlane(lua_State *L) {
	if(lua::is<GLenum>(L, 1)) {
		GLenum plane = lua::to<GLenum>(L, 1);
		GLdouble equation[4];
		glGetClipPlane(plane, equation);
		
		lua_newtable(L);
		for(int i=0; i < 4; i++) {
			lua::push(L, equation[i]);
			lua_rawseti(L, -2, i+1);
		}
	}
	else {
		luaL_error(L, "gl.GetClipPlane: invalid arguments");
	}
	return 1;
}

/*! Return error information
	OpenGL Documentation: <a target="_blank" href="http://www.opengl.org/sdk/docs/man/xhtml/glGetError.xml">glGetError</a>

	@ret The value of the error flag
					
	@name GetError
*/
int gl_GetError(lua_State *L) {
	GLenum err = glGetError();
	lua::push(L, err);
	return 1;
}

/*! Return light source parameter values
	OpenGL Documentation: <a target="_blank" href="http://www.opengl.org/sdk/docs/man/xhtml/glGetLight.xml">glGetLight</a>
	
	@param light Specifies a light source. The number of possible lights depends 
					on the implementation, but at least eight lights are supported. 
					They are identified by symbolic names of the form GL_LIGHTi 
					where i ranges from 0 to the value of GL_MAX_LIGHTS - 1.
					
	@param pname Specifies a light source parameter for light. Accepted symbolic 
				names are GL_AMBIENT, GL_DIFFUSE, GL_SPECULAR, GL_POSITION, 
				GL_SPOT_DIRECTION, GL_SPOT_EXPONENT, GL_SPOT_CUTOFF, 
				GL_CONSTANT_ATTENUATION, GL_LINEAR_ATTENUATION, and 
				GL_QUADRATIC_ATTENUATION.
	
	@ret The requested parameter values as a table.
					
	@name GetLight
*/
int gl_GetLight(lua_State *L) {
	if(lua::is<GLenum>(L, 1) && lua::is<GLenum>(L, 2)) {
		GLenum light = lua::to<GLenum>(L, 1);
		GLenum pname = lua::to<GLenum>(L, 2);
		int n = 1;
		
		switch(pname) {
			// 4 float
			case GL_AMBIENT:
			case GL_DIFFUSE:
			case GL_SPECULAR:
			case GL_POSITION:
				n = 4;
			
			// 3 float
			case GL_SPOT_DIRECTION:
				n = 3;
			
			// 1 float
			case GL_SPOT_EXPONENT:
			case GL_SPOT_CUTOFF:
			case GL_CONSTANT_ATTENUATION:
			case GL_LINEAR_ATTENUATION:
			case GL_QUADRATIC_ATTENUATION:
				n = 1;
				
			default:
				break;
			
		}
		
		GLfloat params[4];
		glGetLightfv(light, pname, params);
		
		lua_newtable(L);
		for(int i=0; i < n; i++) {
			lua::push(L, params[i]);
			lua_rawseti(L, -2, i+1);
		}
	}
	else {
		luaL_error(L, "gl.GetLight: invalid arguments");
	}
	return 1;
}

/*! Return material parameters
	OpenGL Documentation: <a target="_blank" href="http://www.opengl.org/sdk/docs/man/xhtml/glGetMaterial.xml">glGetMaterial</a>
	
	@param target Specifies which of the two materials is being queried. GL_FRONT or GL_BACK 
					are accepted, representing the front and back materials, respectively.
					
	@param pname Specifies the material parameter to return. GL_AMBIENT, GL_DIFFUSE, 
					GL_SPECULAR, GL_EMISSION, GL_SHININESS, and GL_COLOR_INDEXES are 
					accepted.
	
	@ret The requested data values as a table.
					
	@name GetMaterial
*/
int gl_GetMaterial(lua_State *L) {
	if(lua::is<GLenum>(L, 1) && lua::is<GLenum>(L, 2)) {
		GLenum face = lua::to<GLenum>(L, 1);
		GLenum pname = lua::to<GLenum>(L, 2);
		int n = 1;
		
		switch(pname) {
			// 4 float
			case GL_AMBIENT:
			case GL_DIFFUSE:
			case GL_SPECULAR:
			case GL_EMISSION:
				n = 4;
				break;
			
			// 3 float
			case GL_COLOR_INDEXES:
				n = 3;
				break;
			
			// 1 float
			case GL_SHININESS:
				n = 1;
				break;
				
			default:
				break;
			
		}
		
		GLfloat params[4];
		glGetMaterialfv(face, pname, params);
		
		lua_newtable(L);
		for(int i=0; i < n; i++) {
			lua::push(L, params[i]);
			lua_rawseti(L, -2, i+1);
		}
	}
	else {
		luaL_error(L, "gl.GetMaterial: invalid arguments");
	}
	return 1;
}

/*! Return a string describing the current GL connection
	OpenGL Documentation: <a target="_blank" href="http://www.opengl.org/sdk/docs/man/xhtml/glGetString.xml">glGetString</a>
	
	@param name Specifies a symbolic constant, one of GL_VENDOR, GL_RENDERER, GL_VERSION, 
				GL_SHADING_LANGUAGE_VERSION, or GL_EXTENSIONS.
	
	@ret The string result
					
	@name GetString
*/
int gl_GetString(lua_State *L) {
	if(lua::is<GLenum>(L, 1)) {
		GLenum name = lua::to<GLenum>(L, 1);
		const GLubyte *res = glGetString(name);
		lua_pushstring(L, (const char *)res);
	}
	else {
		luaL_error(L, "gl.GetString: invalid arguments");
	}
	return 1;
}

/*! Return texture environment parameters
	OpenGL Documentation: <a target="_blank" href="http://www.opengl.org/sdk/docs/man/xhtml/glGetTexEnv.xml">glGetTexEnv</a>
	
	@param target Specifies a texture environment. May be GL_TEXTURE_ENV, 
					GL_TEXTURE_FILTER_CONTROL, or GL_POINT_SPRITE.
					
	@param pname Specifies the symbolic name of a texture environment parameter. Accepted 
					values are GL_TEXTURE_ENV_MODE, GL_TEXTURE_ENV_COLOR, GL_TEXTURE_LOD_BIAS, 
					GL_COMBINE_RGB, GL_COMBINE_ALPHA, GL_SRC0_RGB, GL_SRC1_RGB, GL_SRC2_RGB, 
					GL_SRC0_ALPHA, GL_SRC1_ALPHA, GL_SRC2_ALPHA, GL_OPERAND0_RGB, 
					GL_OPERAND1_RGB, GL_OPERAND2_RGB, GL_OPERAND0_ALPHA, GL_OPERAND1_ALPHA, 
					GL_OPERAND2_ALPHA, GL_RGB_SCALE, GL_ALPHA_SCALE, or GL_COORD_REPLACE.
	
	@ret The requested data values as a table.
					
	@name GetTexEnv
*/
int gl_GetTexEnv(lua_State *L) {
	if(lua::is<GLenum>(L, 1) && lua::is<GLenum>(L, 2)) {
		GLenum target = lua::to<GLenum>(L, 1);
		GLenum pname = lua::to<GLenum>(L, 2);
		
		switch(pname) {
			// 4 float
			case GL_TEXTURE_ENV_COLOR: {
				GLfloat params[4];
				glGetTexEnvfv(target, pname, params);
			
				lua_newtable(L);
				for(int i=0; i < 4; i++) {
					lua::push(L, params[i]);
					lua_rawseti(L, -2, i+1);
				}
			} break;

				
			// 1 float
			case GL_TEXTURE_LOD_BIAS:
			case GL_RGB_SCALE:
			case GL_ALPHA_SCALE: {
				GLfloat params[1];
				glGetTexEnvfv(target, pname, params);
			
				lua_newtable(L);
				lua::push(L, params[0]);
				lua_rawseti(L, -2, 1);
			} break;

			
			// 1 int
			case GL_TEXTURE_ENV_MODE:
			case GL_COMBINE_RGB:
			case GL_COMBINE_ALPHA:
			case GL_SRC0_RGB:
			case GL_SRC1_RGB:
			case GL_SRC2_RGB:
			case GL_SRC0_ALPHA:
			case GL_SRC1_ALPHA:
			case GL_SRC2_ALPHA:
			case GL_OPERAND0_RGB:
			case GL_OPERAND1_RGB:
			case GL_OPERAND2_RGB:
			case GL_OPERAND0_ALPHA:
			case GL_OPERAND1_ALPHA:
			case GL_OPERAND2_ALPHA:
			
			// 1 boolean
			case GL_COORD_REPLACE: {
				GLint params[1];
				glGetTexEnviv(target, pname, params);
			
				lua_newtable(L);
				lua::push(L, params[0]);
				lua_rawseti(L, -2, 1);
			} break;
				
			default:
				break;
			
		}
	}
	else {
		luaL_error(L, "gl.GetTexEnv: invalid arguments");
	}
	return 1;
}

/*! Return texture coordinate generation parameters
	OpenGL Documentation: <a target="_blank" href="http://www.opengl.org/sdk/docs/man/xhtml/glGetTexGen.xml">glGetTexGen</a>
	
	@param coord Specifies a texture coordinate. Must be GL_S, GL_T, GL_R, or GL_Q.
					
	@param pname Specifies the symbolic name of the value(s) to be returned. Must be 
					either GL_TEXTURE_GEN_MODE or the name of one of the texture generation 
					plane equations: GL_OBJECT_PLANE or GL_EYE_PLANE.
	
	@ret The requested data values as a table.
					
	@name GetTexGen
*/
int gl_GetTexGen(lua_State *L) {
	if(lua::is<GLenum>(L, 1) && lua::is<GLenum>(L, 2)) {
		GLenum coord = lua::to<GLenum>(L, 1);
		GLenum pname = lua::to<GLenum>(L, 2);
		
		switch(pname) {
			// 4 float
			case GL_OBJECT_PLANE: 
			case GL_EYE_PLANE: {
				GLdouble params[4];
				glGetTexGendv(coord, pname, params);
			
				lua_newtable(L);
				for(int i=0; i < 4; i++) {
					lua::push(L, params[i]);
					lua_rawseti(L, -2, i+1);
				}
			} break;

				
			// 1 int
			case GL_TEXTURE_GEN_MODE: {
				GLint params[1];
				glGetTexGeniv(coord, pname, params);
			
				lua_newtable(L);
				lua::push(L, params[0]);
				lua_rawseti(L, -2, 1);
			} break;
				
			default:
				break;
			
		}
	}
	else {
		luaL_error(L, "gl.GetTexGen: invalid arguments");
	}
	return 1;
}

/*! Return texture parameter values for a specific level of detail
	OpenGL Documentation: <a target="_blank" href="http://www.opengl.org/sdk/docs/man/xhtml/glGetTexLevelParameter.xml">glGetTexLevelParameter</a>
	
	@param target Specifies the symbolic name of the target texture, either GL_TEXTURE_1D, GL_TEXTURE_2D, 
					GL_TEXTURE_3D, GL_PROXY_TEXTURE_1D, GL_PROXY_TEXTURE_2D, GL_PROXY_TEXTURE_3D, 
					GL_TEXTURE_CUBE_MAP_POSITIVE_X, GL_TEXTURE_CUBE_MAP_NEGATIVE_X, 
					GL_TEXTURE_CUBE_MAP_POSITIVE_Y, GL_TEXTURE_CUBE_MAP_NEGATIVE_Y, 
					GL_TEXTURE_CUBE_MAP_POSITIVE_Z, GL_TEXTURE_CUBE_MAP_NEGATIVE_Z, or 
					GL_PROXY_TEXTURE_CUBE_MAP.
					
	@param level Specifies the level-of-detail number of the desired image. Level 0 is the base image level. 
					Level n is the nth mipmap reduction image.
					
	@param pname Specifies the symbolic name of a texture parameter. GL_TEXTURE_WIDTH, GL_TEXTURE_HEIGHT, 
					GL_TEXTURE_DEPTH, GL_TEXTURE_INTERNAL_FORMAT, GL_TEXTURE_BORDER, GL_TEXTURE_RED_SIZE, 
					GL_TEXTURE_GREEN_SIZE, GL_TEXTURE_BLUE_SIZE, GL_TEXTURE_ALPHA_SIZE, 
					GL_TEXTURE_LUMINANCE_SIZE, GL_TEXTURE_INTENSITY_SIZE, GL_TEXTURE_DEPTH_SIZE, 
					GL_TEXTURE_COMPRESSED, and GL_TEXTURE_COMPRESSED_IMAGE_SIZE are accepted.
	
	@ret The requested data values unpacked.
					
	@name GetTexLevelParameter
*/
int gl_GetTexLevelParameter(lua_State *L) {
	if(
		lua::is<GLenum>(L, 1) && 
		lua::is<GLint>(L, 2) && 
		lua::is<GLenum>(L, 3)
	) {
		GLenum target = lua::to<GLenum>(L, 1);
		GLint level = lua::to<GLint>(L, 2);
		GLenum pname = lua::to<GLenum>(L, 3);
		
		GLint params[1];
		glGetTexLevelParameteriv(target, level, pname, params);
		lua::push(L, params[0]);
	}
	else {
		luaL_error(L, "gl.GetTexLevelParameter: invalid arguments");
	}
	return 1;
}

/*! Return texture parameter values
	OpenGL Documentation: <a target="_blank" href="http://www.opengl.org/sdk/docs/man/xhtml/glGetTexParameter.xml">glGetTexParameter</a>
	
	@param target Specifies the symbolic name of the target texture. GL_TEXTURE_1D, GL_TEXTURE_2D, 
					GL_TEXTURE_3D, and GL_TEXTURE_CUBE_MAP are accepted.

	@param pname Specifies the symbolic name of a texture parameter. GL_TEXTURE_MAG_FILTER, 
					GL_TEXTURE_MIN_FILTER, GL_TEXTURE_MIN_LOD, GL_TEXTURE_MAX_LOD, 
					GL_TEXTURE_BASE_LEVEL, GL_TEXTURE_MAX_LEVEL, GL_TEXTURE_WRAP_S, 
					GL_TEXTURE_WRAP_T, GL_TEXTURE_WRAP_R, GL_TEXTURE_BORDER_COLOR, 
					GL_TEXTURE_PRIORITY, GL_TEXTURE_RESIDENT, GL_TEXTURE_COMPARE_MODE, 
					GL_TEXTURE_COMPARE_FUNC, GL_DEPTH_TEXTURE_MODE, and GL_GENERATE_MIPMAP 
					are accepted.
	
	@ret The requested data values as a table.
					
	@name GetTexParameter
*/
int gl_GetTexParameter(lua_State *L) {
	if(
		lua::is<GLenum>(L, 1) && 
		lua::is<GLenum>(L, 2)
	) {
		GLenum target = lua::to<GLenum>(L, 1);
		GLenum pname = lua::to<GLenum>(L, 2);
		
		switch(pname) {
			// 1 int
			case GL_TEXTURE_MAG_FILTER:
			case GL_TEXTURE_MIN_FILTER:
			case GL_TEXTURE_WRAP_S:
			case GL_TEXTURE_WRAP_T:
			case GL_TEXTURE_WRAP_R:
			case GL_TEXTURE_PRIORITY:
			case GL_TEXTURE_RESIDENT:
			case GL_TEXTURE_COMPARE_MODE:
			case GL_TEXTURE_COMPARE_FUNC:
			case GL_DEPTH_TEXTURE_MODE:
			case GL_GENERATE_MIPMAP: {
				GLint params[1];
				glGetTexParameteriv(target, pname, params);
				lua_newtable(L);
				lua::push(L, params[0]);
				lua_rawseti(L, -2, 1);
			} break;
			
			
			// 1 float
			case GL_TEXTURE_MIN_LOD:
			case GL_TEXTURE_MAX_LOD:
			case GL_TEXTURE_BASE_LEVEL:
			case GL_TEXTURE_MAX_LEVEL: {
				GLfloat params[1];
				glGetTexParameterfv(target, pname, params);
				lua_newtable(L);
				lua::push(L, params[0]);
				lua_rawseti(L, -2, 1);
			} break;
			
			// 4 float
			case GL_TEXTURE_BORDER_COLOR: {
				GLfloat params[4];
				glGetTexParameterfv(target, pname, params);
				lua_newtable(L);
				for(int i=0; i < 4; i++) {
					lua::push(L, params[i]);
					lua_rawseti(L, -2, i+1);
				}
			} break;
			
		}
	}
	else {
		luaL_error(L, "gl.GetTexParameter: invalid arguments");
	}
	return 1;
}

/*! Specify implementation-specific hints
	OpenGL Documentation: <a target="_blank" href="http://www.opengl.org/sdk/docs/man/xhtml/glHint.xml">glHint</a>
	
	@param target Specifies a symbolic constant indicating the behavior to be controlled. 
					GL_FOG_HINT, GL_GENERATE_MIPMAP_HINT, GL_LINE_SMOOTH_HINT, 
					GL_PERSPECTIVE_CORRECTION_HINT, GL_POINT_SMOOTH_HINT, 
					GL_POLYGON_SMOOTH_HINT, GL_TEXTURE_COMPRESSION_HINT, and 
					GL_FRAGMENT_SHADER_DERIVATIVE_HINT are accepted.

	@param mode Specifies a symbolic constant indicating the desired behavior. 
				GL_FASTEST, GL_NICEST, and GL_DONT_CARE are accepted.
					
	@name Hint
*/
int gl_Hint(lua_State *L) {
	if(
		lua::is<GLenum>(L, 1) && 
		lua::is<GLenum>(L, 2)
	) {
		GLenum target = lua::to<GLenum>(L, 1);
		GLenum mode = lua::to<GLenum>(L, 2);
		glHint(target, mode);
	}
	else {
		luaL_error(L, "gl.Hint: invalid arguments");
	}
	return 0;
}

/*! Set the current color index
	OpenGL Documentation: <a target="_blank" href="http://www.opengl.org/sdk/docs/man/xhtml/glIndex.xml">glIndex</a>
	
	@param c Specifies the new value for the current color index.
					
	@name Index
*/
int gl_Index(lua_State *L) {
	if(lua::is<GLdouble>(L, 1)) {
		GLdouble c = lua::to<GLdouble>(L, 1);
		glIndexd(c);
	}
	else {
		luaL_error(L, "gl.Index: invalid arguments");
	}
	return 0;
}

/*! Control the writing of individual bits in the color index buffers
	OpenGL Documentation: <a target="_blank" href="http://www.opengl.org/sdk/docs/man/xhtml/glIndex.xml">glIndexMask</a>
	
	@param mask Specifies a bit mask to enable and disable the writing of individual 
				bits in the color index buffers. Initially, the mask is all 1's.
					
	@name IndexMask
*/
int gl_IndexMask(lua_State *L) {
	if(lua::is<GLuint>(L, 1)) {
		GLuint mask = lua::to<GLuint>(L, 1);
		glIndexMask(mask);
	}
	else {
		luaL_error(L, "gl.IndexMask: invalid arguments");
	}
	return 0;
}

/*! Initialize the name stack
	OpenGL Documentation: <a target="_blank" href="http://www.opengl.org/sdk/docs/man/xhtml/glInitNames.xml">glInitNames</a>

	@name InitNames
*/
int gl_InitNames(lua_State *L) {
	glInitNames();
	return 0;
}

/*! Test whether a capability is enabled
	OpenGL Documentation: <a target="_blank" href="http://www.opengl.org/sdk/docs/man/xhtml/glIsEnabled.xml">glIsEnabled</a>
	
	@param cap Specifies a symbolic constant indicating a GL capability.
	@ret Returns GL_TRUE if cap is an enabled capability and returns GL_FALSE otherwise
	
	@name IsEnabled
*/
int gl_IsEnabled(lua_State *L) {
	if(lua::is<GLenum>(L, 1)) {
		GLenum cap = lua::to<GLenum>(L, 1);
		GLboolean res = glIsEnabled(cap);
		lua::push(L, res);
	}
	else {
		luaL_error(L, "gl.IsEnabled: invalid arguments");
	}
	return 1;
}

/*! Determine if a name corresponds to a display list
	OpenGL Documentation: <a target="_blank" href="http://www.opengl.org/sdk/docs/man/xhtml/glIsList.xml">glIsList</a>
	
	@param list Specifies a symbolic constant indicating a GL capability.
	@ret Returns GL_TRUE if list is the name of a display list and returns GL_FALSE 
		if it is not, or if an error occurs.
	
	@name IsList
*/
int gl_IsList(lua_State *L) {
	if(lua::is<GLuint>(L, 1)) {
		GLuint listv = lua::to<GLuint>(L, 1);
		GLboolean res = glIsList(listv);
		lua::push(L, res);
	}
	else {
		luaL_error(L, "gl.IsList: invalid arguments");
	}
	return 1;
}

/*! Determine if a name corresponds to a texture
	OpenGL Documentation: <a target="_blank" href="http://www.opengl.org/sdk/docs/man/xhtml/glIsTexture.xml">glIsTexture</a>
	
	@param texture Specifies a value that may be the name of a texture.
	@ret Returns GL_TRUE if texture is currently the name of a texture. If texture is 
			zero, or is a non-zero value that is not currently the name of a texture, 
			or if an error occurs, glIsTexture returns GL_FALSE.
	
	@name IsTexture
*/
int gl_IsTexture(lua_State *L) {
	if(lua::is<GLuint>(L, 1)) {
		GLuint texture = lua::to<GLuint>(L, 1);
		GLboolean res = glIsTexture(texture);
		lua::push(L, res);
	}
	else {
		luaL_error(L, "gl.IsTexture: invalid arguments");
	}
	return 1;
}

/*! Set light source parameters
	OpenGL Documentation: <a target="_blank" href="http://www.opengl.org/sdk/docs/man/xhtml/glLight.xml">glLight</a>
	
	@param light Specifies a light. The number of lights depends on the implementation, 
				but at least eight lights are supported. They are identified by symbolic 
				names of the form GL_LIGHTi, where i ranges from 0 to the value of 
				GL_MAX_LIGHTS - 1.
				
	@param pname Specifies a light source parameter for light. GL_AMBIENT, GL_DIFFUSE, 
				GL_SPECULAR, GL_POSITION, GL_SPOT_CUTOFF, GL_SPOT_DIRECTION, GL_SPOT_EXPONENT, 
				GL_CONSTANT_ATTENUATION, GL_LINEAR_ATTENUATION, and GL_QUADRATIC_ATTENUATION are 
				accepted.
					
	@param params Specifies the value that parameter pname of light source light will be set to.
	
	@name Light
*/
int gl_Light(lua_State *L) {
	GLfloat params[] = {0, 0, 0, 1};
	if(
		lua::is<GLenum>(L, 1) && 
		lua::is<GLenum>(L, 2) && 
		lua::to_vec<GLfloat>(L, 3, 4, params)
	) {
		GLenum light = lua::to<GLenum>(L, 1);
		GLenum pname = lua::to<GLenum>(L, 2);
		glLightfv(light, pname, params);
	}
	else {
		luaL_error(L, "gl.Light: invalid arguments");
	}
	return 0;
}

/*! Set the lighting model parameters
	OpenGL Documentation: <a target="_blank" href="http://www.opengl.org/sdk/docs/man/xhtml/glLightModel.xml">glLightModel</a>
	
	@param pname Specifies a single-valued lighting model parameter. GL_LIGHT_MODEL_LOCAL_VIEWER, 
					GL_LIGHT_MODEL_COLOR_CONTROL, and GL_LIGHT_MODEL_TWO_SIDE are accepted.
					
	@param params Specifies the value that param will be set to.
	
	@name LightModel
*/
int gl_LightModel(lua_State *L) {
	GLfloat params[] = {0, 0, 0, 1};
	if(
		lua::is<GLenum>(L, 1) && 
		lua::to_vec<GLfloat>(L, 2, 4, params)
	) {
		GLenum pname = lua::to<GLenum>(L, 1);
		glLightModelfv(pname, params);
	}
	else {
		luaL_error(L, "gl.LightModel: invalid arguments");
	}
	return 0;
}

/*! Specify the line stipple pattern
	OpenGL Documentation: <a target="_blank" href="http://www.opengl.org/sdk/docs/man/xhtml/glLineStipple.xml">glLineStipple</a>
	
	@param factor Specifies a multiplier for each bit in the line stipple pattern. If factor is 
					3, for example, each bit in the pattern is used three times before the next 
					bit in the pattern is used. factor is clamped to the range [1, 256] and 
					defaults to 1.
					
	@param pattern Specifies a 16-bit integer whose bit pattern determines which fragments of a 
					line will be drawn when the line is rasterized. Bit zero is used first; the 
					default pattern is all 1's.
	
	@name LineStipple
*/
int gl_LineStipple(lua_State *L) {
	if(lua::is<GLint>(L, 1) && lua::is<GLushort>(L, 2)) {
		GLint factor = lua::to<GLint>(L, 1);
		GLushort pattern = lua::to<GLushort>(L, 2);
		glLineStipple(factor, pattern);
	}
	else {
		luaL_error(L, "gl.LineStipple: invalid arguments");
	}
	return 0;
}

/*! Specify the width of rasterized lines
	OpenGL Documentation: <a target="_blank" href="http://www.opengl.org/sdk/docs/man/xhtml/glLineWidth.xml">glLineWidth</a>
	
	@param width Specifies the width of rasterized lines. The initial value is 1.
	
	@name LineWidth
*/
int gl_LineWidth(lua_State *L) {
	if(lua::is<GLfloat>(L, 1)) {
		GLfloat width = lua::to<GLfloat>(L, 1);
		glLineWidth(width);
	}
	else {
		luaL_error(L, "gl.LineWidth: invalid arguments");
	}
	return 0;
}

/*! Set the display-list base for glCallLists
	OpenGL Documentation: <a target="_blank" href="http://www.opengl.org/sdk/docs/man/xhtml/glListBase.xml">glListBase</a>
	
	@param base Specifies an integer offset that will be added to glCallLists offsets 
				to generate display-list names. The initial value is 0.
	
	@name ListBase
*/
int gl_ListBase(lua_State *L) {
	if(lua::is<GLuint>(L, 1)) {
		GLuint base = lua::to<GLuint>(L, 1);
		glListBase(base);
	}
	else {
		luaL_error(L, "gl.ListBase: invalid arguments");
	}
	return 0;
}

/*! Replace the current matrix with the identity matrix
	OpenGL Documentation: <a target="_blank" href="http://www.opengl.org/sdk/docs/man/xhtml/glLoadIdentity.xml">glLoadIdentity</a>

	@name LoadIdentity
*/
int gl_LoadIdentity(lua_State *L) {
	glLoadIdentity();
	return 0;
}

/*! Replace the current matrix with the specified matrix
	OpenGL Documentation: <a target="_blank" href="http://www.opengl.org/sdk/docs/man/xhtml/glLoadMatrix.xml">glLoadMatrix</a>
	
	@param m Specifies 16 consecutive values, which are used as the elements of a 
				4×4 column-major matrix.
	
	@name LoadMatrix
*/
int gl_LoadMatrix(lua_State *L) {
	GLdouble m[] = {
		1, 0, 0, 0, 
		0, 1, 0, 0, 
		0, 0, 1, 0, 
		0, 0, 0, 1
	};
	if(lua::to_vec<GLdouble>(L, 1, 16, m)) {
		glLoadMatrixd(m);
	}
	else {
		luaL_error(L, "gl.LoadMatrix: invalid arguments");
	}
	return 0;
}

/*! Load a name onto the name stack
	OpenGL Documentation: <a target="_blank" href="http://www.opengl.org/sdk/docs/man/xhtml/glLoadName.xml">glLoadName</a>
	
	@param name Specifies a name that will replace the top value on the name stack.
	
	@name LoadName
*/
int gl_LoadName(lua_State *L) {
	if(lua::is<GLuint>(L, 1)) {
		GLuint name = lua::to<GLuint>(L, 1);
		glLoadName(name);
	}
	else {
		luaL_error(L, "gl.LoadName: invalid arguments");
	}
	return 0;
}

/*! Specify a logical pixel operation for color index rendering
	OpenGL Documentation: <a target="_blank" href="http://www.opengl.org/sdk/docs/man/xhtml/glLogicOp.xml">glLogicOp</a>
	
	@param opcode Specifies a symbolic constant that selects a logical operation. 
					The following symbols are accepted: GL_CLEAR, GL_SET, GL_COPY, 
					GL_COPY_INVERTED, GL_NOOP, GL_INVERT, GL_AND, GL_NAND, GL_OR, 
					GL_NOR, GL_XOR, GL_EQUIV, GL_AND_REVERSE, GL_AND_INVERTED, 
					GL_OR_REVERSE, and GL_OR_INVERTED. The initial value is GL_COPY.
	
	@name LogicOp
*/
int gl_LogicOp(lua_State *L) {
	if(lua::is<GLenum>(L, 1)) {
		GLenum opcode = lua::to<GLenum>(L, 1);
		glLogicOp(opcode);
	}
	else {
		luaL_error(L, "gl.LogicOp: invalid arguments");
	}
	return 0;
}

/*! Specify material parameters for the lighting model
	OpenGL Documentation: <a target="_blank" href="http://www.opengl.org/sdk/docs/man/xhtml/glMaterial.xml">glMaterial</a>
	
	@param face Specifies which face or faces are being updated. Must be one of GL_FRONT, 
				GL_BACK, or GL_FRONT_AND_BACK.
				
	@param pname Specifies the material parameter of the face or faces that is being updated. Must 
					be one of GL_AMBIENT, GL_DIFFUSE, GL_SPECULAR, GL_EMISSION, GL_SHININESS, 
					GL_AMBIENT_AND_DIFFUSE, or GL_COLOR_INDEXES.
					
	@param params Specifies a pointer to the value or values that pname will be set to.
	
	@name Material
*/
int gl_Material(lua_State *L) {
	GLfloat params[] = {0, 0, 0, 1};
	if(
		lua::is<GLenum>(L, 1) && 
		lua::is<GLenum>(L, 2) && 
		lua::to_vec<GLfloat>(L, 3, 4, params)
	) {
		GLenum face = lua::to<GLenum>(L, 1);
		GLenum pname = lua::to<GLenum>(L, 2);
		glMaterialfv(face, pname, params);
	}
	else {
		luaL_error(L, "gl.Material: invalid arguments");
	}
	return 0;
}

/*! Specify which matrix is the current matrix
	OpenGL Documentation: <a target="_blank" href="http://www.opengl.org/sdk/docs/man/xhtml/glMatrixMode.xml">glMatrixMode</a>
	
	@param mode Specifies which matrix stack is the target for subsequent 
				matrix operations. Three values are accepted: GL_MODELVIEW, 
				GL_PROJECTION, and GL_TEXTURE. The initial value is GL_MODELVIEW. 
				Additionally, if the ARB_imaging extension is supported, GL_COLOR 
				is also accepted.
	
	@name MatrixMode
*/
int gl_MatrixMode(lua_State *L) {
	if(lua::is<GLenum>(L, 1)) {
		GLenum mode = lua::to<GLenum>(L, 1);
		glMatrixMode(mode);
	}
	else {
		luaL_error(L, "gl.MatrixMode: invalid arguments");
	}
	return 0;
}

/*! Multiply the current matrix with the specified matrix
	OpenGL Documentation: <a target="_blank" href="http://www.opengl.org/sdk/docs/man/xhtml/glMultMatrix.xml">glMultMatrix</a>
	
	@param m Specifies 16 consecutive values, which are used as the elements of a 
				4×4 column-major matrix.
	
	@name MultMatrix
*/
int gl_MultMatrix(lua_State *L) {
	GLdouble m[] = {
		1, 0, 0, 0, 
		0, 1, 0, 0, 
		0, 0, 1, 0, 
		0, 0, 0, 1
	};
	if(lua::to_vec<GLdouble>(L, 1, 16, m)) {
		glMultMatrixd(m);
	}
	else {
		luaL_error(L, "gl.MultMatrix: invalid arguments");
	}
	return 0;
}

/*! Create or replace a display list
	OpenGL Documentation: <a target="_blank" href="http://www.opengl.org/sdk/docs/man/xhtml/glNewList.xml">glNewList</a>
	
	@param list Specifies the display-list name.
	@param mode Specifies the compilation mode, which can be GL_COMPILE or GL_COMPILE_AND_EXECUTE.
	
	@name NewList
*/
int gl_NewList(lua_State *L) {
	if(lua::is<GLuint>(L, 1) && lua::is<GLenum>(L, 2)) {
		GLuint listv = lua::to<GLuint>(L, 1);
		GLenum mode = lua::to<GLenum>(L, 2);
		glNewList(listv, mode);
	}
	else {
		luaL_error(L, "gl.NewList: invalid arguments");
	}
	return 0;
}

/*! Set the current normal vector
	OpenGL Documentation: <a target="_blank" href="http://www.opengl.org/sdk/docs/man/xhtml/glNormal.xml">glNormal</a>
	
	@param ... Either a 3 numbers or a table specifying the x, y, and z coordinates 
				of the new current normal. The initial value of the current normal 
				is the unit vector, (0, 0, 1).
	
	@name Normal
*/
int gl_Normal(lua_State *L) {
	GLdouble v[] = {0, 0, 0};
	if(lua::to_vec<GLdouble>(L, 1, 3, v)) {
		glNormal3dv(v);
	}
	else {
		luaL_error(L, "gl.Normal: invalid arguments");
	}
	return 0;
}

/*! Multiply the current matrix with an orthographic matrix
	OpenGL Documentation: <a target="_blank" href="http://www.opengl.org/sdk/docs/man/xhtml/glOrtho.xml">glOrtho</a>
	
	@param left Specify the coordinates for the left vertical clipping plane.
	@param right Specify the coordinates for the right vertical clipping plane.
	@param bottom Specify the coordinates for the bottom horizontal clipping plane.
	@param top Specify the coordinates for the top horizontal clipping plane.
	@param near Specify the distances to the nearer depth clipping plane. 
				This value is negative if the plane is to be behind the viewer.
	@param far Specify the distances to the farther depth clipping plane. 
				This value is negative if the plane is to be behind the viewer.
	
	@name Ortho
*/
int gl_Ortho(lua_State *L) {
	if(
		lua::is<GLdouble>(L, 1) && 
		lua::is<GLdouble>(L, 2) && 
		lua::is<GLdouble>(L, 3) && 
		lua::is<GLdouble>(L, 4) && 
		lua::is<GLdouble>(L, 5) && 
		lua::is<GLdouble>(L, 6)
	) {
		GLdouble left = lua::to<GLdouble>(L, 1);
		GLdouble right = lua::to<GLdouble>(L, 2);
		GLdouble bottom = lua::to<GLdouble>(L, 3);
		GLdouble top = lua::to<GLdouble>(L, 4);
		GLdouble near = lua::to<GLdouble>(L, 5);
		GLdouble far = lua::to<GLdouble>(L, 6);
		glOrtho(left, right, bottom, top, near, far);
	}
	else {
		luaL_error(L, "gl.Ortho: invalid arguments");
	}
	return 0;
}

/*! Place a marker in the feedback buffer
	OpenGL Documentation: <a target="_blank" href="http://www.opengl.org/sdk/docs/man/xhtml/glPassThrough.xml">glPassThrough</a>
	
	@param token Specifies a marker value to be placed in the feedback buffer 
					following a GL_PASS_THROUGH_TOKEN.
	
	@name PassThrough
*/
int gl_PassThrough(lua_State *L) {
	if(lua::is<GLfloat>(L, 1)) {
		GLfloat token = lua::to<GLfloat>(L, 1);
		glPassThrough(token);
	}
	else {
		luaL_error(L, "gl.PassThrough: invalid arguments");
	}
	return 0;
}

/*! Set pixel storage modes
	OpenGL Documentation: <a target="_blank" href="http://www.opengl.org/sdk/docs/man/xhtml/glPixelStore.xml">glPixelStore</a>
	
	@param pname Specifies the symbolic name of the parameter to be set. Six values affect 
					the packing of pixel data into memory: GL_PACK_SWAP_BYTES, GL_PACK_LSB_FIRST, 
					GL_PACK_ROW_LENGTH, GL_PACK_IMAGE_HEIGHT, GL_PACK_SKIP_PIXELS, GL_PACK_SKIP_ROWS, 
					GL_PACK_SKIP_IMAGES, and GL_PACK_ALIGNMENT. Six more affect the unpacking of 
					pixel data from memory: GL_UNPACK_SWAP_BYTES, GL_UNPACK_LSB_FIRST, 
					GL_UNPACK_ROW_LENGTH, GL_UNPACK_IMAGE_HEIGHT, GL_UNPACK_SKIP_PIXELS, 
					GL_UNPACK_SKIP_ROWS, GL_UNPACK_SKIP_IMAGES, and GL_UNPACK_ALIGNMENT.

	@param param Specifies the value that pname is set to.
	
	@name PixelStore
*/
int gl_PixelStore(lua_State *L) {
	if(lua::is<GLenum>(L, 1) && lua::is<GLfloat>(L, 2)) {
		GLenum pname = lua::to<GLenum>(L, 1);
		GLfloat param = lua::to<GLfloat>(L, 2);
		glPixelStoref(pname, param);
	}
	else {
		luaL_error(L, "gl.PixelStore: invalid arguments");
	}
	return 0;
}

/*! Set pixel transfer modes
	OpenGL Documentation: <a target="_blank" href="http://www.opengl.org/sdk/docs/man/xhtml/glPixelTransfer.xml">glPixelTransfer</a>
	
	@param pname Specifies the symbolic name of the pixel transfer parameter to be set. Must be 
					one of the following: GL_MAP_COLOR, GL_MAP_STENCIL, GL_INDEX_SHIFT, 
					GL_INDEX_OFFSET, GL_RED_SCALE, GL_RED_BIAS, GL_GREEN_SCALE, GL_GREEN_BIAS, 
					GL_BLUE_SCALE, GL_BLUE_BIAS, GL_ALPHA_SCALE, GL_ALPHA_BIAS, GL_DEPTH_SCALE, 
					or GL_DEPTH_BIAS.

	@param param Specifies the value that pname is set to.
	
	@name PixelTransfer
*/
int gl_PixelTransfer(lua_State *L) {
	if(lua::is<GLenum>(L, 1) && lua::is<GLfloat>(L, 2)) {
		GLenum pname = lua::to<GLenum>(L, 1);
		GLfloat param = lua::to<GLfloat>(L, 2);
		glPixelTransferf(pname, param);
	}
	else {
		luaL_error(L, "gl.PixelTransfer: invalid arguments");
	}
	return 0;
}

/*! Specify the pixel zoom factors
	OpenGL Documentation: <a target="_blank" href="http://www.opengl.org/sdk/docs/man/xhtml/glPixelZoom.xml">glPixelZoom</a>
	
	@param xfactor Specify the x zoom factor for pixel write operations.
	@param yfactor Specify the y zoom factor for pixel write operations.
	
	@name PixelZoom
*/
int gl_PixelZoom(lua_State *L) {
	if(lua::is<GLfloat>(L, 1) && lua::is<GLfloat>(L, 2)) {
		GLfloat xfactor = lua::to<GLfloat>(L, 1);
		GLfloat yfactor = lua::to<GLfloat>(L, 2);
		glPixelZoom(xfactor, yfactor);
	}
	else {
		luaL_error(L, "gl.PixelZoom: invalid arguments");
	}
	return 0;
}

/*! Specify the diameter of rasterized points
	OpenGL Documentation: <a target="_blank" href="http://www.opengl.org/sdk/docs/man/xhtml/glPointSize.xml">glPointSize</a>
	
	@param size Specifies the diameter of rasterized points. The initial value is 1.
	
	@name PointSize
*/
int gl_PointSize(lua_State *L) {
	if(lua::is<GLfloat>(L, 1)) {
		GLfloat size = lua::to<GLfloat>(L, 1);
		glPointSize(size);
	}
	else {
		luaL_error(L, "gl.PointSize: invalid arguments");
	}
	return 0;
}

/*! Select a polygon rasterization mode
	OpenGL Documentation: <a target="_blank" href="http://www.opengl.org/sdk/docs/man/xhtml/glPolygonMode.xml">glPolygonMode</a>
	
	@param face Specifies the polygons that mode applies to. Must be GL_FRONT for front-facing polygons, 
				GL_BACK for back-facing polygons, or GL_FRONT_AND_BACK for front- and back-facing polygons.
	@param mode Specifies how polygons will be rasterized. Accepted values are GL_POINT, GL_LINE, and GL_FILL. 
				The initial value is GL_FILL for both front- and back-facing polygons.
	
	@name PolygonMode
*/
int gl_PolygonMode(lua_State *L) {
	if(lua::is<GLenum>(L, 1) && lua::is<GLenum>(L, 2)) {
		GLenum face = lua::to<GLenum>(L, 1);
		GLenum mode = lua::to<GLenum>(L, 2);
		glPolygonMode(face, mode);
	}
	else {
		luaL_error(L, "gl.PolygonMode: invalid arguments");
	}
	return 0;
}

/*! Set the scale and units used to calculate depth values
	OpenGL Documentation: <a target="_blank" href="http://www.opengl.org/sdk/docs/man/xhtml/glPolygonOffset.xml">glPolygonOffset</a>
	
	@param factor Specifies a scale factor that is used to create a variable depth offset for each 
					polygon. The initial value is 0.
	@param units Is multiplied by an implementation-specific value to create a constant depth 
					offset. The initial value is 0.
	
	@name PolygonOffset
*/
int gl_PolygonOffset(lua_State *L) {
	if(lua::is<GLfloat>(L, 1) && lua::is<GLfloat>(L, 2)) {
		GLfloat factor = lua::to<GLfloat>(L, 1);
		GLfloat units = lua::to<GLfloat>(L, 2);
		glPolygonOffset(factor, units);
	}
	else {
		luaL_error(L, "gl.PolygonOffset: invalid arguments");
	}
	return 0;
}

/*! Pop the server attribute stack
	OpenGL Documentation: <a target="_blank" href="http://www.opengl.org/sdk/docs/man/xhtml/glPushAttrib.xml">glPopAttrib</a>

	@name PopAttrib
*/
int gl_PopAttrib(lua_State *L) {
	glPopAttrib();
	return 0;
}

/*! Pop the client attribute stack
	OpenGL Documentation: <a target="_blank" href="http://www.opengl.org/sdk/docs/man/xhtml/glPushClientAttrib.xml">glPopClientAttrib</a>

	@name PopClientAttrib
*/
int gl_PopClientAttrib(lua_State *L) {
	glPopClientAttrib();
	return 0;
}

/*! Pop the current matrix stack
	OpenGL Documentation: <a target="_blank" href="http://www.opengl.org/sdk/docs/man/xhtml/glPushMatrix.xml">glPopMatrix</a>

	@name PopMatrix
*/
int gl_PopMatrix(lua_State *L) {
	glPopMatrix();
	return 0;
}

/*! Pop the name stack
	OpenGL Documentation: <a target="_blank" href="http://www.opengl.org/sdk/docs/man/xhtml/glPushName.xml">glPopName</a>

	@name PopName
*/
int gl_PopName(lua_State *L) {
	glPopName();
	return 0;
}

/*! Push the server attribute stack
	OpenGL Documentation: <a target="_blank" href="http://www.opengl.org/sdk/docs/man/xhtml/glPushAttrib.xml">glPushAttrib</a>

	@param mask Specifies a mask that indicates which attributes to save.

	@name PushAttrib
*/
int gl_PushAttrib(lua_State *L) {
	if(lua::is<GLbitfield>(L, 1)) {
		GLbitfield mask = lua::to<GLbitfield>(L, 1);
		glPushAttrib(mask);
	}
	else {
		luaL_error(L, "gl.PushAttrib: invalid arguments");
	}
	return 0;
}

/*! Push the client attribute stack
	OpenGL Documentation: <a target="_blank" href="http://www.opengl.org/sdk/docs/man/xhtml/glPushClientAttrib.xml">glPushClientAttrib</a>

	@name PushClientAttrib
*/
int gl_PushClientAttrib(lua_State *L) {
	if(lua::is<GLbitfield>(L, 1)) {
		GLbitfield mask = lua::to<GLbitfield>(L, 1);
		glPushClientAttrib(mask);
	}
	else {
		luaL_error(L, "gl.PushClientAttrib: invalid arguments");
	}
	return 0;
}

/*! Push the current matrix stack
	OpenGL Documentation: <a target="_blank" href="http://www.opengl.org/sdk/docs/man/xhtml/glPushMatrix.xml">glPushMatrix</a>

	@name PushMatrix
*/
int gl_PushMatrix(lua_State *L) {
	glPushMatrix();
	return 0;
}

/*! Push the name stack
	OpenGL Documentation: <a target="_blank" href="http://www.opengl.org/sdk/docs/man/xhtml/glPushName.xml">glPushName</a>
	
	@param name Specifies a name that will be pushed onto the name stack.

	@name PushName
*/
int gl_PushName(lua_State *L) {
	if(lua::is<GLuint>(L, 1)) {
		GLuint name = lua::to<GLuint>(L, 1);
		glPushName(name);
	}
	else {
		luaL_error(L, "gl.PushName: invalid arguments");
	}
	return 0;
}

/*! Specify the raster position for pixel operations
	OpenGL Documentation: <a target="_blank" href="http://www.opengl.org/sdk/docs/man/xhtml/glRasterPos.xml">glRasterPos</a>
	
	@param .. Specifies an array or list of values of two, three, or four elements, 
				specifying x, y, z, and w coordinates, respectively.

	@name RasterPos
*/
int gl_RasterPos(lua_State *L) {
	GLdouble v[] = {0, 0, 0, 1};
	int n = 4;
	if(lua::to_vec_n<GLdouble>(L, 1, n, v)) {
		switch(n) {
			case 1:
			case 2: glRasterPos2dv(v); break;
			case 3: glRasterPos3dv(v); break;
			case 4:
			default:
				 glRasterPos4dv(v);
				 break;
		}
		
	}
	else {
		luaL_error(L, "gl.RasterPos: invalid arguments");
	}
	return 0;
}

/*! Select a color buffer source for pixels
	OpenGL Documentation: <a target="_blank" href="http://www.opengl.org/sdk/docs/man/xhtml/glReadBuffer.xml">glReadBuffer</a>
	
	@param mode Specifies a color buffer. Accepted values are GL_FRONT_LEFT, GL_FRONT_RIGHT, 
				GL_BACK_LEFT, GL_BACK_RIGHT, GL_FRONT, GL_BACK, GL_LEFT, GL_RIGHT, and 
				GL_AUXi, where i is between 0 and the value of GL_AUX_BUFFERS minus 1.

	@name ReadBuffer
*/
int gl_ReadBuffer(lua_State *L) {
	if(lua::is<GLenum>(L, 1)) {
		GLuint mode = lua::to<GLenum>(L, 1);
		glReadBuffer(mode);
	}
	else {
		luaL_error(L, "gl.ReadBuffer: invalid arguments");
	}
	return 0;
}

/*! Draw a rectangle
	OpenGL Documentation: <a target="_blank" href="http://www.opengl.org/sdk/docs/man/xhtml/glRect.xml">glRect</a>
	
	@param ... Specify the vertices of a rectangle as an array or list of 4 values.

	@name Rect
*/
int gl_Rect(lua_State *L) {
	GLdouble v[] = {0, 0, 0, 1};
	if(lua::to_vec<GLdouble>(L, 1, 4, v)) {
		glRectdv(v, v+2);
	}
	else {
		luaL_error(L, "gl.Rect: invalid arguments");
	}
	return 0;
}

/*! Set rasterization mode
	OpenGL Documentation: <a target="_blank" href="http://www.opengl.org/sdk/docs/man/xhtml/glRenderMode.xml">glRenderMode</a>
	
	@param mode Specifies the rasterization mode. Three values are accepted: GL_RENDER, 
				GL_SELECT, and GL_FEEDBACK. The initial value is GL_RENDER.

	@name RenderMode
*/
int gl_RenderMode(lua_State *L) {
	if(lua::is<GLenum>(L, 1)) {
		GLuint mode = lua::to<GLenum>(L, 1);
		glRenderMode(mode);
	}
	else {
		luaL_error(L, "gl.RenderMode: invalid arguments");
	}
	return 0;
}

/*! Multiply the current matrix by a rotation matrix
	OpenGL Documentation: <a target="_blank" href="http://www.opengl.org/sdk/docs/man/xhtml/glRotate.xml">glRotate</a>
	
	@param ... Specify an array or list of 4 values with the order being 
				the angle of rotation in degrees and then the the x, y, and z 
				coordinates of a vector, respectively

	@name Rotate
*/
int gl_Rotate(lua_State *L) {
	GLdouble v[] = {0, 0, 0, 1};
	if(lua::to_vec<GLdouble>(L, 1, 4, v)) {
		glRotated(v[0], v[1], v[2], v[3]);
	}
	else {
		luaL_error(L, "gl.Rotate: invalid arguments");
	}
	return 0;
}

/*! Multiply the current matrix by a general scaling matrix
	OpenGL Documentation: <a target="_blank" href="http://www.opengl.org/sdk/docs/man/xhtml/glScale.xml">glScale</a>
	
	@param ... Specify an array or list of 3 values of scale factors along 
				the x, y, and z axes, respectively

	@name Scale
*/
int gl_Scale(lua_State *L) {
	GLdouble v[] = {1, 1, 1};
	if(lua::to_vec<GLdouble>(L, 1, 3, v)) {
		glScaled(v[0], v[1], v[2]);
	}
	else {
		luaL_error(L, "gl.Scale: invalid arguments");
	}
	return 0;
}

/*! Define the scissor box
	OpenGL Documentation: <a target="_blank" href="http://www.opengl.org/sdk/docs/man/xhtml/glScissor.xml">glScissor</a>
	
	@param x Specify the x-coordinate of the lower left corner of the scissor box.
	@param y Specify the y-coordinate of the lower left corner of the scissor box.
	@param width Specify the width of the scissor box.
	@param height Specify the height of the scissor box.

	@name Scissor
*/
int gl_Scissor(lua_State *L) {
	if(
		lua::is<GLint>(L, 1) && 
		lua::is<GLint>(L, 2) && 
		lua::is<GLsizei>(L, 3) && 
		lua::is<GLsizei>(L, 4)
	) {
		GLint x = lua::to<GLint>(L, 1);
		GLint y = lua::to<GLint>(L, 2);
		GLsizei width = lua::to<GLsizei>(L, 3);
		GLsizei height = lua::to<GLsizei>(L, 4);
		glScissor(x, y, width, height);
	}
	else {
		luaL_error(L, "gl.Scissor: invalid arguments");
	}
	return 0;
}

/*! Select flat or smooth shading
	OpenGL Documentation: <a target="_blank" href="http://www.opengl.org/sdk/docs/man/xhtml/glShadeModel.xml">glShadeModel</a>
	
	@param mode Specifies a symbolic value representing a shading technique. Accepted 
				values are GL_FLAT and GL_SMOOTH. The initial value is GL_SMOOTH.

	@name ShadeModel
*/
int gl_ShadeModel(lua_State *L) {
	if(lua::is<GLenum>(L, 1)) {
		GLenum mode = lua::to<GLenum>(L, 1);
		glShadeModel(mode);
	}
	else {
		luaL_error(L, "gl.ShadeModel: invalid arguments");
	}
	return 0;
}

/*! Set front and back function and reference value for stencil testing
	OpenGL Documentation: <a target="_blank" href="http://www.opengl.org/sdk/docs/man/xhtml/glStencilFunc.xml">glStencilFunc</a>
	
	@param func Specifies the test function. Eight symbolic constants are valid: GL_NEVER, 
				GL_LESS, GL_LEQUAL, GL_GREATER, GL_GEQUAL, GL_EQUAL, GL_NOTEQUAL, and GL_ALWAYS. 
				The initial value is GL_ALWAYS.
				
	@param ref Specifies the reference value for the stencil test. ref is clamped to the range 0 
				2 n-1 , where n is the number of bitplanes in the stencil buffer. The initial value is 0.
				
	@param mask Specifies a mask that is ANDed with both the reference value and the stored stencil 
				value when the test is done. The initial value is all 1's.

	@name StencilFunc
*/
int gl_StencilFunc(lua_State *L) {
	if(
		lua::is<GLenum>(L, 1) &&
		lua::is<GLint>(L, 2) &&
		lua::is<GLuint>(L, 3)
	) {
		GLenum func = lua::to<GLenum>(L, 1);
		GLint ref = lua::to<GLint>(L, 2);
		GLuint mask = lua::to<GLuint>(L, 3);
		glStencilFunc(func, ref, mask);
	}
	else {
		luaL_error(L, "gl.StencilFunc: invalid arguments");
	}
	return 0;
}

/*! Control the front and back writing of individual bits in the stencil planes
	OpenGL Documentation: <a target="_blank" href="http://www.opengl.org/sdk/docs/man/xhtml/glStencilMask.xml">glStencilMask</a>
	
	@param mask Specifies a bit mask to enable and disable writing of individual bits in the 
				stencil planes. Initially, the mask is all 1's.

	@name StencilMask
*/
int gl_StencilMask(lua_State *L) {
	if(lua::is<GLuint>(L, 1)) {
		GLuint mask = lua::to<GLuint>(L, 1);
		glStencilMask(mask);
	}
	else {
		luaL_error(L, "gl.StencilMask: invalid arguments");
	}
	return 0;
}

/*! Set front and back stencil test actions
	OpenGL Documentation: <a target="_blank" href="http://www.opengl.org/sdk/docs/man/xhtml/glStencilOp.xml">glStencilOp</a>
	
	@param sfail Specifies the action to take when the stencil test fails. Eight symbolic 
					constants are accepted: GL_KEEP, GL_ZERO, GL_REPLACE, GL_INCR, GL_INCR_WRAP, 
					GL_DECR, GL_DECR_WRAP, and GL_INVERT. The initial value is GL_KEEP.
				
	@param dpfail Specifies the stencil action when the stencil test passes, but the depth test 
					fails. dpfail accepts the same symbolic constants as sfail. The initial 
					value is GL_KEEP.
				
	@param dppass Specifies the stencil action when both the stencil test and the depth test pass, 
					or when the stencil test passes and either there is no depth buffer or depth 
					testing is not enabled. dppass accepts the same symbolic constants as sfail. 
					The initial value is GL_KEEP.

	@name StencilOp
*/
int gl_StencilOp(lua_State *L) {
	if(
		lua::is<GLenum>(L, 1) &&
		lua::is<GLenum>(L, 2) &&
		lua::is<GLenum>(L, 3)
	) {
		GLenum sfail = lua::to<GLenum>(L, 1);
		GLenum dpfail = lua::to<GLenum>(L, 2);
		GLenum dppass = lua::to<GLenum>(L, 3);
		glStencilOp(sfail, dpfail, dppass);
	}
	else {
		luaL_error(L, "gl.StencilOp: invalid arguments");
	}
	return 0;
}

/*! Set the current texture coordinates
	OpenGL Documentation: <a target="_blank" href="http://www.opengl.org/sdk/docs/man/xhtml/glTexCoord.xml">glTexCoord</a>
	
	@param .. Specifies an array or list of one, two, three, or four elements, 
				which in turn specify the s, t, r, and q texture coordinates.

	@name TexCoord
*/
int gl_TexCoord(lua_State *L) {
	GLdouble v[] = {0, 0, 0, 1};
	int n = 4;
	if(lua::to_vec_n<GLdouble>(L, 1, n, v)) {
		switch(n) {
			case 1: glTexCoord1dv(v); break;
			case 2: glTexCoord2dv(v); break;
			case 3: glTexCoord3dv(v); break;
			case 4:
			default:
				 glTexCoord4dv(v);
				 break;
		}
		
	}
	else {
		luaL_error(L, "gl.TexCoord: invalid arguments");
	}
	return 0;
}

/*! Set texture environment parameters
	OpenGL Documentation: <a target="_blank" href="http://www.opengl.org/sdk/docs/man/xhtml/glTexEnv.xml">glTexEnv</a>
	
	@param target Specifies a texture environment. May be GL_TEXTURE_ENV, 
					GL_TEXTURE_FILTER_CONTROL or GL_POINT_SPRITE.
				
	@param pname Specifies the symbolic name of a single-valued texture environment parameter. 
					May be either GL_TEXTURE_ENV_MODE, GL_TEXTURE_LOD_BIAS, GL_COMBINE_RGB, 
					GL_COMBINE_ALPHA, GL_SRC0_RGB, GL_SRC1_RGB, GL_SRC2_RGB, GL_SRC0_ALPHA, 
					GL_SRC1_ALPHA, GL_SRC2_ALPHA, GL_OPERAND0_RGB, GL_OPERAND1_RGB, 
					GL_OPERAND2_RGB, GL_OPERAND0_ALPHA, GL_OPERAND1_ALPHA, GL_OPERAND2_ALPHA, 
					GL_RGB_SCALE, GL_ALPHA_SCALE, or GL_COORD_REPLACE.
				
	@param param Specifies a single symbolic constant, one of GL_ADD, GL_ADD_SIGNED, GL_INTERPOLATE, 
					GL_MODULATE, GL_DECAL, GL_BLEND, GL_REPLACE, GL_SUBTRACT, GL_COMBINE, GL_TEXTURE, 
					GL_CONSTANT, GL_PRIMARY_COLOR, GL_PREVIOUS, GL_SRC_COLOR, GL_ONE_MINUS_SRC_COLOR, 
					GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA, a single boolean value for the point sprite 
					texture coordinate replacement, a single floating-point value for the texture 
					level-of-detail bias, or 1.0, 2.0, or 4.0 when specifying the GL_RGB_SCALE or 
					GL_ALPHA_SCALE.

	@name TexEnv
*/
int gl_TexEnv(lua_State *L) {
	if(
		lua::is<GLenum>(L, 1) &&
		lua::is<GLenum>(L, 2) &&
		lua::is<GLfloat>(L, 3)
	) {
		GLenum target = lua::to<GLenum>(L, 1);
		GLenum pname = lua::to<GLenum>(L, 2);
		GLfloat param = lua::to<GLfloat>(L, 3);
		glTexEnvf(target, pname, param);
	}
	else {
		luaL_error(L, "gl.TexEnv: invalid arguments");
	}
	return 0;
}

/*! Control the generation of texture coordinates
	OpenGL Documentation: <a target="_blank" href="http://www.opengl.org/sdk/docs/man/xhtml/glTexGen.xml">glTexGen</a>
	
	@param coord Specifies a texture coordinate. Must be one of GL_S, GL_T, GL_R, or GL_Q.
				
	@param pname Specifies the symbolic name of the texture-coordinate generation function. 
					Must be GL_TEXTURE_GEN_MODE.
					
	@param param Specifies a single-valued texture generation parameter, one of GL_OBJECT_LINEAR, 
					GL_EYE_LINEAR, GL_SPHERE_MAP, GL_NORMAL_MAP, or GL_REFLECTION_MAP.
	@name TexGen
*/
int gl_TexGen(lua_State *L) {
	GLdouble param[] = {0, 0, 0, 1};
	if(
		lua::is<GLenum>(L, 1) &&
		lua::is<GLenum>(L, 2) &&
		lua::to_vec<GLdouble>(L, 3, 4, param)
	) {
		GLenum target = lua::to<GLenum>(L, 1);
		GLenum pname = lua::to<GLenum>(L, 2);
		glTexGendv(target, pname, param);
	}
	else {
		luaL_error(L, "gl.TexEnv: invalid arguments");
	}
	return 0;
}

/*! Set texture parameters
	OpenGL Documentation: <a target="_blank" href="http://www.opengl.org/sdk/docs/man/xhtml/glTexParameter.xml">glTexParameter</a>
	
	@param target Specifies the target texture, which must be either GL_TEXTURE_1D, GL_TEXTURE_2D, 
					GL_TEXTURE_3D, or GL_TEXTURE_CUBE_MAP.
				
	@param pname Specifies the symbolic name of a single-valued texture parameter. pname can be one 
					of the following: GL_TEXTURE_MIN_FILTER, GL_TEXTURE_MAG_FILTER, GL_TEXTURE_MIN_LOD, 
					GL_TEXTURE_MAX_LOD, GL_TEXTURE_BASE_LEVEL, GL_TEXTURE_MAX_LEVEL, GL_TEXTURE_WRAP_S, 
					GL_TEXTURE_WRAP_T, GL_TEXTURE_WRAP_R, GL_TEXTURE_PRIORITY, GL_TEXTURE_COMPARE_MODE, 
					GL_TEXTURE_COMPARE_FUNC, GL_DEPTH_TEXTURE_MODE, or GL_GENERATE_MIPMAP.
					
	@param param Specifies the value of pname.

	@name TexParameter
*/
int gl_TexParameter(lua_State *L) {
	if(
		lua::is<GLenum>(L, 1) &&
		lua::is<GLenum>(L, 2) &&
		lua::is<GLfloat>(L, 3)
	) {
		GLenum target = lua::to<GLenum>(L, 1);
		GLenum pname = lua::to<GLenum>(L, 2);
		GLfloat param = lua::to<GLfloat>(L, 3);
		glTexParameterf(target, pname, param);
	}
	else {
		luaL_error(L, "gl.TexParameter: invalid arguments");
	}
	return 0;
}

/*! Multiply the current matrix by a translation matrix
	OpenGL Documentation: <a target="_blank" href="http://www.opengl.org/sdk/docs/man/xhtml/glTranslate.xml">glTranslate</a>
	
	@param ... Specify an array or list of 3 values of translation vector

	@name Translate
*/
int gl_Translate(lua_State *L) {
	GLdouble v[] = {1, 1, 1};
	if(lua::to_vec<GLdouble>(L, 1, 3, v)) {
		glTranslated(v[0], v[1], v[2]);
	}
	else {
		luaL_error(L, "gl.Translate: invalid arguments");
	}
	return 0;
}

/*! Specify a vertex
	OpenGL Documentation: <a target="_blank" href="http://www.opengl.org/sdk/docs/man/xhtml/glVertex.xml">glVertex</a>
	
	@param .. Specifies an array or list of two, three, or four elements, 
				which in turn specify the x, y, z, and w vertex coordinates.

	@name Vertex
*/
int gl_Vertex(lua_State *L) {
	GLdouble v[] = {0, 0, 0, 1};
	int n = 4;
	if(lua::to_vec_n<GLdouble>(L, 1, n, v)) {
		switch(n) {
			case 1:
			case 2: glVertex2dv(v); break;
			case 3: glVertex3dv(v); break;
			case 4:
			default:
				 glVertex4dv(v);
				 break;
		}
		
	}
	else {
		luaL_error(L, "gl.Vertex: invalid arguments");
	}
	return 0;
}

/*! Set the viewport
	OpenGL Documentation: <a target="_blank" href="http://www.opengl.org/sdk/docs/man/xhtml/glViewport.xml">glViewport</a>
	
	@param x Specify the x-coordinate of the lower left corner of the viewport rectangle.
	@param y Specify the y-coordinate of the lower left corner of the viewport rectangle.
	@param width Specify the width of the viewport rectangle.
	@param height Specify the height of the viewport rectangle.

	@name Viewport
*/
int gl_Viewport(lua_State *L) {
	if(
		lua::is<GLint>(L, 1) && 
		lua::is<GLint>(L, 2) && 
		lua::is<GLsizei>(L, 3) && 
		lua::is<GLsizei>(L, 4)
	) {
		GLint x = lua::to<GLint>(L, 1);
		GLint y = lua::to<GLint>(L, 2);
		GLsizei width = lua::to<GLsizei>(L, 3);
		GLsizei height = lua::to<GLsizei>(L, 4);
		glViewport(x, y, width, height);
	}
	else {
		luaL_error(L, "gl.Viewport: invalid arguments");
	}
	return 0;
}

#ifndef TABLE_ENTRY
#define TABLE_ENTRY(PREFIX, NAME)		{ #NAME, PREFIX##_##NAME }
#endif

static const luaL_reg gl_lib[] = {
	// for context creation & notification
	TABLE_ENTRY(gl, initctx),
	TABLE_ENTRY(gl, closectx),

	// OpenGL API
	TABLE_ENTRY(gl, Accum),
	TABLE_ENTRY(gl, AlphaFunc),
	TABLE_ENTRY(gl, Begin),
	TABLE_ENTRY(gl, BlendFunc),
	TABLE_ENTRY(gl, BlendEquation),
	TABLE_ENTRY(gl, CallList),
	TABLE_ENTRY(gl, Clear),
	TABLE_ENTRY(gl, ClearAccum),
	TABLE_ENTRY(gl, ClearColor),
	TABLE_ENTRY(gl, ClearDepth),
	TABLE_ENTRY(gl, ClearIndex),
	TABLE_ENTRY(gl, ClearStencil),
	TABLE_ENTRY(gl, ClipPlane),
	TABLE_ENTRY(gl, Color),
	TABLE_ENTRY(gl, ColorMask),
	TABLE_ENTRY(gl, ColorMaterial),
	TABLE_ENTRY(gl, CopyPixels),
	TABLE_ENTRY(gl, CopyTexImage),
	TABLE_ENTRY(gl, CopyTexSubImage),
	TABLE_ENTRY(gl, CullFace),
	TABLE_ENTRY(gl, DeleteLists),
	TABLE_ENTRY(gl, DepthFunc),
	TABLE_ENTRY(gl, DepthMask),
	TABLE_ENTRY(gl, DepthRange),
	TABLE_ENTRY(gl, Disable),
	TABLE_ENTRY(gl, DrawBuffer),
	TABLE_ENTRY(gl, EdgeFlag),
	TABLE_ENTRY(gl, Enable),
	TABLE_ENTRY(gl, End),
	TABLE_ENTRY(gl, EndList),
	TABLE_ENTRY(gl, Finish),
	TABLE_ENTRY(gl, Flush),
	TABLE_ENTRY(gl, Fog),
	TABLE_ENTRY(gl, FrontFace),
	TABLE_ENTRY(gl, Frustum),
	TABLE_ENTRY(gl, GenLists),
	TABLE_ENTRY(gl, Get),
	TABLE_ENTRY(gl, GetClipPlane),
	TABLE_ENTRY(gl, GetError),
	TABLE_ENTRY(gl, GetLight),
	TABLE_ENTRY(gl, GetMaterial),
	TABLE_ENTRY(gl, GetString),
	TABLE_ENTRY(gl, GetTexEnv),
	TABLE_ENTRY(gl, GetTexGen),
	TABLE_ENTRY(gl, GetTexLevelParameter),
	TABLE_ENTRY(gl, GetTexParameter),
	TABLE_ENTRY(gl, Hint),
	TABLE_ENTRY(gl, Index),
	TABLE_ENTRY(gl, IndexMask),
	TABLE_ENTRY(gl, InitNames),
	TABLE_ENTRY(gl, IsEnabled),
	TABLE_ENTRY(gl, IsList),
	TABLE_ENTRY(gl, IsTexture),
	TABLE_ENTRY(gl, Light),
	TABLE_ENTRY(gl, LightModel),
	TABLE_ENTRY(gl, LineStipple),
	TABLE_ENTRY(gl, LineWidth),
	TABLE_ENTRY(gl, ListBase),
	TABLE_ENTRY(gl, LoadIdentity),
	TABLE_ENTRY(gl, LoadMatrix),
	TABLE_ENTRY(gl, LoadName),
	TABLE_ENTRY(gl, LogicOp),
	TABLE_ENTRY(gl, Material),
	TABLE_ENTRY(gl, MatrixMode),
	TABLE_ENTRY(gl, MultMatrix),
	TABLE_ENTRY(gl, NewList),
	TABLE_ENTRY(gl, Normal),
	TABLE_ENTRY(gl, Ortho),
	TABLE_ENTRY(gl, PassThrough),
	TABLE_ENTRY(gl, PixelStore),
	TABLE_ENTRY(gl, PixelTransfer),
	TABLE_ENTRY(gl, PixelZoom),
	TABLE_ENTRY(gl, PointSize),
	TABLE_ENTRY(gl, PolygonMode),
	TABLE_ENTRY(gl, PolygonOffset),
	TABLE_ENTRY(gl, PopAttrib),
	TABLE_ENTRY(gl, PopClientAttrib),
	TABLE_ENTRY(gl, PopMatrix),
	TABLE_ENTRY(gl, PopName),
	TABLE_ENTRY(gl, PushAttrib),
	TABLE_ENTRY(gl, PushClientAttrib),
	TABLE_ENTRY(gl, PushMatrix),
	TABLE_ENTRY(gl, PushName),
	TABLE_ENTRY(gl, RasterPos),
	TABLE_ENTRY(gl, ReadBuffer),
	TABLE_ENTRY(gl, Rect),
	TABLE_ENTRY(gl, RenderMode),
	TABLE_ENTRY(gl, Rotate),
	TABLE_ENTRY(gl, Scale),
	TABLE_ENTRY(gl, Scissor),
	TABLE_ENTRY(gl, ShadeModel),
	TABLE_ENTRY(gl, StencilFunc),
	TABLE_ENTRY(gl, StencilMask),
	TABLE_ENTRY(gl, StencilOp),
	TABLE_ENTRY(gl, TexCoord),
	TABLE_ENTRY(gl, TexEnv),
	TABLE_ENTRY(gl, TexGen),
	TABLE_ENTRY(gl, TexParameter),
	TABLE_ENTRY(gl, Translate),
	TABLE_ENTRY(gl, Vertex),
	TABLE_ENTRY(gl, Viewport),
	{NULL, NULL}
};

#ifdef __cplusplus
extern "C" {
#endif

extern int luaopen_opengl_glu(lua_State *L);
extern int luaopen_opengl_sketch(lua_State *L);
extern int luaopen_opengl_Mesh(lua_State *L);
extern int luaopen_opengl_Shader(lua_State *L);
extern int luaopen_opengl_Slab(lua_State *L);
extern int luaopen_opengl_Texture(lua_State *L);
extern int luaopen_opengl_Camera(lua_State *L);
extern int luaopen_opengl_Tesselator(lua_State *L);

int luaopen_opengl(lua_State *L) {
	const char *libname = lua_tostring(L, -1);
	luaL_register(L, libname, gl_lib);
	
	for(int i=0; gl_enums[i].name; i++) {
		lua_pushstring(L, gl_enums[i].name);
		lua::push(L, gl_enums[i].value);
		lua_rawset(L, -3);
	}
	
	lua::preloadlib(L, "opengl.glu", luaopen_opengl_glu);
	lua::preloadlib(L, "opengl.sketch", luaopen_opengl_sketch);
	lua::preloadlib(L, "opengl.Mesh", luaopen_opengl_Mesh);
	lua::preloadlib(L, "opengl.Shader", luaopen_opengl_Shader);
	lua::preloadlib(L, "opengl.Slab", luaopen_opengl_Slab);
	lua::preloadlib(L, "opengl.Texture", luaopen_opengl_Texture);
	lua::preloadlib(L, "opengl.Camera", luaopen_opengl_Camera);
	lua::preloadlib(L, "opengl.Tesselator", luaopen_opengl_Tesselator);
	
	return 1;
}

#ifdef __cplusplus
}
#endif
