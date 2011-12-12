#include "lua_opengl.h"
#include <vector>
#include "lua_glue.h"
#include "lua_utility.h"
#include "lua_array.h"
#include "Mat4.h"

using space::Mat4;
using std::vector;

#ifndef M_PI
	#define M_PI (3.1415926535898)
#endif

float fastsqrt(float number) {
   int i;
   float x, y;
   x = number * 0.5;
   y  = number;
   i  = * (int * ) &y;
   i  = 0x5f3759df - (i >> 1);
   y  = * ( float * ) &i;
   y  = y * (1.5 - (x * y * y));
   y  = y * (1.5 - (x * y * y));
   return number * y;
}

struct Color{
	float r;
	float g;
	float b;
	float a;
	
	Color(float r=1., float g=1., float b=1., float a=1.)
	:	r(r), g(g), b(b), a(a)
	{}
};

struct Vec2{
	float x;
	float y;
	
	Vec2(float x=0, float y=0)
	:	x(x), y(y)
	{}
	
	Vec2(const Vec2 &v) {
		x = v.x; y = v.y;
	}
	
	const Vec2 operator- () const {
		return Vec2(-x, -y);
	}
	
	const Vec2 operator- (const Vec2 &v) const {
		return Vec2(x-v.x, y-v.y);
	}
	
	const Vec2 operator+ (const Vec2 &v) const {
		return Vec2(x+v.x, y+v.y);
	}
	
	void normalize() {
		float mag = fastsqrt(x*x+y*y);
		x = x/mag;
		y = y/mag;
	}
	
	float mag() {
		return fastsqrt(x*x+y*y);
	}
};

const Vec2 operator* (float k, const Vec2 &v) {
	return Vec2(k*v.x, k*v.y);
}

float dot(const Vec2 &v1, const Vec2 &v2) {
	return v1.x*v2.x+v1.y*v2.y;
}

Vec2 line_intersect(const Vec2 &p1, const Vec2 &p2, const Vec2 &p3, const Vec2 &p4) {
	float m1, m2;

	if(p2.x == p1.x) {
		m1 = 1e10;
	}
	else {
		m1 = (p2.y-p1.y)/(p2.x-p1.x);
	}
	if(p4.x == p3.x) {
		m2 = 1e10;
	}
	else {
		m2 = (p4.y-p3.y)/(p4.x-p3.x);
	}
	
	float c1 = p1.y - m1*p1.x;
	float c2 = p3.y - m2*p3.x;
	float detinv = 1./(m2-m1);
	return Vec2(
		(-c2 + c1)*detinv,
		(m2*c1 - m1*c2)*detinv
	);
}

Vec2 bezierSegment(const Vec2 &p1, const Vec2 &c1, const Vec2 &c2, const Vec2 &p2, float t) {
	float tt = t*t;
	float k = (1-t);
	float kk = k*k;
	float k0 = kk*k;
	float k1 = 3*kk*t;
	float k2 = 3*k*tt;
	float k3 = tt*t;

	return k0*p1 + k1*c1 + k2*c2 + k3*p2;
}

Vec2 bezierTangent(const Vec2 &p1, const Vec2 &c1, const Vec2 &c2, const Vec2 &p2, float t) {
	return (3*t*t * (-p1+3*c1-3*c2+p2) +
            6*t * (p1-2*c1+c2) +
            3 * (-p1+c1));
}

void splineForward(int segments, Mat4<float> &m) {
	float f = 1./(float)segments;
	float ff = f*f;
	float fff = ff*f;
	
	m.col[0].set(0, fff, 6*fff, 6*fff);
	m.col[1].set(0, ff, 2*ff, 0);
	m.col[2].set(0, f, 0, 0);
	m.col[3].set(1, 0, 0, 0);
}

struct Draw{
	enum ShapePathMode{
		ShapeNone = 0,
		ShapePoints,
		ShapeLines,
		ShapeTriangles,
		ShapeTriangleStrip,
		ShapeTriangleFan,
		ShapeQuads,
		ShapeQuadStrip,
		ShapeClose
	};
	
	enum ShapeMode{
		Center = 0,
		Radius,
		Corner,
		Corners
	};

	enum StrokeCap{
		CapSquare = 0,
		CapProject,
		CapRound = 2
	};
	
	enum StrokeJoin{
		JoinMiter = 0,
		JoinBevel,
		JoinRound = 2
	};
	
	enum ColorMode{
		RGB = 0,
		HSB,
	};
	
	enum ShapeVertexType{
		Vertex = 0,
		BezierVertex,
		BezierControl,
		CurveVertex
	};
	
	struct ShapeVertex{
		ShapeVertex(ShapeVertexType type, const Vec2& vertex)
		:	type(type), vertex(vertex)
		{}
		
		ShapeVertexType type;
		Vec2 vertex;
	};

	Draw() 
	:	ellipseMode(Center),
		rectMode(Corner),
		strokeCap(CapSquare),
		strokeJoin(JoinMiter),
		strokeWeight(0.1),
		colorMode(RGB),
		fill(true),
		stroke(true),
		pathClosed(false),
		bezierDetail(20),
		curveTightness(0.),
		curveDetail(20),
		shapePathMode(ShapeNone)
	{
		colorRange[0] = 1.;
		colorRange[1] = 1.;
		colorRange[2] = 1.;
		colorRange[3] = 1.;
		
		setupCurves();
	}
	
	~Draw() {}
	
	void setupCurves() {
		float s = curveTightness;
		curveBasisMatrix.set(
			(s-1)/2, (1-s), (s-1)/2, 0,
			(s+3)/2, (-5-s)/2, 0, 1,
			(-3-s)/2, (s+2), (1-s)/2, 0,
			(1-s)/2, (s-1)/2, 0, 0
		);
		splineForward(curveDetail, curveDrawMatrix);
		curveDrawMatrix.mul(curveBasisMatrix);
	}
	
	/** 
	*	Attribute Setters
	*/
	void setCurveTightness(float v) {
		curveTightness = v;
		setupCurves();
	}
	
	void setCurveDetail(int v) {
		curveDetail = v;
		setupCurves();
	}
	
	void CbezierSegment(const Vec2 &p1, const Vec2 &c1, const Vec2 &c2, const Vec2 &p2, bool first = true) {
		if(first) {
			float t = 0;
			float dt = 1./(float)bezierDetail;
			for(int i=0; i <= bezierDetail; i++) {
				Vec2 p = bezierSegment(p1, c1, c2, p2, t);
				appendPath(p);
				t += dt;
			}
		}
		else {
			float dt = 1./(float)bezierDetail;
			float t = dt;
			for(int i=1; i <= bezierDetail; i++) {
				Vec2 p = bezierSegment(p1, c1, c2, p2, t);
				appendPath(p);
				t += dt;
			}
		}
	}
	
	/** 
	*	Bezier Splines
	*/ 
	void bezier(const Vec2 &p1, const Vec2 &c1, const Vec2 &c2, const Vec2 &p2) {
		startPath();
			CbezierSegment(p1, c1, c2, p2);
		endPath();
		glColor4fv(&strokeColor.r);
		strokeCurvedPath();
	}
	
	/** 
	*	Curves
	*/ 
	Vec2 curvePoint(const Vec2 &p1, const Vec2 &p2, const Vec2 &p3, const Vec2 &p4, float t) {
		float tt = t * t;
		float ttt = t * tt;
		
		return 
			(ttt*curveBasisMatrix.col[0].x + tt*curveBasisMatrix.col[0].y + t*curveBasisMatrix.col[0].z + curveBasisMatrix.col[0].w)*p1 +
			(ttt*curveBasisMatrix.col[1].x + tt*curveBasisMatrix.col[1].y + t*curveBasisMatrix.col[1].z + curveBasisMatrix.col[1].w)*p2 +
			(ttt*curveBasisMatrix.col[2].x + tt*curveBasisMatrix.col[2].y + t*curveBasisMatrix.col[2].z + curveBasisMatrix.col[2].w)*p3 +
			(ttt*curveBasisMatrix.col[3].x + tt*curveBasisMatrix.col[3].y + t*curveBasisMatrix.col[3].z + curveBasisMatrix.col[3].w)*p4;
	}
	
	Vec2 curveTangent(const Vec2 &p1, const Vec2 &p2, const Vec2 &p3, const Vec2 &p4, float t) {
		float tt3 = t*t*3;
		float t2 = t*2;

		return	
			(tt3*curveBasisMatrix.col[0].x + t2*curveBasisMatrix.col[0].y + curveBasisMatrix.col[0].z)*p1 +
			(tt3*curveBasisMatrix.col[1].x + t2*curveBasisMatrix.col[1].y + curveBasisMatrix.col[1].z)*p2 +
			(tt3*curveBasisMatrix.col[2].x + t2*curveBasisMatrix.col[2].y + curveBasisMatrix.col[2].z)*p3 +
			(tt3*curveBasisMatrix.col[3].x + t2*curveBasisMatrix.col[3].y + curveBasisMatrix.col[3].z)*p4;
	  }
	
