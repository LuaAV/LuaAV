#ifndef SPACE_NEIGHBORHOOD_H
#define SPACE_NEIGHBORHOOD_H 1

#include <math.h>
using namespace std;

#include "neighand.h"

using namespace neighand;

#include "Nav.h"

namespace space {

//template<typename T, int exp2divx, int exp2divy, int exp2divz, bool wrapX, bool wrapY, bool wrapZ>
class Space {
public:
	Space(float minX, float minY, float minZ, float cellSize)
	:	NH(minX, minY, minZ, cellSize),
		mMinCorner(minX, minY, minZ),
		mMaxCorner(minX+pow(2., (float)4)*cellSize, 
					minY+pow(2., (float)4)*cellSize, 
					minZ+pow(2., (float)4)*cellSize),
		mNCells((int)pow(2., (float)4))
	{
		mSize.x = mMaxCorner.x - mMinCorner.x;
		mSize.y = mMaxCorner.y - mMinCorner.y;
		mSize.z = mMaxCorner.z - mMinCorner.z;
		
		mHalfSize.x = mSize.x*0.5;
		mHalfSize.y = mSize.y*0.5;
		mHalfSize.z = mSize.z*0.5;
	}
	
	~Space() {
		neighand::ObjectProxy<Nav> *proxies = NH.getAllProxies();
		uint32_t n = NH.getNumProxies();
		for(uint32_t i=0; i < n; i++) {
			proxies[i].object->set_space(NULL);
		}
	}
	
	Nav * point(float x, float y, float z) {
		return new Nav(this, x, y, z);
	}
	
	void insert(Nav *p, float x, float y, float z) {
		p->proxy = NH.insert(x, y, z, p, remap);
	}
	
	void remove(Nav *p) {
		NH.remove(p->proxy, remap);
	}
	
	void wrap(float &x, float &y, float &z) {
		// probably a faster way than fmod
		x = fmodf(x-mMinCorner.x, mSize.x) + mMinCorner.x;
		y = fmodf(y-mMinCorner.y, mSize.y) + mMinCorner.y;
		z = fmodf(z-mMinCorner.z, mSize.z) + mMinCorner.z;
	}
	
	void move(Nav *p, float &x, float &y, float &z) {
		wrap(x, y, z);
		NH.update(p->proxy, x, y, z);
	}
	
	Vec3 mincorner() {
		return mMinCorner;
	}
	
	Vec3 maxcorner() {
		return mMaxCorner;
	}
	
	Vec3 size() {
		return mSize;
	}
	
	Vec3 halfsize() {
		return mHalfSize;
	}
	
	int ncells() {
		return mNCells;
	}
	
	int within(float x, float y, float z, neighand::NearestNeighbor<Nav> *nn, float r, int nmax) {
		return NH.findNearestNeighbors(x, y, z, r, nn, nmax);
	}
	
//protected:
	static void remap(Nav *p, neighand::ObjectProxy<Nav> *proxy) {
		p->proxy = proxy;
	}
	
	NeighborhoodHandler<Nav, 4, 4, 4, true, true, true> NH;
	
protected:
	
	Vec3	mMinCorner;
	Vec3	mMaxCorner;
	Vec3	mSize;
	Vec3	mHalfSize;
	int		mNCells;
};

}	// space::


#endif	// SPACE_NEIGHBORHOOD_H
