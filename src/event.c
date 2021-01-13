#include "event.h"

event* site_event_new(Site* site) {
	event* e = (event*)malloc(sizeof(event));
	e->is_circle = false;
	e->pos = site->pos;
	e->site = site;
	e->c = NULL;
	e->a = NULL;
	return e;
}

event* circle_event_new(xy* pos, xy* c, arc* a) {
	event* e = (event*)malloc(sizeof(event));
	e->is_circle = true;
	e->pos = pos;
	e->site = NULL;
	e->c = c;
	e->a = a;
	return e;
}

bool is_cicrle(event* e) {
	return e->is_circle;
}

bool less_event(const void* o1, const void* o2) {
	const event* e1 = o1;
	const event* e2 = o2;
	if (e1->pos->y < e2->pos->y)
		return true;
	else if (e1->pos->y > e2->pos->y)
		return false;
	if (e1->pos->x < e2->pos->x)
		return false;
	else if (e1->pos->x > e2->pos->x)
		return true;
	return false; //same point
}
