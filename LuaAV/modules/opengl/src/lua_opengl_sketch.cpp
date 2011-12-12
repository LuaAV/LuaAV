#include "lua_opengl.h"
#include "lua_utility.h"
#include <math.h>

#ifndef PI
#define PI       3.14159265358979323846
#endif

#ifndef DEG2RAD
#define DEG2RAD	0.017453292519943
#endif

#ifndef FLOAT_EPS
#define FLOAT_EPS		(10e-6f)
#endif

void vec3_add(float *res, const float *v1, const float *v2) {
	res[0] = v1[0]+v2[0];
	res[1] = v1[1]+v2[1];
	res[2] = v1[2]+v2[2];
}

void vec3_sub(float *res, const float *v1, const float *v2) {
	res[0] = v1[0]-v2[0];
	res[1] = v1[1]-v2[1];
	res[2] = v1[2]-v2[2];
}

void vec3_scale(float *res, const float *v, float s) {
	res[0] = v[0]*s;
	res[1] = v[1]*s;
	res[2] = v[2]*s;
}

float vec3_dot(const float *v1, const float *v2) {
	float result = v1[0] * v2[0];
	result += v1[1] * v2[1];
	result += v1[2] * v2[2];

	return result;
}

void vec3_cross(float *res, const float *v1, const float *v2) {
	res[0] = v1[1] * v2[2] - v1[2] * v2[1];
	res[1] = v1[2] * v2[0] - v1[0] * v2[2];
	res[2] = v1[0] * v2[1] - v1[1] * v2[0];
}

void vec3_normalize(float *res, const float *v) {
	float magnitude_sqr = vec3_dot(v, v);

	if(magnitude_sqr > FLOAT_EPS) {
		float scale = sqrt(magnitude_sqr);
		scale = 1.0f / scale;
		res[0] = v[0]*scale;
		res[1] = v[1]*scale;
		res[2] = v[2]*scale;
	}
	else {
		res[0] = FLOAT_EPS;
		res[1] = FLOAT_EPS;
		res[2] = FLOAT_EPS;
	}
}

void vec3_tangent(float *res, const float *v) {
	static float eps = 0.0001;
	static float arbitrary_axis1[] = {1, 0, 0};
	
	float axis1[3];
	vec3_cross(axis1, v, arbitrary_axis1);
	vec3_cross(res, v, axis1);
	
	if(vec3_dot(res, res) < eps) {
		static float arbitrary_axis2[] = {0.70710678118655, 0, 0.70710678118655};
		vec3_cross(axis1, v, arbitrary_axis2);
		vec3_cross(res, v, axis1);
		
		if(vec3_dot(res, res) < eps) {
			static float arbitrary_axis3[] = {0, 0, 1};
			vec3_cross(axis1, v, arbitrary_axis3);
			vec3_cross(res, v, axis1);
		}
	}
}


/*! Handy sketching functions for OpenGL
	The sketch module contains functions for commonly used drawing 
	actions such as setting up orthographic drawing or drawing a textured 
	quad.
	
	@module opengl.sketch
*/

