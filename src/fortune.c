#include <assert.h>

#include "fortune.h"

DoublyConnectedEdgeList* fortuneAlgo(xy** points, int n_sites, Animation* animation) {
	DoublyConnectedEdgeList* dcel = DoublyConnectedEdgeList_new();
	PriorityQueue* pq = PriorityQueue_new(less_event);
	BBT* bbt = BBT_new();
	Site** sites = sites_build(dcel, points, n_sites);

	initializeEventQueue(pq, sites, n_sites);

	int iter = 0;
	while (!PriorityQueue_is_empty(pq)) {
		event* e = PriorityQueue_pop(pq);
		bbt->L = e->pos->y; //update the height of the sweep line

		if (!e->is_circle)
			handleSiteEvent(e, bbt, pq, dcel);
		else
			handleCircleEvent(e, bbt, pq, dcel);

		// visualisation
		if (animation != NULL && !window_should_close(animation->window))
		{
			arc* left_most = minimum(bbt->root);
			BeachLine *beach_line = BeachLine_new(left_most, bbt->L);
			display_animation(dcel,pq,beach_line,animation);
			Beachline_delete(beach_line);
		}

		free(e);
		iter += 1;
	}
	PriorityQueue_delete(pq);
	BBT_delete(bbt);
	free(sites);
	return dcel;
}


void handleSiteEvent(event* e, BBT* bbt, PriorityQueue* pq, DoublyConnectedEdgeList* dcel) {
	Site* site = e->site;
	// 1) check if the beachline is empty
	if (BBT_empty(bbt)){
		insert_root(bbt,site);
		return;
	}
	// 2) find the arc above the site
	arc* arc_to_cut = findArc(bbt, site->pos);
	delete_event_false_alarm(arc_to_cut, pq);

	// 3) replace this arc by the new arcs
	arc* arc_middle = arc_break(bbt, arc_to_cut, site);//build the new subtree
	arc* arc_left = arc_middle->prev;
	arc* arc_right = arc_middle->next;
	// 4) add an edge in the diagram
	add_edge(arc_left, arc_middle, dcel);
	arc_middle->rightHalfEdge = arc_middle->leftHalfEdge;
	arc_right->leftHalfEdge = arc_left->rightHalfEdge;
	// 5) check if circle events
	// left triplet
	if (arc_left->prev != NULL)
		add_circle_event(pq, bbt, arc_left->prev, arc_left, arc_middle);
	// right triplet
	if (arc_right->next != NULL)
		add_circle_event(pq, bbt, arc_middle, arc_right, arc_right->next);
}

void handleCircleEvent(event* e, BBT* bbt, PriorityQueue* pq, DoublyConnectedEdgeList* dcel)
{
	xy* pt = e->c;
	free(e->pos);
	arc* a = e->a;
	// 1) add vertex
	Vertex* v = add_vertex(dcel, pt);
	// 2) delete all the events with this arc
	arc* arc_left = a->prev;
	arc* arc_right = a->next;
	delete_event_false_alarm(arc_left, pq);
	delete_event_false_alarm(arc_right, pq);
	// 3) update the beachline
	arc_remove(dcel, bbt, a, v);
	// 4) add new circle events
	// left triplet
	if (arc_left->prev != NULL)
		add_circle_event(pq, bbt, arc_left->prev, arc_left, arc_right);
	// right triplet
	if (arc_right->next != NULL)
		add_circle_event(pq, bbt, arc_left, arc_right, arc_right->next);
}

void add_circle_event(PriorityQueue* pq, BBT* bbt, arc* a1, arc* a2, arc* a3)
{
	double r;
	xy* center = computeCircleCenter(a1->site->pos, a2->site->pos, a3->site->pos, &r);
	if(r != INFINITY) { // if 3 colinear points
		xy* bottom = xy_new(center->x, center->y - r);
		bool isBelow = bottom->y <= bbt->L;
		bool leftBreakpointMovingRight = isMovingRight(a1, a2);
		bool rightBreakpointMovingRight = isMovingRight(a2, a3);
		double leftInitialX = getInitialX(a1, a2, leftBreakpointMovingRight);
		double rightInitialX = getInitialX(a2, a3, rightBreakpointMovingRight);
		bool isValid =
			((leftBreakpointMovingRight && leftInitialX < center->x) ||
			(!leftBreakpointMovingRight && leftInitialX > center->x)) &&
				((rightBreakpointMovingRight && rightInitialX < center->x) ||
			(!rightBreakpointMovingRight && rightInitialX > center->x));
		if (isValid && isBelow) {
			event* circle_event = circle_event_new(bottom, center, a2);
			PriorityQueueNode* event_node = PriorityQueue_insert(pq, circle_event);
			a2->event_node = event_node;
		}
		else
		{
			free(bottom);
			free(center);
		}
	} else free(center);
}

xy* computeCircleCenter(xy* p1, xy* p2, xy* p3, double* r)
{
	xy* q1 = xy_new((p1->x + p2->x) / 2, (p1->y + p2->y) / 2);
	xy* q2 = xy_new((p2->x + p3->x) / 2, (p2->y + p3->y) / 2);

	double a1 = (p2->y - p1->y) / (p2->x - p1->x);
	double m1 = -1 / a1;
	double b1 = q1->y - m1 * q1->x;

	double a2 = (p3->y - p2->y) / (p3->x - p2->x);
	double m2 = -1 / a2;
	double b2 = q2->y - m2 * q2->x;
	free(q1);free(q2);
	xy* center = xy_new((b2 - b1) / (m1 - m2), 0);
	center->y =  m1 * center->x + b1;
	*r = sqrt(pow(center->x - p1->x, 2) + pow(center->y - p1->y, 2));
	return center;
}

bool isMovingRight(arc* left, arc* right)
{
	return left->site->pos->y < right->site->pos->y;
}

double getInitialX(arc* left, arc* right, bool movingRight)
{
	if (movingRight)
		return left->site->pos->x;
	else
		return right->site->pos->x;
}

void initializeEventQueue(PriorityQueue* pq, Site** sites, int n_sites)
{
	for (int i = 0; i < n_sites; i++)
	{
		event* e = site_event_new(sites[i]);
		PriorityQueue_insert(pq, e);
	}
}