	void curveSegment(const Vec2 &p1, const Vec2 &p2, const Vec2 &p3, const Vec2 &p4) {
		Vec2 p0 = p2;
		
		Vec2 plot1 =	curveDrawMatrix.col[0].y*p1 + 
						curveDrawMatrix.col[1].y*p2 + 
						curveDrawMatrix.col[2].y*p3 +
						curveDrawMatrix.col[3].y*p4;

		Vec2 plot2 =	curveDrawMatrix.col[0].z*p1 + 
						curveDrawMatrix.col[1].z*p2 + 
						curveDrawMatrix.col[2].z*p3 +
						curveDrawMatrix.col[3].z*p4;
		
		Vec2 plot3 =	curveDrawMatrix.col[0].w*p1 + 
						curveDrawMatrix.col[1].w*p2 + 
						curveDrawMatrix.col[2].w*p3 +
						curveDrawMatrix.col[3].w*p4;
		
		appendPath(p0);
		for(int i=0; i < curveDetail; i++) {
			p0 = p0+plot1;
			plot1 = plot1+plot2;
			plot2 = plot2+plot3;
			appendPath(p0);
		}
	}
	
	void curve(const Vec2 &p1, const Vec2 &p2, const Vec2 &p3, const Vec2 &p4) {
		startPath();
			curveSegment(p1, p2, p3, p4);
		endPath();
		glColor4fv(&strokeColor.r);
		strokeCurvedPath();
	}
	
	/** 
	*	Line Caps
	*/ 
	void roundCap(const Vec2 &center, const Vec2 &offset) {
		float dtheta = M_PI/15.;
		float a = cos(dtheta);
		float b = sin(dtheta);
		
		float c = offset.x;
		float s = offset.y;
		
		for(int i=0; i <= 15; i++) {
			glVertex2f(c+center.x, s+center.y);
		
			float ns = b*c + a*s;
			float nc = a*c - b*s;
			c = nc;
			s = ns;
		}
	}
	
	void projectCap(const Vec2 &center, const Vec2 &offset) {
		glVertex2f(center.x+offset.x, center.y+offset.y);
		glVertex2f(center.x+offset.x-offset.y, center.y+offset.y+offset.x);
		glVertex2f(center.x-offset.x-offset.y, center.y-offset.y+offset.x);
		glVertex2f(center.x-offset.x, center.y-offset.y);
	}
	
	void squareCap(const Vec2 &center, const Vec2 &offset) {
		glVertex2f(center.x+offset.x, center.y+offset.y);
		glVertex2f(center.x-offset.x, center.y-offset.y);
	}
	
	void cap(const Vec2 &center, const Vec2 &offset) {
		switch(strokeCap) {
			case CapSquare:		squareCap(center, offset); break;
			case CapProject:	projectCap(center, offset); break;
			case CapRound:		roundCap(center, offset); break;
		}
	}
	
	/** 
	*	Primitives
	*/
	void point(float x, float y, float z) {
		glColor4fv(&strokeColor.r);
		glBegin(GL_POINTS);
			glVertex3f(x, y, z);
		glEnd();
	}
	
	void line(const Vec2 &p1, const Vec2 &p2) {
		Vec2 tangent = p2-p1;
		tangent.normalize();
		Vec2 offset(-tangent.y*strokeWeight*0.5, tangent.x*strokeWeight*0.5);
		glColor4fv(&strokeColor.r);
		glBegin(GL_POLYGON);
			cap(p1, offset);
			cap(p2, -offset);
		glEnd();
	}
	
	void triangle(const Vec2 &v1, const Vec2 &v2, const Vec2 &v3) {
		startPath();
			appendPath(v1);
			appendPath(v2);
			appendPath(v3);
		closePath();
		drawPath();
	}
	
	void quad(const Vec2 &v1, const Vec2 &v2, const Vec2 &v3, const Vec2 &v4) {
		startPath();
			appendPath(v1);
			appendPath(v2);
			appendPath(v3);
			appendPath(v4);
		closePath();
		drawPath();
	}
	
	
	/** 
	*	Arc
	*/
	void drawArcCenter(const Vec2 &pos, const Vec2 &size, float theta1, float theta2, int n, float dtheta) {
		float rx = size.x*0.5;
		float ry = size.y*0.5;
		float a = cos(dtheta);
		float b = sin(dtheta);	
		float c = cos(theta1);
		float s = sin(theta1);
		
		for(int i=0; i <= n; i++) {
			appendPath(rx*c+pos.x, ry*s+pos.y);
			const float ns = b*c + a*s;
			const float nc = a*c - b*s;
			c = nc;
			s = ns;
		}
	}
	
	void drawArcRadius(const Vec2 &pos, const Vec2 &size, float theta1, float theta2, int n, float dtheta) {
		float rx = size.x;
		float ry = size.y;
		float a = cos(dtheta);
		float b = sin(dtheta);	
		float c = cos(theta1);
		float s = sin(theta1);
		
		for(int i=0; i <= n; i++) {
			appendPath(rx*c+pos.x, ry*s+pos.y);
			const float ns = b*c + a*s;
			const float nc = a*c - b*s;
			c = nc;
			s = ns;
		}
	}
	
	void drawArcCorner(const Vec2 &pos, const Vec2 &size, float theta1, float theta2, int n, float dtheta) {
		float cx, cy, rx, ry;		
		if(size.x > 0) {
			cx = pos.x+size.x*0.5;
			rx = size.x*0.5;
		}
		else {
			cx = pos.x+size.x*0.5;
			rx = -size.x*0.5;
		}
		if(size.y > 0) {
			cy = pos.y+size.y*0.5;
			ry = size.y*0.5;
		}
		else {
			cy = pos.y+size.y*0.5;
			ry = -size.y*0.5;
		}
		
		float a = cos(dtheta);
		float b = sin(dtheta);	
		float c = cos(theta1);
		float s = sin(theta1);
		
		for(int i=0; i <= n; i++) {
			appendPath(rx*c+cx, ry*s+cy);
			const float ns = b*c + a*s;
			const float nc = a*c - b*s;
			c = nc;
			s = ns;
		}
	}
	
	void drawArcCorners(const Vec2 &pos, const Vec2 &size, float theta1, float theta2, int n, float dtheta) {
		float cx = (pos.x+size.x)*0.5;
		float cy = (pos.y+size.y)*0.5;
		float rx = (pos.x-size.x)*0.5;
		float ry = (pos.y-size.y)*0.5;
		
		float a = cos(dtheta);
		float b = sin(dtheta);	
		float c = cos(theta1);
		float s = sin(theta1);
		
		for(int i=0; i <= n; i++) {
			appendPath(rx*c+cx, ry*s+cy);
			const float ns = b*c + a*s;
			const float nc = a*c - b*s;
			c = nc;
			s = ns;
		}
	}
	
	void drawArc(const Vec2 &pos, const Vec2 &size, float theta1, float theta2) {
		float dtheta = theta2-theta1;
		int n = (int)(dtheta/(M_PI/30));
		n = (n < 1) ? 1 : n;
		dtheta = dtheta/(float)(n);
		
		switch(ellipseMode) {
			case Center:
				drawArcCenter(pos, size, theta1, theta2, n, dtheta);
				break;
			
			case Radius:
				drawArcRadius(pos, size, theta1, theta2, n, dtheta);
				break;
			
			case Corner:
				drawArcCorner(pos, size, theta1, theta2, n, dtheta);
				break;
			
			case Corners:
				drawArcCorners(pos, size, theta1, theta2, n, dtheta);
				break;
		}
	}
	
	void arc(const Vec2 &pos, const Vec2 &size, float theta1, float theta2) {
		startPath();
			drawArc(pos, size, theta1, theta2);
			ellipseCenter(pos, size);
		closePath();
			
		if(fill) {
			glColor4fv(&fillColor.r);
			fillPath();
		}
		
		if(stroke) {
			glColor4fv(&strokeColor.r);
			strokePath(pathLength()-1);
		}
	}
	
	/** 
	*	Ellipse
	*/
	void ellipseCenter(const Vec2 &pos, const Vec2 &size) {
		switch(ellipseMode) {
			case Center:	appendPath(pos); break;
			case Radius:	appendPath(pos); break;
			case Corner:	appendPath(pos.x+size.x*0.5, pos.y+size.y*0.5); break;
			case Corners:	appendPath((pos.x+size.x)*0.5, (pos.y+size.y)*0.5); break;
		}
	}
	
