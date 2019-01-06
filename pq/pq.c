/******************/
/* PRIORITY QUEUE */
/******************/

#include <stdio.h>
#include <stdlib.h>
#include "pq.h"

/*
	Initialise the priority queue to empty.
	Return value is a pointer to a new priority queue structure.
	This should be free'd by calling pq_destroy.
*/
struct priority_queue *pq_create()
{	
	struct priority_queue *pq = (struct priority_queue*)
								malloc(sizeof(struct priority_queue));
	if (pq != NULL ) {		
		pq->heap = (struct heapnode*)malloc(sizeof(struct heapnode) * HEAP_ARRAY_SIZE);
		if (pq->heap == NULL) { 
			free(pq);
			pq = NULL;
		}
		else {			
			pq->size = 0;
			pq->heap->priority = 0;
		}
	}
	return pq;
}


/*
	Cleanup the priority queue and free any memory within.	
*/
void pq_destroy(struct priority_queue *pq)
{	
	if (pq->heap != NULL) {
		free(pq);
	}	
}

/*
	Return the number of elements in the priority queue.
	This is needed to test the priority queue.
	returns: int, the size of the pq
*/
int pq_size(struct priority_queue *pq) {
	return pq->size;
}


/*
	Insert a value with attached min.prority in the priority queue.
	priority should be non-negative (>= 0)
	returns: 1 if successful, 0 if failed (pq is full)
*/
int pq_enqueue(struct priority_queue *pq, int val, int priority)
{	
	int index;	
	int parentIndex;
	if (pq->size >= MAX_HEAP_SIZE) {
		return 0;
	}	
	index = ++(pq->size);
	pq->heap[index].priority = priority;
	pq->heap[index].val = val;		
	
	while (index > 1) {
		parentIndex = index/2;
		if (pq_cmp(pq, parentIndex, index) <= 0) {
			break;
		}		
		pq_swap(pq, parentIndex, index);
		index = parentIndex;
	}	
	return 1;
}

/*
	Return the value with the lowest priority in the queue.
	This keeps the value stored within the priority queue.
	returns: 1 if successful, 0 if failed (pq is empty)
	val and priority are returned in the pointers
*/	
int pq_find(struct priority_queue *pq, int *val, int *priority)
{	
	if (pq->size == 0) {
		return 0;
	}
	
	(*val) = pq->heap[1].val;
	(*priority) = pq->heap[1].priority;
	
	return 1;
}

/*
	Removes the lowest priority object from the priority queue
	returns: 1 if successful, 0 if failed (pq is empty)

	note: may remove this from the spec (do cleanup)
*/
int pq_delete(struct priority_queue *pq)
{
	int index, child_left, child_right;
	
	if (pq->size == 0) {
		return 0;
	}
	
	if (pq->size == 1) {
		pq->size = 0;
	}
	else {
		pq_swap(pq, 1, pq->size);
		(pq->size)--;
		
		index = 1;
		while (1) {
			child_left = index*2;
			child_right = (index*2)+1;
			if (child_left > (pq->size)) { 
				break; 
			}
			if (child_right > (pq->size)) {
				if (pq_cmp(pq, index, child_left) <= 0) {
					break; 
				}
				pq_swap(pq, index, child_left);
				index = child_left;
			}
			else {
				if (pq_cmp(pq, index, child_left) <= 0) {					
					if (pq_cmp(pq, index, child_right) <= 0) {
						break; 
					}					
					pq_swap(pq, index, child_right);
					index = child_right;
				}
				else {
					if (pq_cmp(pq, index, child_right) <= 0) { 					
						pq_swap(pq, index, child_left);
						index = child_left;
					}
					else {
						if (pq_cmp(pq, child_left, child_right) <= 0) {
							pq_swap(pq, index, child_left);
							index = child_left;
						}
						else {
							pq_swap(pq, index, child_right);
							index = child_right;
						}
					}
				}
			}
		}
	}
	
	return 1; 
}


/*
	Returns the value with the lowest priority and removes it
	from the queue.
	This is the same as pq_find followed by pq_delete.
	returns: 1 if successful, 0 if failed (pq is empty)
	val and priority are returned in the pointers
*/	
int pq_dequeue(struct priority_queue *pq, int *val, int *priority)
{
	if (1 != pq_find(pq, val, priority)) {
		return 0; 
	}
	pq_delete(pq);
	return 1;
}

/*
	Compares two pq elements with each other.
	This is needed to test the heap contained within.
	returns: <0 if a < b, >0 if a > b, 0 if a == b
	note that the root of the heap is at index 1, not zero
*/
int pq_cmp(struct priority_queue *pq, int a, int b)
{
	return ((pq->heap[a].priority) - (pq->heap[b].priority));
}

/*
	Swaps two pq elements with each other.
	This is an internal function that will help with the heap.
*/
void pq_swap(struct priority_queue *pq, int a, int b)
{	
	
	(pq->heap[0]) = pq->heap[a];
	pq->heap[a] = pq->heap[b];
	pq->heap[b] = (pq->heap[0]);
	pq->heap->priority = 0;
}




