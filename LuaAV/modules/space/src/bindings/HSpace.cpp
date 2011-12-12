#include "HSpace.h"
#include <vector>;

#ifndef MAX
	#define MAX(a, b) ((a > b) ? (a) : (b))
#endif
#define ABS(a) ((a < 0) ? (-a) : (a))

#define NEGATIVE(v) ((*((int *)(&v))) & (0x80000000))

using std::vector;

int bpos(int x, int y, int z) {
	return ((x<<Sx)&Mx) + ((y<<Sy)&My) + ((z<<Sz)&Mz);
}

void ccoords(int pos, int &x, int &y, int &z) {
	x = (pos & Mx) >> Sx;
	y = (pos & My) >> Sy;
	z = (pos & Mz) >> Sz;
	
	x = (x > Tx) ? (x-Lx) : x;
	y = (y > Ty) ? (y-Ly) : y;
	z = (z > Tz) ? (z-Lz) : z;
}

int unpack(int pos) {
	return (pos & (~My)) + ((pos & My) << Oz);
}

int pack(int pos) {
	pos = (pos & Mup);
	return ((pos & Myu) >> Oz) + (pos & ~Myu);
}

int dist2(int a1, int a2, int a3) {
	int x = MAX(0, ABS(a1)-1);
	int y = MAX(0, ABS(a2)-1);
	int z = MAX(0, ABS(a3)-1);
	return x*x+y*y+z*z;
}

void setA(vector< vector<int> > &A, int i, int j, int k) {
	int d2 = dist2(i, j, k);
	A[d2].push_back(
		unpack(bpos(i, j, k))
	);
}


void print_bin(float f) {
	int *x = (int *)(&f);
	char s[33];
	for(int i=31; i >= 0; i--) {
		s[31-i] = (*x)&(1<<i) ? '1' : '0';
	}
	s[32] = '\0';
	printf("%f\t%s\n", f, s);
}

void print_bini(int f) {
	int *x = (int *)(&f);
	char s[33];
	for(int i=31; i >= 0; i--) {
		s[31-i] = (*x)&(1<<i) ? '1' : '0';
	}
	s[32] = '\0';
	printf("%d\t%s\n", f, s);
}


HSpace :: HSpace(int size)
:	mSize(size),
	mCells(0),
	mNObjects(0),
	mObjects(0)
{	
	int maxd = dist2(size-1, size-1, size-1);
	mMaxd = maxd;
	mCells = new HCell[mSize*mSize*mSize];

	mValidMask = ~bpos(mSize-1, mSize-1, mSize-1);
	
	vector< vector<int> > A(maxd+1);

	for(int i=0; i < size; i++) {
		for(int j=0; j < size; j++) {
			for(int k=0; k < size; k++) {
				setA(A, i, j, k);
				if(k != 0) setA(A, i, j, -k);
			}
			if(j != 0) {
				for(int k=0; k < size; k++) {
					setA(A, i, -j, k);
					if(k != 0) setA(A, i, -j, -k);
				}
			}
		}
		if(i != 0) {
			for(int j=0; j < size; j++) {
				for(int k=0; k < size; k++) {
					setA(A, -i, j, k);
					if(k != 0) setA(A, -i, j, -k);
				}
				if(j != 0) {
					for(int k=0; k < size; k++) {
						setA(A, -i, -j, k);
						if(k != 0) setA(A, -i, -j, -k);
					}
				}
			}
		}
	}
	
	mD.reserve(A.size());
	
	for(int i=0; i < A.size(); i++) {
		vector<int> &ilist = A[i];
		if(ilist.size() > 0) {
			for(int j=0; j < ilist.size(); j++) {
				mG.push_back(ilist[j]);
			}
			mD[i] = mG.size();
		}
		else {
			mD[i] = mD[i-1];
		}
	}	
}

HSpace :: ~HSpace() {
	destroy();
}


