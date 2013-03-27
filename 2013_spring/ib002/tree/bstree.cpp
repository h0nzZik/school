#include <cstdlib>
#include <ctime>
#include <cstdio>
#include "bstree.h"


BSTree::BSTree()
{
	parent = NULL;
	left = NULL;
	right = NULL;
}

void BSTree::add(int key)
{
	/* empty tree? */
	if (this->parent == NULL) {
		this->key = key;
		this->parent = this;
		return;
	}

	/* go right? */
	if (key > this->key) {
		if (this->right == NULL) {
			this->right = new BSTree();
			this->right->key = key;
			this->right->parent = this;
			return;
		} else {
			this->right->add(key);
			return ;
		}
	} else {
		if (this->left == NULL) {
			this->left = new BSTree();
			this->left->key = key;
			this->left->parent = this;
			return;
		} else {
			this->left->add(key);
		}

	}

}


BSTree *BSTree::find_subtree(int key)
{
	if (parent == NULL)
		return NULL;
	if (key == this->key)
		return this;;
	if (key < this->key) {
		if (this->left)
			return this->left->find_subtree(key);
		return NULL;
	}

	if (key > this->key) {
		if (this->right)
			return this->right->find_subtree(key);
		return NULL;
	}

	// never happens
	return NULL;

}


bool BSTree::find(int key)
{
	/* empty tree */
	if (parent == NULL)
		return false;
	if (key == this->key)
		return true;
	if (key < this->key) {
		if (this->left)
			return this->left->find(key);
		return false;
	}

	if (key > this->key) {
		if (this->right)
			return this->right->find(key);
		return false;
	}

	// never happens
	return false;
}

void BSTree::print_in()
{
	if (left)
		left->print_in();
	if (parent)
		printf("key: %d\n", key);
	if (right)
		right->print_in();
}

void BSTree::add_random(int n, unsigned long seed)
{
	srand(seed);
	int i;
	int x;

	x=0;
	for (i=0; i<n; i++) {
//		x = x + rand() % (n/3 + 1);
		x = rand() % (n*2);
		printf("adding %d\n", x);
		printf("height == %d\n", height());
		add(x);
	}
}


/* TODO: remove */
void BSTree::remove(int key)
{
	BSTree *sub;
	BSTree *maxleft;

	/* root && empty tree */
	if (parent == NULL)
		return;

	sub = this->find_subtree(key);
	printf("subtree at %p\n", sub);
	if (!sub)
		return;

	if (sub->left == NULL) {
		
	}
}


int BSTree::height()
{
	int lh, rh;
	if (parent == NULL)
		return -1;

	lh = rh = -1;
	if (left)
		lh = left->height();
	if (right)
		rh = right->height();
	if (lh > rh)
		return lh + 1;
	else
		return rh + 1;
}


int main(void)
{
	BSTree *b;

	b = new BSTree();

	b->add(5);
	b->add(10);
	b->add(2);
	b->add_random(100, time(NULL));

	b->print_in();
	b->remove(2000);

	

	delete b;
	return 0;
}
