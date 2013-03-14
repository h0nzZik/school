/**
 * Binary search
 */

#include <stdio.h>
#include <stdlib.h>
#include <time.h>

/**
 * @param lbound	minimal valid index
 * @param rbound	maximal valid index
 */
int my_bsearch_internal(int *arr, int lbound, int rbound, int value)
{
	int idx;
	/* calculate index */
	idx = lbound + (rbound - lbound)/2;

	printf("trying %3d == %3d at [%3d] --- [%3d] --- [%3d]\n",
			value, arr[idx], lbound, idx, rbound);

	if (arr[idx] == value) {
		printf("found\n");
		return idx;
	}

	if (rbound - lbound < 1) {
		printf("not found\n");
		return -1;
	}

	if (arr[idx] > value) {
		printf("left\n");
		return  my_bsearch_internal(arr, lbound, idx-1, value);
	}

	if (arr[idx] < value) {
		printf("right\n");
		return my_bsearch_internal(arr, idx + 1, rbound, value);
	}

	printf("some bug\n");
	return -1;
}

/**
 *
 */
int my_bsearch(int *arr, int size, int value)
{

	return my_bsearch_internal(arr, 0, size-1, value);
}

void arr_print(int *arr, int sz)
{
	int i;
	for (i=0; i<sz; i++) {
		printf("[%3d] %d\n", i, arr[i]);
	}
}

int my_bsearch_norecursive(int *arr, int size, int value)
{
	int l,r;
	int idx;
	l = 0;
	r = size - 1;

	while (1) {
		idx = l + (r - l)/2;

		printf("================================\n");
		printf(" %d ?= %d\n", arr[idx], value);
		printf(" %4d  -----  %4d  -----  %4d \n", l, idx, r);
		printf("[%4d] ----- [%4d] ----- [%4d]\n",
				arr[l], arr[idx], arr[r]);

		/* missing else? nop ;) */
		if (arr[idx] == value) {
			printf("found\n");
			return idx;
		}

		if (r - l < 1) {
			printf("not found, r: %d, l: %d\n", r, l);
			return -1;
		}

		if (arr[idx] < value) {
			printf("too small. going right\n");
			l = idx + 1;
		}

		if (arr[idx] > value) {
			printf("too big. going left\n");
			r = idx - 1;
		}

	}

	return -1;
}


int my_lsearch(int *arr, int size, int value)
{
	int i;
	for (i=0; i<size; i++)
		if (arr[i] == value)
			return i;
	return -1;
}

int test_search(int *arr, int size, int value)
{
	int a, b;

	a = my_lsearch(arr, size, value);
	b = my_bsearch(arr, size, value);

	if (a == b) {
		printf("ok\n");
		return 1;
	} else {
		printf("missmatch\n");
		return 0;
	}
}

int main(void)
{
	const int sz = 100;
	int i;
	int arr[sz];
	int last = 0;

	srand(time(NULL));

	for (i=0; i<sz; i++) {
		arr[i] = last + rand()%10;
		last = arr[i];
	}

//	arr_print(arr, sz);
	/*
	i = my_bsearch_norecursive(arr, sz, rand() % (arr[sz - 1] + 1) );
	if (i < 0)
		printf("smula\n");
	else
		printf("found at %d\n", i);
	*/
	i = test_search(arr, sz, rand() % (arr[sz - 1] + 1));
	/* missmatch */
	if (i == 0) {
		arr_print(arr, sz);
		return 1;
	}

	return 0;
}

