#include "lua_openglu.h"
#include "lua_utility_opengl.h"


/*! Define a viewing transformation
	OpenGL Documentation: <a target="_blank" href="http://www.opengl.org/sdk/docs/man/xhtml/gluLookAt.xml">gluLookAt</a>
	
	If LookAt has nine arguments, it expects unpacked vectors.  Otherwise, it will look 
	for 3 vectors as arguments.
	
	@param eye Specifies the position of the eye point.
	@param center Specifies the position of the reference point.
	@param up Specifies the direction of the up vector.

	@name LookAt
*/
int glu_LookAt(lua_State *L) {
	GLdouble eye[3];
	GLdouble center[3];
	GLdouble up[3];
	
	if(
		lua_gettop(L) == 9 &&
		lua::is<GLdouble>(L, 1) && 
		lua::is<GLdouble>(L, 2) && 
		lua::is<GLdouble>(L, 3) && 
		lua::is<GLdouble>(L, 4) && 
		lua::is<GLdouble>(L, 5) && 
		lua::is<GLdouble>(L, 6) && 
		lua::is<GLdouble>(L, 7) && 
		lua::is<GLdouble>(L, 8) && 
		lua::is<GLdouble>(L, 9)
	) {
		eye[0] = lua::to<GLdouble>(L, 1);
		eye[1] = lua::to<GLdouble>(L, 2);
		eye[2] = lua::to<GLdouble>(L, 3);
		
		center[0] = lua::to<GLdouble>(L, 4);
		center[1] = lua::to<GLdouble>(L, 5);
		center[2] = lua::to<GLdouble>(L, 6);
		
		up[0] = lua::to<GLdouble>(L, 7);
		up[1] = lua::to<GLdouble>(L, 8);
		up[2] = lua::to<GLdouble>(L, 9);
	}
	else if(
		lua::to_vec_t(L, 1, 3, eye) && 
		lua::to_vec_t(L, 2, 3, center) && 
		lua::to_vec_t(L, 3, 3, up)
	) {
	
	}
	else {
		luaL_error(L, "glu.LookAt: invalid arguments");
	}
	
	
	gluLookAt(
		eye[0], eye[1], eye[2],
		center[0], center[1], center[2],
		up[0], up[1], up[2]
	);
	
	return 0;
}

/*! Define a 2D orthographic projection matrix
	OpenGL Documentation: <a target="_blank" href="http://www.opengl.org/sdk/docs/man/xhtml/gluOrtho2D.xml">gluOrtho2D</a>
	
	@param left Specify the coordinates for the left vertical clipping planes.
	@param right Specify the coordinates for the right vertical clipping planes.
	@param bottom Specify the coordinates for the bottom horizontal clipping planes.
	@param up Specify the coordinates for the up horizontal clipping planes.

	@name Ortho2D
*/
int glu_Ortho2D(lua_State *L) {
	if(
		lua::is<GLdouble>(L, 1) && 
		lua::is<GLdouble>(L, 2) && 
		lua::is<GLdouble>(L, 3) && 
		lua::is<GLdouble>(L, 4)
	) {
		GLdouble left = lua::to<GLdouble>(L, 1);
		GLdouble right = lua::to<GLdouble>(L, 2);
		GLdouble bottom = lua::to<GLdouble>(L, 3);
		GLdouble top = lua::to<GLdouble>(L, 4);
		gluOrtho2D(left, right, bottom, top);
	}
	else {
		luaL_error(L, "glu.Ortho2D: invalid arguments");
	}
	
	return 0;
}

