#include <vector>
#include <cstdio>
#include <cstdlib>

#include "heap.h"


template <class T>
bool Heap<T>::exist(int id)
{
	if (arr.size() > id)
		return true;
	return false;
}

template <class T>
bool Heap<T>::empty()
{
	return this->exist(0);
}

template <class T>
void Heap<T>::bottom_up(int id)
{
	int parent;
	if (id == 0)
		return;

	parent = (id - 1) / 2;
	if (arr[id] > arr[parent]){
		T tmp;
		tmp = arr[id];
		arr[id] = arr[parent];
		arr[parent] = tmp;
		bottom_up(parent);
	}
}

template <class T>
void Heap<T>::add(T data)
{
	int id;

	id = arr.size();
	arr.resize(id + 1);
	arr[id] = data;
	bottom_up(id);
}

template <class T>
void Heap<T>::top_down(int id)
{
	bool go_left;
	int l,r;
	T tmp;

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
template <class T>
T Heap<T>::top()
{
	T top;
	top = arr[0];
	arr[0] = arr[arr.size() - 1];
	arr.resize(arr.size() - 1);
	top_down(0);
}

template <class T>
void Heap<T>::load_array(T *arr, int len)
{
	int i;
	for (i=0; i<len; i++)
		add(arr[i]);
}


int main(int argc, char **argv)
{
	Heap<int> *h;

	h = new Heap<int>();

	int i;
	for (i=0; i<50; i++) {
		h->add(i);
	}
//	h->load_random(10, 10);
//	h->dump();

	delete h;
	return 0;
}
