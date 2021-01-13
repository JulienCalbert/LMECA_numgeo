#ifndef PRINT_VORONOI_H
#define PRINT_VORONOI_H

#include "draw_tools.h"
#include "doubly_connected_edge_list.h"
#include "priority_queue.h"
#include "event.h"
#include "binary_search_tree.h"
#include "animation.h"


Box* box_new(double left, double right, double top, double bottom);
Box* build_box(DoublyConnectedEdgeList* dcel,double eps);
void Box_delete(Box* box);
List* bound_box(DoublyConnectedEdgeList* dcel, Box* box);
xy** compute_intersection_box_1(Box* box, HalfEdge* he);
xy* compute_intersection_box_2(Box* box, HalfEdge* he);
void segmentInBox(Segment* seg, Box* box);
List* connect_halfEdges(BeachLine* beach_line, Box* box);
double cross_product(xy* v1, xy* v2);
bool isInside(Face* f, xy* p);

//Displaying
void display_animation(DoublyConnectedEdgeList* dcel, PriorityQueue*pq, BeachLine* beach_line, Animation* animation);
void display_animated_Voronoi(DoublyConnectedEdgeList* dcel, BeachLine *beach_line, Animation* animation, bool ended);
void display_final_Voronoi(DoublyConnectedEdgeList* dcel, double resize, int flag);
points_t* load_sites(DoublyConnectedEdgeList* dcel,double resize);
points_t* load_vertex(DoublyConnectedEdgeList* dcel,double resize);
points_t* load_edge(DoublyConnectedEdgeList* dcel,double resize);
points_t* load_edge_Delaunay(DoublyConnectedEdgeList* dcel, double resize);
points_t* load_box(Box* box,double resize);
points_t* load_segs(List *seglist, double resize);
points_t* load_Beachline(BeachLine* beach_line, Box* box, double resize);
points_t* load_sweepLine(BeachLine* beach_line, Box* box, double resize);

#endif
