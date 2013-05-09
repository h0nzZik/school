#ifndef _AVL_CXX
#define _AVL_CXX

#include <cstdio>
#include <cstdlib>
#include <cstring>

#include <iterator>
#include <exception>
#include <stdexcept>
#include "AVLTree.hxx"

#include "AVLTreeNode.hxx"

/* AVLTreeNode of a BST */

/**
 * The AVLTreeNode class represents a AVLTreeNode inside a BST
 *
 */


//template class AVLTreeNode<int, int>;

template <class K, class D>
int AVLTree<K,D>::do_fix(void)
{
	AVLTreeNode<K,D> *n;
	AVLTreeNode<K,D> *p;
	AVLTreeNode<K,D> *new_root;

	AVLTreeNode<K,D> *child; // temporary

	if (!root)
		return 0;

	/* perform some tests */
	int i;
	for (i=0; i<2; i++){
		child = root->getChild(i);
		if (child)
			child->check();
	}
	n = root->get_imbalanced();
	if (!n) {
		//printf("tree is ok\n");
		return 0;

	}
	//printf("found problem: %d\n", n->key);
	p = n->getParent();

	int l,r;

	l=r=-1;
	child = n->getChild(0);
	if (child)
		l = child->depth();
	child = n->getChild(1);
	if (child)
		r = child->depth();

	new_root = NULL;
	/* Perform Rrot or RLrot */
	if (l < r) {
		int l,r;
		l=r=-1;
		
		child = n->getChild(1);
		if (child->getChild(0))
			l = child->getChild(0)->depth();
		if (child->getChild(1))
			r = child->getChild(1)->depth();
		if (l < r)
			new_root = Rrot(n);
		else
			new_root = RLrot(n);

	}

	/* Perform Lrot or LRrot*/ 
	if (r < l) {
		int l,r;
		l=r=-1;
		child = n->getChild(0);
		if (child->getChild(0))
			l = child->getChild(0)->depth();
		if (child->getChild(1))
			r = child->getChild(1)->depth();
		if (r < l)
			new_root = Lrot(n);
		else
			new_root = LRrot(n);
	}


	/* exchange it */
	if (n == root) {
		root = new_root;
		root->setParent(NULL);
	} else {
		if (p->getChild(0) == n)
			p->setChild(0, new_root);
		if (p->getChild(1) == n)
			p->setChild(1, new_root);
		new_root->setParent(p);
	}


	return 1;

}
template <class K, class D>
void AVLTree<K,D>::fix()
{
	while(do_fix())
		;
}

template <class K, class D>
AVLTree<K,D>::AVLTree()
{
	root = NULL;
}

template <class K, class D>
void AVLTree<K,D>::add(K key, D data)
{
	AVLTreeNode<K,D> *n;
	n = new AVLTreeNode<K,D>(this, key, data);

	if (root == NULL) {
		root = n;
		root->setParent(NULL);
	} else {
		root->add(n);
	}
	fix();
}


/**
 * Performs L-rotation.
 * @return	new root
 */
template <class K, class D>
AVLTreeNode<K,D> * AVLTree<K,D>::Lrot(AVLTreeNode<K,D> *root)
{
	AVLTreeNode<K,D> *n;

	/* new root */
	n = root->getChild(0);
	n->setParent(NULL);

	/* set right child of new root as left child of old root */
	root->setChild(0, n->getChild(1));


	if (root->getChild(0))
		root->getChild(0)->setParent(root);

	/* set old root as right child of new root */
	n->setChild(1, root);
	root->setParent(n);

	/* return new root */
	return n;
}

template <class K, class D>
AVLTreeNode<K,D> * AVLTree<K,D>::Rrot(AVLTreeNode<K,D> *root)
{
	AVLTreeNode<K,D> *n;

	/* new root */
	n = root->getChild(1);
	n->setParent(NULL);

	/* set left child of new root as right child of old root */
	root->setChild(1, n->getChild(0));

	if (root->getChild(1))
		root->getChild(1)->setParent(root);

	/* set old root as left child of new root */
	n->setChild(0, root);
	root->setParent(n);

	/* return new root */
	return n;
}

template <class K, class D>
AVLTreeNode<K,D> * AVLTree<K,D>::LRrot(AVLTreeNode<K,D> *root)
{

	AVLTreeNode<K,D> *n;

	n = root->getChild(0)->getChild(1);

	/**/
	root->setChild(0, n->getChild(1));
	if (root->getChild(0))
		root->getChild(0)->setParent(root);

	/**/
	n->setChild(1, root);
	root->setParent(n);

	/**/
	n->getParent()->setChild(1, n->getChild(0));
	if (n->getParent()->getChild(1))
		n->getParent()->getChild(1)->setParent(n->getParent());

	/**/
	n->setChild(0, n->getParent());
	n->getChild(0)->setParent(n);
	/**/
	n->setParent(NULL);
	return n;

}


template <class K, class D>
AVLTreeNode<K,D> * AVLTree<K,D>::RLrot(AVLTreeNode<K,D> *root)
{

	AVLTreeNode<K,D> *n;

	n = root->getChild(1)->getChild(0);

	/**/
	root->setChild(1, n->getChild(0));
	if (root->getChild(1))
		root->getChild(1)->setParent(root);

	/**/
	n->setChild(0, root);
	root->setParent(n);

	/**/
	n->getParent()->setChild(0, n->getChild(1));
	if (n->getParent()->getChild(0))
		n->getParent()->getChild(0)->setParent(n->getParent());

	/**/
	n->setChild(1, n->getParent());
	n->getChild(1)->setParent(n);

	/**/
	n->setParent(NULL);
	return n;

}

