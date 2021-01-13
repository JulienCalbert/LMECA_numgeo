#include "doubly_connected_edge_list.h"
#include "event.h"

BBT* BBT_new()
{
	BBT* bbt = (BBT*)malloc(sizeof(BBT));
	bbt->root = NULL;
	bbt->L = 0;
	return bbt;
}

void insert_root(BBT* bbt, Site* site)
{
	arc* a = arc_build(site);
	bbt->root = a;
	bbt->root->color = BLACK;
}

bool BBT_empty(BBT* bbt)
{
	return bbt->root == NULL;
}

void BBT_delete(BBT* bbt)
{
	BBT_delete_recursive(bbt->root);
	free(bbt);
}

void BBT_delete_recursive(arc* a)
{
	if (a == NULL)
		return;
	else
	{
		BBT_delete_recursive(a->left);
		BBT_delete_recursive(a->right);
		free(a);
	}
}

arc* arc_build(Site* s)
{
	arc* a = (arc*)malloc(sizeof(arc));
	//BBT structure
	a->parent = NULL;
	a->left = NULL;
	a->right = NULL;
	a->color = RED;
	//for beachline structure
	a->site = s;
	a->leftHalfEdge = NULL;
	a->rightHalfEdge = NULL;
	a->event_node = NULL;
	a->prev = NULL;
	a->next = NULL;
	return a;
}

void arc_delete(arc* a)
{
	free(a);
}

arc* findArc(BBT* bbt, xy* pt)
{
	arc* node = bbt->root;
	bool found = false;
	while (!found)
	{
		double breakpointLeft_x = -INFINITY;
		double breakpointRight_x = INFINITY;
		if (node->prev != NULL)
		{
			xy* breakpointLeft = computeBreakpoint(node->prev->site->pos, node->site->pos, bbt->L);
			breakpointLeft_x = breakpointLeft->x;
			free(breakpointLeft);
		}
		if (node->next != NULL)
		{
			xy* breakpointRight = computeBreakpoint(node->site->pos, node->next->site->pos, bbt->L);
			breakpointRight_x = breakpointRight->x;
			free(breakpointRight);
		}
		if (pt->x < breakpointLeft_x)
			node = node->left;
		else if (pt->x > breakpointRight_x)
			node = node->right;
		else
			found = true;
	}
	return node;
}

xy* computeBreakpoint(xy* p1, xy* p2, double L)
{
	double a1 = 1 / (2 * (p1->y - L));
	double b1 = a1 * (-2 * p1->x);
	double c1 = a1 * (pow(p1->x, 2) + pow(p1->y, 2) - pow(L, 2));
	double a2 = 1 / (2 * (p2->y - L));
	double b2 = a2 * (-2 * p2->x);
	double c2 = a2 * (pow(p2->x, 2) + pow(p2->y, 2) - pow(L, 2));

	double a = a1 - a2, b = b1 - b2, c = c1 - c2;
	double rho = b * b - 4 * a*c;
	double x_1 = (-b + sqrt(rho)) / (2 * a);

	double x = x_1;

	double y = a1 * x*x + b1 * x + c1;
	xy* breakpoint = xy_new(x, y);
	return breakpoint;
}

//insert b before a
void insert_before(BBT* bbt,arc* a, arc* b)
{
	if (a->left == NULL)
	{
		a->left = b;
		b->parent = a;
	}
	else
	{
		a->prev->right = b;
		b->parent = a->prev;
	}
	b->prev = a->prev;
	if (b->prev != NULL)
		b->prev->next = b;
	b->next = a;
	a->prev = b;
	// balance the tree
	//insert_balance(bbt, b);
}

//insert b after a
void insert_after(BBT* bbt,arc* a, arc* b)
{
	if (a->right == NULL)
	{
		a->right = b;
		b->parent = a;
	}
	else
	{
		a->next->left = b;
		b->parent = a->next;
	}
	b->next = a->next;
	if (b->next != NULL)
		b->next->prev = b;
	b->prev = a;
	a->next = b;
	// balance the tree
	//insert_balance(bbt,b);
}

//replace a by b
void arc_replace(BBT* bbt, arc* a, arc* b)
{
	transplant(bbt, a, b);

	b->left = a->left;
	b->right = a->right;
	if (b->left != NULL)
		b->left->parent = b;
	if (b->right != NULL)
		b->right->parent = b;
	b->prev = a->prev;
	b->next = a->next;
	if (b->prev != NULL)
		b->prev->next = b;
	if (b->next != NULL)
		b->next->prev = b;
	b->color = a->color;
}

