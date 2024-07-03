#include <ogdf/basic/Graph_d.h>
#include <ogdf/basic/EdgeArray.h>
#include <ogdf/basic/simple_graph_alg.h>
#include <ogdf/decomposition/BCTree.h>
#include <ogdf/fileformats/GraphIO.h>
#include <ogdf/energybased/FMMMLayout.h>
#include <iostream>

using namespace ogdf;

/*
dag.addEdge(0, 1);
dag.addEdge(0, 5);
dag.addEdge(1, 2);
dag.addEdge(3, 4);
dag.addEdge(4, 1);
dag.addEdge(4, 10);
dag.addEdge(4, 11);
dag.addEdge(5, 3);
dag.addEdge(5, 4);
dag.addEdge(6, 5);
dag.addEdge(7, 5);
dag.addEdge(7, 6);
dag.addEdge(7, 8);
dag.addEdge(8, 6);
dag.addEdge(9, 1);
dag.addEdge(9, 2);
dag.addEdge(10, 11);
*/

void draw(Graph G, string fileName)
{
	GraphAttributes GA(G, GraphAttributes::nodeGraphics |
		GraphAttributes::edgeGraphics |
        GraphAttributes::nodeLabel |
        GraphAttributes::nodeStyle |
        GraphAttributes::edgeType |
        GraphAttributes::edgeArrow |
        GraphAttributes::edgeStyle );

	for(node v : G.nodes) {
		GA.fillColor(v) = Color( "#FFDDAA" ); 
        GA.height(v) = 15.0;
        GA.width(v) = 15.0;
        GA.shape(v) = Shape::Ellipse; 

        string s = to_string(v->index());
        char const *pchar = s.c_str();
        GA.label(v) = pchar;
	}

	for(edge e : G.edges) {
        GA.arrowType(e) = ogdf::EdgeArrow::Last;
        GA.strokeColor(e) = Color("#000000");
	}

	FMMMLayout fmmm;
    fmmm.call(GA);
	
	GraphIO::drawSVG(GA, "OutputSVG-" + fileName);
}

Graph getGraph()
{
	Graph G = Graph();
    
	node n0 = G.newNode();
	node n1 = G.newNode();
	node n2 = G.newNode();
	node n3 = G.newNode();
	node n4 = G.newNode();
	node n5 = G.newNode();
	node n6 = G.newNode();
	node n7 = G.newNode();
	node n8 = G.newNode();
	node n9 = G.newNode();
	node n10 = G.newNode();
	node n11 = G.newNode();

	G.newEdge(n0, n1);
	G.newEdge(n0, n5);
	G.newEdge(n1, n2);
	G.newEdge(n3, n4);
	G.newEdge(n4, n1);
	G.newEdge(n4, n10);
	G.newEdge(n4, n11);
	G.newEdge(n5, n3);
	G.newEdge(n5, n4);
	G.newEdge(n6, n5);
	G.newEdge(n7, n5);
	G.newEdge(n7, n6);
	G.newEdge(n7, n8);
	G.newEdge(n8, n6);
	G.newEdge(n9, n1);
	G.newEdge(n9, n2);
	G.newEdge(n10, n11);

	return G;
}

int main()
{
	Graph G = getGraph();

	EdgeArray<int> edgeArray = EdgeArray<int>(G);
	NodeArray<int> nodeArray = NodeArray<int>(G);

	std::cout << "grafo con " << G.numberOfNodes() << " nodi e " << G.numberOfEdges() << " archi" << std::endl;
	std::cout << "trovate " << biconnectedComponents(G, edgeArray) << " componenti biconnesse" << std::endl;

	int i = 1;
	for(edge e : G.edges) {
		std::cout << "l'arco " << i << " appartiene al blocco " << edgeArray[e] << std::endl;
		i++;
	}

	BCTree bcTree(G);
	std::cout << "numero blocchi: " << bcTree.numberOfBComps() << ", numero cutpoint: " << bcTree.numberOfCComps() << std::endl;

	topologicalNumbering(G, nodeArray);

	for(node v : G.nodes) {
		std::cout << "il nodo " << v->index() << " sta in posizione " << nodeArray[v] << std::endl;
	}

	Graph bcTreeGraph = bcTree.auxiliaryGraph();

	node node = bcTree.rep(G.firstNode());

	draw(G, "Graph.svg");
	draw(bcTreeGraph, "BC-Tree.svg");

	return 0;
}