	void drawEllipseCenter(const Vec2 &pos, const Vec2 &size, int n, float dtheta) {
		float rx = size.x*0.5;
		float ry = size.y*0.5;
		double a = cos(dtheta);
		double b = sin(dtheta);	
		double c = 0.;
		double s = 1.;
		
		for(int i=0; i < n; i++) {
			appendPath(rx*c+pos.x, ry*s+pos.y);
			const double ns = b*c + a*s;
			const double nc = a*c - b*s;
			c = nc;
			s = ns;
		}
	}
	
	void drawEllipseRadius(const Vec2 &pos, const Vec2 &size, int n, float dtheta) {
		float rx = size.x;
		float ry = size.y;
		double a = cos(dtheta);
		double b = sin(dtheta);	
		double c = 0.;
		double s = 1.;
		
		for(int i=0; i < n; i++) {
			appendPath(rx*c+pos.x, ry*s+pos.y);
			const double ns = b*c + a*s;
			const double nc = a*c - b*s;
			c = nc;
			s = ns;
		}
	}
	
	void drawEllipseCorner(const Vec2 &pos, const Vec2 &size, int n, float dtheta) {
		float cx, cy, rx, ry;		
		if(size.x > 0) {
			cx = pos.x+size.x*0.5;
			rx = size.x*0.5;
		}
		else {
			cx = pos.x+size.x*0.5;
			rx = -size.x*0.5;
		}
		if(size.y > 0) {
			cy = pos.y+size.y*0.5;
			ry = size.y*0.5;
		}
		else {
			cy = pos.y+size.y*0.5;
			ry = -size.y*0.5;
		}
		
		double a = cos(dtheta);
		double b = sin(dtheta);	
		double c = 0.;
		double s = 1.;
		
		for(int i=0; i < n; i++) {
			appendPath(rx*c+cx, ry*s+cy);
			const double ns = b*c + a*s;
			const double nc = a*c - b*s;
			c = nc;
			s = ns;
		}
	}
	
	void drawEllipseCorners(const Vec2 &pos, const Vec2 &size, int n, float dtheta) {
		float cx = (pos.x+size.x)*0.5;
		float cy = (pos.y+size.y)*0.5;
		float rx = (pos.x-size.x)*0.5;
		float ry = (pos.y-size.y)*0.5;
			
		double a = cos(dtheta);
		double b = sin(dtheta);	
		double c = 0.;
		double s = 1.;
		
		for(int i=0; i < n; i++) {
			appendPath(rx*c+cx, ry*s+cy);
			const double ns = b*c + a*s;
			const double nc = a*c - b*s;
			c = nc;
			s = ns;
		}
	}
	
	void drawEllipse(const Vec2 &pos, const Vec2 &size) {
		float dtheta = M_PI/30;
		
		switch(ellipseMode) {
			case Center:
				drawEllipseCenter(pos, size, 60, dtheta);
				break;
			
			case Radius:
				drawEllipseRadius(pos, size, 60, dtheta);
				break;
			
			case Corner:
				drawEllipseCorner(pos, size, 60, dtheta);
				break;
			
			case Corners:
				drawEllipseCorners(pos, size, 60, dtheta);
				break;
		}
	}
	
	void ellipse(const Vec2 &pos, const Vec2 &size) {
		startPath();
			drawEllipse(pos, size);
		closePath();
		drawCurvedPath();
	}
	
	
	/** 
	*	Rect
	*/
	void drawRectCenter(const Vec2 &pos, const Vec2 &size) {
		float rx = size.x*0.5;
		float ry = size.y*0.5;
		appendPath(pos.x+rx, pos.y+ry);
		appendPath(pos.x-rx, pos.y+ry);
		appendPath(pos.x-rx, pos.y-ry);
		appendPath(pos.x+rx, pos.y-ry);
	}
	
	void drawRectRadius(const Vec2 &pos, const Vec2 &size) {
		appendPath(pos.x+size.x, pos.y+size.y);
		appendPath(pos.x-size.x, pos.y+size.y);
		appendPath(pos.x-size.x, pos.y-size.y);
		appendPath(pos.x+size.x, pos.y-size.y);
	}
	
	void drawRectCorner(const Vec2 &pos, const Vec2 &size) {
		appendPath(pos.x, pos.y);
		appendPath(pos.x+size.x, pos.y);
		appendPath(pos.x+size.x, pos.y+size.y);
		appendPath(pos.x, pos.y+size.y);
	}
	
	void drawRectCorners(const Vec2 &pos, const Vec2 &size) {
		appendPath(pos.x, pos.y);
		appendPath(pos.x, size.y);
		appendPath(size.x, size.y);
		appendPath(size.x, pos.y);
	}
	
	void drawRect(const Vec2 &pos, const Vec2 &size) {
		switch(rectMode) {
			case Center:
				drawRectCenter(pos, size);
				break;
			
			case Radius:
				drawRectRadius(pos, size);
				break;
			
			case Corner:
				drawRectCorner(pos, size);
				break;
			
			case Corners:
				drawRectCorners(pos, size);
				break;
		}
	}
	
	void rect(const Vec2 &pos, const Vec2 &size) {
		startPath();
			drawRect(pos, size);
		closePath();
		drawPath();
	}
	
	
	/** 
	*	Paths
	*/
	void startPath() {
		vertices.clear();
	}
	
	void appendPath(const Vec2 &v) {
		vertices.push_back(v);
	}
	
	void appendPath(float x, float y) {
		vertices.push_back(Vec2(x, y));
	}
	
	void closePath() {
		pathClosed = true;
	}
	
	void endPath() {
		pathClosed = false;
	}
	
	void drawPath() {
		if(fill) {
			glColor4fv(&fillColor.r);
			fillPath();
		}
		if(stroke) {
			glColor4fv(&strokeColor.r);
			
			switch(shapePathMode) {
				case ShapePoints:			pointPath(); break;
				case ShapeLines:			linePath();	break;
				case ShapeTriangleStrip:	triangleStripPath();	break;
				default:
					strokePath();
					break;
			}
		}
	}
	
	void drawCurvedPath() {
		if(fill) {
			glColor4fv(&fillColor.r);
			fillPath();
		}
		if(stroke) {
			glColor4fv(&strokeColor.r);
			strokeCurvedPath();
		}
	}
	
	int pathLength() {
		return vertices.size();
	}
	
	Vec2 pathOffset(const Vec2 &v1, const Vec2 &v2) {
		Vec2 tangent = v2-v1;
		tangent.normalize();
		
		return Vec2(
			-tangent.y*strokeWeight*0.5,
			tangent.x*strokeWeight*0.5
		);
	}
	
	void offsetCorners(
		const Vec2 &v1, const Vec2 &v2, 
		Vec2 &p1, Vec2 &p2, Vec2 &p3, Vec2 &p4
	) {
		Vec2 offset = pathOffset(v1, v2);
		p1 = v1+offset;
		p2 = v2+offset;
		p3 = v1-offset;
		p4 = v2-offset;
	}
	
	void strokeClosedPathBevel(int n, const vector<Vec2> &vertices) {
		if(n == 2) {
			const Vec2 *v1 = &(vertices[0]);
			const Vec2 *v2 = v2+1;
			Vec2 vp0, vp1, vn0, vn1;
			offsetCorners(*v1, *v2, vp0, vp1, vn0, vn1);
			glBegin(GL_QUAD_STRIP);
				glVertex2fv(&vp0.x);
				glVertex2fv(&vn0.x);
				glVertex2fv(&vp1.x);
				glVertex2fv(&vn1.x);
			glEnd();
		}
		else {
			const Vec2 *v1 = &(vertices[n-1]);
			const Vec2 *v2 = &(vertices[0]);
			const Vec2 *v3 = v2+1;
			
			Vec2 vp0, vp1, vn0, vn1;
			Vec2 vp2, vp3, vn2, vn3;
			
			offsetCorners(*v1, *v2, vp0, vp1, vn0, vn1);
			offsetCorners(*v2, *v3, vp2, vp3, vn2, vn3);
			
			Vec2 p1 = line_intersect(vp0, vp1, vp2, vp3);
			
			glBegin(GL_QUAD_STRIP);
				glVertex2fv(&p1.x);
				glVertex2fv(&vn1.x);
				
				glVertex2fv(&p1.x);
				glVertex2fv(&vn2.x);
				
				for(int i=1; i < n-1; i++) {
					v1 = v2;
					v2 = v3;
					v3++;
					
					Vec2 vpA, vpB, vnA, vnB;
					offsetCorners(*v2, *v3, vpA, vpB, vnA, vnB);
					
					Vec2 pA = line_intersect(vp2, vp3, vpA, vpB);

					glVertex2fv(&pA.x);
					glVertex2fv(&vn3.x);
					
					glVertex2fv(&pA.x);
					glVertex2fv(&vnA.x);
					
					vp2 = vpA;
					vp3 = vpB;
					vn2 = vnA;
					vn3 = vnB;
				}
				
				{
					v1 = v2;
					v2 = v3;
					v3 = &(vertices[0]);
					
					Vec2 pA = line_intersect(vp2, vp3, vp0, vp1);
					
					glVertex2fv(&pA.x);
					glVertex2fv(&vn3.x);
					
					glVertex2fv(&pA.x);
					glVertex2fv(&vn0.x);
					
					glVertex2fv(&p1.x);
					glVertex2fv(&vn1.x);
				}
				
			glEnd();
		}
	}
	