template <class K, class D>
void AVLTree<K,D>::dump()
{
	printf("dumping tree\n");
	if (!root)
		return;
	if (root->getParent() != NULL){
		printf("**** root->parent != NULL ***\n");
		abort();

	}
	root->dump(0);
}


template <class K, class D>
void AVLTree<K,D>::do_remove(const K key)
{
	AVLTreeNode<K,D> *n,*p;
	if (!root)
		return;
	if (root->getKey() == key) {
		n = root->unlink();
		delete root;
		root = n;
		if (n)
			n->setParent(NULL);
		return;
	}

	n = root->find_subtree(key);
	if (!n)
		return;

	p = n->getParent();

	AVLTreeNode<K,D> *nr;

	nr = n->unlink();
	if (nr)
		nr->setParent(p);

	
	if (n == p->getChild(0))
		p->setChild(0, nr);
	if (n == p->getChild(1))
		p->setChild(1, nr);
	delete n;
}

template <class K, class D>
void AVLTree<K,D>::remove(const K key)
{
	do_remove(key);
	fix();
}

template<class K, class D>
AVLTreeNode<K,D> * AVLTree<K,D>::firstNode(void)
{
//	printf("firstNode()\n");
	AVLTreeNode<K,D> *node;
	node = root;
	if (node == NULL)
		return NULL;
//	printf("firstNode(!=NULL);\n");
	while (node->getChild(0)) {
//		printf("going through %p\n", node->getKey());
		node = node->getChild(0);
	}
//	printf("finished in %p\n", node->getKey());
	return node;
}

template<class K, class D>
AVLTreeNode<K,D> * AVLTree<K,D>::lastNode(void)
{
	AVLTreeNode<K,D> *node;
	node = root;
	if (node == NULL)
		return NULL;
//	printf("lastNode(!=NULL);\n");
	while (node->getChild(1)) {
//		printf("going through %p\n", node->getKey());
		node = node->getChild(1);
	}
//	printf("finished in %p\n", node->getKey());
	return node;
}


template<class K, class D> 
typename AVLTree<K,D>::iterator AVLTree<K,D>::begin(void)
{
	return iterator(firstNode());	
}

template<class K, class D> 
typename AVLTree<K,D>::iterator AVLTree<K,D>::end(void)
{
	return iterator(NULL);	
}




/*	Iterators	*/
template <class K, class D>
class AVLTree<K,D>::iterator
{
public:

	/* some STL - compatible stuff */
    typedef std::forward_iterator_tag iterator_category;
    typedef iterator	self_type;
    typedef AVLTreeNode<K,D>	value_type;
    typedef AVLTreeNode<K,D> *	pointer;
    typedef AVLTreeNode<K,D> &	reference;
    typedef int	size_type;
    //typedef ptrdiff_t difference_type;

	iterator(AVLTreeNode<K,D> *node)
	{
		ptr = node;
		cnt = 0;
		tree = NULL;
		if (ptr)
			tree = ptr->getTree();
	}

	iterator (void)
	{
		ptr = NULL;
		cnt = 0;
		tree = NULL;
	}
	
	#if 0
	void set_end(void)
	{
		ptr = NULL;
	}
	#endif
	/* prefix increment */
	iterator operator ++()
	{		
		/* Incrementing NULL? */
		if (ptr == NULL) {
			cnt++;
			if (cnt == 0) {
				ptr = tree->firstNode();
			}
			return *this;
		}
		
		/* is there right child? */
		if (ptr->getChild(1) != NULL) {
			ptr = ptr->getChild(1);
			while(ptr->getChild(0))
				ptr = ptr->getChild(0);
			return *this;
		}

		/* am I left child? */
		if (ptr->whoAmI() == 0) {
			ptr = ptr->getParent();
			return *this;
		}


		/* find my grand-grand-grandmum */
		while (ptr->whoAmI() == 1) {
			ptr = ptr->getParent();
		}

		/* if she is root, I am the last. */
		if (ptr->isRoot() == true) {
			ptr = NULL;
		}
		/* otherwise she is someone's left child */
		else {
			ptr = ptr->getParent();
		}
		return *this;

	} /* operator ++ */

	
	iterator operator --()
	{
		if (ptr == NULL) {
			cnt--;
			if (cnt == 0) {
				ptr = tree->lastNode();
			}
			return *this;
		}

		/* is there left child? */
		if (ptr->getChild(0) != NULL) {
			ptr = ptr->getChild(0);
			while(ptr->getChild(1))
				ptr = ptr->getChild(1);
			return *this;
		}

		/* am I right child? */
		if (ptr->whoAmI() == 1) {
			ptr = ptr->getParent();
			return *this;
		}


		/* find my grand-grand-grandmum */
		while (ptr->whoAmI() == 0) {
			ptr = ptr->getParent();
		}

		/* if she is root, I am the last. */
		if (ptr->isRoot() == true) {
			ptr = NULL;
		}
		/* otherwise she is someone's right child */
		else {
			ptr = ptr->getParent();
		}
		return *this;		
	}
	
	reference operator * ()
	{
		if (ptr == NULL) {
			throw std::out_of_range("Dereferencing out-of-range AVLTree iterator");
		}
		return *ptr;
	}
	
	bool operator != (const iterator &x)
	{
		if (x.ptr != ptr || x.cnt != cnt) 
			return true;
		return false;
	}
	
	bool operator == (const iterator &x)
	{
		if (false == (*this != x))
			return true;
		return false;
	}

	
private:
	AVLTreeNode<K,D> *ptr;
	AVLTree<K,D> *tree;
	int cnt;
	
};



#endif
