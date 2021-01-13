#ifndef BINARY_SEARCH_TREE_H
#define BINARY_SEARCH_TREE_H

#include <stdlib.h>
#include <stdio.h>
#include <stdbool.h>

#include "doubly_connected_edge_list.h"
#include "priority_queue.h"

typedef struct BBT BBT;
typedef struct arc arc;
typedef enum Color { RED, BLACK } Color;

typedef struct arc {
	//for the BST structure
	arc* parent;
	arc* left;
	arc* right;
	//data
	Site* site;
	HalfEdge* leftHalfEdge;
	HalfEdge* rightHalfEdge;
	PriorityQueueNode* event_node; // if circle event, index of event in pq
	arc* prev;
	arc* next;
	Color color; //for red-black
} arc;

typedef struct BBT {
	arc* root;
	double L;
} BBT;

BBT* BBT_new();
void insert_root(BBT* bbt,Site* site);
void BBT_delete(BBT* bbt);
void BBT_delete_recursive(arc* a);
bool BBT_empty(BBT* bbt);

arc* arc_build(Site* site);
void arc_delete(arc* a);

//search arc
arc* findArc(BBT* bbt, xy* pt);
xy* computeBreakpoint(xy* p1, xy* p2, double L);

//add arc
void insert_before(BBT* bbt, arc* a, arc* b);
void insert_after(BBT* bbt, arc* a, arc* b);
void arc_replace(BBT* bbt, arc* a, arc* b);

//delete arc
void BBT_remove(BBT* bbt, arc* z);
void transplant(BBT* bbt, arc* a, arc* b);
arc* minimum(arc* x);

//RED-BLACK
void insert_balance(BBT* bbt, arc* z);
void remove_balance(BBT* bbt, arc* x);
void rotateLeft(BBT* bbt, arc* x);
void rotateRight(BBT* bbt, arc* y);

//Fortune algo
arc* arc_break(BBT* bbt, arc* a, Site* site);
void arc_remove(DoublyConnectedEdgeList* dcel, BBT* bbt, arc* a, Vertex* v);
void delete_event_false_alarm(arc* a, PriorityQueue* pq);

//Set edges
void add_edge(arc* left, arc* right, DoublyConnectedEdgeList* dcel);
void set_destination(arc* left, arc* right, Vertex* v);
void set_origin(arc* left, arc* right, Vertex* v);

arc* find_first_arc(BBT* bbt);
void printBeachline(BBT* bbt);
#endif