int HSpace :: unpackPos(float x, float y, float z) {
	int i = (int)x;
	int j = (int)y;
	int k = (int)z;
	
	int pos = bpos(i, j, k);
	if(pos & mValidMask || NEGATIVE(x) || NEGATIVE(y) || NEGATIVE(z)) {
		return InvalidPos;
	}
	else {
		return unpack(pos);
	}
}

int HSpace :: cellIdx(int upos) {
	return (upos & Mx) + (((upos & Myu) >> Syu) + ((upos & Mz) >> Sz)*mSize)*mSize;
}

void HSpace :: coords(int upos, int &x, int &y, int &z) {
	ccoords(pack(upos), x, y, z);
}

void HSpace :: cellCoords(int cpos, int &x, int &y, int &z) {
	int size2 = mSize*mSize;
	z = cpos/(size2);
	y = (cpos-z*size2)/mSize;
	x = cpos % mSize;
}

void HSpace :: destroy() {
	if(mCells) {
		delete[] mCells;
		mCells = 0;
	}
	if(mObjects) {
		delete[] mObjects;
		mObjects = 0;
	}
}

void HSpace :: rebuild(int nobjects) {
	for(int i=0; i < mSize*mSize*mSize; i++) {
		mCells[i].objects = 0;
	}
	mObjects = new HObject[nobjects];
	mNObjects = nobjects;
}

void debug_cell(HCell &cell) {
	if(cell.objects) {
		HObject *fo = cell.objects;
		int i=0;
		do {
			printf("\t %d: %p obj: %p %p\n", i, fo, fo->prev, fo->next);
			fo = fo->next;
			i++;
		} while(fo != cell.objects);
	}
	else {
		printf("\t<no objects>\n");
	}
}

// assumes 3-components, FloatTy, 2D Array
void HSpace :: hash(AlloArray *points) {
	int npoints = points->header.dim[0]*points->header.dim[1];
	if(mNObjects != npoints) {
		rebuild(npoints);
	}
	
	float *data = (float *)points->data.ptr;
	HObject *objects = mObjects;
	for(int i=0; i < npoints; i++) {
		objects->x = data[0];
		objects->y = data[1];
		objects->z = data[2];
		
		int upos = unpackPos(data[0], data[1], data[2]);;
		if(upos == InvalidPos) {
			int pcellpos = objects->cellidx;
			if(pcellpos != InvalidCell) {
				HCell &pcell = mCells[pcellpos];
				if(objects == objects->prev) {
					pcell.objects = 0;
				}
				else {
					HObject *prev = objects->prev;
					HObject *next = objects->next;
					prev->next = next;
					next->prev = prev;
					
					if(pcell.objects == objects) {
						pcell.objects = next;
					}
				}
				
				objects->cellidx = InvalidCell;
				objects->prev = 0;
				objects->next = 0;
			}
		}
		else {
			int cpos = cellIdx(upos);
			int pcellpos = objects->cellidx;
			objects->oidx = i;
			objects->idx = upos;
			objects->cellidx = cpos;
			
			if(cpos < 0 || cpos > mSize*mSize*mSize) {
				printf("Error, bad cell: (%f %f %f) -> %d\n", data[0], data[1], data[2], cpos);
			}
			else if(pcellpos != cpos) {
			//	printf("%d: change cell: %d %d\n", i, pcellpos, cpos);
				if(pcellpos != InvalidCell) {
					HCell &pcell = mCells[pcellpos];
					/*
					printf("pcell:\n");
					debug_cell(pcell);
					printf("\n");
					*/
					if(objects == objects->prev) {
						pcell.objects = 0;
					//	printf("\t no objects: %d\n", pcellpos);
					}
					else {
						HObject *prev = objects->prev;
						HObject *next = objects->next;
						prev->next = next;
						next->prev = prev;
						
						if(pcell.objects == objects) {
							pcell.objects = next;
						}
					}
					/*
					printf("pcell:\n");
					debug_cell(pcell);
					printf("\n");
					*/
				}
				
				HCell &cell = mCells[cpos];
				if(cell.objects) {
					HObject *pprev = cell.objects->prev;
					
					cell.objects->prev = objects;
					objects->next = cell.objects;
					
					pprev->next = objects;
					objects->prev = pprev;
				}
				else {
				//	printf("\t 1 object: %d\n", cpos);
					cell.objects = objects;
					objects->next = objects;
					objects->prev = objects;
				}
				/*
				printf("cell:\n");
				debug_cell(cell);
				printf("\n");
				*/
			}
		}
		
		objects++;
		data += 3;
	}
}

