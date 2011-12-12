#ifndef AL_MESH_H
#define AL_MESH_H 1

#include "al_Graphics.hpp"
#include "math/al_Vec.hpp"
#include "types/al_Buffer.hpp"
#include "allocore/types/al_Array.hpp"

namespace al {
namespace gfx {

class Mesh {
public:

	typedef Vec3f	Vertex;
	typedef Vec3f	Normal;
	typedef Vec4f	Color;
	typedef Vec2f	TexCoord2;
	typedef Vec3f	TexCoord3;
	typedef unsigned int	Index;
	
	/// TODO!
	Mesh(GraphicsBackend *backend);

	/// Reset all buffers
	void resetBuffers();
	void equalizeBuffers();
	void getBounds(Vec3f& min, Vec3f& max);
	Vec3f getCenter(); // center at 0,0,0
	
	// destructive edits to internal vertices:
	void unitize();	/// scale to -1..1
	void scale(double x, double y, double z);
	void scale(Vec3f v) { scale(v[0], v[1], v[2]); }
	void scale(double s) { scale(s, s, s); }
	void translate(double x, double y, double z);
	void translate(Vec3f v) { translate(v[0], v[1], v[2]); }
	
	// generates smoothed normals for a set of vertices
	// will replace any normals currently in use
	// angle - maximum angle (in degrees) to smooth across
	void generateNormals(float angle);

	Primitive primitive() const { return mPrimitive; } 
	const Buffer<Vertex>& vertices() const { return mVertices; }
	const Buffer<Normal>& normals() const { return mNormals; }
	const Buffer<Color>& colors() const { return mColors; }
	const Buffer<TexCoord2>& texCoord2s() const { return mTexCoord2s; }
	const Buffer<TexCoord3>& texCoord3s() const { return mTexCoord3s; }
	const Buffer<Index>& indices() const { return mIndices; }

	void addIndex(unsigned int i){ indices().append(i); }
	void addColor(float r, float g, float b, float a=1){ colors().append(Color(r,g,b,a)); }
	void addColor(Color& v) { colors().append(v); }
	void addNormal(float x, float y, float z=0){ normals().append(Normal(x,y,z)); }
	void addNormal(Normal& v) { normals().append(v); }
	void addTexCoord(float u, float v){ texCoord2s().append(TexCoord2(u,v)); }
	void addTexCoord(float u, float v, float w){ texCoord3s().append(TexCoord3(u,v,w)); }
	void addTexCoord(TexCoord2& v){ texCoord2s().append(v); }
	void addTexCoord(TexCoord3& v){ texCoord3s().append(v); }
	void addVertex(float x, float y, float z=0){ vertices().append(Vertex(x,y,z)); }
	void addVertex(Vertex& v){ vertices().append(v); }
	void primitive(Primitive prim){ mPrimitive=prim; }

	Buffer<Vertex>& vertices(){ return mVertices; }
	Buffer<Normal>& normals(){ return mNormals; }
	Buffer<Color>& colors(){ return mColors; }
	Buffer<TexCoord2>& texCoord2s(){ return mTexCoord2s; }
	Buffer<TexCoord3>& texCoord3s(){ return mTexCoord3s; }
	Buffer<Index>& indices(){ return mIndices; }
	
	
	void draw();
	void vertex(AlloArray *lat);
	void normal(AlloArray *lat);
	void color(AlloArray *lat);
	void texcoord(AlloArray *lat);
	void index(AlloArray *lat);

protected:
	GraphicsBackend *mBackend;

	// Only populated (size>0) buffers will be used
	Buffer<Vertex> mVertices;
	Buffer<Normal> mNormals;
	Buffer<Color> mColors;
	Buffer<TexCoord2> mTexCoord2s;
	Buffer<TexCoord3> mTexCoord3s;
	
	Buffer<Index> mIndices;

	Primitive mPrimitive;
};

} // gfx::
} // al::

#endif // AL_MESH_H
