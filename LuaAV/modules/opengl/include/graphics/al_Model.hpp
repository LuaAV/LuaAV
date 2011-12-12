#ifndef INCLUDE_AL_GRAPHICS_MODEL_HPP
#define INCLUDE_AL_GRAPHICS_MODEL_HPP

/*
 *  A collection of functions and classes related to application mainloops
 *  AlloSphere Research Group / Media Arts & Technology, UCSB, 2009
 */

/*
	Copyright (C) 2006-2008. The Regents of the University of California (REGENTS). 
	All Rights Reserved.

	Permission to use, copy, modify, distribute, and distribute modified versions
	of this software and its documentation without fee and without a signed
	licensing agreement, is hereby granted, provided that the above copyright
	notice, the list of contributors, this paragraph and the following two paragraphs 
	appear in all copies, modifications, and distributions.

	IN NO EVENT SHALL REGENTS BE LIABLE TO ANY PARTY FOR DIRECT, INDIRECT,
	SPECIAL, INCIDENTAL, OR CONSEQUENTIAL DAMAGES, INCLUDING LOST PROFITS, ARISING
	OUT OF THE USE OF THIS SOFTWARE AND ITS DOCUMENTATION, EVEN IF REGENTS HAS
	BEEN ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.

	REGENTS SPECIFICALLY DISCLAIMS ANY WARRANTIES, INCLUDING, BUT NOT LIMITED TO,
	THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR
	PURPOSE. THE SOFTWARE AND ACCOMPANYING DOCUMENTATION, IF ANY, PROVIDED
	HEREUNDER IS PROVIDED "AS IS". REGENTS HAS  NO OBLIGATION TO PROVIDE
	MAINTENANCE, SUPPORT, UPDATES, ENHANCEMENTS, OR MODIFICATIONS.
*/

#include <string>
#include <vector>
#include <map>

//#include "allocore/graphics/al_Graphics.hpp"
//#include "allocore/graphics/al_Light.hpp"
#include "protocol/al_Graphics.hpp"
#include "graphics/al_Mesh.hpp"

#define MODEL_PARSER_BUF_LEN (256)

namespace al{
namespace gfx {


/// OBJ file reader. Parses .obj (and .mtl) files. A separate object will be created per group and/or usemtl command.
/// it stores the OBJ data intenally, divided per object. 
/// any of these objects can be instantiated (factory style) into Mesh and Materials
/// the names of objects/materials are available for iteration. 
/// Typically, after parsing the OBJ and instantiating the desired objects and materials, the OBJReader itself can be discarded. 
class OBJReader {
public:

	OBJReader() {}
	OBJReader(std::string fullpath) { readOBJ(fullpath); }
	~OBJReader() {}
	
	void readOBJ(std::string fullpath);
	
	struct Group {
		std::vector<int> indices;
		std::string material;
	};
	
	typedef std::map<std::string, Group>::iterator GroupIterator;
	GroupIterator groupsBegin() { return mGroups.begin(); }
	GroupIterator groupsEnd() { return mGroups.end(); }
	GroupIterator groupsFind(std::string name) { return mGroups.find(name); }
	
	// returns NULL if the group is not found, or has no vertices
	Mesh * createMesh(GroupIterator group, GraphicsBackend *backend);
	
	struct Mtl {
		float shininess, optical_density;
		Color diffuse, ambient, specular;
		std::string diffuseMap, ambientMap, specularMap, bumpMap;
	};
	
	typedef std::map<std::string, Mtl>::iterator MtlIterator;
	MtlIterator materialsBegin() { return mMaterials.begin(); }
	MtlIterator materialsEnd() { return mMaterials.end(); }
	MtlIterator materialsFind(std::string name) { return mMaterials.find(name); }

protected:
	
	std::string mPath;				/* path to this model */
	std::string mFilename;				/* path to this model */
	std::string mMaterialLib;       /* name of the material library */
	
	FILE * file;
	char buf[MODEL_PARSER_BUF_LEN];
	
	struct FaceVertex {
		int vertex, texcoord, normal;
		
		FaceVertex(unsigned int vertex, unsigned int texcoord, unsigned int normal) : vertex(vertex), texcoord(texcoord), normal(normal) {}
		FaceVertex(const FaceVertex& cpy) : vertex(cpy.vertex), texcoord(cpy.texcoord), normal(cpy.normal) {}
	};
	
	std::map<std::string, Group> mGroups;
	std::map<std::string, Mtl> mMaterials;

	/// Strips a qualified path to a file (src) into a path to the containing folder (dst)
	void path2dir(char* dst, const char* src);
	
	// parser:
	void readToken();
	bool hasToken();
	void readLine();
	void eatLine();
	std::string parseMaterial();
	std::string parseMaterialLib();
	std::string parseGroup();
	void parseVertex();
	void parseTexcoord();
	void parseNormal();
	void parseColor(Color& c);
	void parseFace(Group * g);
	
	void readMTL(std::string path, std::string name);
	
	int findFaceVertex(std::string s);
	int addFaceVertex(std::string buf, int v, int t, int n);
	void addTriangle(Group * g, unsigned int id0, unsigned int id1, unsigned int id2);
	
	std::vector<Mesh::Vertex> vertices;
	std::vector<Mesh::TexCoord2> texcoords;
	std::vector<Mesh::Normal> normals;
	std::vector<FaceVertex> face_vertices;
	
	// maps face vertices (as string) to corresponding this->vertices index
	// this way, avoid inserting the same vertex/tex/norm combo twice,
	// and use index buffer instead.
	std::map<std::string, int> vertexMap;
	
	

};

} // ::gfx
} // ::al

#endif
