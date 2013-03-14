#if 0

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <stdbool.h>
#include <time.h>

#endif

#include <vector>
#include <cstdio>
#include <cstdlib>

class Heap {
	std::vector<int> arr;

	public:
	Heap();
	~Heap();
	bool empty();
	bool exist(int node);
	void bottom_up(int id);
	void add(int data);
	void dump();
	void top_down(int id);
	int top();
	void load_array(int *arr, int len);
	void load_random(int cnt, int mod);
};


Heap::Heap()
{
	;
}

Heap::~Heap()
{
	;
}

bool Heap::exist(int id)
{
	if (arr.size() > id)
		return true;
	return false;
}

bool Heap::empty()
{
	return this->exist(0);
}

void Heap::bottom_up(int id)
{
	int parent;
	if (id == 0)
		return;

	parent = (id - 1) / 2;
	if (arr[id] > arr[parent]){
		int tmp;
		tmp = arr[id];
		arr[id] = arr[parent];
		arr[parent] = tmp;
		bottom_up(parent);
	}
}

void Heap::add(int data)
{
	int id;

	id = arr.size();
	arr.resize(id + 1);
	arr[id] = data;
	bottom_up(id);
}


void Heap::dump()
{
	int i;
	printf("*** Dumping heap ***\n");
	for (i=0; i<arr.size(); i++) {
		printf("%lu\t%d\n", i, arr[i]);
	}
}


#if 0
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
#endif

void Heap::top_down(int id)
{
	bool go_left;
	int l,r;
	int tmp;

	l = 2*id + 1;
	r = 2*id + 2;


	if (exist(l) == false)
		return;

	go_left = true;
	if (exist(r) && arr[r] > arr[l])
		go_left = false;

	if (go_left == true) {
		if (arr[l] > arr[id]) {
			tmp = arr[id];
			arr[id] = arr[l];
			arr[l] = tmp;
			top_down(l);
		}
	} else {
		if (arr[r] > arr[id]) {
			tmp = arr[id];
			arr[id] = arr[r];
			arr[r] = tmp;
			top_down(r);
		}
	}
}

int Heap::top()
{
	int top;
	top = arr[0];
	arr[0] = arr[arr.size() - 1];
	arr.resize(arr.size() - 1);
	top_down(0);
}

void Heap::load_array(int *arr, int len)
{
	int i;
	for (i=0; i<len; i++)
		add(arr[i]);
}

void Heap::load_random(int cnt, int mod)
{
	int i;
	for (i=0; i<cnt; i++)
		add(rand() % mod);
}

#if 0

void interactive_destroy(heap_t *h)
{
	char in[2];
	int tmp;
	while (!heap_isempty(h)) {
		heap_dump_graphviz(h, "heap.digraph");
//		fgets(in, 1, stdin);
		fflush(stdin);
		fflush(stdout);
		getchar();
//		scanf("%d", &tmp);
		printf("poped: %d\n", heap_top(h));

	}
	heap_dump_graphviz(h, "heap.digraph");

}
#endif
int main(int argc, char **argv)
{
	Heap *h;

	h = new Heap();

	h->load_random(10, 10);
	h->dump();

	delete h;
	return 0;
}
