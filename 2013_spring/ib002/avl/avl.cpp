#include <cstdio>
#include <cstdlib>
#include <cstring>


/* Node of a BST */

/**
 * The Node class represents a node inside a BST
 *
 */
class Node {
public:
	Node *parent;
	Node *child[2];
	int key;

	Node(int key);
/**
 * Adds node into BST
 * @param n	node to add
 */
	void add(Node *n);

	int depth(void);
	void dump(int level);
	Node *get_imbalanced();
	Node *unlink();
	Node *find_subtree(int key);
	void check();
};


void Node::add(Node *n)
{
	/* child[0]? */
	if (n->key <= this->key) {
		if (child[0])
			child[0]->add(n);
		else {
			child[0] = n;
			child[0]->parent = this;
		}
	} else {
		if (child[1])
			child[1]->add(n);
		else {
			child[1] = n;
			child[1]->parent = this;
		}
	}
}


Node::Node(int key)
{
	parent = (Node *)0xDEADBEEF;
	child[0] = child[1] = NULL;
	this->key = key;
}

void Node::check(void)
{
	if (parent == NULL) {
		printf("node %d has no parent\n", key);
		abort();
	}
	int i;
	for (i=0; i<2; i++)
		if(child[i])
			child[i]->check();

}

int Node::depth()
{
	int l,r;
	l=r=-1;
	if (child[0])
		l = child[0]->depth();
	if (child[1])
		r = child[1]->depth();
	if (l>r)
		return l+1;
	else
		return r+1;
}

void draw_line(int x)
{
	int i;
	for (i=0; i<x; i++)
		putchar('-');
}

void Node::dump(int level)
{
	if (child[0])
		child[0]->dump(level+1);
	else {
		draw_line(level);
		printf("x\n");
	}
	draw_line(level);
	printf("%d\n", key);
#if 0
	printf("parent@%p: ",parent);
	if (parent)
		printf("%d\n", parent->key);
	else
		putchar('\n');
#endif
	if (child[1])
		child[1]->dump(level+1);
	else {
		draw_line(level);
		printf("x\n");
	}

}

Node * Node::get_imbalanced()
{
	Node *n;
	int i;
	n = NULL;

	for (i=0; i<2; i++) {
		if (child[i]) {
			n = child[i]->get_imbalanced();
			if (n)
				return n;
		}
	}
	
	int l,r;
	l=r=-1;
	if (child[0])
		l = child[0]->depth();
	if (child[1])
		r = child[1]->depth();

	if (abs(l-r) > 1)
		return this;

	return NULL;

}
/**
 * Unlinks root of a tree
 * @return	new root
 */
Node * Node::unlink()
{
	Node *n;
	int i,c;

	/* how many children do we have? */
	n = NULL;
	c = 0;
	for (i=0; i<2; i++) {
		if (child[i]) {
			c++;
			n = child[i];
		}

	}

	/* forever alone :-( */
	if (c == 0)
		return NULL;
	/* one child */
	if (c == 1) {
		child[0]=child[1]=NULL;
		return n;
	}
	/* two children */


	/* is there something smaller than my right child? */
	if (child[1]->child[0] == NULL) {
		child[1]->child[0] = child[0];
		child[1]->child[0]->parent = child[1];
		return child[1];
	}

	n = child[1];
	while (n->child[0])
		n = n->child[0];

	n->parent->child[0] = n->child[1];
	if (n->child[1])
		n->child[1]->parent = n->parent;

	n->child[0] = child[0];
	n->child[1] = child[1];
	if (child[0])
		child[0]->parent = n;
	if (child[1])
		child[1]->parent = n;


	return n;

}

Node * Node::find_subtree(int key)
{
	if (this->key == key)
		return this;

	if (this->key > key) {
		if (child[0]) 
			return child[0]->find_subtree(key);
		else
			return NULL;
	}
	if (this->key < key) {
		if (child[1])
			return child[1]->find_subtree(key);
		else
			return NULL;
	}

	/**/
	return NULL;
}

class AVLTree {
private:
	Node *root;

	Node *Lrot(Node *root);
	Node *Rrot(Node *root);
	Node *LRrot(Node *root);
	Node *RLrot(Node *root);


	int do_fix(void);
	void do_remove(int key);

public:
	AVLTree();
	void add(int key);
	void fix(void);
	void dump(void);
	void remove(int key);

};



