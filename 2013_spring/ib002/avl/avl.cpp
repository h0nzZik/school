#include <cstdio>
#include <cstdlib>
#include <cstring>

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
	parent = NULL;
	child[0] = child[1] = NULL;
	this->key = key;
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
 * Unlinks root of a subtree
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

	/* find nearest possible node on the right */
	n->parent = NULL;
	while(n->child[0])
		n = n->child[0];
	/* unlink it */
	if (n->parent)
		n->parent->child[0] = n->child[1];
	else
		child[1] = n->child[1];

	n->child[0] = child[0];
	n->child[1] = child[1];
	return n;
}

Node * Node::find_subtree(int key)
{
	if (this->key == key)
		return this;

	if (this->key > key)
		if (child[0])
			return child[0]->find_subtree(key);
		else
			return NULL;
	if (this->key < key)
		if (child[1])
			return child[1]->find_subtree(key);
		else
			return NULL;

}

class AVLTree {
	Node *root;

public:
	AVLTree();
	void add(int key);
	void fix(void);
	void dump(void);
	void remove(int key);

};


void AVLTree::fix()
{

	Node *n;
	if (!root)
		return;
	n = root->get_imbalanced();
	if (!n) {
		printf("tree is ok\n");

	} else {
		printf("found: %d\n", n->key);

	}

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
		return;
	}
	root->add(n);
}

void AVLTree::dump()
{
	printf("dumping tree\n");
	if (!root)
		return;
	root->dump(0);
}



void AVLTree::remove(int key)
{
	Node *n,*p;
	if (!root)
		return;
	if (root->key == key) {
		n = root->unlink();
		delete root;
		root = n;
		return;
	}

	n = root->find_subtree(key);
	if (!n)
		return;
	p = n->parent;
	if (n == p->child[0])
		p->child[0] = n->unlink();
	if (n == p->child[1])
			p->child[1] = n->unlink();
	delete n;
}

int main(void)
{

	AVLTree *t;
	t = new AVLTree();
	int i;
	for (i=0; i<10; i++){
		t->add(rand()%100);
		t->dump();
		t->fix();

	}
	return 0;
}