/*! Enter orthographic drawing mode
	Orthographic modes can be descibed as <b>normalized</b>, <b>pixel</b>, or <b>arbitrary</b>.  The 
	two most common are noramlized and pixel.  In the sketch module, normalized is defined as a view that 
	spans [-1, -1] x [1, 1] with [-1, -1] in the <b>lower left</b> corner.  Pixel is defined as a view that 
	spans [0, 0] x [w, h] with [0, 0] in the <b>upper left</b> corner.
	
	<pre>
		-- no arguments, uses the default (-1, -1, 2, 2) normalized mode
		-- glOrtho(-1, 1, -1, 1, -100, 100)
		enter_ortho()
		
		-- 2 arguments, sets up a pixel mode (0, 0, w, h)
		-- glOrtho(0, w, h, 0, -100, 100);
		enter_ortho(w, h)
		
		-- 4 arguments, sets an arbitrary mode (x, y, w, h)
		-- glOrtho(x, x+w, y, y+h, -100, 100)
		enter_ortho(x, y, w, h)
	</pre>

	@param ... Either a table or unpacked list of up to 4 values.  If 
				2 values are given, they set the width of the orthographic 
				drawing area with the upper left corner being (0, 0). If 4 values 
				are given, they set the (x, y, w, h) of the orthographic view.
				If no values are given, the view is set to a normalized drawing area 
				of (-1, -1, 2, 2) with a drawing area of [-1, -1] x [1, 1].

	@name enter_ortho
*/
int opengl_sketch_enter_ortho(lua_State *L) {
	float vp[] = {-1, -1, 2, 2};
	int n = 4;
	lua::to_vec_n(L, 1, n, vp);
	if(n > 2) {
		if(n == 3) {
			vp[3] = vp[2];
		}
		glMatrixMode(GL_PROJECTION);
		glPushMatrix();
			glLoadIdentity();
			glOrtho(vp[0], vp[0]+vp[2], vp[1], vp[1]+vp[3], -100, 100);

		glMatrixMode(GL_MODELVIEW);
		glPushMatrix();
			glLoadIdentity();
	}
	else if(n > 0) {
		if(n == 1) {
			vp[1] = vp[0];
		}
		glMatrixMode(GL_PROJECTION);
		glPushMatrix();
			glLoadIdentity();
			glOrtho(0, vp[0], vp[1], 0, -100, 100);

		glMatrixMode(GL_MODELVIEW);
		glPushMatrix();
			glLoadIdentity();
	}
	else {
		glMatrixMode(GL_PROJECTION);
		glPushMatrix();
			glLoadIdentity();
			glOrtho(-1, 1, -1, 1, -100, 100);

		glMatrixMode(GL_MODELVIEW);
		glPushMatrix();
			glLoadIdentity();
	}
	return 0;
}

/*! Leave orthographic drawing mode
	Pop the GL_MODELVIEW and GL_PROJECTION matrices set when entering 
	orthographic drawing.  The glMatrixMode will be GL_MODELVIEW after 
	the function is called.
	
	@name leave_ortho
*/
int opengl_sketch_leave_ortho(lua_State *L) {
	glMatrixMode(GL_PROJECTION);
	glPopMatrix();

	glMatrixMode(GL_MODELVIEW);
	glPopMatrix();
	return 0;
}

/*! Draw a quad with texture coordinates in normalized mode
	Draw a quad in <i>normalized</i> orthographic mode.  Depending on the number of 
	arguments, different parameters of the quad's position and dimension will be set:
	
	<pre>
		-- no arguments, uses (-1, -1, 2, 2) for coordinates [-1, -1] x [1, 1]
		quad() 
		
		-- 2 arguments, uses (-w/2, -h/2, w, h) for coordinates [-w/2, -h/2] x [w/2, h/2]
		-- The quad will be centered on (0, 0)
		quad(w, h) 
		
		-- 4 arguments, uses (x, y, w, h) for coordinates [x, y] x [x+w, y+h]
		quad(x, y, w, h)
	</pre>

	@param ... Either a table or list of up to 4 unpacked values.  If 2 
				arguments are given, they set the width and height of the 
				quad.  If 4 arguments are given, they set the (x, y, w, h) of 
				the quad.  No arguments uses the default (-1, -1, 2, 2).
	@name quad
*/
int opengl_sketch_quad(lua_State *L) {
	float quad[] = {-1, -1, 2, 2};
	int n = 4;
	lua::to_vec_n(L, 1, n, quad);
	
	if(n > 2) {
		if(n == 3) {
			quad[3] = quad[2];
		}
	}
	else if(n > 0) {
		if(n == 1) {
			quad[1] = quad[0];
		}
		
		quad[3] = quad[1];
		quad[2] = quad[0];
		quad[1] = -quad[3]/2.;
		quad[0] = -quad[2]/2.;
	}

	glTexCoord2f(0, 1);	glVertex3f(quad[0], quad[1], 0);
	glTexCoord2f(1, 1);	glVertex3f(quad[0]+quad[2], quad[1], 0);
	glTexCoord2f(1, 0);	glVertex3f(quad[0]+quad[2], quad[1]+quad[3], 0);
	glTexCoord2f(0, 0);	glVertex3f(quad[0], quad[1]+quad[3], 0);
	return 0;
}

