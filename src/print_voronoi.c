#include "print_voronoi.h"

Box* box_new(double left, double right, double bottom, double top)
{
	Box* box = (Box*)malloc(sizeof(Box));
	box->left = left;
	box->right = right;
	box->top = top;
	box->bottom = bottom;
	return box;
}

//build a box at a distance eps from the extreme point (among the site and the vertex of Voronoi)
Box* build_box(DoublyConnectedEdgeList* dcel,double eps)
{
	double left = INFINITY;
	double right = -INFINITY;
	double bottom = INFINITY;
	double top = -INFINITY;
	//for Voronoi vertex
	ListNode* nd = dcel->vertex_list->head;
	for (int i = 0;i < dcel->vertex_list->n;i++)
	{
		Vertex* v = nd->v;
		left = fmin(left, v->pos->x);
		right = fmax(right, v->pos->x);
		bottom = fmin(bottom, v->pos->y);
		top = fmax(top, v->pos->y);
		nd = nd->next;
	}
	//for sites
	nd = dcel->face_list->head;
	for (int i = 0;i < dcel->face_list->n;i++)
	{
		Face* f = nd->v;
		left = fmin(left, f->site->pos->x);
		right = fmax(right, f->site->pos->x);
		bottom = fmin(bottom, f->site->pos->y);
		top = fmax(top, f->site->pos->y);
		nd = nd->next;
	}
	Box* box = box_new(left - eps, right + eps, bottom - eps, top + eps);
	return box;
}
void Box_delete(Box* box)
{
	free(box);
}

// dcel is not modified, a list of Segment (corresponding to the halfedges having to be connected
// to the box) is returned (to be added to the plot)
List* bound_box(DoublyConnectedEdgeList* dcel, Box* box) {
	List* seglist = List_new();

	ListNode* node = dcel->half_edge_list->head;
	for(int i = 0;i < dcel->half_edge_list->n;i++) {
		HalfEdge* he = node->v;
		if(he->destination == NULL && he->origin == NULL) {
			xy** intersections = compute_intersection_box_1(box,he);
			List_append(seglist, Segment_new(intersections[1], intersections[0]));
			free(intersections[0]);free(intersections[1]); free(intersections);

		}
		else if(he->destination == NULL) {
			xy* intersection = compute_intersection_box_2(box, he);
			List_append(seglist, Segment_new(he->origin->pos, intersection));
			free(intersection);
		}
		else if(he->origin == NULL) {
			xy* intersection = compute_intersection_box_2(box, he->twin);
			List_append(seglist, Segment_new(intersection, he->destination->pos));
			free(intersection);
		}
		node = node->next;
	}
	return seglist;
}

xy** compute_intersection_box_1(Box* box, HalfEdge* he) {
	xy* p1 = he->incident_face->site->pos;
	xy* p2 = he->twin->incident_face->site->pos;
	xy* q = xy_new((p1->x + p2->x) / 2, (p1->y + p2->y) / 2);
	double a1 = (p2->y - p1->y) / (p2->x - p1->x);
	double m1 = -1 / a1;
	//y = m1*x+b1
	xy* d = xy_new(1 / sqrt(1 + m1 * m1), m1 / sqrt(1 + m1 * m1));

	xy** inter = (xy**)malloc(2*sizeof(xy*));
	inter[0] = (xy*) malloc(sizeof(xy));
	inter[1] = (xy*) malloc(sizeof(xy));

	double t;
	int i = 0;
	//cas 1
	t = (box->bottom - q->y) / d->y;
	double x = q->x + t * d->x;
	if (x >= box->left && x <= box->right)
	{
		inter[i]->x = x;
		inter[i]->y = box->bottom;
		i++;
	}
	//cas 2
	t = (box->top - q->y) / d->y;
	x = q->x + t * d->x;
	if (x >= box->left && x <= box->right)
	{
		inter[i]->x = x;
		inter[i]->y = box->top;
		i++;
	}
	//cas 3
	t = (box->left - q->x) / d->x;
	double y = q->y + t * d->y;
	if (i<2 && y >= box->bottom && y <= box->top)
	{
		inter[i]->x = box->left;
		inter[i]->y = y;
		i++;
	}
	//cas 4
	t = (box->right - q->x) / d->x;
	y = q->y + t * d->y;
	if (i<2 && y >= box->bottom && y <= box->top)
	{
		inter[i]->x = box->right;
		inter[i]->y = y;
		i++;
	}
	free(q);
	free(d);
	return inter;
}

