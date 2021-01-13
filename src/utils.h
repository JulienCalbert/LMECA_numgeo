/**
 * Some useful structures:
 * Box, Coordinates, Segment, List
 */

#ifndef UTILS_H
#define UTILS_H

#include <stdlib.h>

/**
 * Box structure, defined by the coordinates of its 4 edges.
 */
typedef struct Box {
	double left;
	double right;
	double top;
	double bottom;
} Box;
//Box* Box_new()

/* Coordinates */
typedef struct xy xy; // struct for storing (x,y) coordinates
struct xy {
	double x;
	double y;
};

/* Segment */
typedef struct Segment Segment;
struct Segment {
	xy *a, *b;
};

/* List */

typedef struct List List; // Generic list
typedef struct ListNode ListNode;
typedef void(*destructor)(void *object);
typedef void(*application)(void *object);

struct List {
	int n; // list size
	ListNode *head; // pointer to first node for traversal
	ListNode *tail; // pointer to last node for append
};

struct ListNode {
	void *v; // node content
	ListNode *next; // pointer to next node
};

xy* xy_new(double x, double y); // xy constructor
void xy_tab_delete(xy** tab, int size);
void xy_delete(void* obj);

List* List_new(); // if elements don't need to be freed: des==NULL
void List_append(List *l, void *v);
void List_delete(List *l, destructor des);
void List_apply(List *l, application app);

Segment* Segment_new(xy *a, xy *b);
void Segment_delete(void *obj);
#endif