/*! Draw a quad with texture coordinates in pixel mode
	Draw a quad in <i>pixel</i> orthographic mode.  Depending on the number of 
	arguments, different parameters of the quad's position and dimension will be set:
	
	<pre>
		-- no arguments, uses (0, 0, 512, 512) for coordinates [0, 0] x [512, 512]
		quad() 
		
		-- 2 arguments, uses (0, 0, w, h) for coordinates [0, 0] x [w/2, h/2]
		-- The quad will be centered on (0, 0)
		quad(w, h) 
		
		-- 4 arguments, uses (x, y, w, h) for coordinates [x, y] x [x+w, y+h]
		quad(x, y, w, h)
	</pre>

	@param ... Either a table or list of up to 4 unpacked values.  If 2 
				arguments are given, they set the width and height of the 
				quad.  If 4 arguments are given, they set the (x, y, w, h) of 
				the quad.  No arguments uses the default (0, 0, 512, 512).
	@name pixquad
*/
int opengl_sketch_pixquad(lua_State *L) {
	float quad[] = {0, 0, 512, 512};
	int n = 4;
	lua::to_vec_n(L, 1, n, quad);

	if(n > 2) {
		if(n == 3) {
			quad[3] = quad[2];
		}
	}
	else if(n > 0) {
		if(n == 1) {
			quad[1] = quad[0];
		}
		
		quad[3] = quad[1];
		quad[2] = quad[0];
		quad[1] = 0;
		quad[0] = 0;
	}
	
	glTexCoord2f(0, 1);	glVertex3f(quad[0], quad[1]+quad[3], 0);
	glTexCoord2f(1, 1);	glVertex3f(quad[0]+quad[2], quad[1]+quad[3], 0);
	glTexCoord2f(1, 0);	glVertex3f(quad[0]+quad[2], quad[1], 0);
	glTexCoord2f(0, 0);	glVertex3f(quad[0], quad[1], 0);
	return 0;
}


/*! Draw xyz axes
	The axes have unit length, identity rotation and are 
	centered at the origin.
	
	@param alpha The alpha (default 1)
	@name axes
*/
int opengl_sketch_axes(lua_State *L) {
	float alpha = lua::opt<float>(L, 1, 1);
	
	glBegin(GL_LINES);
		glColor4f(1., 0.2, 0.2, alpha);
		glVertex3f(0, 0, 0);
		glVertex3f(1, 0, 0);
		
		glColor4f(0., 0.63, 0.43, alpha);
		glVertex3f(0, 0, 0);
		glVertex3f(0, 1, 0);
		
		glColor4f(.22, .35, 0.66, alpha);
		glVertex3f(0, 0, 0);
		glVertex3f(0, 0, 1);
	glEnd();
	
	
	return 0;
}

/*! Draw a circle
	Draw a circle of radius 1.  The circle function only specifies the 
	vertices, not the primitive used to actually draw the circle.  circle 
	should be wrapped in glBegin/glEnd calls.
	
	<pre>
		gl.Begin(GL.LINE_LOOP)
			sketch.circle(30)
		gl.End()
	</pre>
	
	@param n Optional number of divisions in the circle (default 20)
	@name circle
*/
int opengl_sketch_circle(lua_State *L) {
	int n = lua::opt<int>(L, 1, 20);
	double dtheta = 2.*PI/(double)n;
	double a = cos(dtheta);
	double b = sin(dtheta);	
	double c = 0.;
	double s = 1.;	
	
	for(int i=0; i < n; i++) {
		glVertex3d(c, s, 0);
		const double ns = b*c + a*s;
		const double nc = a*c - b*s;
		c = nc;
		s = ns;
	}
	return 0;
}

/*! Draw an arc
	Draw an arc of radius 1.  The arc function only specifies the 
	vertices, not the primitive used to actually draw the arc.  arc 
	should be wrapped in glBegin/glEnd calls.
	
	<pre>
		-- draw an arc from -10 degrees to 80 degress in 30 steps
		gl.Begin(GL.LINE_LOOP)
			sketch.arc(-10, 80, 30)
		gl.End()
	</pre>
	
	@param a1 The starting angle
	@param a2 The stopping angle
	@param n Optional number of divisions in the arc (default 20)
	@name arc
*/
int opengl_sketch_arc(lua_State *L) {
	float a1 = lua::opt<float>(L, 1, 0.);
	float a2 = lua::opt<float>(L, 2, 90.);
	int n = lua::opt<int>(L, 3, 20);
	double dtheta = (a2-a1)*DEG2RAD/(double)(n-1);
	double theta = a1*DEG2RAD;
	double a = cos(dtheta);
	double b = sin(dtheta);	
	double c = cos(theta);
	double s = sin(theta);
	
	for(int i=0; i < n; i++) {
		glVertex3d(c, s, 0);
		const double ns = b*c + a*s;
		const double nc = a*c - b*s;
		c = nc;
		s = ns;
	}
	return 0;
}

