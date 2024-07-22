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
#include <queue>

using namespace ogdf;

class TreeNode
{
	public:
		int id;
		int value;
		bool isCutpoint;
		std::vector<TreeNode*> neighbors;
		std::vector<TreeNode*> children;

		TreeNode(int idOfTreeNode, int val, bool cutpoint) {
			id = idOfTreeNode;
			value = val;
			isCutpoint = cutpoint;
		}

		TreeNode() {
			id = -1;
			value = -1;
			isCutpoint = false;
		}

		void addNeighbor(TreeNode* neighbor) {
			neighbors.push_back(neighbor);
		}

		void addChild(TreeNode* child) {
			children.push_back(child);
		}
};

int getParentCutpoint(TreeNode* treeNode, int component) {
	if (treeNode->children.size() == 0) {
		return -1;
	}
	if (treeNode->isCutpoint) {
		for(TreeNode* componentNode: treeNode->children) {
			if (componentNode->value == component) {
				return treeNode->value;
			}
			int result = getParentCutpoint(componentNode, component);
			if (result != -1) {
				return result;
			}
		}
	}
	else {
		for(TreeNode* cutpointNode: treeNode->children) {
			int result = getParentCutpoint(cutpointNode, component);
			if (result != -1) {
				return result;
			}
		}
	}
	return -1;
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

Array<int, int> getSourceAndSink(Graph* G)
{
	Array<int, int> nullResult(2);
	Array<int, int> sourceAndSink(2);
	nullResult[0] = -1;
	nullResult[1] = -1;
	sourceAndSink[0] = -1;
	sourceAndSink[1] = -1;

	for(node v : G->nodes) {
		if (v->indeg() == 0) {
			if (sourceAndSink[0] != -1) {
				return nullResult;
			}
			else {
				sourceAndSink[0] = v->index();
			}
		}
		if (v->outdeg() == 0) {
			if (sourceAndSink[1] != -1) {
				return nullResult;
			}
			else {
				sourceAndSink[1] = v->index();
			}
		}
	}
	return sourceAndSink;
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
		// commentata linea 187 del file EdgeArray.h "OGDF_ASSERT(e->graphOf() == m_pGraph);"
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

	Array<Array<int, int>, int> sourceAndSinkOfComponents(numberOfBiconnectedComponents);

	// verifica se ogni componente ha una sola sorgente e un solo pozzo
	for(int i = 0; i < numberOfBiconnectedComponents; i++) {
		//draw(&biconnectedComponentsGraphs[i], "Component" + to_string(i) + ".svg" );

		Array<int, int> sourceAndSink = getSourceAndSink(&biconnectedComponentsGraphs[i]);
		
		std::cout << "componente " << i << ": sorgente " << sourceAndSink[0] << ", pozzo " << sourceAndSink[1] << std::endl;

		if (sourceAndSink[0] != -1 && sourceAndSink[1] != -1) {
			sourceAndSinkOfComponents[i] = sourceAndSink;
		}
		else {
			std::cout << "la componente biconnessa " << i << " non ha una sola sorgente e un solo pozzo" << std::endl;
			std::cout << "---- FINE FASE 1 ----" << std::endl;
			return;
		}
	}

	std::cout << "ogni componente biconnessa ha una sola sorgente e un solo pozzo" << std::endl;

	// commentata linea 185 del file NodeArray.h "OGDF_ASSERT(v->graphOf() == m_pGraph);"
	Array<Array<int, int>, int> topologicalOrders(numberOfBiconnectedComponents);

	// calcolo dell'ordine topologico dei nodi di ogni componente biconnessa
	for(int i = 0; i < numberOfBiconnectedComponents; i++) {
		NodeArray<int> nodeArray = NodeArray<int>(biconnectedComponentsGraphs[i]);
		topologicalNumbering(biconnectedComponentsGraphs[i], nodeArray);

		Array<int, int> topologicalOrder(biconnectedComponentsGraphs[i].numberOfNodes());

		for(node v : biconnectedComponentsGraphs[i].nodes) {
			topologicalOrder[nodeArray[v]] = v->index();
		}

		topologicalOrders[i] = topologicalOrder;

		std::cout << "ordine topologico della componente " << i << ": ";
		printArray(topologicalOrders[i]);
	}
	std::cout << "---- FINE FASE 1 ----" << std::endl << std::endl;

	std::cout << "---- INIZIO FASE 2 ----" << std::endl;

	// componentsOfNode è una mappa di vettori che contengono per ogni nodo le componenti a cui esso appartiene
	std::unordered_map<int, std::vector<int>> componentsOfNode(G->numberOfNodes());
	std::vector<int> cutpoints;

	// popola componentsOfNode
	for(int i = 0; i < numberOfBiconnectedComponents; i++) {
		for (node v : biconnectedComponentsGraphs[i].nodes) {
			componentsOfNode[v->index()].push_back(i);
		}
	}

	// per memorizzare quali sono i cutpoint di ogni componente
	std::unordered_map<int, std::vector<int>> cutpointsOfComponent(numberOfBiconnectedComponents);

	// trova i cutpoint
	for(node v: G->nodes) {
		if (componentsOfNode[v->index()].size() > 1) {
			cutpoints.push_back(v->index());

			for(int component: componentsOfNode[v->index()]) {
				cutpointsOfComponent[component].push_back(v->index());
			}
		}
	}

	// nodeOfComponent è una mappa che restituisce il TreeNode dall'valore del TreeNode
	std::unordered_map<int, TreeNode*> nodeOfComponent(numberOfBiconnectedComponents);
	int idOfTreeNode = 0;

	// creazione dei TreeNode per le componenti
	for(int i = 0; i < numberOfBiconnectedComponents; i++) {
		TreeNode* n = new TreeNode(idOfTreeNode, i, false);
		nodeOfComponent[i] = n;

		idOfTreeNode++;
	}

	// creazione dei TreeNode per i cutpoint
	for(int cutpoint : cutpoints) {
		TreeNode* n = new TreeNode(idOfTreeNode, cutpoint, true);

		// collegamento dei TreeNode rappresentanti i cutpoint alle componenti di cui i cutpoint fanno parte
		for (int component : componentsOfNode[cutpoint]) {
			n->addNeighbor(nodeOfComponent[component]);
			nodeOfComponent[component]->addNeighbor(n);

			std::cout << "il cutpoint " << cutpoint << " è stato collegato alla componente " << nodeOfComponent[component]->value << std::endl;
		}
		idOfTreeNode++;
	}

	std::cout << "costruito il BCT contenente " << cutpoints.size() << " cutpoint e " << numberOfBiconnectedComponents << " componenti biconnesse" << std::endl;

	// radicazione del BCT
	int componentRoot = 3;
	TreeNode* rootOfBCT = nodeOfComponent[componentRoot];

	std::unordered_map<int, bool> visited;
    std::queue<TreeNode*> queue;

	// ordine delle componenti
	Array<int, int> orderOfComponents(numberOfBiconnectedComponents);
	int i = 0;

    visited[rootOfBCT->id] = true;
    queue.push(rootOfBCT);

	// BST sul BCT non radicato per radicarlo e trovare l'ordine delle componenti (grado di libertà)
    while (!queue.empty()) {
        TreeNode* u = queue.front();
		queue.pop();
		if (!u->isCutpoint) {
			orderOfComponents[i] = u->value;
			i++;
		}

        // esplora tutti i nodi adiacenti e salva i figli del nodo (grado di libertà sull'ordine)
        for (TreeNode* neighbor : u->neighbors) {
            if (!visited[neighbor->id]) {
                visited[neighbor->id] = true;
                queue.push(neighbor);

				u->addChild(neighbor);
            }
        }
    }

	std::cout << "ordine delle componenti: ";
	printArray(orderOfComponents);

	// ricerca dei cutpoint ristretti
	std::unordered_map<int, bool> isCutpointRestricted;
	std::unordered_map<int, int> cutpointRestrictedInComponent;

	for(int i = numberOfBiconnectedComponents - 1; i >= 0; i--) {
		int component = orderOfComponents[i];
		int countRestricted = 0;
		cutpointRestrictedInComponent[component] = -1;

		for(int cutpoint: cutpointsOfComponent[component]) {
			std::cout << "analizzando cutpoint " << cutpoint << " nella componente " << component << std::endl;
			for(int j = i + 1; j < numberOfBiconnectedComponents; j++) {
				int otherComponent = orderOfComponents[j];
				bool isCutpointInOtherComponent = false;
				for(int componentOfCutpoint: componentsOfNode[cutpoint]) {
					if (componentOfCutpoint == otherComponent) {
						isCutpointInOtherComponent = true;
						break;
					}
				}

				if (isCutpointInOtherComponent) {
					std::cout << "il cutpoint " << cutpoint << " fa parte della componente " << otherComponent << std::endl;

					bool isIntermediate = cutpoint != sourceAndSinkOfComponents[otherComponent][0] && cutpoint != sourceAndSinkOfComponents[otherComponent][1];

					std::cout << "il cutpoint " << cutpoint << " è intermedio in " << otherComponent << ": " << isIntermediate << std::endl;

					if (isIntermediate || cutpointRestrictedInComponent[otherComponent] != -1) {
						countRestricted++;
						isCutpointRestricted[cutpoint] = true;
						cutpointRestrictedInComponent[component] = cutpoint;
						std::cout << "il cutpoint " << cutpoint << " è ristretto, trovato con componente " << otherComponent << std::endl;
					}
				}
			}
		}
		std::cout << "NUMERO NODI RISTRETTI PER LA COMPONENTE " << component << ": " << countRestricted << std::endl;
		if (countRestricted > 1) {
			std::cout << "TROVATA COPPIA CONFLITTUALE" << std::endl;
			std::cout << "---- FINE FASE 2 ----" << std::endl;
			return;
		}
	}

	Array<int, int> currentLayout = topologicalOrders[orderOfComponents[0]];

	// fusione dei layout delle componenti biconnesse
	for (int i = 1; i < numberOfBiconnectedComponents; i++) {
		int component = orderOfComponents[i];

		std::cout << "layout attuale: ";
		printArray(currentLayout);

		Array<int, int> orderToAdd = topologicalOrders[component];
		int cutpoint = getParentCutpoint(rootOfBCT, component);

		std::cout << "la componente " << component << " è collegata al grafo G_" << i << " tramite il cutpoint " << cutpoint << std::endl;

		int newLayoutSize = currentLayout.size() + orderToAdd.size() - 1;
		Array<int, int> newLayout(newLayoutSize);

		if (cutpoint == sourceAndSinkOfComponents[component][0]) {
			int j = 0;
			while (j < currentLayout.size()) {
				if (currentLayout[j] != cutpoint) {
					newLayout[j] = currentLayout[j];
				} 
				else {
					newLayout[j] = currentLayout[j];
					j++;
					break;
				}
				j++;
			}
			for (int k = 1; k < orderToAdd.size(); k++) {
				newLayout[j + k - 1] = orderToAdd[k];
			}
			while (j < currentLayout.size()) {
				newLayout[j + orderToAdd.size() - 1] = currentLayout[j];
				j++;
			}
		}
		else if (cutpoint == sourceAndSinkOfComponents[component][1]){
			int j = 0;
			if (cutpoint != currentLayout[0]) {
				while (j + 1 < currentLayout.size()) {
					if (currentLayout[j + 1] != cutpoint) {
						newLayout[j] = currentLayout[j];
					} 
					else {
						newLayout[j] = currentLayout[j];
						j++;
						break;
					}
					j++;
				}
			}			
			for (int k = 0; k < orderToAdd.size() - 1; k++) {
				newLayout[j + k] = orderToAdd[k];
			}
			while (j < currentLayout.size()) {
				newLayout[j + orderToAdd.size() - 1] = currentLayout[j];
				j++;
			}
		}
		else {
			int j = 0;
			while (j < orderToAdd.size()) {
				if (orderToAdd[j] != cutpoint) {
					newLayout[j] = orderToAdd[j];
				}
				else {
					break;
				}
				j++;
			}
			for (int k = 0; k < currentLayout.size(); k++) {
				newLayout[j + k] = currentLayout[k];
			}
			j++;
			while (j < orderToAdd.size()) {
				newLayout[j + currentLayout.size() - 1] = orderToAdd[j];
				j++;
			}
		}
		currentLayout = newLayout;
	}

	std::cout << "RISULTATO: ";
	for (int n: currentLayout) {
		std::cout << n << " ";
	}
	std::cout << std::endl;

	std::cout << "TERMINATO" << std::endl;

}

void test(Graph* G) {
	std::cout << G->firstEdge()->graphOf() << "   " << G << std::endl;
	std::cout << G->firstEdge()->graphOf()->numberOfEdges() << "   " << G->numberOfEdges() << std::endl;
	std::cout << G->firstEdge()->graphOf()->numberOfNodes() << "   " << G->numberOfNodes() << std::endl;
	std::cout << G->firstEdge()->graphOf()->firstNode()->index() << "   " << G->firstNode()->index() << std::endl;
	std::cout << G->firstEdge()->graphOf()->firstEdge()->target()->index() << "   " << G->firstEdge()->target()->index() << std::endl;
	std::cout << std::endl;
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
	if (current.length() > 0) {
		int index = std::stoi(current);
		if (readingNodes) {
			nodes[index] = G->newNode(index);
		}
		else {
			G->newEdge(nodes[sourceNode], nodes[index]);
		}
	}
}

int main(int argc, char* argv[])
{
	Graph G;

	if (argc > 1) {
        std::cout << "Input string: " << argv[1] << std::endl;
		readGraphFromArg(&G, argv[1]);
		draw(&G, "GraphConf.svg");
		find1StackLayout(&G);
    }
	else {
		std::cout << "Input string: nessun argomento" << std::endl;
		std::cout << std::endl;

		Graph GNotOuterplanar;
		Graph GWithMoreSourcesAndSinks;
		
		string graphName1 = "Graph";
		string graphName2 = "GraphNotOuterplanar";
		string graphName3 = "GraphWithMoreSourcesAndSinks";

		readGraph(&G, graphName1);
		readGraph(&GNotOuterplanar, graphName2);
		readGraph(&GWithMoreSourcesAndSinks, graphName3);

		//draw(&G, graphName1 + ".svg");
		//draw(&GNotOuterplanar, graphName2 + ".svg");
		//draw(&GWithMoreSourcesAndSinks, graphName3 + ".svg");

		std::cout << "### esecuzione con grafo: " << graphName1 << " ###" << std::endl;
		find1StackLayout(&G);
		std::cout << std::endl;

		/*
		std::cout << "### esecuzione con grafo: " << graphName2 << " ###" << std::endl;
		find1StackLayout(&GNotOuterplanar);
		std::cout << std::endl;

		std::cout << "### esecuzione con grafo: " << graphName3 << " ###" << std::endl;
		find1StackLayout(&GWithMoreSourcesAndSinks);
		std::cout << std::endl;
		*/
	}

	//populateGraph(&G);

	//draw(&G, "Graph.svg");
	//saveGraph(&G, "graph")

	return 0;
}
