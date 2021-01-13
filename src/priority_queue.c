#include <assert.h>

#include "priority_queue.h"

static int initial_capacity = 50;

typedef struct PriorityQueueNode {
	int index;
	void* elem;
} Node;

struct PriorityQueue {
	int capacity; // capacity (maximum n)
	int n; // number of elements
	Node** array; // array containing the nodes (array[1], ..., array[n])
	less less; // comparator
};

// Private functions
void swap(PriorityQueue* pq, int i1, int i2);
void swim(PriorityQueue* pq, int i);
void sink(PriorityQueue* pq, int i);
void resize(PriorityQueue* pq, int new_capacity);
void shrink(PriorityQueue* pq);

// Priority queue constructor
PriorityQueue* PriorityQueue_new(less less) {
	PriorityQueue* pq = malloc(sizeof(PriorityQueue));
	pq->capacity = 0;
	pq->n = 0;
	pq->array = NULL; // empty array
	pq->less = less;
	return pq;
}

// Inserts an element in the PQ and returns its index
Node* PriorityQueue_insert(PriorityQueue* pq, void* elem) {
	if(pq->capacity == 0)
		resize(pq, initial_capacity);
	else if(pq->n == pq->capacity)
		resize(pq, 2*pq->capacity);

	// Insert at end of array
	Node* nd = (Node*) malloc(sizeof(Node));
	pq->n += 1;
	nd->index = pq->n;
	nd->elem = elem;
	pq->array[pq->n] = nd;
	swim(pq, pq->n);
	return nd;
}

// Deletes greatest element and returns it
void* PriorityQueue_pop(PriorityQueue* pq) {
	// Reduce array memory (optional)
	// Q: why not after pop?
	shrink(pq);

	void* elem = pq->array[1]->elem;
	swap(pq, 1, pq->n); // swap top and bottom
	free(pq->array[pq->n]); // free node
	pq->n -= 1;
	pq->array[pq->n + 1] = NULL;
	sink(pq, 1);
	return elem;
}

void swap(PriorityQueue* pq, int i1, int i2) {
	Node* tmp = pq->array[i1];
	pq->array[i1] = pq->array[i2];	pq->array[i1]->index = i1;
	pq->array[i2] = tmp;			pq->array[i2]->index = i2;
}

void swim(PriorityQueue* pq, int i) {
	while(i > 1 && pq->less(pq->array[i/2]->elem, pq->array[i]->elem)) {
		swap(pq, i/2, i); // swap with parent
		i = i/2;
	}
}

void sink(PriorityQueue* pq, int i) {
	while (2*i <= pq->n) { // while we're not at the bottom
		int j = 2*i; // next index
		if(j < pq->n && pq->less(pq->array[j]->elem, pq->array[j+1]->elem)) // swap with biggest child
			j += 1;
		if(!pq->less(pq->array[i]->elem, pq->array[j]->elem)) break; // if parent is not less, break
		swap(pq, i, j);
		i = j;
	}
}

// Resize priority_queue to accept new capacity
void resize(PriorityQueue* pq, int new_capacity) {
	pq->capacity = new_capacity;
	pq->array = realloc(pq->array, (new_capacity+1) * sizeof(Node*));
}

bool PriorityQueue_is_empty(const PriorityQueue* pq) {
	return (pq->n == 0);
}

void PriorityQueue_delete(PriorityQueue* pq) {
	assert(pq->n == 0); // pq must be empty before free
	free(pq->array);
	free(pq);
}

// Delete specific element and return it
void* PriorityQueue_remove(PriorityQueue* pq, PriorityQueueNode* nd) {
	if (nd != NULL) { // because an arc can have none event
		// Reduce size if necessary
		shrink(pq);

		int i = nd->index;
		void* elem = nd->elem;
		swap(pq, i, pq->n);
		free(nd);
		pq->array[pq->n] = NULL;
		pq->n -= 1;
		if(i != pq->n + 1) {
			swim(pq, i);
			sink(pq, i);
		}
		return elem;
	}
	return NULL;
}

void shrink(PriorityQueue* pq) {
	if(pq->capacity > initial_capacity && pq->n < pq->capacity / 4)
		resize(pq, pq->capacity / 2);
}

void* PriorityQueue_front(PriorityQueue* pq) {
	return pq->array[1]->elem;
}
