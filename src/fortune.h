#ifndef FORTUNE_H
#define FORTUNE_H

#include "doubly_connected_edge_list.h"
#include "priority_queue.h"
#include "event.h"
#include "binary_search_tree.h"
#include "animation.h"
#include "print_voronoi.h"

DoublyConnectedEdgeList* fortuneAlgo(xy** points, int n_sites, Animation* animation);
void handleSiteEvent(event* e, BBT* bbt, PriorityQueue* pq, DoublyConnectedEdgeList* dcel);
void handleCircleEvent(event* e, BBT* bbt, PriorityQueue* pq, DoublyConnectedEdgeList* dcel);

void add_circle_event(PriorityQueue* pq, BBT* bbt, arc* a1, arc* a2, arc* a3);
xy* computeCircleCenter(xy* p1, xy* p2, xy* p3, double* r);
bool isMovingRight(arc* left, arc* right);
double getInitialX(arc* left, arc* right, bool movingRight);

void initializeEventQueue(PriorityQueue* pq, Site** sites, int n_sites);

#endif
