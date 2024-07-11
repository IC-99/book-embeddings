#include <ogdf/basic/Graph_d.h>
#include <ogdf/basic/EdgeArray.h>
#include <ogdf/basic/NodeArray.h>
#include <ogdf/basic/simple_graph_alg.h>
#include <ogdf/basic/extended_graph_alg.h>
#include <ogdf/decomposition/BCTree.h>
#include <ogdf/fileformats/GraphIO.h>
#include <ogdf/energybased/FMMMLayout.h>
#include <iostream>
#include <unordered_map>
#include <sstream>
#include <string>
#include <vector>

using namespace ogdf;

bool readGraphFromString(ogdf::Graph& G, const std::string& graphString) {
    std::istringstream iss(graphString);  // Create a stringstream from graphString
    return ogdf::GraphIO::read(G, iss);   // Use the read function that takes std::istream&
}

void draw(Graph* G, string fileName)
{
	GraphAttributes GA(*G, GraphAttributes::nodeGraphics |
		GraphAttributes::edgeGraphics |
        GraphAttributes::nodeLabel |
        GraphAttributes::nodeStyle |
        GraphAttributes::edgeType |
        GraphAttributes::edgeArrow |
        GraphAttributes::edgeStyle );

	for(node v : G->nodes) {
		GA.fillColor(v) = Color( "#FFDDAA" ); 
        GA.height(v) = 15.0;
        GA.width(v) = 15.0;
        GA.shape(v) = Shape::Ellipse; 

        GA.label(v) = to_string(v->index());
	}

	for(edge e : G->edges) {
        GA.arrowType(e) = ogdf::EdgeArrow::Last;
        GA.strokeColor(e) = Color("#000000");
	}

	FMMMLayout fmmm;
    fmmm.call(GA);
	
	GraphIO::drawSVG(GA, "OutputSVG-" + fileName);
}

void printArray(Array<int, int> array)
{
	std::cout << "[";
	int i = 0;
	while(i < array.size() - 1) {
		std::cout << array[i] << ", ";
		i++;
	}
	std::cout << array[i] << "]" << std::endl;
}

void populateGraph(Graph* G)
{   
	node n0 = G->newNode();
	node n1 = G->newNode();
	node n2 = G->newNode();
	node n3 = G->newNode();
	node n4 = G->newNode();
	node n5 = G->newNode();
	node n6 = G->newNode();
	node n7 = G->newNode();
	node n8 = G->newNode();
	node n9 = G->newNode();
	node n10 = G->newNode();
	node n11 = G->newNode();

	G->newEdge(n0, n1);
	G->newEdge(n0, n5);
	G->newEdge(n1, n2);
	G->newEdge(n3, n4);
	G->newEdge(n4, n1);
	G->newEdge(n4, n10);
	G->newEdge(n4, n11);
	G->newEdge(n5, n3);
	G->newEdge(n5, n4);
	G->newEdge(n6, n5);
	G->newEdge(n7, n5);
	G->newEdge(n7, n6);
	G->newEdge(n7, n8);
	G->newEdge(n8, n6);
	G->newEdge(n9, n1);
	G->newEdge(n9, n2);
	G->newEdge(n10, n11);
}

bool isOuterPlanar(Graph* G)
{
	node dummyNode = G->newNode();
	for(node v : G->nodes) {
		G->newEdge(dummyNode, v);
	}
	if (isPlanar(*G)) {
		G->delNode(dummyNode);
		return true;
	}
	return false;
}

bool hasOneSourceAndOneSink(Graph* G)
{
	bool hasSource = false;
	bool hasSink = false;
	for(node v : G->nodes) {
		if (v->indeg() == 0) {
			if (hasSource) {
				return false;
			}
			else {
				hasSource = true;
			}
		}
		if (v->outdeg() == 0) {
			if (hasSink) {
				return false;
			}
			else {
				hasSink = true;
			}
		}
	}
	return hasSource && hasSink;
}

void saveGraph(Graph* G, string fileName)
{
	GraphIO::write(*G, fileName + ".gml", GraphIO::writeGML);
}

void readGraph(Graph* G, string fileName)
{
	GraphIO::read(*G, fileName + ".gml", GraphIO::readGML);
}

