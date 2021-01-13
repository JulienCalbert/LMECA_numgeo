#include "doubly_connected_edge_list.h"

DoublyConnectedEdgeList* DoublyConnectedEdgeList_new() {
	DoublyConnectedEdgeList* p = (DoublyConnectedEdgeList*)malloc(sizeof(DoublyConnectedEdgeList));

	p->vertex_list = List_new();
	p->half_edge_list = List_new();
	p->face_list = List_new();
	return p;
}

// Only frees position, not half-edge!
void Vertex_delete(void *obj) {
	Vertex *vertex = (Vertex*) obj;
	free(vertex->pos);
	free(vertex);
}

// Only frees site, not outer_edge!
void Face_delete(void *obj) {
	Face *face = (Face*) obj;
	Site_delete(face->site);
	List_delete(face->edges, NULL);
	free(face);
}

void HalfEdge_delete(void *obj) {
	HalfEdge *he = (HalfEdge*)obj;
	free(he);
}

// After calling this function, the only thing which isn't free is the pos, of all the initial site
void DoublyConnectedEdgeList_delete(DoublyConnectedEdgeList* dcel) {
	List_delete(dcel->vertex_list, Vertex_delete);
	List_delete(dcel->face_list, Face_delete);
	List_delete(dcel->half_edge_list, HalfEdge_delete);
	free(dcel);
}

Site** sites_build(DoublyConnectedEdgeList* dcel, xy** points, int n_sites) {
	Site** sites = (Site**)malloc(n_sites * sizeof(Site*));
	for(int i = 0; i < n_sites; i++) {
		sites[i] = Site_new(points[i]);
		sites[i]->f = add_face(dcel, sites[i]);
	}
	return sites;
}

Site* Site_new(xy* pos) {
	Site* site = (Site*) malloc(sizeof(Site));
	site->pos = pos;
	site->f = NULL;
}

void Site_delete(Site* s)
{
	free(s);
}

void DoublyConnectedEdgeList_print(DoublyConnectedEdgeList* dcel)
{
	printf("\n number of faces: %d", dcel->face_list->n);
	printf("\n number of halfedges: %d", dcel->half_edge_list->n);
	printf("\n number of vertices: %d\n", dcel->vertex_list->n);
}

Vertex* add_vertex(DoublyConnectedEdgeList* dcel, xy* pos) {
	Vertex* v = (Vertex*)malloc(sizeof(Vertex));
	v->pos = pos;
	v->incident_edge = NULL; // will be initialized later
	List_append(dcel->vertex_list, v);
	return v;
}

HalfEdge* half_edge_build(DoublyConnectedEdgeList* dcel, Face* f) {
	HalfEdge* he = (HalfEdge*)malloc(sizeof(HalfEdge));
	he->incident_face = f;
	he->destination = NULL;
	he->origin = NULL;
	he->twin = NULL;
	he->next = NULL;
	he->prev = NULL;
	if(f->outer_edge == NULL)
		f->outer_edge = he;
	List_append(dcel->half_edge_list, he);
	List_append(f->edges, he); //only necessary for animation
	return he;
}

Face* add_face(DoublyConnectedEdgeList* dcel, Site* s) {
	Face* f = (Face*)malloc(sizeof(Face));
	f->outer_edge = NULL;
	f->site = s;
	f->edges = List_new(); //only necessary for animation
	List_append(dcel->face_list, f);
	return f;
}
