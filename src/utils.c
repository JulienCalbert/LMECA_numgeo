#include "utils.h"


xy* xy_new(double x, double y) {
	xy* pt = (xy*)malloc(sizeof(xy));
	pt->x = x, pt->y = y;
	return pt;
}

void xy_tab_delete(xy** tab, int size)
{
	for (int i = 0;i < size;i++) {
		free(tab[i]);
	}
	free(tab);
}

void xy_delete(void* obj)
{
	xy* p = (xy*)obj;
	free(obj);
}

List* List_new() {
	List *list = (List*) malloc(sizeof(List));
	list->n = 0;
	list->head = list->tail = NULL;
	return list;
}

void List_append(List *l, void *v) {
	ListNode* node = (ListNode*)malloc(sizeof(ListNode));
	node->v = v;
	node->next = NULL;

	if(l->n == 0)
		l->head = l->tail = node;
	else {
		l->tail->next = node;
		l->tail = node;
	}
	l->n++;
}

void List_delete(List* list, destructor des) {
	ListNode* node1 = list->head;
	ListNode* node2 = NULL;
	for (int i = 0; i < list->n; i++) {
		if(des != NULL) des(node1->v);
        node2 = node1;
        node1 = node1->next;
        free(node2);
	}
	free(list);
}

void List_apply(List* list, application app) {
	ListNode *node = list->head;
	while(node != NULL) {
		app(node->v);
		node = node->next;
	}
}

Segment* Segment_new(xy* a, xy* b) {
	Segment *seg = (Segment*) malloc(sizeof(Segment));
	seg->a = xy_new(a->x, a->y);
	seg->b = xy_new(b->x, b->y);
	return seg;
}

void Segment_delete(void *obj)
{
	Segment* s = (Segment*)obj;
	free(s->a);
	free(s->b);
	free(s);
}