int HSpace :: query(float x, float y, float z, float radius, int nobjs, HObject **res) {
	int nres = 0;
	int upos = unpackPos(x, y, z);
	if(upos != InvalidPos) {
		float d2 = radius*radius;
		int n = (int)d2;
		
		int maxidx = mD[MIN(n, mMaxd)];
		for(int i=0; i < maxidx; i++) {
			int offset = mG[i];
			int cellpos = offset+upos;
			int cidx = cellIdx(cellpos);
			HCell &cell = mCells[cidx];
			if(cell.objects) {
				HObject *o = cell.objects;
				do {
					float dx = x - o->x;
					float dy = y - o->y;
					float dz = z - o->z;
					float od2 = dx*dx+dy*dy+dz*dz;
					if(od2 <= d2) {
						res[nres] = o;
						nres++;
					}
					o = o->next;
				} while(o != cell.objects && nres < nobjs);
			}
			
			if(nres == nobjs) {
				break;
			}
		}
	}
	return nres;
}

int HSpace :: query(float x, float y, float z, float radius, HObject *exclude, int nobjs, HObject **res) {
	int nres = 0;
	int upos = unpackPos(x, y, z);
	if(upos != InvalidPos) {
		float d2 = radius*radius;
		int n = (int)d2;
		
		int maxidx = mD[MIN(n, mMaxd)];
		for(int i=0; i < maxidx; i++) {
			int offset = mG[i];
			int cellpos = offset+upos;
			int cidx = cellIdx(cellpos);
			HCell &cell = mCells[cidx];
			if(cell.objects) {
				HObject *o = cell.objects;
				do {
					if(o != exclude) {
						float dx = x - o->x;
						float dy = y - o->y;
						float dz = z - o->z;
						float od2 = dx*dx+dy*dy+dz*dz;
						if(od2 <= d2) {
							res[nres] = o;
							nres++;
						}
					}
					o = o->next;
				} while(o != cell.objects && nres < nobjs);
			}
			
			if(nres == nobjs) {
				break;
			}
		}
	}
	return nres;
}

int HSpace :: queryRange(float x, float y, float z, float r1, float r2, int nobjs, HObject **res) {
	int nres = 0;
	int upos = unpackPos(x, y, z);
	if(upos != InvalidPos) {
		float d12 = r1*r1;
		int n1 = (int)d12;
		
		float d22 = r2*r2;
		int n2 = (int)d22;
		
		int mind = MIN(n1-2, mMaxd);
		int minidx = mind < 0 ? 0 : mD[mind];
		int maxidx = mD[MIN(n2, mMaxd)];
		for(int i=minidx; i < maxidx; i++) {
			int offset = mG[i];
			int cellpos = offset+upos;
			int cidx = cellIdx(cellpos);
			HCell &cell = mCells[cidx];
			if(cell.objects) {
				HObject *o = cell.objects;
				do {
					float dx = x - o->x;
					float dy = y - o->y;
					float dz = z - o->z;
					float od2 = dx*dx+dy*dy+dz*dz;
					if(d12 <= od2 && od2 <= d22) {
						res[nres] = o;
						nres++;
					}
					o = o->next;
				} while(o != cell.objects && nres < nobjs);
			}
			
			if(nres == nobjs) {
				break;
			}
		}
	}
	return nres;
}