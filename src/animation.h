#ifndef ANIMATION_H
#define ANIMATION_H

#include <time.h>

#include "doubly_connected_edge_list.h"
#include "binary_search_tree.h"
#include "draw_tools.h"
#include "priority_queue.h"
#include "event.h"

typedef struct Animation {
	window_t* window;
	double resize;
	double timeout;
	double delta;
	bool stop_end;
} Animation;

typedef struct BeachLine {
    List* sites;
	double L; //true value of L
    double l;
} BeachLine;

Animation* Animation_new(double resize, double timeout, double delta,bool stop_end);
void Animation_delete(Animation* animation);
BeachLine* BeachLine_new(arc* a, double l);
void Beachline_delete(BeachLine* beach_line);
void BeachLine_print(BeachLine* beach_line);
List* BeachLine_discretize(BeachLine* bl, Box* box);

#endif
