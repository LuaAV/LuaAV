#include <stdio.h>
#include <string.h>

#include "graphics/al_Model.hpp"
//#include "allocore/io/al_File.hpp"

namespace al{
using namespace gfx;

#define AL_PATH_MAX (4096)
void OBJReader :: path2dir(char* dst, const char* src) {
    char* s;
	snprintf(dst, AL_PATH_MAX, "%s", src);
    s = strrchr(dst, '/');
    if (s)
        s[1] = '\0';
    else
        dst[0] = '\0';
}

void OBJReader :: readOBJ(std::string fullpath) {
	std::string mtl = "default";
	std::string group = "default";
	std::string groupname = "default:default";
	
	char path[AL_PATH_MAX];
	path2dir(path, fullpath.c_str());

    // open the file
    file = fopen(fullpath.data(), "r");
    if (!file) {
        fprintf(stderr, "Model readOBJ failed: can't open data file \"%s\".\n",
            fullpath.data());
		return;
    }
	mPath = path;

	//printf("reading OBJ: \"%s\".\n", fullpath.data());

	// create a default group:
	Group * g = &mGroups["default"];

	vertices.push_back(Mesh::Vertex());
	texcoords.push_back(Mesh::TexCoord2());
	normals.push_back(Mesh::Normal());

	readToken();
	while(hasToken()) {
		switch(buf[0]) {
			case 'u':	// usemtl
				//printf("group %s has %d indices\n", groupname.data(), g->indices.size());
				mtl = parseMaterial();
				// if current group is not empty, create new group:
				if (g->indices.size() > 0) {
					groupname = group + ":" + mtl;
					//printf("group %s\n", groupname.data());
					g = &mGroups[groupname];
				}
				g->material = mtl;
                break;

			case 'm':
				mMaterialLib = parseMaterialLib();
				break;

			case 'g':
				// new group:
				//printf("group %s has %d indices\n", groupname.data(), g->indices.size());
				group = parseGroup();
				groupname = group + ":" + mtl;
				//printf("group %s\n", groupname.data());
				g = &mGroups[groupname];
				g->material = mtl;
				break;

			case 'v':               // v, vn, vt
				switch(buf[1]) {
					case '\0':
						parseVertex();
						break;
					case 'n':
						parseNormal();
						break;
					case 't':
						parseTexcoord();
						break;
					default:
						printf("Model: Unknown token \"%s\".\n", buf);
						return;
						break;
				}
				break;	// end of case 'v'

			case 'f':
				parseFace(g);
				break;

			default:
				eatLine();
				break;
        }
	}

//	printf("parsed %d vertices %d texcoords %d normals %d face_vertices\n", vertices.size(), texcoords.size(), normals.size(), face_vertices.size());
//
//	for (int i=0; i<vertices.size(); i++) {
//		printf("%d %f %f %f\n", i, vertices[i][0], vertices[i][1], vertices[i][2]);
//	}
//
//	for (int i=0; i<face_vertices.size(); i++) {
//		printf("%d %d %d %d\n", i, face_vertices[i].vertex, face_vertices[i].texcoord, face_vertices[i].normal);
//	}


	OBJReader::GroupIterator iter = groupsBegin();
	while (iter != groupsEnd()) {

//		Group& g = mGroups[iter->first];

//		printf("group %s with %d indices\n", (iter->first).data(), g.indices.size());
//		for (int i=0; i<g.indices.size(); i++) {
//			int faceindex = g.indices[i];
//			int vertexindex = face_vertices[faceindex].vertex;
//			printf("\t%d group index %d face_vertex %d\n", i, faceindex, vertexindex);
//			printf("\t\t%f %f %f\n",
//				vertices[vertexindex][0],
//				vertices[vertexindex][1],
//				vertices[vertexindex][2]
//			);
//		}


		iter++;
	}

    fclose(file);
    readMTL(mPath, mMaterialLib);
}


// @http://www.fileformat.info/format/material/
void OBJReader :: readMTL(std::string path, std::string name)
{
    std::string filename = path + name;

	//printf("Reading material file: %s\n", filename.data());

    file = fopen(filename.data(), "r");
    if (!file) {
        fprintf(stderr, "readMTL() failed: can't open material file \"%s\".\n",
            filename.data());
        return;
    }

	Color color;
	float shininess;
	//int illum;
	//float Tf[3];
	//float dissolve;
	//float sharpness;
	float optical_density;

	Mtl * m = &mMaterials["default"];

	readToken();
	while(hasToken()) {
		//printf("> %s\n", buf);
		switch(buf[0]) {

			case 'n':               // newmtl
				readLine();
				sscanf(buf, "%s %s", buf, buf);
				//printf(">>> mtl %s\n", buf);
				m = &mMaterials[buf];
				readToken();
				break;

			case 'N':
				switch(buf[1]) {
					case 's':
						readLine();
						sscanf(buf, "%f", &shininess);
						// wavefront shininess is from [0, 1000], so scale for OpenGL
						m->shininess = shininess*128./1000.;
						readToken();
						break;
					case 'i':
						readLine();
						sscanf(buf, "%f", &optical_density);
						// wavefront shininess is from [0, 1000], so scale for OpenGL
						m->shininess = (optical_density*128./1000.);
						readToken();
						break;
					default:
						eatLine();
						break;
					}
				break;
			case 'K':
				switch(buf[1]) {
					case 'd':
						parseColor(m->diffuse);
						break;
					case 's':
						parseColor(m->specular);
						break;
					case 'a':
						parseColor(m->ambient);
						break;
					default:
						eatLine();
						break;
					}
				break;
			case 'm':
				switch(buf[4]) {
					case 'K':
						switch(buf[5]) {
							case 'a':
								fscanf(file, "%s", buf);
								m->ambientMap = buf;
								readToken();
								break;
							case 'd':
								fscanf(file, "%s", buf);
								m->diffuseMap = buf;
								readToken();
								break;
							case 's':
								fscanf(file, "%s", buf);
								m->specularMap = buf;
								readToken();
								break;
							default:
								printf("unhandled: %s\n", buf);
								fgets(buf, MODEL_PARSER_BUF_LEN, file);
								break;
							}
						break;
					default:
						printf("unhandled: %s\n", buf);
						eatLine();
						break;
					}
				break;

			case 'b':
				switch(buf[1]) {
					case 'u':
						fscanf(file, "%s", buf);
						m->bumpMap = buf;
						readToken();
						break;
					default:
						printf("unhandled: %s\n", buf);
						eatLine();
						break;
					}
				break;
//			case 'i':
//				// illumination model
////
////					0	 Color on and Ambient off
////					1	 Color on and Ambient on
////					2	 Highlight on
////					3	 Reflection on and Ray trace on
////					4	 Transparency: Glass on
////					Reflection: Ray trace on
////					5	 Reflection: Fresnel on and Ray trace on
////					6	 Transparency: Refraction on
////					Reflection: Fresnel off and Ray trace on
////					7	 Transparency: Refraction on
////					Reflection: Fresnel on and Ray trace on
////					8	 Reflection on and Ray trace off
////					9	 Transparency: Glass on
////					Reflection: Ray trace off
////					10	 Casts shadows onto invisible surfaces
////
//				fscanf(file, "%d", &illum);
//				current->illumination(illum);
//				break;
//			case 'T':
//				// transmission filter
//				// (what colors are filtered by this object as light passes through)
//				// TODO: handle 'spectral' and 'xyz' flags
//				fscanf(file, "%f %f %f", &Tf[0], &Tf[1], &Tf[2]);
//				printf("unhandled: Tf: %f %f %f\n", Tf[0], Tf[1], Tf[2]);
//				break;
//			case 'd':
//				// todo: handle -halo flag
//				fscanf(file, "%f", &dissolve);
//				printf("unhandled: d: %f\n", dissolve);
//				break;
//			case 's':
//				// sharpness
//				fscanf(file, "%f", &sharpness);
//				printf("unhandled: sharpness: %f\n", sharpness);
//				break;

				/**/
			default:
				eatLine();
				break;
        }
	}

	//int nummaterials = mMaterials.size();
	//printf("%d materials\n", nummaterials);

	// dump materials:
	
	MtlIterator iter = materialsBegin();
	while (iter != materialsEnd()) {
		//printf("material %s\n", iter->first.data());
		//Mtl& m = iter->second;
		//printf("N %f\n", m.shininess());
		//printf("d %f %f %f\n", m.diffuse.r, m.diffuse.g, m.diffuse.b);
		//printf("s %f %f %f\n", m.specular.r, m.specular.g, m.specular.b);
		//printf("a %f %f %f\n", m.ambient.r, m.ambient.g, m.ambient.b);
		//printf("map a: %s s: %s d: %s\n", m.ambientMap.data(), m.specularMap.data(), m.diffuseMap.data());
		iter++;
	}
	

    fclose(file);
}


Mesh * OBJReader::createMesh(GroupIterator iter, GraphicsBackend *backend) {
	if (iter == mGroups.end()) return NULL;
	Group& g = iter->second;

	//printf("create with %d indices\n", (int)g.indices.size());
	if (g.indices.size() <= 0) return NULL;

	Mesh * gd = new Mesh(backend);
	gd->primitive(gfx::TRIANGLES);
	gd->resetBuffers();

	// memoize face_vertices index -> graphicsdata index
	std::map<unsigned int, unsigned int> indexMap;

	for (unsigned int i=0; i<g.indices.size(); i++) {
		int faceindex = g.indices[i];

		if (indexMap.find(faceindex) == indexMap.end()) {
			indexMap[faceindex] = gd->indices().size();

			int vertexindex = face_vertices[faceindex].vertex;
			int texindex = face_vertices[faceindex].texcoord;
			int normalindex = face_vertices[faceindex].normal;
			int newindex = gd->vertices().size();

			gd->addIndex(newindex);
			gd->addVertex(vertices[vertexindex]);
			gd->addTexCoord(texcoords[texindex]);
			gd->addNormal(normals[normalindex]);
		} else {

			// re-use existing index:
			int newindex = gd->indices()[indexMap[faceindex]];
			gd->addIndex(newindex);
		}

	}

	// don't create empty models
	if (gd->indices().size() == 0) {
		delete gd;
		return NULL;
	}

	//printf("%d input indices, %d vertices, %d indices\n", (int)g.indices.size(), gd->vertices().size(), gd->indices().size());

	return gd;
}

void OBJReader::readToken() {
	if (fscanf(file, "%s", buf) == EOF)	{
		buf[0] = '\0';
	}
}

bool OBJReader::hasToken() {
	return buf[0] != '\0';
}

void OBJReader::readLine() {
	fgets(buf, MODEL_PARSER_BUF_LEN, file);
}

void OBJReader::eatLine() {
	readLine();
	readToken();
}

static std::string nastyHackCombineWords(const char * src) {

	char outbuf[MODEL_PARSER_BUF_LEN];
	// nasty hack.
	for (int i=0; i<MODEL_PARSER_BUF_LEN-1; i++) {
		char c = src[i+1];
		if (c == '\n') {
			outbuf[i] = '\0';
			break;
		} else if (c == ' ') {
			outbuf[i] = '_';
		} else {
			outbuf[i] = c;
		}
	}
	return std::string(outbuf);
}

std::string OBJReader::parseMaterial() {
	readLine();
	std::string res = nastyHackCombineWords(buf);
	readToken();
	return res;
}

std::string OBJReader::parseMaterialLib() {
	readLine();
	sscanf(buf, "%s %s", buf, buf);
	std::string lib = buf;
	readToken();
	return lib;
}

std::string OBJReader::parseGroup() {
	readLine();
	std::string res = nastyHackCombineWords(buf);
	readToken();
	return res;
}

void OBJReader::parseVertex() {
	Mesh::Vertex vertex;
	fscanf(file, "%f %f %f",
		&vertex[0],
		&vertex[1],
		&vertex[2]);
	vertices.push_back(vertex);
	readToken();
}

void OBJReader::parseColor(Color& color) {
	fscanf(file, "%f %f %f",
		&color.r,
		&color.g,
		&color.b);
	readToken();
}

void OBJReader::parseTexcoord() {
	Mesh::TexCoord2 texcoord;
	fscanf(file, "%f %f",
		&texcoord[0],
		&texcoord[1]);
	texcoords.push_back(texcoord);
	readToken();
}

void OBJReader::parseNormal() {
	Mesh::Normal normal;
	fscanf(file, "%f %f %f",
		&normal[0],
		&normal[1],
		&normal[2]);
	normals.push_back(normal);
	readToken();
}

void OBJReader::parseFace(Group * g) {
	int v, t, n;
	// indices into the group's data().vertices() buffer.
	// vertexMap holds a reference from vertex token string to this index
	int id0, id1, id2;

	//std::map<std::string, int>::iterator iter;

	readToken();
	/* can be one of %d, %d//%d, %d/%d, %d/%d/%d %d//%d */
	if (strstr(buf, "//")) {
		sscanf(buf, "%d//%d", &v, &n);
		id0 = addFaceVertex(buf, v, 0, n);
		sscanf(buf, "%d//%d", &v, &n);
		id1 = addFaceVertex(buf, v, 0, n);
		sscanf(buf, "%d//%d", &v, &n);
		id2 = addFaceVertex(buf, v, 0, n);
		addTriangle(g, id0, id1, id2);

		while(sscanf(buf, "%d//%d", &v, &n) == 2) {
			id1 = id2;
			id2 = addFaceVertex(buf, v, 0, n);
			addTriangle(g, id0, id1, id2);
		}
	} else if (sscanf(buf, "%d/%d/%d", &v, &t, &n) == 3) {
		id0 = addFaceVertex(buf, v, t, n);
		sscanf(buf, "%d/%d/%d", &v, &t, &n);
		id1 = addFaceVertex(buf, v, t, n);
		sscanf(buf, "%d/%d/%d", &v, &t, &n);
		id2 = addFaceVertex(buf, v, t, n);
		addTriangle(g, id0, id1, id2);

		while(sscanf(buf, "%d/%d/%d", &v, &t, &n) == 3) {
			id1 = id2;
			id2 = addFaceVertex(buf, v, t, n);
			addTriangle(g, id0, id1, id2);
		}
	} else if (sscanf(buf, "%d/%d", &v, &t) == 2) {
		id0 = addFaceVertex(buf, v, t, 0);
		sscanf(buf, "%d/%d", &v, &t);
		id1 = addFaceVertex(buf, v, t, 0);
		sscanf(buf, "%d/%d", &v, &t);
		id2 = addFaceVertex(buf, v, t, 0);
		addTriangle(g, id0, id1, id2);

		while(sscanf(buf, "%d/%d", &v, &t) == 2) {
			id1 = id2;
			id2 = addFaceVertex(buf, v, t, 0);
			addTriangle(g, id0, id1, id2);
		}
	} else {
		sscanf(buf, "%d", &v);
		id0 = addFaceVertex(buf, v, 0, 0);
		sscanf(buf, "%d", &v);
		id1 = addFaceVertex(buf, v, 0, 0);
		sscanf(buf, "%d", &v);
		id2 = addFaceVertex(buf, v, 0, 0);
		addTriangle(g, id0, id1, id2);

		while(sscanf(buf, "%d", &v) == 1) {
			id1 = id2;
			id2 = addFaceVertex(buf, v, 0, 0);
			addTriangle(g, id0, id1, id2);
		}
	}
}

int OBJReader::findFaceVertex(std::string s) {
	std::map<std::string, int>::iterator iter = vertexMap.find(s);
	if (iter != vertexMap.end()) {
		return iter->second;
	} else {
		// new vertex: parse it?
		return -1;
	}
}

int OBJReader::addFaceVertex(std::string buf, int v, int t, int n) {
	int idx = findFaceVertex(buf);
	if (idx == -1) {
		idx = face_vertices.size();
		face_vertices.push_back(FaceVertex(v, t, n));
		vertexMap[buf] = idx;
	}
	readToken();
	return idx;
}

void OBJReader::addTriangle(Group * g, unsigned int id0, unsigned int id1, unsigned int id2) {
	g->indices.push_back(id0);
	g->indices.push_back(id1);
	g->indices.push_back(id2);
}

} // ::al
