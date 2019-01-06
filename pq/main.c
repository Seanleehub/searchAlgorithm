/*
	main.c
	Algorithms and Analysis: Assignment 2
	Test framework for min-priority queue

	Don't change any of the code within this module!
	Implement your priority queue in pq.c
*/

#include <stdio.h>
#include <stdlib.h>
#include <limits.h>

#include "pq.h"

#define NUM_PQ_TESTS		(8)

int main(void);
int heaptest(struct priority_queue *pq);
int run_pq_test(int testcase);


int main(void)
{
	int i, p;

	p = 0;
	for (i = 0; i < NUM_PQ_TESTS; i++) {
		p += run_pq_test(i);
	}
	printf("Results: %d pass, %d fail, %d tests total\n",
		p, (i-p), i);

	return EXIT_SUCCESS;
}

int run_pq_test(int testcase)
{
	struct priority_queue *pq;
	int ret;
	char *testname;
	char *result;
	int val, prio;
	int i, j, n;

	pq = pq_create();

	testname = "undefined";
	ret = 0;

	switch (testcase) {

	case 0:
		testname = "initialisation";
		if (pq == NULL) { break; }
		if (pq_size(pq) != 0) { break; }
		ret = 1;
		break;

	case 1:
		testname = "single insert";
		if (pq == NULL) { break; }
		if (!pq_enqueue(pq, 1, 1)) { break; }
		if (pq_size(pq) != 1) { break; }
		val = prio = 0;
		if (!pq_find(pq, &val, &prio)) { break; }
		if (val != 1 || prio != 1) { break; }
		if (pq_size(pq) != 1) { break; }		
		ret = 1;
		break;

	case 2:
		testname = "single pop";
		if (pq == NULL) { break; }
		if (!pq_enqueue(pq, 2, 1)) { break; }
		if (pq_size(pq) != 1) { break; }
		val = prio = 0;
		if (!pq_dequeue(pq, &val, &prio)) { break; }
		if (val != 2 || prio != 1) { break; }
		if (pq_size(pq) != 0) { break; }
		ret = 1;
		break;

	case 3:
		testname = "small heap";
		if (pq == NULL) { break; }
		ret = 1;
		n = 10;
		for (i = 0; i < n; i++) {
			/* make a non-sorted sequence of priorities */
			prio = (i*7) % 10;
			val = i;
			if (!pq_enqueue(pq, val, prio)) { ret = 0; }
			/* test heap property after each insert */
			if (!heaptest(pq)) { ret = 0; }
			if (pq_size(pq) != i+1) { ret = 0; }
		}
		/* check pop values */
		for (i = 0; i < n; i++) {
			if (!pq_dequeue(pq, &val, &prio)) { ret = 0; }
			if (prio != i) { ret = 0; }
			if (!heaptest(pq)) { ret = 0; }
			if (pq_size(pq) != (n-i-1)) { ret = 0; }			
		}
		break;

	case 4:
		testname = "heap with repeats";
		if (pq == NULL) { break; }
		ret = 1;
		n = 100;
		for (i = 0; i < n; i++) {
			/* make a non-sorted sequence of priorities */
			prio = (i*7) % 10;
			val = i;
			if (!pq_enqueue(pq, val, prio)) { ret = 0; }
			/* test heap property after each insert */
			if (!heaptest(pq)) { ret = 0; }
			if (pq_size(pq) != i+1) { ret = 0; }
		}
		/* check pop values */
		j = -1;
		for (i = 0; i < n; i++) {
			if (!pq_dequeue(pq, &val, &prio)) { ret = 0; }
			if (prio < j) { ret = 0; }
			j = prio;
			if (!heaptest(pq)) { ret = 0; }
			if (pq_size(pq) != (n-i-1)) { ret = 0; }			
		}
		break;

	case 5:
		testname = "large sorted";
		if (pq == NULL) { break; }
		ret = 1;
		n = LARGE_TEST_SIZE;
		for (i = 0; i < n; i++) {
			prio = i;
			val = i;
			if (!pq_enqueue(pq, val, prio)) { ret = 0; }
			if (pq_size(pq) != i+1) { ret = 0; }
		}
		/* too slow to check at every insert, check at end */
		if (!heaptest(pq)) { ret = 0; }
		/* check pop values */
		for (i = 0; i < n; i++) {
			if (!pq_dequeue(pq, &val, &prio)) { ret = 0; }
			if (prio != i) { ret = 0; }
			if (pq_size(pq) != (n-i-1)) { ret = 0; }			
		}
		break;

	case 6:
		testname = "large reverse sorted";
		if (pq == NULL) { break; }
		ret = 1;
		n = LARGE_TEST_SIZE;
		for (i = 0; i < n; i++) {
			prio = n-i-1;
			val = i;
			if (!pq_enqueue(pq, val, prio)) { ret = 0; }
			if (pq_size(pq) != i+1) { ret = 0; }
		}
		/* too slow to check at every insert, check at end */
		if (!heaptest(pq)) { ret = 0; }
		/* check pop values */
		for (i = 0; i < n; i++) {
			if (!pq_dequeue(pq, &val, &prio)) { ret = 0; }
			if (prio != i) { ret = 0; }
			if (pq_size(pq) != (n-i-1)) { ret = 0; }			
		}
		break;

	case 7:
		testname = "large with repeats";
		if (pq == NULL) { break; }
		ret = 1;
		n = LARGE_TEST_SIZE;
		j = 1;
		for (i = 0; i < n; i++) {
			prio = j;
			val = i;
			j = (j+7)%13;
			if (!pq_enqueue(pq, val, prio)) { ret = 0; }
			if (pq_size(pq) != i+1) { ret = 0; }
		}
		/* too slow to check at every insert, check at end */
		if (!heaptest(pq)) { ret = 0; }
		/* check pop values */
		j = -1;
		for (i = 0; i < n; i++) {
			if (!pq_dequeue(pq, &val, &prio)) { ret = 0; }
			if (prio < j) { ret = 0; }
			if (prio > j && !heaptest(pq)) { ret = 0; }
			j = prio;
			if (pq_size(pq) != (n-i-1)) { ret = 0; }			
		}
		break;

	default:
		/* undefined */
		break;
	}

	pq_destroy(pq);

	result = (ret ? "OK" : "FAIL");

	printf("TEST %d\t%24s\t%4s\n", testcase, testname, result);

	return ret;
}

/* tests whether the heap property holds in pq */
int heaptest(struct priority_queue *pq)
{
	int size, i;

	if (pq == NULL) {
		return 0;
	}

	size = pq_size(pq);
	for (i = 1; i <= size; i++) {
		if (pq_cmp(pq, i/2, i) > 0) {
			return 0;
		}
	}

	return 1;
}