xy* compute_intersection_box_2(Box* box, HalfEdge* he) {
	xy* p1 = he->incident_face->site->pos;
	xy* p2 = he->twin->incident_face->site->pos;
	xy* p = he->origin->pos;
	Face* f = he->prev->twin->incident_face;

	double a1 = (p2->y - p1->y) / (p2->x - p1->x);
	double m1 = -1 / a1;
	//y = m1*x+b1
	xy* d = xy_new(1/sqrt(1+m1*m1), m1/sqrt(1+m1*m1));

	double epsilon = 0.05;
	xy* test_point = xy_new(p->x + epsilon * d->x, p->y + epsilon * d->y);

	//to deterfmine if test_point is in face f
	double d1 = pow((test_point->x - p1->x), 2) +pow(test_point->y - p1->y, 2);
	double d2 = pow((test_point->x - f->site->pos->x),2)+pow(test_point->y - f->site->pos->y, 2);
	free(test_point);
	if (d2<d1)//if in face f
	{
		d->x = d->x * (-1);
		d->y = d->y * (-1);
	}

	double t;
    //cas 1
	t = (box->bottom-p->y)/d->y;
	double x = p->x + t * d->x;
	if (t > 0 && x>box->left && x<box->right)
	{
		free(d);
		return xy_new(x,box->bottom);
	}
	//cas 2
	t = (box->top- p->y) / d->y;
	x = p->x + t * d->x;
	if (t > 0 && x > box->left && x < box->right)
	{
		free(d);
		return xy_new(x,box->top);
	}
	//cas 3
	t = (box->left - p->x) / d->x;
	double y = p->y + t * d->y;
	if (t > 0 && y > box->bottom && y < box->top)
	{
		free(d);
		return xy_new(box->left,y);
	}
	//cas 4
	t = (box->right - p->x) / d->x;
	y = p->y + t * d->y;
	if (t > 0 && y > box->bottom && y < box->top)
	{
		free(d);
		return xy_new(box->right,y);
	}
	printf("Box problem");
	free(d);
	return NULL;
}

void segmentInBox(Segment* seg, Box* box)
{
	if (seg->a->x != seg->b->x)
	{
		// y = m*x+p
		double m = (seg->b->y - seg->a->y) / (seg->b->x - seg->a->x);
		double p = seg->b->y - m * seg->b->x;
		//top
		double x = (box->top - p) / m;
		if (seg->a->y >= box->top)
		{
			seg->a->x = x;
			seg->a->y = box->top;
		}
		else if (seg->b->y >= box->top)
		{
			seg->b->x = x;
			seg->b->y = box->top;
		}
		//bottom
		x = (box->bottom - p) / m;
		if (seg->a->y <= box->bottom)
		{
			seg->a->x = x;
			seg->a->y = box->bottom;
		}
		else if (seg->b->y <= box->bottom)
		{
			seg->b->x = x;
			seg->b->y = box->bottom;
		}
		//left
		if (seg->a->x <= box->left)
		{
			seg->a->x = box->left;
			seg->a->y = m * box->left + p;
		}
		else if (seg->b->x <= box->left)
		{
			seg->b->x = box->left;
			seg->b->y = m * box->left + p;
		}
		//right
		if (seg->a->x >= box->right)
		{
			seg->a->x = box->right;
			seg->a->y = m * box->right + p;
		}
		else if (seg->b->x >= box->right)
		{
			seg->b->x = box->right;
			seg->b->y = m * box->right + p;
		}
	}
}

HalfEdge* findHalfEdge(Site *site, Site *next_site)
{
	ListNode* nodeEdges = site->f->edges->head;
	while (nodeEdges != NULL) {
		HalfEdge* he = nodeEdges->v;
		if (he->twin->incident_face->site == next_site)
		{
			return he;
		}
		nodeEdges = nodeEdges->next;
	}
}

