#ifndef EVENT_H
#define EVENT_H

#include "binary_search_tree.h"
#include "doubly_connected_edge_list.h"

#include <math.h>
#include <stdbool.h>

typedef struct event {
	bool is_circle; // true if circle event, false otherwise
	xy* pos;        // event position
	Site* site;     // if site event
	xy* c;          // if circle event (center)
	arc* a;         // if circle event (arc)
} event;

event* site_event_new(Site* st);
event* circle_event_new(xy* pos, xy* c, arc* a);

bool less_event(const void* a, const void* b); // event comparator
#endif
