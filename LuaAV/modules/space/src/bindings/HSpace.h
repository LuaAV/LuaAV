#ifndef HSPACE_H
#define HSPACE_H

#include "allocore/types/al_Array.hpp"
#include <vector>
#include <map>
using std::vector;

#define Bx 5
#define By 5
#define Bz 5

#define Tx ((1<<(Bx-1))-1)
#define Ty ((1<<(By-1))-1)
#define Tz ((1<<(Bz-1))-1)

#define Lx (1<<Bx)
#define Ly (1<<By)
#define Lz (1<<Bz)

#define Ox Bx
#define Oy (Ox+By)
#define Oz (Oy+Bz)
#define Oxu (Ox+Oz)
#define Oyu (Oy+Oz)

#define Sx 0
#define Sy Bx
#define Sz (Sy+By)
#define Syu (Sy+Oz)

#define Mx ((1<<Ox)-1)
#define My (((1<<Oy)-1)-Mx)
#define Mz (((1<<Oz)-1)-(My+Mx))
#define Myu (((1<<Oyu)-1)-((1<<Oxu)-1))
#define Mup (Mx + Mz + Myu)

#define InvalidCell (-1)
#define InvalidPos (0xFFFFFFFF)

typedef void (*datafree)(void *);

struct HObject{
	float x;
	float y;
	float z;
	int oidx;		// my index in HSpace.mObjects
	int idx;		
	int cellidx;
	HObject *next;
	HObject *prev;
	void *data;
	datafree dfree;
	
	HObject()
	:	cellidx(InvalidCell),
		next(0), 
		prev(0),
		data(0),
		dfree(0)
	{}
	
	~HObject() {
		if(dfree && data) {
			dfree(data);
		}
	}
};

struct HCell{
	HObject *objects;
	
	HCell()
	: objects(0)
	{}
};


class HSpace {
public:
	
	// size must be power of 2, 32 is max
	HSpace(int size);
	~HSpace();
	
	int cellIdx(int upos);
	int unpackPos(float x, float y, float z);
	void coords(int upos, int &x, int &y, int &z);
	void cellCoords(int cpos, int &x, int &y, int &z);
	
	void destroy();
	void rebuild(int nobjects);
	// points[N] corresponds to mObjects[N]
	void hash(AlloArray *points);
	
	int query(float x, float y, float z, float radius, int nobjs, HObject **res);
	int query(float x, float y, float z, float radius, HObject *exclude, int nobjs, HObject **res);
	int queryRange(float x, float y, float z, float r1, float r2, int nobjs, HObject **res);
	
//protected:
	int mSize;
	int mMaxd;
	int mValidMask;
	vector<int> mD;
	vector<int> mG;
	HCell *mCells;
	int mNObjects;
	HObject *mObjects;
};


#endif // HSPACE_H