	void strokeClosedPathMiter(int n, const vector<Vec2> &vertices) {
		if(n == 2) {
			const Vec2 *v1 = &(vertices[0]);
			const Vec2 *v2 = v2+1;
			Vec2 vp0, vp1, vn0, vn1;
			offsetCorners(*v1, *v2, vp0, vp1, vn0, vn1);
			glBegin(GL_QUAD_STRIP);
				glVertex2fv(&vp0.x);
				glVertex2fv(&vn0.x);
				glVertex2fv(&vp1.x);
				glVertex2fv(&vn1.x);
			glEnd();
		}
		else {
			const Vec2 *v1 = &(vertices[n-1]);
			const Vec2 *v2 = &(vertices[0]);
			const Vec2 *v3 = v2+1;
			
			Vec2 vp0, vp1, vn0, vn1;
			Vec2 vp2, vp3, vn2, vn3;
			
			offsetCorners(*v1, *v2, vp0, vp1, vn0, vn1);
			offsetCorners(*v2, *v3, vp2, vp3, vn2, vn3);
			
			Vec2 p1 = line_intersect(vp0, vp1, vp2, vp3);
			Vec2 p2 = line_intersect(vn0, vn1, vn2, vn3);
			
			glBegin(GL_QUAD_STRIP);
				glVertex2fv(&p1.x);
				glVertex2fv(&p2.x);
				
				for(int i=1; i < n-1; i++) {
					v1 = v2;
					v2 = v3;
					v3++;
					
					Vec2 vpA, vpB, vnA, vnB;
					offsetCorners(*v2, *v3, vpA, vpB, vnA, vnB);
					
					Vec2 pA = line_intersect(vp2, vp3, vpA, vpB);
					Vec2 pB = line_intersect(vn2, vn3, vnA, vnB);

					glVertex2fv(&pA.x);
					glVertex2fv(&pB.x);
					
					vp2 = vpA;
					vp3 = vpB;
					vn2 = vnA;
					vn3 = vnB;
				}
				
				{
					v1 = v2;
					v2 = v3;
					v3 = &(vertices[0]);
					
					Vec2 pA = line_intersect(vp2, vp3, vp0, vp1);
					Vec2 pB = line_intersect(vn2, vn3, vn0, vn1);
					
					glVertex2fv(&pA.x);
					glVertex2fv(&pB.x);
					
					glVertex2fv(&p1.x);
					glVertex2fv(&p2.x);
				}
				
			glEnd();
		}
	}
	
	void joinRound(const Vec2 &center, const Vec2 &p1, const Vec2 &p2) {
		Vec2 v1 = p1-center;
		Vec2 v2 = p2-center;
		float mag = v1.mag();
		float dtheta = acosf(dot(v1, v2)/(mag*mag));
		int n = (int)(dtheta/(M_PI/8));
		dtheta = dtheta/(float)n;
		
		float a = cos(dtheta);
		float b = sin(dtheta);	
		float c = v1.x;
		float s = v1.y;
		
		for(int i=0; i < n; i++) {
			glVertex2f(c+center.x, s+center.y);
			const float ns = b*c + a*s;
			const float nc = a*c - b*s;
			c = nc;
			s = ns;
		}
		glVertex2fv(&p2.x);
	}
	
	void strokeClosedPathRound(int n, const vector<Vec2> &vertices) {
		if(n == 2) {
			const Vec2 *v1 = &(vertices[0]);
			const Vec2 *v2 = v2+1;
			Vec2 vp0, vp1, vn0, vn1;
			offsetCorners(*v1, *v2, vp0, vp1, vn0, vn1);
			glBegin(GL_QUAD_STRIP);
				glVertex2fv(&vp0.x);
				glVertex2fv(&vn0.x);
				glVertex2fv(&vp1.x);
				glVertex2fv(&vn1.x);
			glEnd();
		}
		else {
			const Vec2 *v1 = &(vertices[n-1]);
			const Vec2 *v2 = &(vertices[0]);
			const Vec2 *v3 = v2+1;
			
			Vec2 vp0, vp1, vn0, vn1;
			Vec2 vp2, vp3, vn2, vn3;
			
			offsetCorners(*v1, *v2, vp0, vp1, vn0, vn1);
			offsetCorners(*v2, *v3, vp2, vp3, vn2, vn3);
			
			Vec2 p1 = line_intersect(vp0, vp1, vp2, vp3);
			
			
			glBegin(GL_TRIANGLE_FAN);
				glVertex2fv(&p1.x);
				joinRound(*v2, vn1, vn2);
			glEnd();

			glBegin(GL_QUAD_STRIP);
				glVertex2fv(&p1.x);
				glVertex2fv(&vn2.x);
				
				for(int i=1; i < n-1; i++) {
					v1 = v2;
					v2 = v3;
					v3++;
					
					Vec2 vpA, vpB, vnA, vnB;
					offsetCorners(*v2, *v3, vpA, vpB, vnA, vnB);
					
					Vec2 pA = line_intersect(vp2, vp3, vpA, vpB);

					glVertex2fv(&pA.x);
					glVertex2fv(&vn3.x);
			glEnd();
			
					glBegin(GL_TRIANGLE_FAN);
						glVertex2fv(&pA.x);
						joinRound(*v2, vn3, vnA);
					glEnd();
			
			
			glBegin(GL_QUAD_STRIP);
					glVertex2fv(&pA.x);
					glVertex2fv(&vnA.x);
					
					vp2 = vpA;
					vp3 = vpB;
					vn2 = vnA;
					vn3 = vnB;
				}
				
				{
					v1 = v2;
					v2 = v3;
					v3 = &(vertices[0]);
					
					Vec2 pA = line_intersect(vp2, vp3, vp0, vp1);
					
					glVertex2fv(&pA.x);
					glVertex2fv(&vn3.x);
			glEnd();
			
					glBegin(GL_TRIANGLE_FAN);
						glVertex2fv(&pA.x);
						joinRound(*v2, vn3, vn0);
					glEnd();
			
			
			glBegin(GL_QUAD_STRIP);
					glVertex2fv(&pA.x);
					glVertex2fv(&vn0.x);
					
					glVertex2fv(&p1.x);
					glVertex2fv(&vn1.x);
				}
				
			glEnd();
		}
	}
	
	void strokeClosedPath(int n, const vector<Vec2> &vertices) {
		switch(strokeJoin) {
			case JoinBevel:	strokeClosedPathBevel(n, vertices); break;
			case JoinMiter:	strokeClosedPathMiter(n, vertices); break;
			case JoinRound:	strokeClosedPathRound(n, vertices); break;
			
		}
	}
	
	void strokePath(int n=0) {
		if(n <= 0) {
			n = vertices.size();
		}
		if(n >= 2) {
			if(pathClosed) {
				strokeClosedPath(n, vertices);
			}
			else {
				const Vec2 *v1 = &(vertices[0]);
				const Vec2 *v2 = v1+1;
				
				Vec2 offset;
				
				glBegin(GL_QUAD_STRIP);
					for(int i=1; i < n; i++) {
						offset = pathOffset(*v1, *v2);
						glVertex2f(v1->x+offset.x, v1->y+offset.y);
						glVertex2f(v1->x-offset.x, v1->y-offset.y);
						
						glVertex2f(v2->x+offset.x, v2->y+offset.y);
						glVertex2f(v2->x-offset.x, v2->y-offset.y);
						
						v1 = v2;
						v2++;
					}
				glEnd();
			}
		}
	}
	
