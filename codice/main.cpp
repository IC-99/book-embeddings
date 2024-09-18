#include <ogdf/basic/Graph_d.h>
#include <ogdf/basic/EdgeArray.h>
#include <ogdf/basic/NodeArray.h>
#include <ogdf/basic/simple_graph_alg.h>
#include <ogdf/basic/extended_graph_alg.h>
#include <ogdf/decomposition/BCTree.h>
#include <ogdf/fileformats/GraphIO.h>
#include <ogdf/energybased/FMMMLayout.h>
#include <ogdf/basic/CombinatorialEmbedding.h>
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
		std::unordered_map<int, std::vector<std::vector<TreeNode*>>> children;
		std::unordered_map<int, TreeNode*> parent;

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

		void addChild(int treeId, TreeNode* child, int childType) {
			if (children.find(treeId) == children.end()) {
				std::vector<std::vector<TreeNode*>> newTree(3);
				children[treeId] = newTree;
			}
			children[treeId][childType].push_back(child);
		}

		void setParent(int treeId, TreeNode* parentNode) {
			parent[treeId] = parentNode;
		}

		TreeNode* getParent(int treeId) {
			if (parent.find(treeId) == parent.end()) {
				return nullptr;
			}
			return parent[treeId];
		}

		std::vector<TreeNode*> getChildren(int treeId, int childType) {
			if (children.find(treeId) == children.end()) {
				std::vector<std::vector<TreeNode*>> newTree(3);
				children[treeId] = newTree;
				return std::vector<TreeNode*>();
			}
			if (children.size() == 0) {
				return std::vector<TreeNode*>();
			}
			return children[treeId][childType];
		}

		std::vector<TreeNode*> getNeighbors() {
			return neighbors;
		}

		bool hasRooting(int treeId) {
			return !(children.find(treeId) == children.end());
		}
};

class BCTPermutationEnumerator {
public:
    TreeNode* root;
    int treeId;
    std::unordered_map<int, std::unordered_map<int, std::vector<int>>> permutations;
    bool hasNext;

    BCTPermutationEnumerator(TreeNode* rootNode) {
        root = rootNode;
        treeId = rootNode->value;
		hasNext = true;
        _initialize();
    }

    bool hasNextPermutation() {
        return hasNext;
    }

    std::vector<int> getNextPermutation() {
        bool toPermute = true;
        std::vector<int> order;
        std::queue<TreeNode*> queue;
        queue.push(root);

        while (!queue.empty()) {
            TreeNode* u = queue.front();
            queue.pop();

            if (u->isCutpoint) {
                //std::cout << "nel cutpoint " << u->value << std::endl;

				for (int childType: {0, 2}) {
					std::vector<TreeNode*> children = u->getChildren(treeId, childType);

					if (children.size() > 1) {
						for (int indexOfChild: permutations[u->value][childType]) {
							queue.push(children[indexOfChild]);
						}
						if (toPermute) {
							if (std::next_permutation(permutations[u->value][childType].begin(), permutations[u->value][childType].end())) {
								toPermute = false;
							}
							else {
								std::vector<int> firstPermutation;
								for (int i = 0; i < permutations[u->value][childType].size(); i++) {
									firstPermutation.push_back(i);
								}
								permutations[u->value][childType] = firstPermutation;
							}
						}
					} else {
						for (TreeNode* child: children) {
							queue.push(child);
						}
					}
				}
            }
            else {
                //std::cout << "nella componente " << u->value << std::endl;
                order.push_back(u->value);
                for (int childType = 0; childType < 3; childType++) {
                    for (TreeNode* child: u->getChildren(treeId, childType)) {
                        queue.push(child);
                    }
                }
            }
        }

        if (toPermute) {
            hasNext = false;
        }

        return order;
    }

private:

    void _initialize() {
        std::queue<TreeNode*> queue;
        queue.push(root);

        while (!queue.empty()) {
            TreeNode* u = queue.front();
            queue.pop();

            if (u->isCutpoint) {

                std::vector<TreeNode*> sourceChildren = u->getChildren(treeId, 0);
                if (sourceChildren.size() > 1) {
                    std::vector<int> permutation;
                    for (int i = 0; i < sourceChildren.size(); i++) {
                        permutation.push_back(i);
                    }
                    permutations[u->value][0] = permutation;
                    std::cout << "il cutpoint " << u->value << " ha " << permutation.size() << " figli sorgente permutabili" << std::endl;
                }

                std::vector<TreeNode*> sinkChildren = u->getChildren(treeId, 2);
                if (sinkChildren.size() > 1) {
                    std::vector<int> permutation;
                    for (int i = 0; i < sinkChildren.size(); i++) {
                        permutation.push_back(i);
                    }
                    permutations[u->value][2] = permutation;
                    std::cout << "il cutpoint " << u->value << " ha " << permutation.size() << " figli pozzo permutabili" << std::endl;
                }
                
            }
			for (int childType = 0; childType < 3; childType++) {
				for (TreeNode* child: u->getChildren(treeId, childType)) {
					queue.push(child);
				}
			}
        }
    }
};

void draw(Graph* G, string fileName)
{
	GraphAttributes GA(*G, GraphAttributes::nodeGraphics |
		GraphAttributes::edgeGraphics |
        GraphAttributes::nodeLabel |
        GraphAttributes::nodeStyle |
        GraphAttributes::edgeType |
        GraphAttributes::edgeArrow |
        GraphAttributes::edgeStyle );

	for (node v : G->nodes) {
		GA.fillColor(v) = Color( "#FFDDAA" ); 
        GA.height(v) = 15.0;
        GA.width(v) = 15.0;
        GA.shape(v) = Shape::Ellipse; 

        GA.label(v) = to_string(v->index());
	}

	for (edge e : G->edges) {
        GA.arrowType(e) = ogdf::EdgeArrow::Last;
        GA.strokeColor(e) = Color("#000000");
	}

	FMMMLayout fmmm;
    fmmm.call(GA);
	
	GraphIO::drawSVG(GA, "OutputSVG-" + fileName);
}

bool isOuterPlanar(Graph* G)
{
	node dummyNode = G->newNode();
	for (node v : G->nodes) {
		G->newEdge(dummyNode, v);
	}
	if (isPlanar(*G)) {
		G->delNode(dummyNode);
		return true;
	}
	return false;
}

bool hasHamiltonianPath(int source, int sink, std::unordered_map<int, std::vector<int>>* adj, int targetSize) {
	if (source == sink){
		return targetSize == 1;
	}
	bool result = false;
	for (int target: (*adj)[source]) {
		result = result || hasHamiltonianPath(target, sink, adj, targetSize - 1);
	}
	return result;
}

Array<node, int> getSourceAndSink(Graph* G)
{
	Array<node, int> nullResult({nullptr, nullptr});
	Array<node, int> sourceAndSink({nullptr, nullptr});

	for (node v : G->nodes) {
		if (v->indeg() == 0) {
			if (sourceAndSink[0] != nullptr) {
				return nullResult;
			}
			else {
				sourceAndSink[0] = v;
			}
		}
		if (v->outdeg() == 0) {
			if (sourceAndSink[1] != nullptr) {
				return nullResult;
			}
			else {
				sourceAndSink[1] = v;
			}
		}
	}
	return sourceAndSink;
}