/*! Draw a cone
	Draw a cone with a specified radius and direction. The cone function 
	
	<pre>
		-- cone at the origin pointing along the y-axis
		-- with radius 0.5 and 30 divisions
		gl.Begin(GL.TRIANGLES)
			sketch.cone({0, 0, 0}, {0, 1, 0}, 0.5, 30)
		gl.End()
	</pre>
	
	@param apex The apex
	@param dir The direction
	@param r The radius
	@param n The number of divisions (default 20)
	@name cone
*/
int opengl_sketch_cone(lua_State *L) {
	float pos[] = {0, 0, 0};
	float dir[] = {0, 1, 0};
	if(
		lua::to_vec_t(L, 1, 3, pos) && 
		lua::to_vec_t(L, 2, 3, dir)
	) {
		float r = lua::opt<float>(L, 3, 1.);
		int n = lua::opt<int>(L, 4, 20);
		
		float normal[] = {0, 1, 0};
		vec3_normalize(normal, dir);
		
		float tangent[] = {0, 1, 0};
		vec3_tangent(tangent, normal);
		vec3_normalize(tangent, tangent);
		
		float bitangent[] = {0, 1, 0};
		vec3_cross(bitangent, normal, tangent);
		vec3_normalize(bitangent, bitangent);
		
		
		double dtheta = 2.*PI/(double)n;
		double a = cos(dtheta);
		double b = sin(dtheta);	
		double c = 0.;
		double s = 1.;
		
		float pwprev[3], pw1[3];
		float pprev[3], p[3], p1[3];
		float norm[3];
		for(int i=0; i < n; i++) {
			float v1[3], v2[3];
			vec3_scale(v1, tangent, r*c);
			vec3_scale(v2, bitangent, r*s);
			vec3_add(p, v1, v2);
			vec3_add(p, p, dir);
			
			float pw[3];
			vec3_add(pw, p, pos);
			
			if(i==0) {
				p1[0] = p[0];
				p1[1] = p[1];
				p1[2] = p[2];
				
				pw1[0] = pw[0];
				pw1[1] = pw[1];
				pw1[2] = pw[2];
			}
			else {
				// cross p, pprev with normalization
				vec3_cross(norm, p, pprev);
				vec3_normalize(norm, norm);
			
				glNormal3fv(norm);
				glVertex3fv(pos);
				glVertex3fv(pw);
				glVertex3fv(pwprev);
			}
			const double ns = b*c + a*s;
			const double nc = a*c - b*s;
			c = nc;
			s = ns;
			
			pprev[0] = p[0];
			pprev[1] = p[1];
			pprev[2] = p[2];
			
			pwprev[0] = pw[0];
			pwprev[1] = pw[1];
			pwprev[2] = pw[2];
		}
		
		vec3_cross(norm, p1, pprev);
		vec3_normalize(norm, norm);
		
		glNormal3fv(norm);
		glVertex3fv(pos);
		glVertex3fv(pw1);
		glVertex3fv(pwprev);
		
	}
	else {
		luaL_error(L, "sketch.cone: invalid arguments");
	}
	
	return 0;
}