	void strokeCurvedPath(int n=0) {
		if(n <= 0) {
			n = vertices.size();
		}
		if(n >= 2) {
			if(pathClosed) {
				const Vec2 *v1 = &(vertices[n-1]);
				const Vec2 *v2 = &(vertices[0]);
				
				Vec2 offset = pathOffset(*v1, *v2);
				
				Vec2 p1(v1->x+offset.x, v1->y+offset.y);
				Vec2 p2(v1->x-offset.x, v1->y-offset.y);
				
				v1 = v2;
				v2 = v2++;
				
				glBegin(GL_QUAD_STRIP);
					glVertex2fv(&p1.x);
					glVertex2fv(&p2.x);
					
					for(int i=1; i < n; i++) {
						offset = pathOffset(*v1, *v2);
						glVertex2f(v1->x+offset.x, v1->y+offset.y);
						glVertex2f(v1->x-offset.x, v1->y-offset.y);
						
						v1 = v2;
						v2 = v2++;
					}

					glVertex2fv(&p1.x);
					glVertex2fv(&p2.x);
				glEnd();
			}
			else {
				const Vec2 *v1 = &(vertices[0]);
				const Vec2 *v2 = v1+1;
				
				Vec2 offset = pathOffset(*v1, *v2);
				
				glBegin(GL_POLYGON);
					cap(*v1, offset);
				glEnd();
				
				glBegin(GL_QUAD_STRIP);
					glVertex2f(v1->x+offset.x, v1->y+offset.y);
					glVertex2f(v1->x-offset.x, v1->y-offset.y);
					
					v1 = v2;
					v2 = v2++;
				
					for(int i=1; i < n-1; i++) {
						offset = pathOffset(*v1, *v2);
						glVertex2f(v1->x+offset.x, v1->y+offset.y);
						glVertex2f(v1->x-offset.x, v1->y-offset.y);
						
						v1 = v2;
						v2 = v2++;
					}
					
					glVertex2f(v1->x+offset.x, v1->y+offset.y);
					glVertex2f(v1->x-offset.x, v1->y-offset.y);
				glEnd();
				
				glBegin(GL_POLYGON);
					cap(*v1, -offset);
				glEnd();
			}
		}
	}
	
	void fillPath() {
		int n = vertices.size();
		if(n >= 2) {
			switch(shapePathMode) {
				case ShapeTriangleStrip:
					glBegin(GL_TRIANGLE_STRIP); {
						const Vec2 *v1 = &(vertices[0]);
						for(int i=0; i < n; i++) {
							glVertex2fv(&v1->x);
							v1++;
						}
						if(pathClosed) {
							v1 = &(vertices[0]);
							glVertex2fv(&v1->x);
						}
					}
					glEnd();
					break;
				
				
				default:
					glBegin(GL_POLYGON); {
						const Vec2 *v1 = &(vertices[0]);
						for(int i=0; i < n; i++) {
							glVertex2fv(&v1->x);
							v1++;
						}
						if(pathClosed) {
							v1 = &(vertices[0]);
							glVertex2fv(&v1->x);
						}
					}
					glEnd();
					break;
			}
		}
	}
	
	void pointPath() {
		int n = vertices.size();
		if(n >= 2) {
			glBegin(GL_POINTS);
				const Vec2 *v1 = &(vertices[0]);
				for(int i=0; i < n; i++) {
					glVertex2fv(&v1->x);
					v1++;
				}
			glEnd();
		}
	}
	
	void linePath(int n=0) {
		if(n <= 0) {
			n = vertices.size();
		}
		if(n >= 2) {
			const Vec2 *v1 = &(vertices[0]);
			const Vec2 *v2 = v1+1;
			
			Vec2 offset;
			
			glBegin(GL_QUADS);
				for(int i=0; i < n; i+=2) {
					offset = pathOffset(*v1, *v2);
					glVertex2f(v1->x+offset.x, v1->y+offset.y);
					glVertex2f(v1->x-offset.x, v1->y-offset.y);
					
					glVertex2f(v2->x-offset.x, v2->y-offset.y);
					glVertex2f(v2->x+offset.x, v2->y+offset.y);
					
					
					v1 += 2;
					v2 += 2;
				}
			glEnd();
		}
	}
	
	
	void triangleStripPath(int n=0) {
		if(n <= 0) {
			n = vertices.size();
		}
		if(n >= 3) {
			vector<Vec2> tri(3);
			for(int i=0; i < n-2; i++) {
				tri[0] = vertices[i];
				tri[1] = vertices[i+1];
				tri[2] = vertices[i+2];
				strokeClosedPath(3, tri);
			}
		}
	}
	
	/**
	*	Shapes
	*/
	
	void beginShape(ShapePathMode mode) {
		shapePathMode = mode;
		shapeVertices.clear();
	}
	
	bool isValidShapeVertex(ShapeVertexType type) {
		if(type == BezierControl) {
			int n = shapeVertices.size();
			if(n < 1) {
				return false;
			}
		}
		return true;
	}
	
	void appendShape(ShapeVertexType type, const Vec2 &v) {
		shapeVertices.push_back(ShapeVertex(type, v));
	}
	
	void appendShape(ShapeVertexType type, float x, float y) {
		shapeVertices.push_back(ShapeVertex(type, Vec2(x, y)));
	}
	
	void appendBezierVertex(const Vec2 &c1, const Vec2 &c2, const Vec2 &v) {
		shapeVertices.push_back(ShapeVertex(BezierControl, c1));
		shapeVertices.push_back(ShapeVertex(BezierControl, c2));
		shapeVertices.push_back(ShapeVertex(BezierVertex, v));
	}
	
	void endShape(bool close=false) {
		int n = shapeVertices.size();
		const ShapeVertex *v1 = &(shapeVertices[0]);
		int i=0;
		
		startPath();
		while(i < n) {
			switch(v1->type) {
				case Vertex:
					appendPath(v1->vertex);
					v1++;
					i++;
					break;
					
				case BezierControl:
					CbezierSegment(v1[-1].vertex, v1[0].vertex, v1[1].vertex, v1[2].vertex, false);
					v1 += 3;
					i += 3;
					break;
					
				case CurveVertex:
					if(i > 0 && i < n-2) {
						curveSegment(v1[-1].vertex, v1[0].vertex, v1[1].vertex, v1[2].vertex);
					}
					v1++;
					i++;
					break;
			
			}
			
		}
		if(close) {
			closePath();
		}
		else {
			endPath();
		}
		
		drawPath();
	}

	void closeShape() {
		endShape(true);
	}
	
	
	ShapeMode ellipseMode;
	ShapeMode rectMode;
	StrokeCap strokeCap;
	StrokeJoin strokeJoin;
	float strokeWeight;
	ColorMode colorMode;
	Color strokeColor;
	Color fillColor;
	float colorRange[4];
	bool stroke;
	bool fill;
	bool pathClosed;
	vector<Vec2> vertices;
	int bezierDetail;
	float curveTightness;
	int curveDetail;
	Mat4<float> curveBasisMatrix;
	Mat4<float> curveDrawMatrix;
	ShapePathMode shapePathMode;
	vector<ShapeVertex> shapeVertices;
};


/*! High-level graphics interface for 2D drawing
	Draw is a high-level graphics interface for 2D drawing.  It has a number of modes for 
	filling and stroking common 2D shapes and curves that includes line cap and join 
	styles.
	
	@module opengl.Draw
*/

/*! Draw a line
	@param p1 The first point
	@param p2 The second point
	@LuaMethod METHOD
	@name M:line
*/
int lua_draw_line(lua_State *L) {
	Draw *s = Glue<Draw>::checkto(L, 1);
	Vec2 p1, p2;
	lua::to_vec_t<float>(L, 2, 2, &p1.x);
	lua::to_vec_t<float>(L, 3, 2, &p2.x);
	s->line(p1, p2);
	return 0;
}

/*! Draw a Bezier curve
	@param  p1 The first point
	@param c1 The first control point
	@param c2 The second control point
	@param p2 The second point
	@LuaMethod METHOD
	@name M:bezier
*/
int lua_draw_bezier(lua_State *L) {
	Draw *s = Glue<Draw>::checkto(L, 1);
	Vec2 p1, c1, c2, p2;
	lua::to_vec_t<float>(L, 2, 2, &p1.x);
	lua::to_vec_t<float>(L, 3, 2, &c1.x);
	lua::to_vec_t<float>(L, 4, 2, &c2.x);
	lua::to_vec_t<float>(L, 5, 2, &p2.x);
	s->bezier(p1, c1, c2, p2);
	return 0;
}

