#ifndef MY_HEAP_H
#define MY_HEAP_H

#include <vector>

template <class T>
class Heap {
	std::vector<T> arr;

	void bottom_up(int id);
	void top_down(int id);

public:
	bool empty();
	bool exist(int node);
	void add(T data);
	T top();
	void load_array(T *arr, int len);
};


#endif
