#include "graphics/al_Mesh.hpp"
#include "allocore/system/al_Config.h"
#include "protocol/al_GraphicsBackend.hpp"
#include <stdio.h>

namespace al {
namespace gfx{

Mesh :: Mesh(GraphicsBackend *backend) 
:	mBackend(backend),
	mPrimitive(POINTS)
{
	//printf("created mesh with backend %p\n", backend);
}

void Mesh::resetBuffers() {
	vertices().clear();
	normals().clear();
	colors().clear();
	texCoord2s().clear();
	texCoord3s().clear();
	indices().clear();	
}

void Mesh::equalizeBuffers() {
	int VS = vertices().size();
	int NS = normals().size();
	int CS = colors().size();
	int T2S = texCoord2s().size();
	int T3S = texCoord3s().size();
	
	if(NS > 0) {
		for(int i=NS; i < VS; i++) {
			normals().append(normals()[NS-1]);
		}
	}
	if(CS > 0) {
		for(int i=CS; i < VS; i++) {
			colors().append(colors()[CS-1]);
		}
	}
	if(T2S > 0) {
		for(int i=T2S; i < VS; i++) {
			texCoord2s().append(texCoord2s()[T2S-1]);
		}
	}
	if(T3S > 0) {
		for(int i=T3S; i < VS; i++) {
			texCoord3s().append(texCoord3s()[T3S-1]);
		}
	}
}

class TriFace {
public:
	Mesh::Vertex vertices[3];
	Vec3f norm;
	
