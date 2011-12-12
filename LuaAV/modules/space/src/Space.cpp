#include "Space.h"

#ifdef __cplusplus
extern "C" {
#endif

/*	Data structure for holding proxy and user data.  If used in C source, 
	replace the proxy pointer with an int type capable of holding a pointer 
	value or a void *.  Addition fields can be added as long as the proxy field
	remains first for struct offset reasons.
*/
typedef struct _Agent {
	struct neighand::ObjectProxy<struct _Agent> *proxy;
	void *udata;
} Agent;


/*	Data structure for holding nearest neighbor info

*/
/*
template <typename UserObject>
struct NearestNeighbor {
    UserObject* object;
    FloatType squaredDistance;
};
*/
typedef struct _AgentNN {
    void *object;
    float squaredDistance;
} AgentNN;


// Declarations (paste this into C source using these functions
void * space_neighborhood_new(float minX, float minY, float minZ, float cellSize);
void space_neighborhood_free(void *an);
void space_neighborhood_insert(void *an, void *a, float x, float y, float z);
void space_neighborhood_remove(void *an, void *a);
void space_neighborhood_move(void *an, void *a, float x, float y, float z);
int space_neighborhood_nearestN(void *an, void *a, float d, int nNN, void *NN);


typedef space::Space		AgentHood;

#define AGENTHOOD(x)	((AgentHood *)(x))
#define AGENT(x)		((Agent *)(x))
#define AGENTNN(x)		((NearestNeighbor<Agent> *)(x))


void * space_neighborhood_new(float minX, float minY, float minZ, float cellSize) {
//	return new AgentHood(minX, minY, minZ, cellSize);
	return 0;
}

void space_neighborhood_free(void *an) {
//	delete AGENTHOOD(an);
}

void space_neighborhood_insert(void *an, void *a, float x, float y, float z) {
//	AGENTHOOD(an)->insert(AGENT(a), x, y, z);
}

void space_neighborhood_remove(void *an, void *a) {
//	AGENTHOOD(an)->remove(AGENT(a));
}

void space_neighborhood_move(void *an, void *a, float x, float y, float z) {
//	AGENTHOOD(an)->move(AGENT(a), x, y, z);
}

int space_neighborhood_nearestN(void *an, void *a, float d, int nNN, void *NN) {
//	return AGENTHOOD(an)->NH.findNearestNeighbors(AGENT(a)->proxy, d, AGENTNN(NN), nNN);
	return 0;
}

#ifdef __cplusplus
}
#endif