// connect halfedges to the beachline
List* connect_halfEdges(BeachLine* beach_line, Box* box)
{
	List *seglist = List_new();

	bool* isSeg = (bool*) malloc(beach_line->sites->n * sizeof(bool));
	for (int i = 0;i < beach_line->sites->n;i++)
		isSeg[i] = false;

	int i = 0;
	ListNode *node = beach_line->sites->head;
	while (node->next != NULL) {
		Site *site = node->v, *next_site = node->next->v;

		if (!isSeg[i] && site->pos->y - beach_line->l > 1e-6 && next_site->pos->y - beach_line->l > 1e-6) {
			HalfEdge* heBeachline = findHalfEdge(site, next_site);

			xy *intersect = computeBreakpoint(site->pos, next_site->pos, beach_line->l);

			ListNode *node2 = node->next;
			int j = i + 1;
			if (node2->next != NULL)
			{
				Site* site2 = node2->next->v;
				while (node2->next->next != NULL && site2 != site)
				{
					node2 = node2->next;
					site2 = node2->next->v;
					j++;
				}
			}
			if (heBeachline->destination == NULL && heBeachline->origin == NULL && node2->next != NULL)
			{
				Site* site2 = node2->v;
				Site* site3 = node2->next->v;
				HalfEdge* next_he = findHalfEdge(site2,site3);

				if (next_he->destination == NULL)
				{
					isSeg[j] = true;
					xy *intersect2 = computeBreakpoint(site2->pos, site3->pos, beach_line->l);
					Segment* seg = Segment_new(intersect, intersect2);
					segmentInBox(seg, box);
					List_append(seglist, seg);
					free(intersect2);
				}
			}
			else if (heBeachline->destination == NULL) {
				xy** intersections_box = compute_intersection_box_1(box, heBeachline);
				if (intersections_box[0]->y <= intersections_box[1]->y)
				{
					Segment* seg = Segment_new(intersections_box[1], intersect);
					segmentInBox(seg, box);
					List_append(seglist, seg);
				}
				else
				{
					Segment* seg = Segment_new(intersections_box[0], intersect);
					segmentInBox(seg, box);
					List_append(seglist,seg);
				}
				free(intersections_box[0]);free(intersections_box[1]);free(intersections_box);
				isSeg[i] = true;
			}
			else {
				Segment* seg = Segment_new(heBeachline->destination->pos, intersect);
				segmentInBox(seg, box);
				List_append(seglist, seg);
			}
			free(intersect);
		}
		isSeg[i] = true;
		node = node->next;
		i++;
	}
	free(isSeg);
	return seglist;
}

/////////////////////////////////////////Displaying///////////////////////////////////////////

void display_animation(DoublyConnectedEdgeList* dcel, PriorityQueue* pq,BeachLine* beach_line,Animation* animation) {
	if (PriorityQueue_is_empty(pq)) {
		if (animation->stop_end)
			display_animated_Voronoi(dcel, beach_line, animation, true);
		else {
			while (!window_should_close(animation->window)) {
				display_animated_Voronoi(dcel, beach_line, animation, false);
				beach_line->l -= animation->delta;beach_line->L -= animation->delta;
			}
		}
	}
	else {
		event* next_e = PriorityQueue_front(pq);
		double nextL = next_e->pos->y;
		double delta = animation->delta;
		int n_step = fmax(1, ceil((beach_line->L - nextL) / delta));
		delta = (beach_line->L - nextL) / n_step;
		display_animated_Voronoi(dcel, beach_line, animation,false);
		for (int i = 0;i < n_step ;i++) {
			beach_line->l -= delta;
			beach_line->L -= delta;
			display_animated_Voronoi(dcel, beach_line, animation,false);
		}
	}
}

void display_animated_Voronoi(DoublyConnectedEdgeList* dcel, BeachLine *beach_line,Animation* animation,bool ended) {
	Box *box = build_box(dcel, 25);
	box->bottom = fmin(box->bottom, beach_line->L);
	double resize = animation->resize;
	//load sites
	points_t* pointset1 = load_sites(dcel, resize);
	//load vertex of voronoi
	points_t* pointset2 = load_vertex(dcel, resize);
	//load halfedge not connected to the beachline
	points_t* pointset3 = load_edge(dcel, resize);
	//load halfedges connected to the beachline
	List *seglist = connect_halfEdges(beach_line, box);
	points_t* pointset4 = load_segs(seglist, resize);
	List_delete(seglist, Segment_delete);
	//load box
	points_t* pointset5 = load_box(box, resize);
	//load beachline
	points_t* pointset6 = load_Beachline(beach_line, box, resize);
	//load sweep line
	points_t* pointset7 = load_sweepLine(beach_line, box, resize);

	window_t* window = animation->window;
	double tbegin = window_get_time(window);
	while((window_get_time(window) - tbegin < animation->timeout || ended) && !window_should_close(window)) {
		line_strip_draw(window, pointset6, 0, pointset6->vboLen);
		lines_draw(window, pointset3, 0, pointset3->vboLen);
		lines_draw(window, pointset4, 0, pointset4->vboLen);
		line_loop_draw(window, pointset5, 0, pointset5->vboLen);
		line_strip_draw(window, pointset7, 0, pointset7->vboLen);
		points_draw(window, pointset2, 0, pointset2->vboLen);
		points_draw(window, pointset1, 0, pointset1->vboLen);
		window_update(window);
	}
	points_delete(pointset1);
	points_delete(pointset2);
	points_delete(pointset3);
	points_delete(pointset4);
	points_delete(pointset5);
	points_delete(pointset6);
	points_delete(pointset7);
	Box_delete(box);
}