void BBT_remove(BBT* bbt, arc* z)
{
	arc* y = z;
	Color yOriginalColor = y->color;
	arc* x;
	if (z->left == NULL)//mieux apres de distinguer le cas NULL-NULL
	{
		x = z->right;
		transplant(bbt, z, z->right);
	}
	else if (z->right == NULL)
	{
		x = z->left;
		transplant(bbt, z, z->left);
	}
	else
	{
		y = minimum(z->right);
		yOriginalColor = y->color;
		x = y->right;

		transplant(bbt, y, y->right);
		y->right = z->right;
		if(z->right != NULL)
			y->right->parent = y;

		transplant(bbt, z, y);
		y->left = z->left;
		y->left->parent = y;
		y->color = z->color;
	}
	//if (yOriginalColor == BLACK)
		//remove_balance(bbt, x);

	// Update next and prev
	if (z->prev != NULL)
		z->prev->next = z->next;
	if (z->next != NULL)
		z->next->prev = z->prev;
}

//part which replace a by b for the parents of a
void transplant(BBT* bbt, arc* a, arc* b)
{
	if (a->parent == NULL)
		bbt->root = b;
	else if (a == a->parent->left) //if a is the left node of a->parent
		a->parent->left = b;
	else
		a->parent->right = b;
	if(b!=NULL)
		b->parent = a->parent;
}

arc* minimum(arc* x) {
	while (x->left != NULL)
		x = x->left;
	return x;
}

void insert_balance(BBT* bbt,arc* z)
{
	while (z->parent != NULL && z->parent->color == RED)
	{
		if (z->parent->parent != NULL && z->parent == z->parent->parent->left)
		{
			arc* y = z->parent->parent->right;
			// Case 1
			if (y!=NULL && y->color == RED)
			{
				z->parent->color = BLACK;
				y->color = BLACK;
				z->parent->parent->color = RED;
				z = z->parent->parent;
			}
			else
			{
				// Case 2
				if (z == z->parent->right)
				{
					z = z->parent;
					rotateLeft(bbt,z);
				}
				// Case 3

				z->parent->color = BLACK;
				z->parent->parent->color = RED;
				rotateRight(bbt,z->parent->parent);
			}
		}
		else
		{
			arc* y = z->parent->parent->left;
			// Case 1
			if (y != NULL && y->color == RED)
			{
				z->parent->color = BLACK;
				y->color = BLACK;
				z->parent->parent->color = RED;
				z = z->parent->parent;
			}
			else
			{
				// Case 2
				if (z == z->parent->left)
				{
					z = z->parent;
					rotateRight(bbt,z);
				}
				// Case 3
				z->parent->color = BLACK;
				z->parent->parent->color = RED;
				rotateLeft(bbt,z->parent->parent);
			}
		}
	}
	bbt->root->color = BLACK;
}

void remove_balance(BBT* bbt, arc* x)
{

	while (x != bbt->root && x!=NULL && x->color == BLACK)
	{
		arc* w;
		if (x == x->parent->left)
		{
			w = x->parent->right;
			// Case 1
			if (w != NULL && w->color == RED)
			{
				w->color = BLACK;
				x->parent->color = RED;
				rotateLeft(bbt,x->parent);
				w = x->parent->right;
			}
			// Case 2
			if (w->left->color == BLACK && w->right->color == BLACK)
			{
				w->color = RED;
				x = x->parent;
			}
			else
			{
				// Case 3
				if (w->right != NULL && w->right->color == BLACK)
				{
					w->left->color = BLACK;
					w->color = RED;
					rotateRight(bbt,w);
					w = x->parent->right;
				}
				// Case 4
				w->color = x->parent->color;
				x->parent->color = BLACK;
				if(w->right!=NULL)
					w->right->color = BLACK;
				rotateLeft(bbt,x->parent);
				x = bbt->root;
			}
		}
		else
		{
			w = x->parent->left;
			// Case 1
			if (w!=NULL && w->color == RED)
			{
				w->color = BLACK;
				x->parent->color = RED;
				rotateRight(bbt,x->parent);
				w = x->parent->left;
			}
			// Case 2
			if (w!= NULL && w->left != NULL && w->left->color == BLACK && w->right->color == BLACK)
			{
				w->color = RED;
				x = x->parent;
			}
			else
			{
				// Case 3
				if (w!= NULL && w->left != NULL && w->left->color == BLACK)
				{
					w->right->color = BLACK;
					w->color = RED;
					rotateLeft(bbt,w);
					w = x->parent->left;
				}
				// Case 4
				if(w!=NULL)
					w->color = x->parent->color;
				x->parent->color = BLACK;
				if(w!=NULL && w->left != NULL)
					w->left->color = BLACK;
				rotateRight(bbt,x->parent);
				x = bbt->root;
			}
		}
	}
	if(x!=NULL)
		x->color = BLACK;
}