int AVLTree::do_fix(void)
{
	Node *n;
	Node *p;
	Node *new_root;
	if (!root)
		return 0;

	/* perform some tests */
	int i;
	for (i=0; i<2; i++){
		if (root->child[i])
			root->child[i]->check();
	}
	n = root->get_imbalanced();
	if (!n) {
		printf("tree is ok\n");
		return 0;

	}
	printf("found problem: %d\n", n->key);
	p = n->parent;

	int l,r;

	l=r=-1;
	if (n->child[0])
		l = n->child[0]->depth();
	if (n->child[1])
		r = n->child[1]->depth();

	new_root = NULL;
	/* Perform Rrot or RLrot */
	if (l < r) {
		int l,r;
		l=r=-1;
		if (n->child[1]->child[0])
			l = n->child[1]->child[0]->depth();
		if (n->child[1]->child[1])
			r = n->child[1]->child[1]->depth();
		if (l < r)
			new_root = Rrot(n);
		else
			new_root = RLrot(n);

	}

	/* Perform Lrot or LRrot*/ 
	if (r < l) {
		int l,r;
		l=r=-1;
		if (n->child[0]->child[0])
			l = n->child[0]->child[0]->depth();
		if (n->child[0]->child[1])
			r = n->child[0]->child[1]->depth();
		if (r < l)
			new_root = Lrot(n);
		else
			new_root = LRrot(n);
	}


	/* exchange it */
	if (n == root) {
		root = new_root;
		root->parent = NULL;
	} else {
		if (p->child[0] == n)
			p->child[0] = new_root;
		if (p->child[1] == n)
			p->child[1] = new_root;
		new_root->parent = p;
	}


	return 1;

}

void AVLTree::fix()
{
	while(do_fix())
		;
}

AVLTree::AVLTree()
{
	root = NULL;
}

void AVLTree::add(int key)
{

	Node *n;
	n = new Node(key);

	if (root == NULL) {
		root = n;
		root->parent = NULL;
	} else {
		root->add(n);
	}

	fix();
}

/**
 * Performs L-rotation.
 * @return	new root
 */
Node * AVLTree::Lrot(Node *root)
{
	Node *n;

	/* new root */
	n = root->child[0];
	n->parent = NULL;

	/* set right child of new root as left child of old root */
	root->child[0] = n->child[1];
	if (root->child[0])
		root->child[0]->parent = root;

	/* set old root as right child of new root */
	n->child[1] = root;
	root->parent = n;

	/* return new root */
	return n;
}

Node * AVLTree::Rrot(Node *root)
{
	Node *n;

	/* new root */
	n = root->child[1];
	n->parent = NULL;

	/* set left child of new root as right child of old root */
	root->child[1] = n->child[0];
	if (root->child[1])
		root->child[1]->parent = root;

	/* set old root as left child of new root */
	n->child[0] = root;
	root->parent = n;

	/* return new root */
	return n;
}

Node * AVLTree::LRrot(Node *root)
{

	Node *n;

	n = root->child[0]->child[1];

	/**/
	root->child[0] = n->child[1];
	if (root->child[0])
		root->child[0]->parent = root;

	/**/
	n->child[1] = root;
	root->parent = n;

	/**/
	n->parent->child[1] = n->child[0];
	if (n->parent->child[1])
		n->parent->child[1]->parent = n->parent;

	/**/
	n->child[0] = n->parent;
	n->child[0]->parent = n;

	/**/
	n->parent = NULL;
	return n;

}


Node * AVLTree::RLrot(Node *root)
{

	Node *n;

	n = root->child[1]->child[0];

	/**/
	root->child[1] = n->child[0];
	if (root->child[1])
		root->child[1]->parent = root;

	/**/
	n->child[0] = root;
	root->parent = n;

	/**/
	n->parent->child[0] = n->child[1];
	if (n->parent->child[0])
		n->parent->child[0]->parent = n->parent;

	/**/
	n->child[1] = n->parent;
	n->child[1]->parent = n;

	/**/
	n->parent = NULL;
	return n;

}

void AVLTree::dump()
{
	printf("dumping tree\n");
	if (!root)
		return;
	if (root->parent != NULL){
		printf("**** root->parent != NULL ***\n");
		abort();

	}
	root->dump(0);
}



void AVLTree::do_remove(int key)
{
	Node *n,*p;
	if (!root)
		return;
	if (root->key == key) {
		n = root->unlink();
		delete root;
		root = n;
		if (n)
			n->parent = NULL;
		return;
	}

	n = root->find_subtree(key);
	if (!n)
		return;

	p = n->parent;

	Node *nr;

	nr = n->unlink();
	if (nr)
		nr->parent = p;

	if (n == p->child[0])
		p->child[0] = nr;
	if (n == p->child[1])
		p->child[1] = nr;
	delete n;
}

void AVLTree::remove(int key)
{

	do_remove(key);
	fix();
}

int main(void)
{

	AVLTree *t;
	t = new AVLTree();
	int i;
	for (i=0; i<20; i++){
		t->add(rand()%100);
	}
	t->dump();
	while (1 == scanf("%d", &i)) {
		t->remove(i);
		t->dump();
	}

	delete t;
	return 0;
}
