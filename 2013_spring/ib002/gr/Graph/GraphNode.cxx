#ifndef _GraphNode_CXX
#define _GraphNode_CXX

#include <iostream>
using namespace std;

#include "avl/AVLTree.hxx"
#include "GraphNode.hxx"
#include "GraphEdge.hxx"
#include "GraphEdgeSet.hxx"

GraphNode::GraphNode(void)
{
	static int last_id = 0;
	id = last_id;
	last_id++;	
}
#if 0
void GraphNode::addEdgeIn(GraphEdge *e)
{
	edges_in.add(e->getID(), e);
}

void GraphNode::addEdgeOut(GraphEdge *e)
{
	edges_out.add(e->getID(), e);
}



#endif



void GraphNode::dump(ostream &fw)
{
	GraphEdgeSet::iterator it;
	int i;

//	std::cout << "Ahoj\n";
		fw << "\t" <<  id << ";" << endl;	
	it = edgesOut.begin();
	i = 0;
	while (it != edgesOut.end()) {

//		printf("[%3d] edge %p\n", i, it);
		(*it).dump(fw);
		++it;
		++i;
	}
}

int GraphNode::getID(void) const
{
	return id;
}

#if 0

/* Some comparison stuff */

int GraphNode::compare(const GraphNode &n) const
{
	printf("compare()\n");
	if (id < n.getID())
		return -1;
	if (id > n.getID())
		return 1;
	
	return 0;
	
}

bool GraphNode::operator == (const GraphNode &n) const
{
	return (compare(n) == 0);
}

bool GraphNode::operator >  (const GraphNode &n) const
{
	return (compare(n) > 0);
}

bool GraphNode::operator <  (const GraphNode &n) const
{
	return (compare(n) < 0);
}

bool GraphNode::operator >= (const GraphNode &n) const
{
	return (compare(n) >= 0);
}

bool GraphNode::operator <= (const GraphNode &n) const
{
	return (compare(n) <= 0);
}




#endif



//GraphNode::edgeOut::


#endif