Array<int, int> mergeLayouts(Array<Array<int, int>, int>* topologicalOrders,
	std::vector<int>* orderOfComponents,
	TreeNode* rootOfBCT,
	Array<Array<node, int>,
	int>* sourceAndSinkOfComponents,
	Array<TreeNode*, int>* treeNodeOfComponent) {
	
	Array<int, int> currentLayout = (*topologicalOrders)[(*orderOfComponents)[0]];

	// fusione dei layout delle componenti biconnesse
	for (int i = 1; i < orderOfComponents->size(); i++) {
		int component = (*orderOfComponents)[i];

		//std::cout << "layout attuale: ";
		//printArray(currentLayout);

		Array<int, int> orderToAdd = (*topologicalOrders)[component];
		int cutpoint = (*treeNodeOfComponent)[component]->getParent(rootOfBCT->value)->value;

		//std::cout << "la componente " << component << " è collegata al grafo G_" << i << " tramite il cutpoint " << cutpoint << std::endl;
		//printArray(orderToAdd);

		int newLayoutSize = currentLayout.size() + orderToAdd.size() - 1;
		Array<int, int> newLayout(newLayoutSize);

	
		if (cutpoint == (*sourceAndSinkOfComponents)[component][0]->index()) {
			//std::cout << "CASISTICA SORGENTE" << std::endl;
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
		else if (cutpoint == (*sourceAndSinkOfComponents)[component][1]->index()){
			//std::cout << "CASISTICA POZZO" << std::endl;
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
		
		currentLayout = newLayout;
	}
	return currentLayout;
}

Array<Graph, int> getBiconnectedComponentsGraphs(Graph* G) {
	EdgeArray<int> edgeArray = EdgeArray<int>(*G);

	int numberOfBiconnectedComponents = biconnectedComponents(*G, edgeArray);
	std::cout << "il grafo ha " << numberOfBiconnectedComponents << " componenti biconnesse" << std::endl;

	Array<Graph, int> biconnectedComponentsGraphs(numberOfBiconnectedComponents);

	Array<Graph, int> test;
	test.init(numberOfBiconnectedComponents);
	test[0] = *G;

	Array<std::unordered_map<int, node>, int> biconnectedComponentsNodeMaps(numberOfBiconnectedComponents);

	int edgeIndex = 0;
	for (edge e: G->edges) {
		std::cout << "edge,component = " << edgeIndex << "," << edgeArray[e] << std::endl;
		edgeIndex++;
	}

	for (edge e: G->edges) {
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

	return biconnectedComponentsGraphs;
}

bool getSourceAndSinkOfComponents(Array<Graph, int>* biconnectedComponentsGraphs, Array<Array<node, int>, int>* sourceAndSinkOfComponents) {
	sourceAndSinkOfComponents->init(biconnectedComponentsGraphs->size());

	// verifica se ogni componente ha una sola sorgente e un solo pozzo
	for (int i = 0; i < biconnectedComponentsGraphs->size(); i++) {
		//draw(&biconnectedComponentsGraphs[i], "Component" + to_string(i) + ".svg" );

		Array<node, int> sourceAndSink = getSourceAndSink(&(*biconnectedComponentsGraphs)[i]);
		//std::cout << "componente " << i << ": sorgente " << sourceAndSink[0] << ", pozzo " << sourceAndSink[1] << std::endl;

		if (sourceAndSink[0] != nullptr && sourceAndSink[1] != nullptr) {
			(*sourceAndSinkOfComponents)[i] = sourceAndSink;
		}
		else {
			std::cout << "la componente biconnessa " << i << " non ha una sola sorgente e un solo pozzo" << std::endl;
			std::cout << "---- FINE FASE 1 ----" << std::endl;
			return false;
		}
	}
	return true;
}

bool checkHamiltonianPaths(Array<Graph, int>* biconnectedComponentsGraphs, Array<Array<node, int>, int>* sourceAndSinkOfComponents) {
	for (int i = 0; i < sourceAndSinkOfComponents->size(); i++) {
		node source = (*sourceAndSinkOfComponents)[i][0];
		node sink = (*sourceAndSinkOfComponents)[i][1];

		std::unordered_map<int, std::vector<int>> adj;
		for (edge e: (*biconnectedComponentsGraphs)[i].edges) {
			adj[e->source()->index()].push_back(e->target()->index());
		}

		bool found = false;
		for (int target: adj[source->index()]){
			if (target == sink->index()) {
				found = true;
				break;
			}
		}
		bool hasHamiltonian = hasHamiltonianPath(source->index(), sink->index(), &adj, (*biconnectedComponentsGraphs)[i].numberOfNodes());
		if (!found || !hasHamiltonian) {
			std::cout << "la componente biconnessa " << i << " non contiene un cammino hamiltoniano sulla outerface" << std::endl;
			std::cout << "---- FINE FASE 1 ----" << std::endl;
			return false;
		}
	}
	return true;
}

Array<Array<int, int>, int> getTopologicalOrders(Array<Graph, int>* biconnectedComponentsGraphs) {
	Array<Array<int, int>, int> topologicalOrders(biconnectedComponentsGraphs->size());

	// calcolo dell'ordine topologico dei nodi di ogni componente biconnessa
	for (int i = 0; i < biconnectedComponentsGraphs->size(); i++) {
		NodeArray<int> nodeArray = NodeArray<int>((*biconnectedComponentsGraphs)[i]);
		topologicalNumbering((*biconnectedComponentsGraphs)[i], nodeArray);

		Array<int, int> topologicalOrder((*biconnectedComponentsGraphs)[i].numberOfNodes());

		for (node v : (*biconnectedComponentsGraphs)[i].nodes) {
			topologicalOrder[nodeArray[v]] = v->index();
		}

		topologicalOrders[i] = topologicalOrder;

		std::cout << "ordine topologico della componente " << i << ": ";
		std::cout << "[";
		int j = 0;
		while (j < topologicalOrders[i].size() - 1) {
			std::cout << topologicalOrders[i][j] << ", ";
			j++;
		}
		std::cout << topologicalOrders[i][j] << "]" << std::endl;
	}
	return topologicalOrders;
}

Array<TreeNode*, int> createBCT(int numberOfBiconnectedComponents, std::vector<int>* cutpoints, std::unordered_map<int, std::vector<int>>* componentsOfNode) {
	Array<TreeNode*, int> treeNodeOfComponent(numberOfBiconnectedComponents);

	int idOfTreeNode = 0;

	// creazione dei TreeNode per le componenti
	for (int i = 0; i < numberOfBiconnectedComponents; i++) {
		TreeNode* componentTreeNode = new TreeNode(idOfTreeNode, i, false);
		treeNodeOfComponent[i] = componentTreeNode;

		idOfTreeNode++;
	}

	// creazione dei TreeNode per i cutpoint
	for (int cutpoint : *cutpoints) {
		TreeNode* cutpointTreeNode = new TreeNode(idOfTreeNode, cutpoint, true);

		// collegamento dei TreeNode rappresentanti i cutpoint alle componenti di cui i cutpoint fanno parte
		for (int component : (*componentsOfNode)[cutpoint]) {
			cutpointTreeNode->addNeighbor(treeNodeOfComponent[component]);
			treeNodeOfComponent[component]->addNeighbor(cutpointTreeNode);
			//std::cout << "il cutpoint " << cutpoint << " è stato collegato alla componente " << treeNodeOfComponent[component]->value << std::endl;
		}
		idOfTreeNode++;
	}

	std::cout << "costruito il BCT contenente " << cutpoints->size() << " cutpoint e " << numberOfBiconnectedComponents << " componenti biconnesse" << std::endl;
	return treeNodeOfComponent;
}

std::vector<int> rootBCT(TreeNode* rootOfBCT, Array<Array<node, int>, int>* sourceAndSinkOfComponents, int parentCutpoint = -1) {
	std::unordered_map<int, bool> visited;
    std::queue<TreeNode*> queue;

	// ordine delle componenti
	std::vector<int> orderOfComponents;

    visited[rootOfBCT->id] = true;
    queue.push(rootOfBCT);

	bool existsRooting = rootOfBCT->hasRooting(rootOfBCT->value);

	// BST sul BCT non radicato per radicarlo e trovare l'ordine delle componenti (grado di libertà)
    while (!queue.empty()) {
        TreeNode* u = queue.front();
		queue.pop();
		if (!u->isCutpoint) {
			orderOfComponents.push_back(u->value);
		}

        // esplora tutti i nodi adiacenti e salva i figli del nodo (grado di libertà sull'ordine)
        for (TreeNode* neighbor : u->neighbors) {
            if (!visited[neighbor->id]) {
                visited[neighbor->id] = true;
                queue.push(neighbor);
				if (!existsRooting) {
					int childrenType = -1;
					if (neighbor->isCutpoint) {
						if ((*sourceAndSinkOfComponents)[u->value][0]->index() == neighbor->value) {
							childrenType = 0; // sorgente
						}
						else if ((*sourceAndSinkOfComponents)[u->value][1]->index() == neighbor->value) {
							childrenType = 2; // pozzo
						}
						else {
							childrenType = 1; // intermedio
						}
					}
					else {
						if ((*sourceAndSinkOfComponents)[neighbor->value][0]->index() == u->value) {
							childrenType = 0;
						}
						else if ((*sourceAndSinkOfComponents)[neighbor->value][1]->index() == u->value) {
							childrenType = 2;
						}
						else {
							childrenType = 1;
						}
						neighbor->setParent(rootOfBCT->id, u);
					}
					u->addChild(rootOfBCT->id, neighbor, childrenType);
				}
            }
        }
    }
	return orderOfComponents;
}

void findOtherRoots(TreeNode* treeNode, int treeId, int parentType, std::vector<int>* componentRoots) {
	if (treeNode->isCutpoint) {
		//std::cout << "-----------" << std::endl;
		//std::cout << "cutpoint: " << treeNode->value << ", parentType: " << parentType << std::endl;
		for (int childType = 0; childType < 3; childType++) {
			for (TreeNode* componentNode: treeNode->getChildren(treeId, childType)) {
				if (childType == parentType) {
					componentRoots->push_back(componentNode->value);
				}
				findOtherRoots(componentNode, treeId, childType, componentRoots);
			}
		}
	}
	else {
		//std::cout << "-----------" << std::endl;
		//std::cout << "componente: " << treeNode->value << ", parentType: " << parentType << std::endl;
		for (TreeNode* cutpointNode: treeNode->getChildren(treeId, 0)) {
			findOtherRoots(cutpointNode, treeId, 0, componentRoots);
		}
		for (TreeNode* cutpointNode: treeNode->getChildren(treeId, 2)) {
			findOtherRoots(cutpointNode, treeId, 2, componentRoots);
		}
	}
}

bool computeRestrictions(Array<TreeNode*, int>* treeNodeOfComponent, std::vector<int>* orderOfComponents, Array<Array<node, int>, int>* sourceAndSinkOfComponents, int* betterRoot) {
	
	int rootBlock = (*orderOfComponents)[0];
	Array<bool, int> restrictedInComponent(orderOfComponents->size());
	Array<int, int> positionOfComponent(orderOfComponents->size());

	for (int i = 0; i < orderOfComponents->size(); i++) {
		positionOfComponent[(*orderOfComponents)[i]] = i;
		restrictedInComponent[i] = false;
	}

	for (int i = orderOfComponents->size() - 1; i >= 0; i--) {
		int currentBlock = (*orderOfComponents)[i];
		TreeNode* currentBlockTreeNode = (*treeNodeOfComponent)[currentBlock];
		int countRestricted = 0;

		for (TreeNode* cutpointTreeNode: currentBlockTreeNode->getNeighbors()) {
			//std::cout << "analizzando cutpoint " << cutpointTreeNode->value << " nella componente " << currentBlock << std::endl;
			for (int childType = 0; childType < 3; childType++) {
				bool found = false;
				for (TreeNode* otherComponentTreeNode: cutpointTreeNode->getChildren(rootBlock, childType)) {
					int otherComponent = otherComponentTreeNode->value;
					if (positionOfComponent[otherComponent] > positionOfComponent[currentBlock]) {
						//std::cout << "il cutpoint " << cutpointTreeNode->value << " fa parte della componente " << otherComponent << std::endl;
						//std::cout << "il cutpoint " << cutpointTreeNode->value << " in " << otherComponent << " è di tipo: " << childType << std::endl;

						if (childType == 1 || restrictedInComponent[otherComponent]) {
							/*
							if (cutpointTreeNode->value != (*sourceAndSinkOfComponents)[currentBlock][0]->index() && cutpointTreeNode->value != (*sourceAndSinkOfComponents)[currentBlock][1]->index()){
								std::cout << "TROVATA COPPIA CONFLITTUALE" << std::endl;
								std::cout << "---- FINE FASE 2 ----" << std::endl;
								return false;
							}*/
							countRestricted++;
							restrictedInComponent[currentBlock] = true;
							//std::cout << "il cutpoint " << cutpointTreeNode->value << " è ristretto, trovato con componente " << otherComponent << std::endl;
							if (*betterRoot == -1) {
								*betterRoot = otherComponent;
							}
							found = true;
							break;
						}
					}
				}
				if (found) {
					break;
				}
			}
		}
		//std::cout << "NUMERO NODI RISTRETTI PER LA COMPONENTE " << component << ": " << countRestricted << std::endl;
		if (countRestricted > 1) {
			std::cout << "TROVATA COPPIA CONFLITTUALE" << std::endl;
			std::cout << "---- FINE FASE 2 ----" << std::endl;
			return false;
		}
	}
	return true;
}

void printRestrictions(std::vector<int>* cutpoints, std::unordered_map<int, TreeNode*>* restrictingComponentOfCutpoint) {
	std::cout << "CAUSE RESTRIZIONI:" << std::endl;
	for (int cutpoint: *cutpoints) {
		if (restrictingComponentOfCutpoint->find(cutpoint) != restrictingComponentOfCutpoint->end()) {
			std::cout << "il cutpoint " << cutpoint << " è ristretto a causa della componente " << (*restrictingComponentOfCutpoint)[cutpoint]->value << std::endl;
		}
	}
}

void enumerate1StackLayouts(Graph* G) {
	std::cout << "---- INIZIO FASE 1 ----" << std::endl;
	std::cout << "il grafo ha " << G->numberOfNodes() << " nodi e " << G->numberOfEdges() << " archi" << std::endl;

	// verifica se il grafo è outerplanare
	if (!isOuterPlanar(G)) {
		std::cout << "il grafo non è outerplanare" << std::endl;
		std::cout << "---- FINE FASE 1 ----" << std::endl;
		return;
	}

	// calcolo delle componenti biconnesse
	Array<Graph, int> biconnectedComponentsGraphs = getBiconnectedComponentsGraphs(G);
	int numberOfBiconnectedComponents = biconnectedComponentsGraphs.size();

	// verifica se ogni componente ha una sola sorgente e un solo pozzo
	Array<Array<node, int>, int> sourceAndSinkOfComponents;
	if (!getSourceAndSinkOfComponents(&biconnectedComponentsGraphs, &sourceAndSinkOfComponents)) {
		return;
	}

	// verifica se esiste un cammino hamiltoniano sulla outerface di ogni componente biconnessa
	if (!checkHamiltonianPaths(&biconnectedComponentsGraphs, &sourceAndSinkOfComponents)) {
		return;
	}

	// commentata linea 185 del file NodeArray.h "OGDF_ASSERT(v->graphOf() == m_pGraph);"
	// ottieni l'ordine topologico di ogni componente biconnessa
	Array<Array<int, int>, int> topologicalOrders = getTopologicalOrders(&biconnectedComponentsGraphs);

	std::cout << "---- FINE FASE 1 ----" << std::endl << std::endl;

	std::cout << "---- INIZIO FASE 2 ----" << std::endl;

	// componentsOfNode è una mappa di vettori che contengono per ogni nodo le componenti a cui esso appartiene
	std::unordered_map<int, std::vector<int>> componentsOfNode(G->numberOfNodes());

	// popola componentsOfNode
	for (int i = 0; i < numberOfBiconnectedComponents; i++) {
		for (node v : biconnectedComponentsGraphs[i].nodes) {
			componentsOfNode[v->index()].push_back(i);
		}
	}

	// per memorizzare quali sono i cutpoint di ogni componente
	std::vector<int> cutpoints;

	// trova i cutpoint
	for (node v: G->nodes) {
		if (componentsOfNode[v->index()].size() > 1) {
			cutpoints.push_back(v->index());
		}
	}

	// treeNodeOfComponent è una mappa che restituisce il TreeNode dal valore del TreeNode
	Array<TreeNode*, int> treeNodeOfComponent = createBCT(numberOfBiconnectedComponents, &cutpoints, &componentsOfNode);

	// radicazione del BCT
	int rootBlock = 0;
	TreeNode* rootOfBCT = treeNodeOfComponent[rootBlock];
	std::vector<int> orderOfComponents = rootBCT(rootOfBCT, &sourceAndSinkOfComponents);

	int betterRoot = -1;
	if (!computeRestrictions(&treeNodeOfComponent, &orderOfComponents, &sourceAndSinkOfComponents, &betterRoot)) {
		return;
	}

	if (betterRoot != -1) {
		std::cout << "trovata una radice migliore: " << betterRoot << std::endl;
		// cambio di radice iniziale
		rootBlock = betterRoot;
		rootOfBCT = treeNodeOfComponent[betterRoot];
		rootBCT(rootOfBCT, &sourceAndSinkOfComponents);
	}

	std::cout << "cerco nuove radici dal radicamento in " << rootOfBCT->value << std::endl;
	// contiene una lista di coppie [componente su cui si può radicare, cutpoint causa di tale scelta] se questo cutpoint poi risulta essere ristretto allora l'albero deve essere ignorato
	std::vector<int> otherRoots;
	findOtherRoots(rootOfBCT, rootBlock, -1, &otherRoots);

	std::cout << "POSSIBILI RADICI (oltre alla radice attuale = " << rootBlock << "): ";
	for (int root: otherRoots) {
		std::cout << root << " ";
	}
	std::cout << std::endl;

	int resultsCounter = 0;

	std::cout << "################################### BCT RADICATO NELLA COMPONENTE " << rootBlock << std::endl;
	
	BCTPermutationEnumerator enumerator(rootOfBCT);
	std::vector<int> order;
	
	while (enumerator.hasNextPermutation()) {
		order = enumerator.getNextPermutation();
		
		Array<int, int> result = mergeLayouts(&topologicalOrders, &order, rootOfBCT, &sourceAndSinkOfComponents, &treeNodeOfComponent);
		resultsCounter++;

		std::cout << "RESULT: ";
		for (int n: result) {
			std::cout << n << " ";
		}
		std::cout << std::endl;
	}

	for (int otherRoot: otherRoots){
		TreeNode* otherRootOfBCT = treeNodeOfComponent[otherRoot];
		rootBCT(otherRootOfBCT, &sourceAndSinkOfComponents);

		std::cout << "################################### BCT RADICATO NELLA COMPONENTE " << otherRoot << std::endl;

		BCTPermutationEnumerator enumerator(otherRootOfBCT);
		std::vector<int> order;
		
		while (enumerator.hasNextPermutation()) {
			order = enumerator.getNextPermutation();

			Array<int, int> result = mergeLayouts(&topologicalOrders, &order, otherRootOfBCT, &sourceAndSinkOfComponents, &treeNodeOfComponent);
			resultsCounter++;

			std::cout << "RESULT: ";
			for (int n: result) {
				std::cout << n << " ";
			}
			std::cout << std::endl;
		}
	}

	std::cout << "embedding trovati: " << resultsCounter << std::endl;
	std::cout << "TERMINATO" << std::endl;

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

	if (argc == 2) {
        std::cout << "input string: " << argv[1] << std::endl;
		try {
			readGraphFromArg(&G, argv[1]);
			draw(&G, "DAG.svg");
			enumerate1StackLayouts(&G);
		}
		catch (const std::runtime_error& e) {
			std::cerr << "runtime error: " << e.what() << std::endl;
		}
    }
	else {
		std::cout << "error with the number of arguments (must be one string)" << std::endl;
		return 1;
	}

	return 0;
}