#ifndef _GraphEdgeSet_Hxx
#define _GraphEdgeSet_Hxx

#include "GraphEdge.hxx"
#include "GraphNode.hxx"

class GraphEdge;
class GraphNode;

class GraphEdgeSet
{
public:
	GraphEdgeSet(void);
	void add(GraphEdge *e);
	void remove(GraphEdge *e);
	
	
	class iterator;
	iterator begin(void);
	iterator end(void);
	
	class iterator
	{
	public:
	    typedef iterator	self_type;
		typedef GraphEdge	value_type;
		typedef GraphEdge *	pointer;
		typedef GraphEdge &	reference;
		
		iterator(void);
		iterator(AVLTree<int, GraphEdge *> &tree, bool begin);

		iterator operator ++(void);
		reference operator * (void);
		
		bool operator != (const iterator &x);
		bool operator == (const iterator &x);
		
		
	private:
//		GraphEdge *e;
		AVLTree<int, GraphEdge *>::iterator itr;
	};
	
private:
	AVLTree<int, GraphEdge *> edges;
};


#endif
