#ifndef _AVL_HXX
#define _AVL_HXX

/* some private stuff */
template <class K, class D>
class AVLTreeNode;

template <class K, class D>
class AVLTree {
public:
	AVLTree();

//	void add(const K key);
	void add(const K key, const D data);
	void fix(void);
	void dump(void);
	void remove(const K key);
	
	class iterator;
	iterator begin(void);
	iterator end(void); 

//	typedef iterator AVLTreeIterator;
private:
	AVLTreeNode<K,D> *root;

	AVLTreeNode<K,D> *  Lrot(AVLTreeNode<K,D> *root);
	AVLTreeNode<K,D> *  Rrot(AVLTreeNode<K,D> *root);
	AVLTreeNode<K,D> * LRrot(AVLTreeNode<K,D> *root);
	AVLTreeNode<K,D> * RLrot(AVLTreeNode<K,D> *root);


	int do_fix(void);
	void do_remove(const K key);
	

	AVLTreeNode<K,D> * firstNode(void);
	AVLTreeNode<K,D> * lastNode(void);	



};


#include "AVLTree.cxx"

#endif
