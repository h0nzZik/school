#ifndef _GraphNode_HXX
#define _GraphNode_HXX

#include <iostream>
using namespace std;

#include "avl/AVLTree.hxx"
#include "GraphEdge.hxx"
#include "GraphEdgeSet.hxx"

class GraphEdge;

class GraphNode {
public:
	GraphNode(void);

#if 0	
	int compare(const GraphNode &n) const;
	bool operator == (const GraphNode &n) const;
	bool operator <=  (const GraphNode &n) const;
	bool operator >=  (const GraphNode &n) const;
	bool operator <  (const GraphNode &n) const;
	bool operator >  (const GraphNode &n) const;
#endif	
//	void addEdgeIn(GraphEdge *e);
//	void addEdgeOut(GraphEdge *e);
	
	int getID(void) const;
	void dump(ostream &fw);

	GraphEdgeSet edgesOut;
	GraphEdgeSet edgesIn;

private:
	int id;
//	AVLTree<int, GraphEdge *> edges_out;
//	AVLTree<int, GraphEdge *> edges_in;
	

};



//#include "GraphNode.cxx"

#endif
