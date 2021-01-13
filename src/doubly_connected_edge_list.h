/**
 * Generic Doubly-Connected Edge List Structure
 */

#ifndef DOUBLY_CONNECTED_EDGE_LIST_H
#define DOUBLY_CONNECTED_EDGE_LIST_H

#include <math.h>
#include <stdio.h>
#include <stdlib.h>

#include "utils.h"

// Public structures
typedef struct DoublyConnectedEdgeList DoublyConnectedEdgeList;
typedef struct Site Site;
typedef struct Vertex Vertex;
typedef struct HalfEdge HalfEdge;
typedef struct Face Face;

// Public structure members

struct DoublyConnectedEdgeList {
	List* vertex_list;
	List* half_edge_list;
	List* face_list;
};

struct Site {
	xy* pos; // position of the site
	Face* f; // corresponding face
};

struct Vertex {
	xy* pos;
	HalfEdge* incident_edge;
};

struct HalfEdge {
	Vertex* origin;
	Vertex* destination; // optional since destination == twin(next)->origin
	HalfEdge* twin;
	Face* incident_face; // face on the left of half-edge
	HalfEdge* next;
	HalfEdge* prev;
};

struct Face {
	Site* site;
	HalfEdge* outer_edge; // arbitrary
	List* edges; //only necessecary for animation
};


/* Public functions */

DoublyConnectedEdgeList* DoublyConnectedEdgeList_new(); // DCEL constructor
void DoublyConnectedEdgeList_delete(DoublyConnectedEdgeList* p); // DCEL destructor

Site* Site_new(xy* pos); // Site constructor (does not initialize f)
void Site_delete(Site* s); // Site destructor (TODO: does not deallocate pos nor face!)

Site** sites_build(DoublyConnectedEdgeList* dcel, xy** points, int n_sites);

void DoublyConnectedEdgeList_print(DoublyConnectedEdgeList* dcel);

Vertex* add_vertex(DoublyConnectedEdgeList* dcel, xy* pt);
HalfEdge* half_edge_build(DoublyConnectedEdgeList* dcel, Face* f); //et add
Face* add_face(DoublyConnectedEdgeList* dcel, Site* s);

#endif
