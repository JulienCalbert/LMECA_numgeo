/**
 * Generic Priority Queue Structure
 */

#ifndef PRIORITY_QUEUE_H
#define PRIORITY_QUEUE_H

#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <math.h>

// Public structures
typedef struct PriorityQueue PriorityQueue;
typedef struct PriorityQueueNode PriorityQueueNode; // node of priority queue
typedef bool(*less)(const void* element1, const void* element2); // element comparator

// Public functions
PriorityQueue* PriorityQueue_new(less less); // PQ constructor
void PriorityQueue_delete(PriorityQueue* pq); // PQ destructor
PriorityQueueNode* PriorityQueue_insert(PriorityQueue* pq, void* elem); // insert elem in pq
void* PriorityQueue_front(PriorityQueue* pq); // get biggest element from pq and return it
void* PriorityQueue_pop(PriorityQueue* pq); // pop biggest element from pq and return it
void* PriorityQueue_remove(PriorityQueue* pq, PriorityQueueNode* nd); // remove node nd from pq
bool PriorityQueue_is_empty(const PriorityQueue* pq); // checks if pq is empty
#endif