void rotateLeft(BBT* bbt,arc* h)
{
	arc* x = h->right;
	h->right = x->left;
	if (x->left != NULL)
		x->left->parent = h;
	x->parent = h->parent;
	if (h->parent == NULL)
		bbt->root = x;
	else if (h->parent->left == h)
		h->parent->left = x;
	else
		h->parent->right = x;
	x->left = h;
	h->parent = x;
}

void rotateRight(BBT* bbt,arc* h)
{
	arc* x = h->left;
	h->left = x->right;
	if (x->right != NULL)
		x->right->parent = h;
	x->parent = h->parent;
	if (h->parent == NULL)
		bbt->root = x;
	else if (h->parent->left == h)
		h->parent->left = x;
	else
		h->parent->right = x;
	x->right = h;
	h->parent = x;
}

arc* find_first_arc(BBT* bbt)
{
	arc* node = bbt->root;
	while (node->left != NULL)
		node = node->left;
	return node;
}

void printBeachline(BBT* bbt)
{
	arc* node = find_first_arc(bbt);
	while (node != NULL)
	{
		printf("(%lf ,%lf) , ", node->site->pos->x ,node->site->pos->y);
		node = node->next;
	}
}


/////////////////////////////////////////////////////////////////////////////////////////
//break arc a and rebuild two times the arc a, with a new arc between the 2 new arc a
arc* arc_break(BBT* bbt,arc* a,Site* site)
{
	// build the new subtree
	arc* arc_middle = arc_build(site);

	arc* arc_left = arc_build(a->site);
	arc_left->leftHalfEdge = a->leftHalfEdge;
	arc* arc_right = arc_build(a->site);
	arc_right->rightHalfEdge = a->rightHalfEdge;
	// insert in the beachline
	arc_replace(bbt,a, arc_middle);
	insert_before(bbt,arc_middle, arc_left);
	insert_after(bbt,arc_middle, arc_right);
	// delete old arc
	arc_delete(a);
	// return middle arc (the new arc)
	return arc_middle;
}

void arc_remove(DoublyConnectedEdgeList* dcel,BBT* bbt, arc* a, Vertex* v)
{
	// set the ends of the edges
	set_destination(a->prev, a, v);
	set_destination(a, a->next, v);
	// join the edges
	a->leftHalfEdge->next = a->rightHalfEdge;
	a->rightHalfEdge->prev = a->leftHalfEdge;
	// update beachline
	BBT_remove(bbt,a);
	// create a new edge and set all the pointers
	HalfEdge* prevHalfEdge = a->prev->rightHalfEdge;
	HalfEdge* nextHalfEdge = a->next->leftHalfEdge;
	add_edge(a->prev, a->next,dcel); //les 2 nouveaux half-edge (1 edge)
	set_origin(a->prev, a->next, v);

	a->prev->rightHalfEdge->next = prevHalfEdge;
	prevHalfEdge->prev = a->prev->rightHalfEdge;

	nextHalfEdge->next = a->next->leftHalfEdge;
	a->next->leftHalfEdge->prev = nextHalfEdge;
	// delete arc
	arc_delete(a);
}

//build edge between arc left and arc right
void add_edge(arc* left, arc* right, DoublyConnectedEdgeList* dcel) {
	left->rightHalfEdge = half_edge_build(dcel,left->site->f);
	right->leftHalfEdge = half_edge_build(dcel,right->site->f);
	// set the half edges to be twins
	left->rightHalfEdge->twin = right->leftHalfEdge; //logique car 2 face voisine
	right->leftHalfEdge->twin = left->rightHalfEdge;
}

void set_destination(arc* left, arc* right, Vertex* v)
{
	left->rightHalfEdge->origin = v; //car sens anti-horlogique
	right->leftHalfEdge->destination = v;
}
void set_origin(arc* left, arc* right, Vertex* v)
{
	left->rightHalfEdge->destination = v;
	right->leftHalfEdge->origin = v;
}

//this circle event was a false alert
//arc a can only contain a circle event
void delete_event_false_alarm(arc* a, PriorityQueue* pq)
{
	event* e = PriorityQueue_remove(pq, a->event_node);
	if (e != NULL) { //il est possible qu un arc n'ait pas de circle event associé
		if (e->is_circle) {
			free(e->pos);
			free(e->c);
			free(e);
		}
		else
			printf("IMPOSSIBLE"); //never happend
	}
}
