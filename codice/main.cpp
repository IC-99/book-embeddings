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

		bool hasRooting(int treeId) {
			return !(children.find(treeId) == children.end());
		}

};

class BCTPermutationIterator {
public:
    TreeNode* root;
    int treeId;
    std::unordered_map<int, TreeNode*>* restrictions;
    std::unordered_map<int, std::unordered_map<int, std::vector<int>>> indexOfChildrenOfCutpoint;
    std::unordered_map<int, std::unordered_map<int, std::vector<int>>> permutations;
    bool hasNextPermutation = true;

    BCTPermutationIterator(TreeNode* rootNode, int treeIdentifier, std::unordered_map<int, TreeNode*>* restrictingComponentOfCutpoint) {
        root = rootNode;
        treeId = treeIdentifier;
        restrictions = restrictingComponentOfCutpoint;
        _initialize();
    }

    bool hasNext() {
        return hasNextPermutation;
    }

    std::vector<int> next() {
        bool toPermute = true;
        std::vector<int> order;
        std::queue<TreeNode*> queue;
        queue.push(root);

        while (!queue.empty()) {
            TreeNode* u = queue.front();
            queue.pop();

            if (u->isCutpoint) {
                //std::cout << "nel cutpoint " << u->value << std::endl;

                if (restrictions->find(u->value) != restrictions->end()) {
                    queue.push((*restrictions)[u->value]);
                }

				bool hasSourcePermutations = false;
				bool hasSinkPermutations = false;

				if (permutations.find(u->value) != permutations.end()) {
					if (permutations[u->value].find(0) != permutations[0].end()) {
						hasSourcePermutations = true;
					}
					if (permutations[u->value].find(2) != permutations[2].end()) {
						hasSinkPermutations = true;
					}
				}

                std::vector<TreeNode*> sourceChildren = u->getChildren(treeId, 0);

                if (hasSourcePermutations) {
					//std::cout << "nel cutpoint " << u->value << " ci sono delle permutazioni di sorgente" << std::endl;
					for (int i: permutations[u->value][0]) {
						int index = indexOfChildrenOfCutpoint[u->value][0][i];
						queue.push(sourceChildren[index]);
					}
					if (toPermute) {
						if (std::next_permutation(permutations[u->value][0].begin(), permutations[u->value][0].end())) {
							toPermute = false;
						}
						else {
							std::vector<int> permutation;
							for (int i = 0; i < permutations[u->value][0].size(); i++) {
								permutation.push_back(i);
							}
							permutations[u->value][0] = permutation;
						}
					}
                } else {
                    for (TreeNode* child: sourceChildren) {
                        if (restrictions->find(u->value) != restrictions->end()) {
                            if ((*restrictions)[u->value] != child) {
                                queue.push(child);
                            }
                        }
                        else {
                            queue.push(child);
                        }
                    }
                }
                
                for (TreeNode* child: u->getChildren(treeId, 1)) {
					if (restrictions->find(u->value) != restrictions->end()) {
						if ((*restrictions)[u->value] != child) {
							queue.push(child);
						}
					}
					else {
						queue.push(child);
					}
                }

                std::vector<TreeNode*> sinkChildren = u->getChildren(treeId, 2);
                
                if (hasSinkPermutations) {
					//std::cout << "nel cutpoint " << u->value << " ci sono delle permutazioni di pozzo" << std::endl;
					for (int i: permutations[u->value][2]) {
						int index = indexOfChildrenOfCutpoint[u->value][2][i];
						queue.push(sinkChildren[index]);
					}
					if (toPermute) {
						if (std::next_permutation(permutations[u->value][2].begin(), permutations[u->value][2].end())) {
							toPermute = false;
						}
						else {
							std::vector<int> permutation;
							for (int i = 0; i < permutations[u->value][2].size(); i++) {
								permutation.push_back(i);
							}
							permutations[u->value][2] = permutation;
						}
					}  
                } else {
                    for (TreeNode* child: sinkChildren) {
                        if (restrictions->find(u->value) != restrictions->end()) {
                            if ((*restrictions)[u->value] != child) {
                                queue.push(child);
                            }
                        }
                        else {
                            queue.push(child);
                        }
                    }
                }
            }
            else {
                //std::cout << "nella componente " << u->value << std::endl;
                order.push_back(u->value);
                for (int childType = 0; childType < 3; childType++) {
                    for(TreeNode* child: u->getChildren(treeId, childType)) {
                        queue.push(child);
                    }
                }
            }
        }

        if (toPermute) {
            hasNextPermutation = false;
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
                    std::vector<int> childrenToPermute;
                    for (int i = 0; i < sourceChildren.size(); i++) {
                        if (restrictions->find(u->value) == restrictions->end()) {
                            childrenToPermute.push_back(i);
                        }
                        else {
                            if (sourceChildren[i] != (*restrictions)[u->value]) {
                                childrenToPermute.push_back(i);
                            }
                        }
                    } 
                    indexOfChildrenOfCutpoint[u->value][0] = childrenToPermute;

                    std::vector<int> permutation;
                    for (int i = 0; i < childrenToPermute.size(); i++) {
                        permutation.push_back(i);
                    }
                    permutations[u->value][0] = permutation;
                    std::cout << "il cutpoint " << u->value << " ha " << childrenToPermute.size() << " figli sorgente permutabili" << std::endl;
                }
                
                for (TreeNode* child: sourceChildren) {
                    queue.push(child);
                }
                
                for (TreeNode* child: u->getChildren(treeId, 1)) {
                    queue.push(child);
                }

                std::vector<TreeNode*> sinkChildren = u->getChildren(treeId, 2);
                if (sinkChildren.size() > 1) {
                    std::vector<int> childrenToPermute;
                    for (int i = 0; i < sinkChildren.size(); i++) {
                        if (restrictions->find(u->value) == restrictions->end()) {
                            childrenToPermute.push_back(i);
                        }
                        else {
                            if (sinkChildren[i] != (*restrictions)[u->value]) {
                                childrenToPermute.push_back(i);
                            }
                        }
                    } 
                    indexOfChildrenOfCutpoint[u->value][2] = childrenToPermute;

                    std::vector<int> permutation;
                    for (int i = 0; i < childrenToPermute.size(); i++) {
                        permutation.push_back(i);
                    }
                    permutations[u->value][2] = permutation;
                    std::cout << "il cutpoint " << u->value << " ha " << childrenToPermute.size() << " figli pozzo permutabili" << std::endl;
                }
                
                for (TreeNode* child: sinkChildren) {
                    queue.push(child);
                }
            }
            else {
                for (int childType = 0; childType < 3; childType++) {
                    for(TreeNode* child: u->getChildren(treeId, childType)) {
                        queue.push(child);
                    }
                }
            }

        }
    }
};