/*! Bezier detail
	@LuaMethod GETSET
	@name M.bezierDetail
*/
int lua_draw_bezierDetail(lua_State *L) {
	Draw *s = Glue<Draw>::checkto(L, 1);
	if(lua::is<int>(L, 2)) {
		s->bezierDetail = lua::to<int>(L, 2);
	}
	else {
		lua::push<int>(L, s->bezierDetail);
		return 1;
	}
	return 0;
}

/*! Bezier point
	@param p1 The first point
	@param c1 The first control point
	@param c2 The second control point
	@param p2 The second point
	@param t The position along the curve [0, 1]
	@ret The point
	@LuaMethod METHOD
	@name M:bezierPoint
*/
int lua_draw_bezierPoint(lua_State *L) {
	Draw *s = Glue<Draw>::checkto(L, 1);
	Vec2 p1, c1, c2, p2;
	lua::to_vec_t<float>(L, 2, 2, &p1.x);
	lua::to_vec_t<float>(L, 3, 2, &c1.x);
	lua::to_vec_t<float>(L, 4, 2, &c2.x);
	lua::to_vec_t<float>(L, 5, 2, &p2.x);
	float t = lua::to<float>(L, 6);
	Vec2 p = bezierSegment(p1, c1, c2, p2, t);
	lua::push_vec_t<float>(L, 2, &p.x);
	return 1;
}

/*! Bezier tangent
	@param p1 The first point
	@param c1 The first control point
	@param c2 The second control point
	@param p2 The second point
	@param t The position along the curve [0, 1]
	@ret The tangent
	@LuaMethod METHOD
	@name M:bezierTangent
*/
int lua_draw_bezierTangent(lua_State *L) {
	Draw *s = Glue<Draw>::checkto(L, 1);
	Vec2 p1, c1, c2, p2;
	lua::to_vec_t<float>(L, 2, 2, &p1.x);
	lua::to_vec_t<float>(L, 3, 2, &c1.x);
	lua::to_vec_t<float>(L, 4, 2, &c2.x);
	lua::to_vec_t<float>(L, 5, 2, &p2.x);
	float t = lua::to<float>(L, 6);
	Vec2 p = bezierTangent(p1, c1, c2, p2, t);
	lua::push_vec_t<float>(L, 2, &p.x);
	return 1;
}

/*! Curve tightness
	@LuaMethod GETSET
	@name M.curveTightness
*/
int lua_draw_curveTightness(lua_State *L) {
	Draw *s = Glue<Draw>::checkto(L, 1);
	if(lua::is<float>(L, 2)) {
		s->setCurveTightness(lua::to<float>(L, 2));
	}
	else {
		lua::push<float>(L, s->curveTightness);
		return 1;
	}
	return 0;
}

/*! Curve detail
	@LuaMethod GETSET
	@name M.curveDetail
*/
int lua_draw_curveDetail(lua_State *L) {
	Draw *s = Glue<Draw>::checkto(L, 1);
	if(lua::is<int>(L, 2)) {
		s->setCurveDetail(lua::to<int>(L, 2));
	}
	else {
		lua::push<int>(L, s->curveDetail);
		return 1;
	}
	return 0;
}

/*! Draw a curve
	@param p1 The first point
	@param p2 The second point
	@param p3 The third point
	@param p4 The fourth point
	@LuaMethod METHOD
	@name M:curve
*/
int lua_draw_curve(lua_State *L) {
	Draw *s = Glue<Draw>::checkto(L, 1);
	Vec2 p1, p2, p3, p4;
	lua::to_vec_t<float>(L, 2, 2, &p1.x);
	lua::to_vec_t<float>(L, 3, 2, &p2.x);
	lua::to_vec_t<float>(L, 4, 2, &p3.x);
	lua::to_vec_t<float>(L, 5, 2, &p4.x);
	s->curve(p1, p2, p3, p4);
	return 0;
}

/*! Curve point
	@param p1 The first point
	@param p2 The second point
	@param p3 The third point
	@param p4 The fourth point
	@param t The position along the curve [0, 1]
	@ret The point
	@LuaMethod METHOD
	@name M:curvePoint
*/
int lua_draw_curvePoint(lua_State *L) {
	Draw *s = Glue<Draw>::checkto(L, 1);
	Vec2 p1, p2, p3, p4;
	lua::to_vec_t<float>(L, 2, 2, &p1.x);
	lua::to_vec_t<float>(L, 3, 2, &p2.x);
	lua::to_vec_t<float>(L, 4, 2, &p3.x);
	lua::to_vec_t<float>(L, 5, 2, &p4.x);
	float t = lua::to<float>(L, 6);
	Vec2 p = s->curvePoint(p1, p2, p3, p4, t);
	lua::push_vec_t<float>(L, 2, &p.x);
	return 1;
}

/*! Curve tangent
	@param p1 The first point
	@param p2 The second point
	@param p3 The third point
	@param p4 The fourth point
	@param t The position along the curve [0, 1]
	@ret The tangent
	@LuaMethod METHOD
	@name M:curveTangent
*/
int lua_draw_curveTangent(lua_State *L) {
	Draw *s = Glue<Draw>::checkto(L, 1);
	Vec2 p1, p2, p3, p4;
	lua::to_vec_t<float>(L, 2, 2, &p1.x);
	lua::to_vec_t<float>(L, 3, 2, &p2.x);
	lua::to_vec_t<float>(L, 4, 2, &p3.x);
	lua::to_vec_t<float>(L, 5, 2, &p4.x);
	float t = lua::to<float>(L, 6);
	Vec2 p = s->curveTangent(p1, p2, p3, p4, t);
	lua::push_vec_t<float>(L, 2, &p.x);
	return 1;
}

/*! Begin drawing a shape
	Possible values for mode are no argument, Draw.NONE, Draw.POINTS,
	Draw.LINES, Draw.TRIANGLES, Draw.TRIANGLE_STRIP, Draw.TRIANGLE_FAN,
	Draw.QUADS
	@param [mode] The drawing mode
	@LuaMethod METHOD
	@name M:beginShape
*/
int lua_draw_beginShape(lua_State *L) {
	Draw *s = Glue<Draw>::checkto(L, 1);
	Draw::ShapePathMode mode = (Draw::ShapePathMode)lua::opt<int>(L, 2, Draw::ShapeNone);
	s->beginShape(mode);
	return 0;
}

/*! Add a vertex to a shape
	@param v The vertex
	@LuaMethod METHOD
	@name M:vertex
*/
int lua_draw_vertex(lua_State *L) {
	Draw *s = Glue<Draw>::checkto(L, 1);
	Vec2 v;
	lua::to_vec_t<float>(L, 2, 2, &v.x);
	s->appendShape(Draw::Vertex, v);
	
	return 0;
}

/*! Add a Bezier vertex to a shape
	This function must be preceeded by a call to vertex.
	@param c1 The first control point
	@param c2 The second control point
	@param v The vertex
	@LuaMethod METHOD
	@name M:bezierVertex
*/
int lua_draw_bezierVertex(lua_State *L) {
	Draw *s = Glue<Draw>::checkto(L, 1);
	Vec2 c1, c2, v;
	lua::to_vec_t<float>(L, 2, 2, &c1.x);
	lua::to_vec_t<float>(L, 3, 2, &c2.x);
	lua::to_vec_t<float>(L, 4, 2, &v.x);
	if(s->isValidShapeVertex(Draw::BezierControl)) {
		s->appendBezierVertex(c1, c2, v);
	}
	else {
		luaL_error(L, "Draw.bezierVertex: Bezier vertices must be preceeded by a vertex");
	}
	return 0;
}

/*! Add a curve vertex to a shape
	@param v The vertex
	@LuaMethod METHOD
	@name M:curveVertex
*/
int lua_draw_curveVertex(lua_State *L) {
	Draw *s = Glue<Draw>::checkto(L, 1);
	Vec2 v;
	lua::to_vec_t<float>(L, 2, 2, &v.x);
	s->appendShape(Draw::CurveVertex, v);
	return 0;
}

/*! End a shape
	Possible arguments are no arugment or Draw.CLOSE
	@param [close] The close flag
	@LuaMethod METHOD
	@name M:endShape
*/
int lua_draw_endShape(lua_State *L) {
	Draw *s = Glue<Draw>::checkto(L, 1);
	Draw::ShapePathMode mode = (Draw::ShapePathMode)lua::opt<int>(L, 2, Draw::ShapeNone);
	if(mode == Draw::ShapeClose) {
		s->closeShape();
	}
	else {
		s->endShape();
	}
	return 0;
}

