#ifndef _BSTree_h
#define _BSTree_h


/* Node of a BST */

/**
 * The Node class represents a node inside a BST
 *
 */




class BStree {
private:
	/* some nested class? */
	class Node {
	public:
		Node *parent;
		Node *child[2];
		int key;

		Node(int key);
		void add(Node *n);

		int depth(void);
		void dump(int level);
		Node *get_imbalanced();
		Node *unlink();
		Node *find_subtree(int key);
		void check();
	};

	
	
	Node *root;

	int do_fix(void);
	void do_remove(int key);

public:
	BStree();
	void add(int key);
	void dump(void);
	void remove(int key);

};


#endif
