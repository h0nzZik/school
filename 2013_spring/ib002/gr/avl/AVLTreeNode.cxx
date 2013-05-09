#ifndef _AVLTreeNode_CXX
#define _AVLTreeNode_CXX

#include <cstdlib>
#include <cstdio>
#include "AVLTreeNode.hxx"

/**
 * Some setters/getters
 */

template <class K, class D>
AVLTreeNode<K,D> * AVLTreeNode<K,D>::getChild(int which) const
{
	return child[which];
}

template <class K, class D>
void AVLTreeNode<K,D>::setChild(int which, AVLTreeNode<K,D> *node)
{
	child[which] = node;
}

template <class K, class D>
AVLTreeNode<K,D> * AVLTreeNode<K,D>::getParent(void) const
{
	return parent;
}

template <class K, class D>
void AVLTreeNode<K,D>::setParent(AVLTreeNode<K,D> *node)
{
	parent = node;
}

template <class K, class D>
K AVLTreeNode<K,D>::getKey(void)
{
	return key;
}

template <class K, class D>
D AVLTreeNode<K,D>::getData(void)
{
	return data;
}


template <class K, class D>
void AVLTreeNode<K,D>::add(AVLTreeNode<K,D> *n)
{
	//printf("AVLNode: add %p %p\n", (void *)n->key, (void *)n->data);
	/* child[0]? */ /*FIXME: <= */
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

/**
 * 
 * Constructor
 *
 */

template <class K, class D>
AVLTreeNode<K,D>::AVLTreeNode(AVLTree<K,D> *tree, const K key, const D data)
{
	//printf("new node: %p %p\n", (void *)key, (void *)data);
	parent = (AVLTreeNode<K,D> *)0xDEADBEEF;
	child[0] = child[1] = NULL;
	this->key = key;
	this->data = data;
	this->tree = tree;
}

template <class K, class D>
void AVLTreeNode<K,D>::check(void)
{
	if (parent == NULL) {
		printf("AVLTreeNode %d has no parent\n", key);
		abort();
	}
	int i;
	for (i=0; i<2; i++)
		if(child[i])
			child[i]->check();

}

template <class K, class D>
bool AVLTreeNode<K,D>::isRoot(void) const
{
	if(parent == NULL || parent == this)
		return true;
	else
		return false;
}
template <class K, class D>
int AVLTreeNode<K,D>::whoAmI(void) const
{
	AVLTreeNode<K,D> *p;
	p = parent;
	if (p == NULL)
		return -1;
	
	int i;
	for (i=0; i<2; i++) {
		if (p->getChild(i) == this)
			return i;
	}
	fprintf(stderr, "[debug] whoAmI(): mistake\n");
	return -2;
}


template <class K, class D>
int AVLTreeNode<K,D>::depth() const
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


inline void draw_line(int x)
{
	int i;
	for (i=0; i<x; i++)
		putchar('-');
}


template <class K, class D>
void AVLTreeNode<K,D>::dump(int level)
{
	if (child[0])
		child[0]->dump(level+1);
	else {
		draw_line(level);
		printf("x\n");
	}
	draw_line(level);
	printf("%d:%d\n", key, data);
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

template <class K, class D>
AVLTreeNode<K,D> * AVLTreeNode<K,D>::get_imbalanced()
{
	AVLTreeNode<K,D> *n;
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
template <class K, class D>
AVLTreeNode<K,D> * AVLTreeNode<K,D>::unlink()
{
	AVLTreeNode<K,D> *n;
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

template <class K, class D>
AVLTreeNode<K,D> * AVLTreeNode<K,D>::find_subtree(const K key)
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

template <class K, class D>
AVLTree<K,D> * AVLTreeNode<K,D>::getTree(void)
{
	return tree;
}


/* export something */
//template class AVLTreeNode<int>;
//template class AVLTreeNode<void *>;

#endif // _AVLTreeNode_CXX
