#ifndef BSTREE_H
#define BSTREE_H


class BSTree {

private:

	int key;
	BSTree *left;
	BSTree *right;
	BSTree *parent;


	BSTree *find_node_notop(int key);
	BSTree *node_max_notop();

	int do_height();
	
	int n_childs();

	void print_infix_notop(int level);
public:
	BSTree();

	void add(int key);
	void remove(int key);
	void print_infix();
	void add_random(int how_much, unsigned long seed);
	bool find(int key);
	int height();

	BSTree *find_node(int key);
	BSTree *node_max();

	void add_node(BSTree *node);
};





#endif