/*! Stroke cap
	Possible values are Draw.SQUARE, Draw.PROJECT, Draw.ROUND
	@LuaMethod GETSET
	@name M.strokeCap
*/
int lua_draw_strokeCap(lua_State *L) {
	Draw *s = Glue<Draw>::checkto(L, 1);
	if(lua::is<int>(L, 2)) {
		s->strokeCap = (Draw::StrokeCap)lua::to<int>(L, 2);
	}
	else {
		lua::push<int>(L, s->strokeCap);
		return 1;
	}
	return 0;
}

/*! Stroke join
	Possible values are Draw.MITER, Draw.BEVEL, Draw.ROUND
	@LuaMethod GETSET
	@name M.strokeJoin
*/
int lua_draw_strokeJoin(lua_State *L) {
	Draw *s = Glue<Draw>::checkto(L, 1);
	if(lua::is<int>(L, 2)) {
		s->strokeJoin = (Draw::StrokeJoin)lua::to<int>(L, 2);
	}
	else {
		lua::push<int>(L, s->strokeJoin);
		return 1;
	}
	return 0;
}

/*! Stroke weight
	The weight of a stroked path.
	@LuaMethod GETSET
	@name M.strokeWeight
*/
int lua_draw_strokeWeight(lua_State *L) {
	Draw *s = Glue<Draw>::checkto(L, 1);
	if(lua::is<float>(L, 2)) {
		s->strokeWeight = lua::to<float>(L, 2);
	}
	else {
		lua::push<float>(L, s->strokeWeight);
		return 1;
	}
	return 0;
}

/*! Rect drawing mode
	Possible values are Draw.CENTER, Draw.RADIUS, Draw.CORNER, Draw.CORNERS
	@LuaMethod GETSET
	@name M.rectMode
*/
int lua_draw_rectMode(lua_State *L) {
	Draw *s = Glue<Draw>::checkto(L, 1);
	if(lua::is<int>(L, 2)) {
		s->rectMode = (Draw::ShapeMode)lua::to<int>(L, 2);
	}
	else {
		lua::push<int>(L, s->rectMode);
		return 1;
	}
	return 0;
}

/*! Ellipse drawing mode
	Possible values are Draw.CENTER, Draw.RADIUS, Draw.CORNER, Draw.CORNERS
	@LuaMethod GETSET
	@name M.ellipseMode
*/
int lua_draw_ellipseMode(lua_State *L) {
	Draw *s = Glue<Draw>::checkto(L, 1);
	if(lua::is<int>(L, 2)) {
		s->ellipseMode = (Draw::ShapeMode)lua::to<int>(L, 2);
	}
	else {
		lua::push<int>(L, s->ellipseMode);
		return 1;
	}
	return 0;
}

/*! Fill flag
	Sets whether or not to fill shapes
	@LuaMethod GETSET
	@name M.fill
*/
int lua_draw_fill(lua_State *L) {
	Draw *s = Glue<Draw>::checkto(L, 1);
	if(lua::is<bool>(L, 2)) {
		s->fill = lua::to<bool>(L, 2);
	}
	else {
		lua::push<bool>(L, s->fill);
		return 1;
	}
	return 0;
}

/*! Stroke flag
	Sets whether or not to stroke shapes
	@LuaMethod GETSET
	@name M.stroke
*/
int lua_draw_stroke(lua_State *L) {
	Draw *s = Glue<Draw>::checkto(L, 1);
	if(lua::is<bool>(L, 2)) {
		s->stroke = lua::to<bool>(L, 2);
	}
	else {
		lua::push<bool>(L, s->stroke);
		return 1;
	}
	return 0;
}

/*! Draw an arc
	Arc drawing is affected by ellipseMode.
	@param pos The position of the arc
	@param size The size of the arc
	@LuaMethod METHOD
	@name M:arc
*/
int lua_draw_arc(lua_State *L) {
	Draw *s = Glue<Draw>::checkto(L, 1);
	Vec2 pos;
	Vec2 size;
	lua::to_vec_t<float>(L, 2, 2, &pos.x);
	lua::to_vec_t<float>(L, 3, 2, &size.x);
	
	float theta1 = lua::opt<float>(L, 4, 0.);
	float theta2 = lua::opt<float>(L, 5, M_PI*0.25); 
	
	s->arc(pos, size, theta1, theta2);
	
	return 0;
}

/*! Draw an ellipse
	Arc drawing is affected by ellipseMode.
	@param pos The position of the ellipse
	@param size The size of the ellipse
	@LuaMethod METHOD
	@name M:ellipse
*/
int lua_draw_ellipse(lua_State *L) {
	Draw *s = Glue<Draw>::checkto(L, 1);
	Vec2 pos;
	Vec2 size;
	lua::to_vec_t<float>(L, 2, 2, &pos.x);
	lua::to_vec_t<float>(L, 3, 2, &size.x);
	
	s->ellipse(pos, size);
	
	return 0;
}

/*! Draw a point
	@param pos The position of the point
	@LuaMethod METHOD
	@name M:point
*/
int lua_draw_point(lua_State *L) {
	Draw *s = Glue<Draw>::checkto(L, 1);
	float v[] = {0, 0, 0};
	lua::to_vec_t<float>(L, 2, 3, v);
	
	s->point(v[0], v[1], v[2]);
	
	return 0;
}

/*! Draw a triangle
	@param p1 First position
	@param p2 Second position
	@param p3 Third position
	@LuaMethod METHOD
	@name M:triangle
*/
int lua_draw_triangle(lua_State *L) {
	Draw *s = Glue<Draw>::checkto(L, 1);
	Vec2 v1, v2, v3;
	lua::to_vec_t<float>(L, 2, 2, &v1.x);
	lua::to_vec_t<float>(L, 3, 2, &v2.x);
	lua::to_vec_t<float>(L, 4, 2, &v3.x);
	
	s->triangle(v1, v2, v3);
	
	return 0;
}

/*! Draw a quad
	@param p1 First position
	@param p2 Second position
	@param p3 Third position
	@param p4 Fourth position
	@LuaMethod METHOD
	@name M:quad
*/
int lua_draw_quad(lua_State *L) {
	Draw *s = Glue<Draw>::checkto(L, 1);
	Vec2 v1, v2, v3, v4;
	lua::to_vec_t<float>(L, 2, 2, &v1.x);
	lua::to_vec_t<float>(L, 3, 2, &v2.x);
	lua::to_vec_t<float>(L, 4, 2, &v3.x);
	lua::to_vec_t<float>(L, 5, 2, &v4.x);
	
	s->quad(v1, v2, v3, v4);
	
	return 0;
}

/*! Draw a rectangle
	Rectangle drawing is affected by rectMode
	@param pos The position of the rectangle
	@param size The size of the rectangle
	@LuaMethod METHOD
	@name M:rect
*/
int lua_draw_rect(lua_State *L) {
	Draw *s = Glue<Draw>::checkto(L, 1);
	Vec2 v1, v2;
	lua::to_vec_t<float>(L, 2, 2, &v1.x);
	lua::to_vec_t<float>(L, 3, 2, &v2.x);
	
	s->rect(v1, v2);
	
	return 0;
}

static float Hue_2_RGB(float v1, float v2, float vH ) {
   if ( vH < 0 ) vH += 1;
   if ( vH > 1 ) vH -= 1;
   if ( ( 6 * vH ) < 1 ) return ( v1 + ( v2 - v1 ) * 6 * vH );
   if ( ( 2 * vH ) < 1 ) return ( v2 );
   if ( ( 3 * vH ) < 2 ) return ( v1 + ( v2 - v1 ) * ( ( 2./3 ) - vH ) * 6 );
   return ( v1 );
}

void ccHSLtoRGB(float h, float s, float l, float* r, float* g, float* b) {
	float v1, v2;

	if(s == 0) {
		*r = l;
		*g = l;
		*b = l;
	}
	else {
		if ( l < 0.5 ) { 
			v2 = l * ( 1 + s );
		}
		else {
			v2 = ( l + s ) - ( s * l );
		}

		v1 = 2 * l - v2;

		*r = Hue_2_RGB( v1, v2, h + ( 1./3 ) );
		*g = Hue_2_RGB( v1, v2, h );
		*b = Hue_2_RGB( v1, v2, h - ( 1./3 ) );
	} 
}