/*! Draw a disc
	Draw a disc in 3D with a specified radius and direction.  The circle 
	function could be used to achieve the same result, but it requires 
	calculating the correct rotation and applying it with glRotate.  This 
	function explicitly calculates the vertices in 3D
	
	<pre>
		-- disc at the origin pointing along the y-axis
		-- with radius 0.5 and 30 divisions
		gl.Begin(GL.POLYGON)
			gl.Normal(0, 1, 0)
			sketch.disc({0, 0, 0}, {0, 1, 0}, 0.5, 30)
		gl.End()
	</pre>
	
	@param pos The position
	@param dir The direction
	@param r The radius (default 1)
	@param n The number of divisions (default 20)
	@name disc
*/
int opengl_sketch_disc(lua_State *L) {
	float pos[] = {0, 0, 0};
	float dir[] = {0, 1, 0};
	if(
		lua::to_vec_t(L, 1, 3, pos) && 
		lua::to_vec_t(L, 2, 3, dir)
	) {
		float r = lua::opt<float>(L, 3, 1.);
		int n = lua::opt<int>(L, 4, 20);
		
		float normal[] = {0, 1, 0};
		vec3_normalize(normal, dir);
		
		float tangent[] = {0, 1, 0};
		vec3_tangent(tangent, normal);
		vec3_normalize(tangent, tangent);
		
		float bitangent[] = {0, 1, 0};
		vec3_cross(bitangent, normal, tangent);
		
		
		double dtheta = 2.*PI/(double)n;
		double a = cos(dtheta);
		double b = sin(dtheta);	
		double c = 0.;
		double s = 1.;	
		
		for(int i=0; i < n; i++) {
			float v1[3], v2[3], p[3];
			vec3_scale(v1, tangent, r*c);
			vec3_scale(v2, bitangent, r*s);
			vec3_add(p, v1, v2);
			vec3_add(p, p, pos);
			
			glVertex3fv(p);
			
			
			const double ns = b*c + a*s;
			const double nc = a*c - b*s;
			c = nc;
			s = ns;
		}
	}
	else {
		luaL_error(L, "sketch.disc: invalid arguments");
	}
	
	return 0;
}

/*! Draw a tube
	Draw a tube in 3D with a specified radius, direction, and thickness.
	
	<pre>
		-- tube at the origin pointing along the y-axis
		-- with radius 0.5, 30 divisions, and 0.25 thickness
		gl.Begin(GL.QUAD_STRIP)
			gl.Normal(0, 1, 0)
			sketch.tube({0, 0, 0}, {0, 1, 0}, 0.5, 30, 0.25)
		gl.End()
	</pre>
	
	@param pos The position
	@param dir The direction
	@param r The radius (default 1)
	@param n The number of divisions (default 20)
	@param thickness The thickness (default 0.1)
	@name tube
*/
int opengl_sketch_tube(lua_State *L) {
	float pos[] = {0, 0, 0};
	float dir[] = {0, 1, 0};
	if(
		lua::to_vec_t(L, 1, 3, pos) && 
		lua::to_vec_t(L, 2, 3, dir)
	) {
		float r = lua::opt<float>(L, 3, 1.);
		int n = lua::opt<int>(L, 4, 20);
		float thick = lua::opt<float>(L, 5, 0.1);
		
		float normal[] = {0, 1, 0};
		vec3_normalize(normal, dir);
		
		float tangent[] = {0, 1, 0};
		vec3_tangent(tangent, normal);
		vec3_normalize(tangent, tangent);
		
		float bitangent[] = {0, 1, 0};
		vec3_cross(bitangent, normal, tangent);
		
		float pos1[3];
		float pos2[3];
		float along[3];
		vec3_scale(along, normal, thick*0.5);
		vec3_add(pos1, pos, along);
		vec3_sub(pos2, pos, along);
		
		double dtheta = 2.*PI/(double)n;
		double a = cos(dtheta);
		double b = sin(dtheta);	
		double c = 0.;
		double s = 1.;
		
		for(int i=0; i <= n; i++) {
			float v1[3], v2[3], p[3], p1[3], p2[3];
			vec3_scale(v1, tangent, r*c);
			vec3_scale(v2, bitangent, r*s);
			vec3_add(p, v1, v2);
			vec3_add(p1, p, pos1);
			vec3_add(p2, p, pos2);
			
			glVertex3fv(p1);
			glVertex3fv(p2);
			
			const double ns = b*c + a*s;
			const double nc = a*c - b*s;
			c = nc;
			s = ns;
		}
	}
	else {
		luaL_error(L, "sketch.tube: invalid arguments");
	}
	
	return 0;
}