void display_final_Voronoi(DoublyConnectedEdgeList* dcel, double resize, int flag)
{
	window_t* window = window_new(1080, 640, "name_window");
	Box* box = build_box(dcel, 1);
	points_t* pointset1; points_t* pointset2; points_t* pointset3; points_t* pointset4; points_t* pointset5; points_t* pointset6;
	//load sites
	pointset1 = load_sites(dcel, resize);
	if (flag == 0 || flag == 2)
	{
		//load vertex of voronoi
		pointset2 = load_vertex(dcel, resize);
		//load halfedges not connected to the box
		pointset3 = load_edge(dcel, resize);
		//load halfedges connected to the box
		List* seglist = bound_box(dcel, box);
		pointset4 = load_segs(seglist, resize);
		List_delete(seglist, Segment_delete);
		//load box
		pointset5 = load_box(box, resize);
	}
	if (flag == 1 || flag == 2)
	{
		//load halfedge
		pointset6 = load_edge_Delaunay(dcel, resize);
	}

	while (!window_should_close(window)) {
		if (flag == 0 || flag == 2)
		{
			lines_draw(window, pointset3, 0, pointset3->vboLen);
			lines_draw(window, pointset4, 0, pointset4->vboLen);
			line_loop_draw(window, pointset5, 0, pointset5->vboLen);
			points_draw(window, pointset2, 0, pointset2->vboLen);
		}
		if (flag == 1 || flag == 2)
			lines_draw(window, pointset6, 0, pointset6->vboLen);

		points_draw(window, pointset1, 0, pointset1->vboLen);
		window_update(window);
	}
	points_delete(pointset1);
	if (flag == 0 || flag == 2)
	{
		points_delete(pointset2);
		points_delete(pointset3);
		points_delete(pointset4);
		points_delete(pointset5);
	}
	if (flag == 1 || flag == 2)
		points_delete(pointset6);
	Box_delete(box);
	window_delete(window);
}

points_t* load_Beachline(BeachLine* beach_line, Box* box, double resize) {

	List* points = BeachLine_discretize(beach_line, box);
	GLfloat (*points_float)[2] = malloc(2*sizeof(GLfloat)*points->n);
	ListNode* node = points->head;
	int i = 0;
	while (node != NULL) {
		xy *pos = (xy*)node->v;
		points_float[i][0] = pos->x / resize;
		points_float[i][1] = pos->y / resize;

		node = node->next;
		i += 1;
	}
	points_t *pointset = points_new(points_float, points->n, GL_STATIC_DRAW);
	points_set_color(pointset, (float[4]) { 0, 0, 255, 1 });
	free(points_float);
	List_delete(points, xy_delete);
	return pointset;
}

points_t* load_sweepLine(BeachLine* beach_line, Box* box, double resize) {
	points_t* pointset = points_new((float[2][2]) {
		{box->left / resize,	beach_line->l / resize},
		{box->right / resize,	beach_line->l / resize},
	}, 2, GL_STATIC_DRAW);
	points_set_color(pointset, (float[4]) { 255, 0, 0, 1 });
	return pointset;
}
points_t* load_sites(DoublyConnectedEdgeList* dcel,double resize) {
	GLfloat (*sites)[2] = malloc(2*dcel->face_list->n * sizeof(GLfloat));
	ListNode* node = dcel->face_list->head;
	for (int i = 0;i < dcel->face_list->n; i++)
	{
		Face* f = node->v;
		sites[i][0] = (float)f->site->pos->x / resize;
		sites[i][1] = (float)f->site->pos->y / resize;
		node = node->next;
	}
	points_t* pointset = points_new(sites, dcel->face_list->n, GL_STATIC_DRAW);
	free(sites);
	return pointset;
}

points_t* load_vertex(DoublyConnectedEdgeList* dcel,double resize) {

	GLfloat (*vertex_voronoi)[2] = malloc(2*dcel->vertex_list->n * sizeof(GLfloat));
	ListNode* node = dcel->vertex_list->head;
	for (int i = 0;i < dcel->vertex_list->n; i++)
	{
		Vertex* v = node->v;
		vertex_voronoi[i][0] = v->pos->x / resize;
		vertex_voronoi[i][1] = v->pos->y / resize;
		node = node->next;
	}
	points_t* pointset = points_new(vertex_voronoi, dcel->vertex_list->n, GL_STATIC_DRAW);
	points_set_color(pointset, (float[4]) { 255, 0, 0, 1 });
	free(vertex_voronoi);
	return pointset;
}

