#ifndef _GraphEdgeSet_cxx
#define _GraphEdgeSet_cxx

#include <iostream>
using namespace std;

/**
 * TODO: try some UUID generators
 */

#include "GraphEdge.hxx"


GraphEdge::GraphEdge(void)
{
	/* generate id */
	static int last_id = 0;
	id = last_id;
	last_id++;	
	
	this->weight = 0;
	this->dest = NULL;
	this->src = NULL;
}
void GraphEdge::connect(GraphNode *from, GraphNode *to)
{
	from->edgesOut.add(this);
	to->edgesIn.add(this);
}

void GraphEdge::disconnect(void)
{
	dest->edgesIn.remove(this);
	src->edgesOut.remove(this);
}

GraphEdge::~GraphEdge(void)
{
	;
}

int GraphEdge::getID(void) const
{
	return id;
}

void GraphEdge::dump(ostream &fw) const
{
	fw << "\t" << src->getID() << " -> " << dest->getID() << ";" << endl;
}


#if 0

int GraphEdge::compare(const GraphEdge &e) const
{
	if (id < e.getID())
		return -1;
	if (id > e.getID())
		return 1;
	
	return 0;
}

/**
 * Comparison operators
 */

bool GraphEdge::operator != (const GraphEdge &e) const
{
	return (compare(e) != 0);
}

bool GraphEdge::operator == (const GraphEdge &e) const
{
	return (compare(e) == 0);
}

bool GraphEdge::operator >= (const GraphEdge &e) const
{
	return (compare(e) >= 0);
}

bool GraphEdge::operator <= (const GraphEdge &e) const
{
	return (compare(e) <= 0);
}

bool GraphEdge::operator >  (const GraphEdge &e) const
{
	return (compare(e) > 0);
}

bool GraphEdge::operator <  (const GraphEdge &e) const
{
	return (compare(e) < 0);
}
#endif


/**
 * Destination getters/setters
 */

GraphNode * GraphEdge::getDestinationNode(void) const
{
	return dest;
}

void GraphEdge::setDestinationNode(GraphNode *node)
{
	dest = node;	
}

/**
 * Source getters/setters
 */

GraphNode * GraphEdge::getSourceNode(void) const
{
	return src;
}

void GraphEdge::setSourceNode(GraphNode *node)
{
	src = node;	
}

/**
 * Weight getters/setters
 */

int GraphEdge::getWeight(void) const
{
	return weight;
}

void GraphEdge::setWeight(int weight)
{
	this->weight = weight;
}

#endif