void find1StackLayout(Graph* G) {
	std::cout << "---- INIZIO FASE 1 ----" << std::endl;
	std::cout << "il grafo ha " << G->numberOfNodes() << " nodi e " << G->numberOfEdges() << " archi" << std::endl;

	// verifica se il grafo è outerplanare
	if (isOuterPlanar(G)) {
		std::cout << "il grafo è outerplanare" << std::endl;
	}
	else {
		std::cout << "il grafo non è outerplanare" << std::endl;
		std::cout << "---- FINE FASE 1 ----" << std::endl;
		return;
	}

	// calcolo delle componenti biconnesse
	EdgeArray<int> edgeArray = EdgeArray<int>(*G);
	int numberOfBiconnectedComponents = biconnectedComponents(*G, edgeArray);
	std::cout << "il grafo ha " << numberOfBiconnectedComponents << " componenti biconnesse" << std::endl;

	Array<Graph, int> biconnectedComponentsGraphs(numberOfBiconnectedComponents);
	Array<std::unordered_map<int, node>, int> biconnectedComponentsNodeMaps(numberOfBiconnectedComponents);

	for(edge e : G->edges) {
		Graph* currentGraph = &biconnectedComponentsGraphs[edgeArray[e]];
		std::unordered_map<int, node>* currentMap = &biconnectedComponentsNodeMaps[edgeArray[e]];

		int u_index = e->source()->index();
		int v_index = e->target()->index();

		if (currentMap->find(u_index) == currentMap->end()) {
			(*currentMap)[u_index] = currentGraph->newNode(u_index);
		}
		if (currentMap->find(v_index) == currentMap->end()) {
			(*currentMap)[v_index] = currentGraph->newNode(v_index);
		}

		currentGraph->newEdge((*currentMap)[u_index], (*currentMap)[v_index]);
	}

	// verifica se ogni componente ha una sola sorgente e un solo pozzo
	for(int i = 0; i < numberOfBiconnectedComponents; i++) {
		//draw(&biconnectedComponentsGraphs[i], "Component" + to_string(i) + ".svg" );

		if (!hasOneSourceAndOneSink(&biconnectedComponentsGraphs[i])) {
			std::cout << "la componente biconnessa " << i << " non ha una sola sorgente e un solo pozzo" << std::endl;
			std::cout << "---- FINE FASE 1 ----" << std::endl;
			return;
		}
	}

	std::cout << "ogni componente biconnessa ha una sola sorgente e un solo pozzo" << std::endl;
	std::cout << "---- FINE FASE 1 ----" << std::endl;

	// FASE 2 da scrivere
}

void readGraphFromArg(Graph* G, const char* graphString)
{
	std::unordered_map<int, node> nodes;
	int i = 0;
	string current = "";
	bool readingNodes = true;
	int sourceNode = -1;

	while (graphString[i] != '\0') {
		if (graphString[i] == '\n') {
			int index = std::stoi(current);
			if (readingNodes) {
			    nodes[index] = G->newNode(index);
			}
			else {
				G->newEdge(nodes[sourceNode], nodes[index]);
			}
			current = "";
		}
		else if (graphString[i] == ',') {
			sourceNode = std::stoi(current);
			current = "";
			readingNodes = false;
		}
		else {
			current = current + graphString[i];
		}
		i++;
	}
}

int main(int argc, char* argv[])
{
	if (argc > 1) {
        std::cout << "Input string: " << argv[1] << std::endl;
    }
	else {
		std::cout << "Input string: nessun argomento" << std::endl;
	}

	Graph G;
	Graph GNotOuterplanar;
	Graph GWithMoreSourcesAndSinks;

	//readGraph(&G, "graph");
	//readGraph(&GNotOuterplanar, "graphNotOuterplanar");
	//readGraph(&GWithMoreSourcesAndSinks, "graphWithMoreSourcesAndSinks");

	//populateGraph(&G);

	//const char* graphString = "0\n1\n2\n3\n4\n5\n6\n7\n8\n9\n10\n11\n0,1\n0,5\n1,2\n3,4\n4,1\n4,10\n4,11\n5,3\n5,4\n6,5\n7,5\n7,6\n7,8\n8,6\n9,1\n9,2\n10,11\n\0";
	
	readGraphFromArg(&G, argv[1]);

	//draw(&G, "Graph.svg");
	//saveGraph(&G, "graphSmall");

	std::cout << "RISULTATO: 1 7 8 9 6 4 5 2 3 10" << std::endl; 

	find1StackLayout(&G);

	/*
	NodeArray<int> nodeArray = NodeArray<int>(G);
	topologicalNumbering(G, nodeArray);

	Array<int, int> topologicalOrder(G.numberOfNodes());

	for(node v : G.nodes) {
		topologicalOrder[nodeArray[v]] = v->index();
	}

	std::cout << "ordine topologico del grafo G: ";
	printArray(topologicalOrder);
	*/

	return 0;
}