// Pay attention to not load edges that are infinite
points_t* load_edge(DoublyConnectedEdgeList* dcel, double resize) {
	int n = 0;
	ListNode* node = dcel->half_edge_list->head;
	while (node != NULL) {
		HalfEdge* he = node->v;
		if (he->origin != NULL && he->destination != NULL)
			n += 1;
		node = node->next;
	}

	GLfloat (*points)[2] = malloc(4*n*sizeof(GLfloat));
	node = dcel->half_edge_list->head;
	int i = 0; // index for points
	while(node != NULL) {
		HalfEdge* he = node->v;
		if(he->origin != NULL && he->destination != NULL) {
			points[2*i][0] = he->origin->pos->x / resize;
			points[2*i][1] = he->origin->pos->y / resize;
			points[2*i+1][0] = he->destination->pos->x / resize;
			points[2*i+1][1] = he->destination->pos->y / resize;
			i += 1;
		}
		node = node->next;
	}
	points_t* pointset = points_new(points, 2 * n, GL_STATIC_DRAW);
	points_set_color(pointset, (float[4]) { 0, 255, 0, 1 });
	free(points);
	return pointset;
}

points_t* load_box(Box* box,double resize) {
	GLfloat (*corner_box)[2] = malloc(8*sizeof(GLfloat));
	corner_box[0][0] = box->left / resize;	corner_box[0][1] = box->top / resize;
	corner_box[1][0] = box->right / resize;	corner_box[1][1] = box->top / resize;
	corner_box[2][0] = box->right / resize;	corner_box[2][1] = box->bottom / resize;
	corner_box[3][0] = box->left / resize;	corner_box[3][1] = box->bottom / resize;
	points_t* pointset = points_new(corner_box, 4, GL_STATIC_DRAW);
	points_set_color(pointset, (float[4]) { 0, 255, 0, 1 });
	free(corner_box);
	return pointset;
}

points_t* load_edge_Delaunay(DoublyConnectedEdgeList* dcel, double resize) {
	GLfloat (*points)[2] = malloc(4*dcel->half_edge_list->n * sizeof(GLfloat));
	ListNode* node = dcel->half_edge_list->head;
	for (int i = 0;i < dcel->half_edge_list->n;i++) {
		HalfEdge* he = node->v;
		points[2*i][0]		= he->incident_face->site->pos->x / resize;
		points[2*i][1]		= he->incident_face->site->pos->y / resize;
		points[2*i+1][0]	= he->twin->incident_face->site->pos->x / resize;
		points[2*i+1][1]	= he->twin->incident_face->site->pos->y / resize;
		node = node->next;
	}
	points_t* pointset = points_new(points, 2 * dcel->half_edge_list->n, GL_STATIC_DRAW);
	points_set_color(pointset, (float[4]) { 255, 0, 0, 1 });
	free(points);
	return pointset;
}

// Load segments that connect the diagram to the box
points_t* load_segs(List *seglist, double resize) {
	GLfloat (*points)[2] = malloc(4*seglist->n * sizeof(GLfloat));
	ListNode *node = seglist->head;
	int i = 0;
	while(node != NULL) {
		Segment *seg = node->v;
		points[2*i][0]		= (GLfloat)seg->a->x / resize;
		points[2*i][1]		= (GLfloat)seg->a->y / resize;
		points[2*i+1][0]	= (GLfloat)seg->b->x / resize;
		points[2*i+1][1]	= (GLfloat)seg->b->y / resize;
		node = node->next;
		i += 1;
	}
	points_t* pointset = points_new(points, 2*seglist->n, GL_STATIC_DRAW);
	points_set_color(pointset, (float[4]) { 0, 255, 0, 1 });
	free(points);
	return pointset;
}



//compute cross-product: v1 et v2
double cross_product(xy* v1, xy* v2) {
	return v1->x * v2->y - v1->y * v2->x;
}

//fonction pour savoir si un point est dans une face(plus utilisee)
bool isInside(Face* f, xy* p) {
	HalfEdge* he_start = f->outer_edge;
	HalfEdge* he = he_start;
	while (he->next != he_start)
	{
		if (he->origin == NULL || he->destination == NULL)
		{
			return true;
		}
		xy* p1 = he->origin->pos;
		xy* p2 = he->destination->pos;

		xy* v1 = xy_new(p2->x - p1->x, p2->y - p1->y);
		xy* v2 = xy_new(p->x - p1->x, p->y - p1->y);


		if (cross_product(v1, v2) < 0)
		{
			free(v1);free(v2);
			return false;
		}
		free(v1);free(v2);
		he = he->next;
	}
	return true;
}
