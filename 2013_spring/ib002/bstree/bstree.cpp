#include <cstdlib>
#include <ctime>
#include <cstdio>
#include <climits>
#include "bstree.h"


BSTree::BSTree()
{
	parent = NULL;
	left = NULL;
	right = NULL;
	key = INT_MAX;
}

void BSTree::add_node(BSTree *sub)
{
	sub->parent = this;

	/* this may never happen on toplevel */
	if (sub->key > this->key) {
		printf("adding %d to the right of %d\n", sub->key, this->key);
		if (this->right == NULL) {
			this->right = sub;
		}
		else
			this->right->add_node(sub);
	} else {
		printf("adding %d to the left of %d\n", sub->key, this->key);

		if (this->left == NULL){
			this->left = sub;
		}
		else
			this->left->add_node(sub);
	}
}


void BSTree::add(int key)
{
	BSTree *leaf;

	leaf = new BSTree();
	leaf->key = key;
	add_node(leaf);
}

int BSTree::n_childs()
{
	int n;
	n = 0;
	if (left)
		n++;
	if(right)
		n++;
	return n;
}

/**
 * Tries to find 'key', but don't checks toplevel
 */

BSTree * BSTree::find_node_notop(int key)
{
	if (this->key < key) {
		if (right)
			return right->find_node_notop(key);
		else
			return NULL;
	}
	if (this->key > key) {
		if (left)
			return left->find_node_notop(key);
		else
			return NULL;
	}
}


BSTree * BSTree::find_node(int key)
{
	/* if we are normal node */
	if (parent)
		return find_node_notop(key);

	/* if we are toplevel and there is root */
	if (left)
		return left->find_node_notop(key);
	/* there is no root */
	return NULL;

}

bool BSTree::find(int key)
{
	BSTree *node;

	node = find_node(key);
	return node?true:false;
}



void BSTree::print_infix_notop(int level)
{
	if (left)
		left->print_infix_notop(level+1);
	if (parent) {
		int i;
		putchar('|');
		for (i=0; i<level; i++)
			putchar('-');
		printf("<%3d\n", key);
	}
	if (right)
		right->print_infix_notop(level+1);
}

void BSTree::print_infix()
{
	if (parent == NULL) {
		printf("printing the tree\n");
		left->print_infix_notop(0);
	} else {
		print_infix_notop(0);
	}


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


void BSTree::remove(int key)
{
	BSTree *node;

	node = NULL;

	printf("[remove] at node [%d]\n", this->key);
	/* try to find node */
	if (left && left->key == key)
		node = left;
	if (right && right->key == key)
		node = right;

	printf("[remove] getting deeper, node == %p\n", node);
	/* not found? get deeper or die */
	if (node == NULL) {
		if (right && key > this->key)
			right->remove(key);
		if (right && key < this->key)
			left->remove(key);
		return;
	}

	printf("found it\n");
	/* found it */
	switch (node->n_childs()) {
		case 0:
			delete node;
			if (left == node)
				left = NULL;
			else
				right = NULL;
			break;
		case 1:
			BSTree *child;
			child = node->left?node->left:node->right;
			if (left == node)
				left = child;
			else
				right = child;
			delete node;
			break;
		case 2:
			BSTree *max;
			max = node->left->node_max();

			/* unlink it from the tree */
			if (max->parent->right == max)
				max->parent->right = max->left;
			if (max->parent->left == max)
				max->parent->left = max->left;

			/* set it as a parent of the nodes's children */
			max->left = node->left;
			max->right = node->right;
			max->left->parent = max;
			max->right->parent = max;


			/* set it as node's parent's child */
			if (node->parent->left == node)
				node->parent->left = max;
			if (node->parent->right == node)
				node->parent->right = max;

			/* delete original node */
			delete node;
	}
}


/**
 * Gets a node with the maximal ID
 */

BSTree * BSTree::node_max_notop()
{
	if (right == NULL)
		return this;
	return right->node_max();
}

BSTree * BSTree::node_max()
{
	/* check toplevel */
	if (parent == NULL)
		return left->node_max_notop();
	return this->node_max_notop();
}


int BSTree::do_height()
{
	int lh, rh;
	lh = rh = -1;
	if (left)
		lh = left->do_height();
	if (right)
		rh = right->do_height();

	return 1+((lh>rh)?lh:rh);
}

int BSTree::height()
{
	if (parent)
		return do_height();

	if (left)
		return left->do_height();
	return -1;

}


int main(void)
{
	BSTree *b;

	b = new BSTree();

//	b->add(5);
//	b->add(10);
//	b->add(2);
	b->add_random(100, time(NULL));

	b->print_infix();
	b->remove(2000);

	

	delete b;
	return 0;
}
