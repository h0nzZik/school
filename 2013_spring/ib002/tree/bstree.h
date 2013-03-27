#ifndef BSTREE_H
#define BSTREE_H



class BSTree {
	int key;
	BSTree *left;
	BSTree *right;
	BSTree *parent;

	BSTree *find_subtree(int key);

public:
	BSTree();

	void add(int key);
	void remove(int key);
	void print_in();
	void add_random(int how_much, unsigned long seed);
	bool find(int key);
	int height();
};





#endif