/*! Set up a perspective projection matrix
	OpenGL Documentation: <a target="_blank" href="http://www.opengl.org/sdk/docs/man/xhtml/gluPerspective.xml">gluPerspective</a>
	
	@param fovy Specifies the field of view angle, in degrees, in the y direction.
	@param aspect Specifies the aspect ratio that determines the field of view in the x direction. 
					The aspect ratio is the ratio of x (width) to y (height).
	@param near Specifies the distance from the viewer to the near clipping plane (always positive).
	@param far Specifies the distance from the viewer to the far clipping plane (always positive).
	
	@name Perspective
*/
int glu_Perspective(lua_State *L) {
	if(
		lua::is<GLdouble>(L, 1) && 
		lua::is<GLdouble>(L, 2) && 
		lua::is<GLdouble>(L, 3) && 
		lua::is<GLdouble>(L, 4)
	) {
		GLdouble fovy = lua::to<GLdouble>(L, 1);
		GLdouble aspect = lua::to<GLdouble>(L, 2);
		GLdouble near = lua::to<GLdouble>(L, 3);
		GLdouble far = lua::to<GLdouble>(L, 4);
		gluPerspective(fovy, aspect, near, far);
	}
	else {
		luaL_error(L, "glu.Perspective: invalid arguments");
	}
	
	return 0;
}

/*! Map object coordinates to window coordinates
	OpenGL Documentation: <a target="_blank" href="http://www.opengl.org/sdk/docs/man/xhtml/gluProject.xml">gluProject</a>
	
	@param ... Specify the object coordinates as either a table or unpacked values.
	@ret The window coordinates as a table
	
	@name Project
*/
int glu_Project(lua_State *L) {
	GLdouble obj[] = {0, 0, 0};
	if(lua::to_vec(L, 1, 3, obj)) {
		GLdouble model[16];
		GLdouble proj[16];
		GLint view[4];
		
		glGetDoublev(GL_MODELVIEW_MATRIX, model);
		glGetDoublev(GL_PROJECTION_MATRIX, proj);
		glGetIntegerv(GL_VIEWPORT, view);
		
		GLdouble win[3];
		gluProject(
			obj[0], obj[1], obj[2],
			model,
			proj,
			view,
			win, win+1, win+2
		);
		
		lua_newtable(L);
		for(int i=0; i < 3; i++) {
			lua::push(L, win[i]);
			lua_rawseti(L, -2, i+1);
		}
	}
	else {
		luaL_error(L, "glu.Project: invalid arguments");
	}
	
	return 1;
}

/*! Map window coordinates to object coordinates
	OpenGL Documentation: <a target="_blank" href="http://www.opengl.org/sdk/docs/man/xhtml/gluProject.xml">gluUnProject</a>
	
	@param ... Specify the window coordinates as either a table or unpacked values.
	@ret The object coordinates as a table
	
	@name UnProject
*/
int glu_UnProject(lua_State *L) {
	GLdouble win[] = {0, 0, 0};
	if(lua::to_vec(L, 1, 3, win)) {
		GLdouble model[16];
		GLdouble proj[16];
		GLint view[4];
		
		glGetDoublev(GL_MODELVIEW_MATRIX, model);
		glGetDoublev(GL_PROJECTION_MATRIX, proj);
		glGetIntegerv(GL_VIEWPORT, view);
		
		// correct y coordinate, in screen coordinates y increases towards bottom
		win[1] = (GLdouble) (view[3] - win[1]);

		GLdouble obj[3];
		gluUnProject(
			win[0], win[1], win[2],
			model,
			proj,
			view,
			obj, obj+1, obj+2
		);
		
		lua_newtable(L);
		for(int i=0; i < 3; i++) {
			lua::push(L, obj[i]);
			lua_rawseti(L, -2, i+1);
		}
	}
	else {
		luaL_error(L, "glu.UnProject: invalid arguments");
	}
	
	return 1;
}

#ifndef TABLE_ENTRY
#define TABLE_ENTRY(PREFIX, NAME)		{ #NAME, PREFIX##_##NAME }
#endif

static luaL_reg const glu_lib[] = {
	TABLE_ENTRY(glu, LookAt),
	TABLE_ENTRY(glu, Ortho2D),
	TABLE_ENTRY(glu, Perspective),
	TABLE_ENTRY(glu, Project),
	TABLE_ENTRY(glu, UnProject),
	{NULL, NULL}
};

#ifdef __cplusplus
extern "C" {
#endif

int luaopen_opengl_glu(lua_State *L) {
	const char *libname = lua_tostring(L, -1);
	luaL_register(L, libname, glu_lib);

	return 1;
}

#ifdef __cplusplus
}
#endif