int lua_tocolor(lua_State *L, int idx, Color &c, bool rgb, float *range) {
	float v[4];
	int n = 4;
	lua::to_vec_n(L, idx, n, v);
	switch(n) {
		case 1:	
			c.r = v[0]/range[0];
			c.g = v[0]/range[1];
			c.b = v[0]/range[2];
			c.a = range[3];
			break;
			
		case 2:
			c.r = v[0]/range[0];
			c.g = v[0]/range[1];
			c.b = v[0]/range[2];
			c.a = v[1]/range[3];
			break;
			
		case 3:
			if(rgb) {
				c.r = v[0]/range[0];
				c.g = v[1]/range[1];
				c.b = v[2]/range[2];
			}
			else {
				float r, g, b;
				ccHSLtoRGB(v[0]/range[0], v[1]/range[1], v[2]/range[2], &r, &g, &b);
				c.r = r;
				c.g = g;
				c.b = b;
			}
			c.a = range[3];
			break;
			
		case 4:
			if(rgb) {
				c.r = v[0]/range[0];
				c.g = v[1]/range[1];
				c.b = v[2]/range[2];
			}
			else {
				float r, g, b;
				ccHSLtoRGB(v[0]/range[0], v[1]/range[1], v[2]/range[2], &r, &g, &b);
				c.r = r;
				c.g = g;
				c.b = b;
			}
			c.a = v[3]/range[3];
			break;
	}
	return 0;
}


/*! Fill color
	@LuaMethod GETSET
	@name M:fillColor
*/
int lua_draw_fillColor(lua_State *L) {
	Draw *s = Glue<Draw>::checkto(L, 1);
	if(lua_gettop(L) >= 2) {
		lua_tocolor(L, 2, s->fillColor, s->colorMode == Draw::RGB, s->colorRange);
	}
	else {
		lua::push_vec_t<float>(L, 4, &s->fillColor.r);
		return 1;
	}
	return 0;
}

/*! Stroke color
	@LuaMethod GETSET
	@name M:strokeColor
*/
int lua_draw_strokeColor(lua_State *L) {
	Draw *s = Glue<Draw>::checkto(L, 1);
	if(lua_gettop(L) >= 2) {
		lua_tocolor(L, 2, s->strokeColor, s->colorMode == Draw::RGB, s->colorRange);
	}
	else {
		lua::push_vec_t<float>(L, 4, &s->strokeColor.r);
		return 1;
	}
	return 0;
}

/*! The color mode
	Possible values are Draw.RGB and draw.HSB
	@LuaMethod GETSET
	@name M:colorMode
*/
int lua_draw_colorMode(lua_State *L) {
	Draw *s = Glue<Draw>::checkto(L, 1);
	if(lua::is<int>(L, 2)) {
		s->colorMode = (Draw::ColorMode)lua::to<int>(L, 2);
	}
	else {
		lua::push<int>(L, s->colorMode);
		return 1;
	}
	return 0;
}

/*! The color range
	@LuaMethod GETSET
	@name M:colorRange
*/
int lua_draw_colorRange(lua_State *L) {
	Draw *s = Glue<Draw>::checkto(L, 1);
	if(lua_istable(L, 2)) {
		lua::to_vec(L, 2, 4, s->colorRange);
	}
	else {
		lua::push_vec_t<float>(L, 4, s->colorRange);
		return 1;
	}
	return 0;
}


template<> const char * Glue<Draw>::usr_name() { return "Draw"; }
template<> bool Glue<Draw>::usr_has_index() { return true; }

template<> void Glue<Draw>::usr_push(lua_State * L, Draw * u) {
	Glue<Draw>::usr_attr_push(L);
	Glue<Draw>::usr_attr_prototype(L);
}

template<> Draw * Glue<Draw>::usr_new(lua_State * L) {
	Draw *draw = new Draw();
	return draw;
}

template<> void Glue<Draw>::usr_index(lua_State * L, Draw * u) {
	Glue<Draw>::usr_attr_index(L);
}

template<> void Glue<Draw>::usr_newindex(lua_State * L, Draw * u) {
	Glue<Draw>::usr_attr_newindex(L);
}

template<> int Glue<Draw>::usr_tostring(lua_State * L, Draw * u) {
	lua_pushfstring(L, "%s: %p", Glue<Draw>::usr_name(), u); 
	return 1;
}

template<> void Glue<Draw>::usr_gc(lua_State * L, Draw * u) { 
	delete u;
}

template<> void Glue<Draw>::usr_mt(lua_State * L) {
	static luaL_reg methods[] = {
		{ "line", lua_draw_line },
		{ "bezier", lua_draw_bezier },
		{ "bezierPoint", lua_draw_bezierPoint },
		{ "bezierTangent", lua_draw_bezierTangent },
		{ "curve", lua_draw_curve },
		{ "curvePoint", lua_draw_curvePoint },
		{ "curveTangent", lua_draw_curveTangent },
		{ "beginShape", lua_draw_beginShape },
		{ "vertex", lua_draw_vertex },
		{ "bezierVertex", lua_draw_bezierVertex },
		{ "curveVertex", lua_draw_curveVertex },
		{ "endShape", lua_draw_endShape },
		{ "arc", lua_draw_arc },
		{ "ellipse", lua_draw_ellipse },
		{ "point", lua_draw_point },
		{ "triangle", lua_draw_triangle },
		{ "quad", lua_draw_quad },
		{ "rect", lua_draw_rect },
		{ NULL, NULL}
	};
	static luaL_reg getters[] = {
		{ "bezierDetail", lua_draw_bezierDetail },
		{ "curveTightness", lua_draw_curveTightness },
		{ "curveDetail", lua_draw_curveDetail },
		{ "strokeCap", lua_draw_strokeCap },
		{ "strokeJoin", lua_draw_strokeJoin },
		{ "strokeWeight", lua_draw_strokeWeight },
		{ "rectMode", lua_draw_rectMode },
		{ "ellipseMode", lua_draw_ellipseMode },
		{ "fill", lua_draw_fill },
		{ "stroke", lua_draw_stroke },
		{ "fillColor", lua_draw_fillColor },
		{ "strokeColor", lua_draw_strokeColor },
		{ "colorMode", lua_draw_colorMode },
		{ "colorRange", lua_draw_colorRange },
		{ NULL, NULL}
	};
	static luaL_reg setters[] = {
		{ "bezierDetail", lua_draw_bezierDetail },
		{ "curveTightness", lua_draw_curveTightness },
		{ "curveDetail", lua_draw_curveDetail },
		{ "strokeCap", lua_draw_strokeCap },
		{ "strokeJoin", lua_draw_strokeJoin },
		{ "strokeWeight", lua_draw_strokeWeight },
		{ "rectMode", lua_draw_rectMode },
		{ "ellipseMode", lua_draw_ellipseMode },
		{ "fill", lua_draw_fill },
		{ "stroke", lua_draw_stroke },
		{ "fillColor", lua_draw_fillColor },
		{ "strokeColor", lua_draw_strokeColor },
		{ "colorMode", lua_draw_colorMode },
		{ "colorRange", lua_draw_colorRange },
		{ NULL, NULL}
	};
	Glue<Draw>::usr_attr_mt(L, methods, getters, setters);
}

#ifdef __cplusplus
extern "C" {
#endif

int luaopen_opengl_Draw(lua_State *L) {
	Glue<Draw>::define(L);
	Glue<Draw>::register_class(L);
	luaL_getmetatable(L, Glue<Draw>::mt_name(L));
	
	lua::pushfield<int>(L, -1, "CENTER", Draw::Center);
	lua::pushfield<int>(L, -1, "RADIUS", Draw::Radius);
	lua::pushfield<int>(L, -1, "CORNER", Draw::Corner);
	lua::pushfield<int>(L, -1, "CORNERS", Draw::Corners);
	lua::pushfield<int>(L, -1, "SQUARE", Draw::CapSquare);
	lua::pushfield<int>(L, -1, "PROJECT", Draw::CapProject);
	lua::pushfield<int>(L, -1, "ROUND", Draw::CapRound);
	lua::pushfield<int>(L, -1, "MITER", Draw::JoinMiter);
	lua::pushfield<int>(L, -1, "BEVEL", Draw::JoinBevel);
	lua::pushfield<int>(L, -1, "ROUND", Draw::JoinRound);
	lua::pushfield<int>(L, -1, "RGB", Draw::RGB);
	lua::pushfield<int>(L, -1, "HSB", Draw::HSB);
	lua::pushfield<int>(L, -1, "POINTS", Draw::ShapePoints);
	lua::pushfield<int>(L, -1, "LINES", Draw::ShapeLines);
	lua::pushfield<int>(L, -1, "TRIANGLES", Draw::ShapeTriangles);
	lua::pushfield<int>(L, -1, "TRIANGLE_STRIP", Draw::ShapeTriangleStrip);
	lua::pushfield<int>(L, -1, "TRIANGLE_FAN", Draw::ShapeTriangleFan);
	lua::pushfield<int>(L, -1, "QUADS", Draw::ShapeQuads);
	lua::pushfield<int>(L, -1, "CLOSE", Draw::ShapeClose);

	return 1;
}

#ifdef __cplusplus
}
#endif
