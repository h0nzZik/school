#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <stdbool.h>
#include <time.h>


typedef struct {
	size_t size;
	size_t n;
	int *arr;
} heap_t;


/**
 * Creates new heap, calls abort() on error
 * @return	new heap
 */
heap_t *heap_new(void)
{
	heap_t *h;
	h = malloc(sizeof(*h));
	if (!h) {
		perror("malloc\n");
		abort();
	}

	memset(h, 0, sizeof(*h));
	h->size = 16;
	h->arr = malloc(h->size * sizeof(int));
	if (!h->arr) {
		free(h);
		perror("malloc\n");
		abort();
	}

	memset(h->arr, 0, sizeof(int) * h->size);
	return h;
}

bool heap_isempty(heap_t *h)
{
	if (h->n == 0)
		return true;
	return false;
}

void heap_check_enlarge(heap_t *h)
{
	int *na;
	if (h->n != h->size)
		return;

	na = realloc(h->arr, h->size * 2 * sizeof(int));
	if (!na) {
		perror("malloc\n");
		free(h->arr);
		free(h);
	}
	h->arr = na;
	h->size*=2;
}

bool heap_idx_exist(heap_t *h, size_t idx)
{
	if (idx < h->n)
		return true;
	return false;
}

void heap_check_bottom_up(heap_t *h, int which)
{
	int parent;

	/* root? */
	if (which == 0)
		return;

	parent = (which - 1) / 2;
	if (h->arr[which] > h->arr[parent]) {
		int tmp;
		tmp = h->arr[which];
		h->arr[which] = h->arr[parent];
		h->arr[parent] = tmp;

		heap_check_bottom_up(h, parent);
	}
}

void heap_print(heap_t *h)
{
	size_t i;
	printf("*** Dumping heap ***\n");
	for (i=0; i<h->n; i++) {
		printf("%lu\t%d\n", i, h->arr[i]);
	}
}

void graphviz_put_line(FILE *fw, size_t a, size_t b)
{
	fprintf(fw, "\t%lu -> %lu\n", a, b);
}

void heap_graphviz_create_node(heap_t *h, FILE *fw, size_t node)
{
	fprintf(fw, "\t%lu [label=\"%d\"]\n", node, h->arr[node]);

}
void heap_dump_graphviz_node(heap_t *h, FILE *fw, size_t node)
{

	size_t l, r;

	l = 2*node + 1;
	r = 2*node + 2;

	if (heap_idx_exist(h, l)){

		heap_graphviz_create_node(h, fw, l);
		graphviz_put_line(fw, node, l);
		heap_dump_graphviz_node(h, fw, l);
	}

	if (heap_idx_exist(h, r)){
		heap_graphviz_create_node(h, fw, r);
		graphviz_put_line(fw, node, r);
		heap_dump_graphviz_node(h, fw, r);
	}
}

void heap_dump_graphviz(heap_t *h, const char *fname)
{
	FILE *fw;
	fw = fopen(fname, "wt");
	if (!fw) {
		perror("fopen");
		return;
	}
	fprintf(fw, "digraph G {\n");

	if (heap_idx_exist(h, 0)) {
		heap_graphviz_create_node(h, fw, 0);
		heap_dump_graphviz_node(h, fw, 0);
	}

	fprintf(fw, "}\n");
	fclose(fw);
}

void heap_add(heap_t *h, int data)
{
	heap_check_enlarge(h);

	/* append */
	h->arr[h->n] = data;

	/* bubble up */
	heap_check_bottom_up(h, h->n);
	h->n++;
}


void heap_check_top_down(heap_t *h, int which)
{
	bool go_left;

	int l, r;

	l = 2*which + 1;
	r = 2*which + 2;

	/* stop */
	if (heap_idx_exist(h, l) == false) {
		return;
	}

	printf("top_down: %d\n", which);
	go_left = true;
	if (heap_idx_exist(h, r) && h->arr[r] > h->arr[l])
		go_left = false;

	if (go_left == true) {
		printf("going left\n");
		if (h->arr[l] > h->arr[which]) {
			printf("swapping\n");
			int tmp;
			tmp = h->arr[l];
			h->arr[l] = h->arr[which];
			h->arr[which] = tmp;
			heap_check_top_down(h, l);
		}
	} else {
		printf("going right\n");
		if (h->arr[l] > h->arr[which]) {
			printf("swapping\n");
			int tmp;
			tmp = h->arr[r];
			h->arr[r] = h->arr[which];
			h->arr[which] = tmp;
			heap_check_top_down(h, r);
		}
	}
}

int heap_top(heap_t *h)
{
	int top;

	top = h->arr[0];
	h->n--;
	h->arr[0] = h->arr[h->n];

	heap_check_top_down(h, 0);
	return top;
}


void arr_to_heap(heap_t *h, int *arr, size_t len)
{
	size_t i;
	for (i=0; i<len; i++) {
		printf("adding %d\n", arr[i]);
		heap_add(h, arr[i]);
	}
}


void rand_to_heap(heap_t *h, int how_much)
{
	int i;
	int x;
	srand(time(NULL));
	for (i=0; i<how_much; i++) {
		x = rand() % 500;
		printf("adding %d\n", x);
		heap_add(h, x);
	}
}

void interactive_destroy(heap_t *h)
{
	while (!heap_isempty(h)) {
		heap_dump_graphviz(h, "heap.digraph");
		fflush(stdin);
		fflush(stdout);
		getchar();
		printf("poped: %d\n", heap_top(h));

	}
	heap_dump_graphviz(h, "heap.digraph");

}

int main(int argc, char **argv)
{
	heap_t *h;
	unsigned int num;

	num = 100;
	if (argc >= 2) {
		if (1 != sscanf(argv[1], "%u", &num)) {
			printf("bad argument\n");
			exit(1);
		}
	}

	h = heap_new();

	rand_to_heap(h, num);
	interactive_destroy(h);
	return 0;
}
