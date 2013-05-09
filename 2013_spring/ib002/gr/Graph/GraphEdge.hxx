#ifndef _GraphEdge_H
#define _GraphEdge_H
#include <iostream>
using namespace std;
#include "avl/AVLTree.hxx"
#include "GraphNode.hxx"

class GraphNode;

class GraphEdge {

public:
	GraphEdge(void);
	~GraphEdge(void);
	
	void connect(GraphNode *from, GraphNode *to);
	void disconnect(void);
//	GraphEdge(int weight);

#if 0
	/* Comparison operators */
	int compare(const GraphEdge &e) const;		
	bool operator == (const GraphEdge &e) const;
	bool operator <= (const GraphEdge &e) const;
	bool operator >= (const GraphEdge &e) const;
	bool operator <  (const GraphEdge &e) const;
	bool operator >  (const GraphEdge &e) const;
	bool operator != (const GraphEdge &e) const;
#endif	
	/* Destination getters/setters */
	GraphNode *getDestinationNode(void) const;
	void  setDestinationNode(GraphNode *n);

	GraphNode *getSourceNode(void) const;
	void  setSourceNode(GraphNode *n);
	
	/* Weight getters/setters */
	int  getWeight(void) const;
	void setWeight(int weight);

	int getID(void) const;
	
	/* dump */
	void dump(ostream &fw) const;
protected:

private:
	int id;
	int weight;
	GraphNode *dest;
	GraphNode *src;
};





#endif
