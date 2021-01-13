#include "animation.h"

Animation* Animation_new(double resize, double timeout, double delta,bool stop_end)
{
	Animation* animation = (Animation*)malloc(sizeof(Animation));
	animation->window = window_new(1080, 640, "animation");
	animation->resize = resize;
	animation->timeout = timeout;
	animation->delta = delta;
	animation->stop_end = stop_end;//necessary if we have for example 2 pts in order to see the drawing of the halfedges
								   //also, if we don't want wait until the end
	return animation;
}
void Animation_delete(Animation* animation)
{
	window_delete(animation->window);
	free(animation);
}

BeachLine* BeachLine_new(arc* a, double L) {
    BeachLine* bl = (BeachLine*) malloc(sizeof(BeachLine));
    bl->sites = List_new();
	bl->L = L;
    bl->l = L-1e-7;
    while(a != NULL) {
        List_append(bl->sites, a->site);
        a = a->next;
    }
    return bl;
}

//a enlever
BeachLine* BeachLine_new2(BBT* bbt, Box* box,double L) {
	BeachLine* bl = (BeachLine*)malloc(sizeof(BeachLine));
	bl->sites = List_new();
	bl->L = L;
	bl->l = L - 1e-7;
	xy* p1 = xy_new(box->left, 0);
	xy* p2 = xy_new(box->right, 0);
	arc* start = findArc(bbt, p1);
	arc* end = findArc(bbt, p2);
	arc* a = start;
	while (a != end) {
		List_append(bl->sites, a->site);
		a = a->next;
	}
	List_append(bl->sites, a->site);
	return bl;
}

void Beachline_delete(BeachLine* beach_line)
{
	List_delete(beach_line->sites, NULL);
	free(beach_line);
}

void Site_print(void *obj) {
    Site *site = (Site*) obj;
    printf("(%lf,%lf) ", site->pos->x, site->pos->y);
}

void BeachLine_print(BeachLine* beach_line) {
    printf("\n\nSweep line height = %lf\n", beach_line->l);
    List_apply(beach_line->sites, Site_print);
}

//a enlever
xy* computeBreakpoint2(xy* p1, xy* p2, double L)
{
	double a1 = 1 / (2 * (p1->y - L));
	double b1 = a1 * (-2 * p1->x);
	double c1 = a1 * (pow(p1->x, 2) + pow(p1->y, 2) - pow(L, 2));
	double a2 = 1 / (2 * (p2->y - L));
	double b2 = a2 * (-2 * p2->x);
	double c2 = a2 * (pow(p2->x, 2) + pow(p2->y, 2) - pow(L, 2));

	double a = a1 - a2, b = b1 - b2, c = c1 - c2;
	double rho = b * b - 4 * a*c;
	double x_1 = (-b - sqrt(rho)) / (2 * a);

	double x = x_1;

	double y = a1 * x*x + b1 * x + c1;
	xy* breakpoint = xy_new(x, y);
	return breakpoint;
}

List* BeachLine_discretize(BeachLine* bl, Box* box) {
	double step = 1e-1;

    double left_breakpoint = box->left;
    double right_breakpoint;
	List* points = List_new();
    ListNode* node = bl->sites->head;
	//printf("box : %lf,%lf\n", box->left, box->right);

    while(node != NULL) {
        Site* s1 = node->v;
        if(node->next == NULL) {
            right_breakpoint = box->right;
        } else {
            Site* s2 = (Site*) node->next->v;
            xy* intersect = computeBreakpoint(s1->pos, s2->pos, bl->l);
			right_breakpoint = intersect->x;
			free(intersect);
        }
		//printf("points : %lf,%lf\n", left_breakpoint, right_breakpoint);

		if (right_breakpoint >= left_breakpoint)// && left_breakpoint>=box->left && right_breakpoint<=box->right)
		{
			int n_points = fmax(2, ceil((right_breakpoint - left_breakpoint) / step));
			double step_ = (right_breakpoint - left_breakpoint) / n_points;
			for (int i = fmax(0, ceil((box->left - left_breakpoint) / step_)); i < fmin(ceil((box->right - left_breakpoint) / step_), n_points); i++) {
				//for (int i =0; i < n_points; i++){
				double x = left_breakpoint + step_ * i;
				xy *p = s1->pos;
				double y = (x*x - 2 * p->x*x + p->x*p->x + p->y*p->y - bl->l*bl->l) / (2 * (p->y - bl->l));
				if(x>= box->left && x<= box->right)
					List_append(points, xy_new(x, y));
			}
		}
        node = node->next;
        left_breakpoint = right_breakpoint;
    }
    return points;
}