/*! Set the current matrix to be view-aligned
	Setting a view-aligned matrix only works when glMatrixMode is set 
	to GL_MODELVIEW and the matrix has unit scale (i.e. no glScale functions 
	have been called).
	
	After this function has been called, any 3D drawing confined to the xy-plane 
	will be orthogonal to the viewport.
	
	View-alignment is useful when drawing sprites or 2D shapes associated with a 
	3D rotation frame.
	
	@name viewalign
*/
int opengl_sketch_viewalign(lua_State *L) {
	GLfloat mv[16];
	glGetFloatv(GL_MODELVIEW_MATRIX, mv);
	
	GLfloat va[16];
	va[0] = mv[0];
	va[1] = mv[4];
	va[2] = mv[8];
	va[3] = 0.;
	
	va[4] = mv[1];
	va[5] = mv[5];
	va[6] = mv[9];
	va[7] = 0.;
	
	va[8] = mv[2];
	va[9] = mv[6];
	va[10] = mv[10];
	va[11] = 0.;
	
	va[12] = 0.;
	va[13] = 0.;
	va[14] = 0.;
	va[15] = 1.;
	
	glMultMatrixf(va);
	
	return 0;
}

int opengl_sketch_eye_pos(lua_State *L) {
	float pos[] = {0, 0, 0, 1.};
	if(lua::to_vec_t<float>(L, 1, 3, pos)) {
		GLfloat mv[16];
		glGetFloatv(GL_MODELVIEW_MATRIX, mv);
		
		float eye_pos[3];
		eye_pos[0] = mv[0]*pos[0] + mv[4]*pos[1] + mv[8]*pos[2] + mv[12]*pos[3];
		eye_pos[1] = mv[1]*pos[0] + mv[5]*pos[1] + mv[9]*pos[2] + mv[13]*pos[3];
		eye_pos[2] = mv[2]*pos[0] + mv[6]*pos[1] + mv[10]*pos[2] + mv[14]*pos[3];
		
		lua::push_vec_t<float>(L, 3, eye_pos);
	}
	else {
		luaL_error(L, "sketch.eye_pos: invalid arguments");
	}
	
	return 1;
}

int opengl_sketch_eye_norm(lua_State *L) {
	float n[] = {0, 0, 0};
	if(lua::to_vec_t<float>(L, 1, 3, n)) {
		GLfloat mv[16];
		glGetFloatv(GL_MODELVIEW_MATRIX, mv);
		
		float eye_n[3];
		eye_n[0] = mv[0]*n[0] + mv[1]*n[1] + mv[2]*n[2];
		eye_n[1] = mv[4]*n[0] + mv[5]*n[1] + mv[6]*n[2];
		eye_n[2] = mv[8]*n[0] + mv[9]*n[1] + mv[10]*n[2];
		
		lua::push_vec_t<float>(L, 3, eye_n);
	}
	else {
		luaL_error(L, "sketch.eye_norm: invalid arguments");
	}
	
	return 1;
}

#ifndef TABLE_ENTRY
#define TABLE_ENTRY(PREFIX, NAME)		{ #NAME, PREFIX##_##NAME }
#endif

static luaL_reg const sketch_lib[] = {
	TABLE_ENTRY(opengl_sketch, enter_ortho),
	TABLE_ENTRY(opengl_sketch, leave_ortho),
	TABLE_ENTRY(opengl_sketch, quad),
	TABLE_ENTRY(opengl_sketch, pixquad),
	TABLE_ENTRY(opengl_sketch, axes),
	TABLE_ENTRY(opengl_sketch, circle),
	TABLE_ENTRY(opengl_sketch, arc),
	TABLE_ENTRY(opengl_sketch, cone),
	TABLE_ENTRY(opengl_sketch, disc),
	TABLE_ENTRY(opengl_sketch, tube),
	TABLE_ENTRY(opengl_sketch, viewalign),
	TABLE_ENTRY(opengl_sketch, eye_pos),
	TABLE_ENTRY(opengl_sketch, eye_norm),
	{NULL, NULL}
};


#ifdef __cplusplus
extern "C" {
#endif

int luaopen_opengl_sketch(lua_State *L) {
	const char *libname = lua_tostring(L, -1);
	luaL_register(L, libname, sketch_lib);

	return 1;
}

#ifdef __cplusplus
}
#endif