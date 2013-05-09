#ifndef _AVLTreeNode_HXX
#define _AVLTreeNode_HXX



/* K = key, D = data */
template <class K, class D>
class AVLTreeNode {
public:

	/**
	 * Creates new node
	 * @param key	Key
	 * @param data	Data
	 */
	AVLTreeNode(AVLTree<K,D> *tree, const K key, const D data);


	void setData(D data);
	D getData(void);

	K getKey(void);

	AVLTreeNode *getParent(void) const;
	void setParent(AVLTreeNode *parent);
	
	AVLTreeNode *getChild(int which) const;
	void setChild(int which, AVLTreeNode *child);
	

	/**
	 * Adds AVLTreeNode into BST
	 * @param n	AVLTreeNode to add
	 */
	void add(AVLTreeNode<K,D> *n);

	bool isRoot(void) const;
	int whoAmI(void) const;

	int depth(void) const;
	void dump(int level);
	AVLTreeNode<K,D> *get_imbalanced();
	AVLTreeNode<K,D> *unlink();
	AVLTreeNode<K,D> *find_subtree(const K key);
	void check();
	
	AVLTree<K,D> * getTree(void);	
	
private:

	K key;
	D data;

	AVLTree<K,D> *tree;
	AVLTreeNode *parent;
	AVLTreeNode *child[2];

};

#include "AVLTreeNode.cxx"

#endif