int getParentCutpoint(TreeNode* treeNode, int component, int treeId) {
	if (treeNode->children.size() == 0) {
		return -1;
	}
	if (treeNode->isCutpoint) {
		for (int childType = 0; childType < 3; childType++) {
			for(TreeNode* componentNode: treeNode->getChildren(treeId, childType)) {
				if (componentNode->value == component) {
					return treeNode->value;
				}
				int result = getParentCutpoint(componentNode, component, treeId);
				if (result != -1) {
					return result;
				}
			}
		}
	}
	else {
		for (int childType = 0; childType < 3; childType++) {
			for(TreeNode* cutpointNode: treeNode->getChildren(treeId, childType)) {
				int result = getParentCutpoint(cutpointNode, component, treeId);
				if (result != -1) {
					return result;
				}
			}
		}
	}
	return -1;
}

void findOtherRoots(TreeNode* treeNode, int parentType, int treeId, std::vector<int>* componentRoots) {
	if (treeNode->isCutpoint) {
		//std::cout << "-----------" << std::endl;
		//std::cout << "cutpoint: " << treeNode->value << ", parentType: " << parentType << std::endl;
		for (int childType = 0; childType < 3; childType++) {
			for(TreeNode* componentNode: treeNode->getChildren(treeId, childType)) {
				if (childType == parentType) {
					componentRoots->push_back(componentNode->value);
				}
			}
			for(TreeNode* componentNode: treeNode->getChildren(treeId, childType)) {
				findOtherRoots(componentNode, childType, treeId, componentRoots);
			}
		}
	}
	else {
		//std::cout << "-----------" << std::endl;
		//std::cout << "componente: " << treeNode->value << ", parentType: " << parentType << std::endl;
		for(TreeNode* cutpointNode: treeNode->getChildren(treeId, 0)) {
			findOtherRoots(cutpointNode, 0, treeId, componentRoots);
		}
		for(TreeNode* cutpointNode: treeNode->getChildren(treeId, 2)) {
			findOtherRoots(cutpointNode, 2, treeId, componentRoots);
		}
	}
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

bool hasHamiltonianPath(node source, node sink, int targetSize) {
	if (source == sink){
		return targetSize == 1;
	}
	bool result = false;
	for (adjEntry adj: source->adjEntries) {
		edge e = adj->theEdge();
		if (e->target() != source) {
			result = result || hasHamiltonianPath(e->target(), sink, targetSize - 1);
		}
	}
	return result;
}

Array<node, int> getSourceAndSink(Graph* G)
{
	Array<node, int> nullResult({nullptr, nullptr});
	Array<node, int> sourceAndSink({nullptr, nullptr});

	for(node v : G->nodes) {
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

Array<int, int> mergeLayouts(Array<Array<int, int>, int>* topologicalOrders, std::vector<int>* orderOfComponents, TreeNode* rootOFBCT, Array<Array<node, int>, int>* sourceAndSinkOfComponents, std::unordered_map<int, TreeNode*>* restrictingComponentOfCutpoint) {
	Array<int, int> currentLayout = (*topologicalOrders)[(*orderOfComponents)[0]];

	// fusione dei layout delle componenti biconnesse
	for (int i = 1; i < orderOfComponents->size(); i++) {
		int component = (*orderOfComponents)[i];

		//std::cout << "layout attuale: ";
		//printArray(currentLayout);

		Array<int, int> orderToAdd = (*topologicalOrders)[component];
		int cutpoint = getParentCutpoint(rootOFBCT, component, rootOFBCT->value);

		//std::cout << "la componente " << component << " è collegata al grafo G_" << i << " tramite il cutpoint " << cutpoint << std::endl;
		//printArray(orderToAdd);

		int newLayoutSize = currentLayout.size() + orderToAdd.size() - 1;
		Array<int, int> newLayout(newLayoutSize);

		bool notAddedYet = true;

		if (restrictingComponentOfCutpoint->find(cutpoint) != restrictingComponentOfCutpoint->end()) {
			if ((*restrictingComponentOfCutpoint)[cutpoint]->value == component) {
				notAddedYet = false;
				//std::cout << "CASISTICA RISTRETTA" << std::endl;
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
		}

		if (notAddedYet) {
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
	for(edge e: G->edges) {
		std::cout << "edge,component = " << edgeIndex << "," << edgeArray[e] << std::endl;
		edgeIndex++;
	}

	for(edge e: G->edges) {
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
	for(int i = 0; i < biconnectedComponentsGraphs->size(); i++) {
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
	for(int i = 0; i < sourceAndSinkOfComponents->size(); i++) {
		node source = (*sourceAndSinkOfComponents)[i][0];
		node sink = (*sourceAndSinkOfComponents)[i][1];
		bool found = false;
		for (adjEntry adj: source->adjEntries){
			edge e = adj->theEdge();
			if (e->source() == source) {
				if (e->target() == sink) {
					found = true;
					break;
				}
			}
		}
		if (!found || !hasHamiltonianPath(source, sink, (*biconnectedComponentsGraphs)[i].numberOfNodes())) {
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
	for(int i = 0; i < biconnectedComponentsGraphs->size(); i++) {
		NodeArray<int> nodeArray = NodeArray<int>((*biconnectedComponentsGraphs)[i]);
		topologicalNumbering((*biconnectedComponentsGraphs)[i], nodeArray);

		Array<int, int> topologicalOrder((*biconnectedComponentsGraphs)[i].numberOfNodes());

		for(node v : (*biconnectedComponentsGraphs)[i].nodes) {
			topologicalOrder[nodeArray[v]] = v->index();
		}

		topologicalOrders[i] = topologicalOrder;

		std::cout << "ordine topologico della componente " << i << ": ";
		printArray(topologicalOrders[i]);
	}
	return topologicalOrders;
}

Array<TreeNode*, int> createBCT(int numberOfBiconnectedComponents, std::vector<int>* cutpoints, std::unordered_map<int, std::vector<int>>* componentsOfNode) {
	Array<TreeNode*, int> treeNodeOfComponent(numberOfBiconnectedComponents);

	int idOfTreeNode = 0;

	// creazione dei TreeNode per le componenti
	for(int i = 0; i < numberOfBiconnectedComponents; i++) {
		TreeNode* componentTreeNode = new TreeNode(idOfTreeNode, i, false);
		treeNodeOfComponent[i] = componentTreeNode;

		idOfTreeNode++;
	}

	// creazione dei TreeNode per i cutpoint
	for(int cutpoint : *cutpoints) {
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

std::vector<int> rootBCT(TreeNode* rootOfBCT, Array<Array<node, int>, int>* sourceAndSinkOfComponents) {
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
					}
					u->addChild(rootOfBCT->id, neighbor, childrenType);
				}
            }
        }
    }
	return orderOfComponents;
}

bool getRestrictions(
	std::unordered_map<int, std::vector<int>>* componentsOfNode,
	std::unordered_map<int, std::vector<int>>* cutpointsOfComponent,
	Array<Array<node, int>, int>* sourceAndSinkOfComponents,
	Array<TreeNode*, int>* treeNodeOfComponent,
	std::vector<int>* orderOfComponents,
	std::unordered_map<int, TreeNode*>* restrictingComponentOfCutpoint,
	int* betterRoot = nullptr) {
	
	// ricerca dei cutpoint ristretti
	std::unordered_map<int, bool> isCutpointRestricted;
	std::unordered_map<int, int> cutpointRestrictedInComponent;

	for(int i = orderOfComponents->size() - 1; i >= 0; i--) {
		int component = (*orderOfComponents)[i];
		int countRestricted = 0;
		cutpointRestrictedInComponent[component] = -1;

		for(int cutpoint: (*cutpointsOfComponent)[component]) {
			//std::cout << "analizzando cutpoint " << cutpoint << " nella componente " << component << std::endl;
			for(int j = orderOfComponents->size() - 1; j > i; j--) {
				int otherComponent = (*orderOfComponents)[j];
				bool isCutpointInOtherComponent = false;
				for(int componentOfCutpoint: (*componentsOfNode)[cutpoint]) {
					if (componentOfCutpoint == otherComponent) {
						isCutpointInOtherComponent = true;
						break;
					}
				}

				if (isCutpointInOtherComponent) {
					//std::cout << "il cutpoint " << cutpoint << " fa parte della componente " << otherComponent << std::endl;

					bool isIntermediate = cutpoint != (*sourceAndSinkOfComponents)[otherComponent][0]->index() && cutpoint != (*sourceAndSinkOfComponents)[otherComponent][1]->index();

					//std::cout << "il cutpoint " << cutpoint << " è intermedio in " << otherComponent << ": " << isIntermediate << std::endl;

					if (isIntermediate || cutpointRestrictedInComponent[otherComponent] != -1) {
						countRestricted++;
						isCutpointRestricted[cutpoint] = true;
						cutpointRestrictedInComponent[component] = cutpoint;
						(*restrictingComponentOfCutpoint)[cutpoint] = (*treeNodeOfComponent)[otherComponent];
						//std::cout << "il cutpoint " << cutpoint << " è ristretto, trovato con componente " << otherComponent << std::endl;
						if (betterRoot != nullptr) {
							if (*betterRoot == -1) {
								*betterRoot = otherComponent;
							}
						}
						break;
					}
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
	for(int i = 0; i < numberOfBiconnectedComponents; i++) {
		for (node v : biconnectedComponentsGraphs[i].nodes) {
			componentsOfNode[v->index()].push_back(i);
		}
	}

	// per memorizzare quali sono i cutpoint di ogni componente
	std::unordered_map<int, std::vector<int>> cutpointsOfComponent(numberOfBiconnectedComponents);
	std::vector<int> cutpoints;

	// trova i cutpoint
	for(node v: G->nodes) {
		if (componentsOfNode[v->index()].size() > 1) {
			cutpoints.push_back(v->index());

			for(int component: componentsOfNode[v->index()]) {
				cutpointsOfComponent[component].push_back(v->index());
			}
		}
	}

	// treeNodeOfComponent è una mappa che restituisce il TreeNode dal valore del TreeNode
	Array<TreeNode*, int> treeNodeOfComponent = createBCT(numberOfBiconnectedComponents, &cutpoints, &componentsOfNode);

	// radicazione del BCT
	int componentRoot = 0;
	TreeNode* rootOfBCT = treeNodeOfComponent[componentRoot];
	std::vector<int> orderOfComponents = rootBCT(rootOfBCT, &sourceAndSinkOfComponents);

	int betterRoot = -1;
	std::unordered_map<int, TreeNode*> restrictingComponentOfCutpoint;
	if (!getRestrictions(&componentsOfNode, &cutpointsOfComponent, &sourceAndSinkOfComponents, &treeNodeOfComponent, &orderOfComponents, &restrictingComponentOfCutpoint, &betterRoot)) {
		return;
	}
	printRestrictions(&cutpoints, &restrictingComponentOfCutpoint);

	if (betterRoot != -1) {
		std::cout << "trovata una radice migliore: " << betterRoot << std::endl;
		// cambio di radice iniziale
		componentRoot = betterRoot;
		rootOfBCT = treeNodeOfComponent[betterRoot];
		orderOfComponents = rootBCT(rootOfBCT, &sourceAndSinkOfComponents); // non serve assegnarlo (per ora assegnato per sicurezza)
	}

	std::cout << "cerco nuove radici dal radicamento in " << rootOfBCT->value << std::endl;
	// contiene una lista di coppie [componente su cui si può radicare, cutpoint causa di tale scelta] se questo cutpoint poi risulta essere ristretto allora l'albero deve essere ignorato
	std::vector<int> componentRoots;
	findOtherRoots(rootOfBCT, -1, componentRoot, &componentRoots);

	std::cout << "POSSIBILI RADICI (oltre alla radice attuale = " << componentRoot << "): ";
	for (int root: componentRoots) {
		std::cout << root << ", ";
	}
	std::cout << std::endl;

	// radicazione degli alberi sulle possibili radici
	for (int i = 0; i < componentRoots.size(); i++) {
		int otherComponentRoot = componentRoots[i];
		TreeNode* otherRootOfBCT = treeNodeOfComponent[otherComponentRoot];
		rootBCT(otherRootOfBCT, &sourceAndSinkOfComponents);
	}

	int resultsCounter = 0;

	std::cout << "################################### BCT RADICATO NELLA COMPONENTE " << componentRoot << std::endl;
	
	std::unordered_map<int, TreeNode*> emptyMap;
	BCTPermutationIterator iterator(rootOfBCT, componentRoot, &emptyMap);

	std::vector<int> order;
	
	while (iterator.hasNext()) {
		order = iterator.next();
		
		Array<int, int> result = mergeLayouts(&topologicalOrders, &order, rootOfBCT, &sourceAndSinkOfComponents, &emptyMap);

		std::cout << "RISULTATO " << resultsCounter + 1 << ": ";
		for (int n: result) {
			std::cout << n << " ";
		}
		std::cout << std::endl;

		resultsCounter++;
	}

	for (int otherComponentRoot: componentRoots){
		TreeNode* otherRootOfBCT = treeNodeOfComponent[otherComponentRoot];

		std::cout << "################################### BCT RADICATO NELLA COMPONENTE " << otherComponentRoot << std::endl;

		std::unordered_map<int, bool> visited;
		std::queue<TreeNode*> queue;

		// ordine delle componenti
		std::vector<int> otherOrderOfComponents;

		visited[otherRootOfBCT->id] = true;
		queue.push(otherRootOfBCT);

		// BST sul BCT per trovare l'ordine delle componenti
		while (!queue.empty()) {
			TreeNode* u = queue.front();
			queue.pop();
			if (!u->isCutpoint) {
				otherOrderOfComponents.push_back(u->value);
			}
			for (int childType = 0; childType < 3; childType++) {
				for(TreeNode* child: u->getChildren(otherComponentRoot, childType)) {
					queue.push(child);
				}
			}
		}

		std::unordered_map<int, TreeNode*> otherRestrictingComponentOfCutpoint;
		getRestrictions(&componentsOfNode, &cutpointsOfComponent, &sourceAndSinkOfComponents, &treeNodeOfComponent, &otherOrderOfComponents, &otherRestrictingComponentOfCutpoint);

		//std::cout << "il parent cutpoint " << parentCutpoint << " è ristretto dalla componente " << otherRestrictingComponentOfCutpoint[parentCutpoint] << std::endl;

		BCTPermutationIterator iterator(otherRootOfBCT, otherComponentRoot, &otherRestrictingComponentOfCutpoint);

		std::vector<int> order;
		
		while (iterator.hasNext()) {
			order = iterator.next();

			Array<int, int> result = mergeLayouts(&topologicalOrders, &order, otherRootOfBCT, &sourceAndSinkOfComponents, &otherRestrictingComponentOfCutpoint);

			std::cout << "RISULTATO " << resultsCounter + 1 << ": ";
			for (int n: result) {
				std::cout << n << " ";
			}
			std::cout << std::endl;

			resultsCounter++;
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