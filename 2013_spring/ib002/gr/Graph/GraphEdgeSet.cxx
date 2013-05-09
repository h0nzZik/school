#ifndef _GraphEdgeSet_CXX
#define _GraphEdgeSet_CXX

#include "avl/AVLTree.hxx"
#include "GraphNode.hxx"
#include "GraphEdge.hxx"
#include "GraphEdgeSet.hxx"


typedef GraphEdgeSet::iterator GraphEdgeSetIterator;

/* constructor */
GraphEdgeSet::GraphEdgeSet(void)
{
	;
}

/**
 * Adds a new edge
 * @param e	edge to add
 */
void GraphEdgeSet::add(GraphEdge *e)
{
	edges.add(e->getID(), e);
}

/**
 * Removes an edge
 * @param e	edge to remove
 */
void GraphEdgeSet::remove(GraphEdge *e)
{
	edges.remove(e->getID());
}

/**
 * Returns iterator, pointing to the first element
 */
typename GraphEdgeSet::iterator GraphEdgeSet::begin(void)
{
	return iterator(edges, true);
}

/**
 * Returns iterator, pointing to the end
 */
typename GraphEdgeSet::iterator GraphEdgeSet::end(void)
{
	return iterator(edges, false);
}

/**
 * Creates uninitialized iterator
 */

GraphEdgeSet::iterator::iterator(void)
{
	
}

/**
 * Creates initialized iterator
 * @param tree	tree where edges are stored
 * @param begin	point to the first element or to the end?
 */
GraphEdgeSet::iterator::iterator(AVLTree<int, GraphEdge *> &tree, bool begin)
{
	//printf("GraphEdgeSet: got tree\n");
	if (begin == true) {
		itr = tree.begin();
	}
	else {
		itr = tree.end();
	}
}



bool GraphEdgeSet::iterator::operator==(const GraphEdgeSet::iterator &x)
{
	return (itr == x.itr);
}

bool GraphEdgeSet::iterator::operator!=(const GraphEdgeSet::iterator &x)
{
	return itr != x.itr;
}


GraphEdgeSetIterator GraphEdgeSet::iterator::operator++ (void)
{
	++itr;	
	return *this;
}

GraphEdge & GraphEdgeSet::iterator::operator* (void)
{
//	AVLTreeNode<int, GraphEdge *> n
//	n = *itr;
	
	return *((*itr).getData());
}


#endif