	TriFace(const TriFace& cpy) 
	: norm(cpy.norm) {	
		vertices[0] = cpy.vertices[0];
		vertices[1] = cpy.vertices[1];
		vertices[2] = cpy.vertices[2];
	}
	TriFace(Mesh::Vertex p0, Mesh::Vertex p1, Mesh::Vertex p2)
	{
		vertices[0] = p0;
		vertices[1] = p1;
		vertices[2] = p2;
		// calculate norm for this face:
		normal<float>(norm, p0, p1, p2);
	}
};

void Mesh::generateNormals(float angle) {
//	/*
//		Multi-pass algorithm:
//			generate a list of faces (assume triangles?)
//				(vary according to whether mIndices is used)
//			calculate normal per face (use normal<float>(dst, p0, p1, p2))
//			vertices may be used in multiple faces; their norm should be an average of the uses
//				easy enough if indices is being used; not so easy otherwise. 
//					create a lookup table by hashing on vertex x,y,z
//			
//			
//			write avg into corresponding normals for each vertex
//				EXCEPT: if edge is sharper than @angle, just use the face normal directly
//	*/
//	std::vector<TriFace> faces;
//	
//	std::map<std::string, int> vertexHash;
//	
//	int Ni = indices().size();
//	int Nv = vertices().size();
//	if (Ni) {
//		for (int i=0; i<Ni;) {
//			TriFace face(
//				mVertices[mIndices[i++]],
//				mVertices[mIndices[i++]],
//				mVertices[mIndices[i++]]
//			);
//			faces.push_back(face);
//		}
//	} else {
//		for (int i=0; i<Nv;) {
//			TriFace face(	
//				mVertices[i++],
//				mVertices[i++],
//				mVertices[i++]
//			);
//			faces.push_back(face);
//		}
//	}
}

void Mesh::getBounds(Vertex& min, Vertex& max) {
	if (mVertices.size() > 0) {
		min.set(mVertices[0][0], mVertices[0][1], mVertices[0][2]);
		min.set(mVertices[0][0], mVertices[0][1], mVertices[0][2]);
		for (int v=1; v<mVertices.size(); v++) {
			Vertex& vt = mVertices[v];
			for (int i=0; i<3; i++) {
				min[i] = MIN(min[i], vt[i]);
				max[i] = MAX(max[i], vt[i]);
			}
		}
	}
}

void Mesh::unitize() {
	Vertex min(0), max(0);
	getBounds(min, max);
	Vertex avg = (max-min)*0.5;
	for (int v=0; v<mVertices.size(); v++) {
		Vertex& vt = mVertices[v];
		for (int i=0; i<3; i++) {
			vt[i] = -1. + (vt[i]-min[i])/avg[i];
		}
	}
}

Vec3f Mesh::getCenter() {
	Vertex min(0), max(0);
	getBounds(min, max);
	return min+(max-min)*0.5;
}

void Mesh::translate(double x, double y, double z) {
	for (int v=0; v<mVertices.size(); v++) {
		Vertex& vt = mVertices[v];
		vt[0] += x;
		vt[1] += y;
		vt[2] += z;
	}
}

void Mesh::scale(double x, double y, double z) {
	for (int v=0; v<mVertices.size(); v++) {
		Vertex& vt = mVertices[v];
		vt[0] *= x;
		vt[1] *= y;
		vt[2] *= z;
	}
}

void Mesh::draw() {
//	printf("backend %p\n", mBackend);
	//printf("prim %d\n", (int)primitive());
	//

//	mBackend->test(*this);
	mBackend->draw(*this);
//	printf("%d\n", mBackend->test());
}

void Mesh::vertex(AlloArray *lat) {
	if(
		lat->header.components == 3 &&
		lat->header.type == AlloFloat32Ty && 
		lat->header.dimcount <= 2
	) {
		int w = lat->header.dim[0];
		int h = (lat->header.dimcount < 2) ? 1 : lat->header.dim[1];
		int ncells = w*h;
		mVertices.size(ncells);
		
		Vertex *v = mVertices.elems();
		int rowstride = lat->header.stride[1];
		int rowsize = w * 3 * allo_type_size(AlloFloat32Ty);
		for(int j=0; j < h; j++) {
			char *vdata = (char *)(v + j*w);
			char *data = lat->data.ptr + j*rowstride;
			memcpy(vdata, data, rowsize);
		}
	}
	else {
		fprintf(stderr, "Mesh::vertex: array must be 3 component Float32");
	}
}

void Mesh::normal(AlloArray *lat) {
	if(
		lat->header.components == 3 &&
		lat->header.type == AlloFloat32Ty && 
		lat->header.dimcount <= 2
	) {
		int w = lat->header.dim[0];
		int h = (lat->header.dimcount < 2) ? 1 : lat->header.dim[1];
		int ncells = w*h;
		mNormals.size(ncells);
		
		Normal *v = mNormals.elems();
		int rowstride = lat->header.stride[1];
		int rowsize = w * 3 * allo_type_size(AlloFloat32Ty);
		for(int j=0; j < h; j++) {
			char *vdata = (char *)(v + j*w);
			char *data = lat->data.ptr + j*rowstride;
			memcpy(vdata, data, rowsize);
		}
	}
	else {
		fprintf(stderr, "Mesh::normal: array must be 3 component Float32");
	}
}

void Mesh::color(AlloArray *lat) {
	if(
		lat->header.components == 4 &&
		lat->header.type == AlloFloat32Ty && 
		lat->header.dimcount <= 2
	) {
		int w = lat->header.dim[0];
		int h = (lat->header.dimcount < 2) ? 1 : lat->header.dim[1];
		int ncells = w*h;
		mColors.size(ncells);
		
		Color *v = mColors.elems();
		int rowstride = lat->header.stride[1];
		int rowsize = w * 4 * allo_type_size(AlloFloat32Ty);
		for(int j=0; j < h; j++) {
			char *vdata = (char *)(v + j*w);
			char *data = lat->data.ptr + j*rowstride;
			memcpy(vdata, data, rowsize);
		}
	}
	else {
		fprintf(stderr, "Mesh::color: array must be 4 component Float32");
	}
}

void Mesh::texcoord(AlloArray *lat) {
	if(
		(lat->header.components == 2 || lat->header.components == 3) &&
		lat->header.type == AlloFloat32Ty && 
		lat->header.dimcount <= 2
	) {
		int components = lat->header.components;
		int w = lat->header.dim[0];
		int h = (lat->header.dimcount < 2) ? 1 : lat->header.dim[1];
		int ncells = w*h;
		
		if(components == 2) {
			mTexCoord2s.size(ncells);
		}
		else {
			mTexCoord3s.size(ncells);
		}
		
		char *v = (components == 2 ? (char *)mTexCoord2s.elems() : (char *)mTexCoord3s.elems());
		int rowstride = lat->header.stride[1];
		int rowsize = w * components * allo_type_size(AlloFloat32Ty);
		for(int j=0; j < h; j++) {
			char *vdata = (char *)(v + j*rowsize);
			char *data = lat->data.ptr + j*rowstride;
			memcpy(vdata, data, rowsize);
		}
	}
	else {
		fprintf(stderr, "Mesh::texcoord: array must be 2 or 3 component Float32");
	}
}

void Mesh::index(AlloArray *lat) {
	if(
		lat->header.components == 1 &&
		lat->header.type == AlloUInt32Ty && 
		lat->header.dimcount <= 2
	) {
		int w = lat->header.dim[0];
		int h = (lat->header.dimcount < 2) ? 1 : lat->header.dim[1];
		int ncells = w*h;
		mIndices.size(ncells);
		
		Index *v = mIndices.elems();
		int rowstride = lat->header.stride[1];
		int rowsize = w * allo_type_size(AlloUInt32Ty);
		for(int j=0; j < h; j++) {
			char *vdata = (char *)(v + j*w);
			char *data = lat->data.ptr + j*rowstride;
			memcpy(vdata, data, rowsize);
		}
	}
	else {
		fprintf(stderr, "Mesh::index: array must be 1 component UInt32");
	}
}

} // ::al::gfx
} // ::al
