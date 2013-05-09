/*
 * main.cpp
 *
 * Copyright 2013 Unknown <jenda@beefeater>
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston,
 * MA 02110-1301, USA.
 *
 *
 */


#include <iostream>
using namespace std;

#include <fstream>
#include <cstdio>
#include <cstdlib>
#include "avl/AVLTree.hxx"
#include "GraphEdge.hxx"
#include "GraphNode.hxx"
#include "GraphEdgeSet.hxx"



class Graph {
public:
	Graph(void);
	GraphNode *newNode(void);
	GraphEdge *newEdge(GraphNode *from, GraphNode *to, int weight);
	void graphvizDump(const char *fname);
private:
	typedef AVLTree<int, GraphEdge *> edgeTree;
	typedef AVLTree<int, GraphNode *> nodeTree;
	edgeTree edges;
	nodeTree nodes;

//	AVLTree<int, GraphEdge *> edges;
//	AVLTree<int, GraphNode *> nodes;
};

Graph::Graph(void)
{

}

GraphNode * Graph::newNode(void)
{
	edgeTree::iterator a;
	
	GraphNode *n;
	n = new GraphNode();
	nodes.add(n->getID(), n);
	return n;
}

GraphEdge * Graph::newEdge(GraphNode *from, GraphNode *to, int weight)
{
	GraphEdge *e;
	e = new GraphEdge();
	
	e->setSourceNode(from);
	e->setDestinationNode(to);
	e->setWeight(weight);
	
	from->edgesIn.add(e);
	to->edgesOut.add(e);
//	from->addEdgeOut(e);
//	to->addEdgeIn(e);
	
	return e;
}

void Graph::graphvizDump(const char *fname)
{
	std::ofstream fw;
	fw.open(fname);
	
	fw << "digraph g {" << endl;
	
	AVLTree<int, GraphNode *>::iterator it;
	it = nodes.begin();
	int i = 0;
	while (it != nodes.end()) {
		(*it).getData()->dump(fw);
		++it;
		++i;
	}
	fw << "};" << endl;
	
	
	fw.close();
	
}

void fill_graph(Graph &g, int nodes, int probab)
{
	srand(time(NULL));
	int i,j;
	GraphNode *nds[nodes];
	for (i=0; i<nodes; i++) {
		nds[i] = g.newNode();
	}
	
	for (i=0; i<nodes; i++) {
		for (j=0; j<nodes; j++) {
			if (rand() % 100 <= probab) {
				g.newEdge(nds[i], nds[j], 2*i+j);
//				g.newEdge(nds[j], nds[i], 2*i+j);
			}
		}
	}
}

int main(int argc, char **argv)
{

//	GraphNode *nodes[10];
	Graph g;
	
	fill_graph(g, 20, 10);
	g.graphvizDump("graph.dot");	
	
#if 0
	int i;
	for (i=0; i<10; i++) {
		nodes[i] = g.newNode();
		if ((i > 2) && (i % 2 == 0)) {
			g.newEdge(nodes[i/2], nodes[i], i);
		}
	}
#endif


}

