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
                std::cout << "nel cutpoint " << u->value << std::endl;

                if (restrictions->find(u->value) != restrictions->end()) {
                    queue.push((*restrictions)[u->value]);
                }

                std::vector<TreeNode*> sourceChildren = u->getChildren(treeId, 0);

                if (permutations.find(u->value) != permutations.end()) {
                    if (permutations[u->value].find(0) != permutations[0].end()) {
                        std::cout << "nel cutpoint " << u->value << " ci sono delle permutazioni di sorgente" << std::endl;
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
                
                if (permutations.find(u->value) != permutations.end()) {
                    if (permutations[u->value].find(2) != permutations[2].end()) {
                        std::cout << "nel cutpoint " << u->value << " ci sono delle permutazioni di pozzo" << std::endl;
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
                std::cout << "nella componente " << u->value << std::endl;
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

void printBCT(TreeNode* treeNode, int treeId) {
	if (treeNode->children.size() == 0) {
		return;
	}
	if (treeNode->isCutpoint) {
		std::cout << "figli del cutpoint " << treeNode->value << ":" << std::endl;
		for (int childType = 0; childType < 3; childType++) {
			std::cout << "  " << childType << ": " ;
			for(TreeNode* componentNode: treeNode->getChildren(treeId, childType)) {
				std::cout << componentNode->value << " ";
			}
			std::cout << std::endl;
		}
		for (int childrenType = 0; childrenType < 3; childrenType++) {
			for(TreeNode* componentNode: treeNode->getChildren(treeId, childrenType)) {
				printBCT(componentNode, treeId);	
			}
		}
	}
	else {
		std::cout << "figli della componente " << treeNode->value << ":" << std::endl;
		for (int childType = 0; childType < 3; childType++) {
			std::cout << "  " << childType << ": " ;
			for(TreeNode* cutpointNode: treeNode->getChildren(treeId, childType)) {
				std::cout << cutpointNode->value << " ";
			}
			std::cout << std::endl;	
		}
		for (int childType = 0; childType < 3; childType++) {
			for(TreeNode* cutpointNode: treeNode->getChildren(treeId, childType)) {
				printBCT(cutpointNode, treeId);
			}
		}
	}
	return;
}

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

/*
// parentNode è il padre di treeNode nel radicamento che vogliamo osservare
bool isRestricted(TreeNode* treeNode, TreeNode* parentNode, Array<Array<int, int>, int>* sourceAndSinkOfComponents) {
	bool result = false;
	if (treeNode->isCutpoint) {
		for(TreeNode* componentNode: treeNode->neighbors) {
			if (componentNode != parentNode) {
				if ((*sourceAndSinkOfComponents)[componentNode->value][0] != treeNode->value && (*sourceAndSinkOfComponents)[componentNode->value][1] != treeNode->value) {
					return true;
				}
				result = result || isRestricted(componentNode, treeNode, sourceAndSinkOfComponents);
			}
		}
	}
	else {
		for(TreeNode* cutpointNode: treeNode->neighbors) {
			if (cutpointNode != parentNode) {
				result = result || isRestricted(cutpointNode, treeNode, sourceAndSinkOfComponents);
			}
		}
	}
	return result;
}
*/

void findOtherRoots(TreeNode* treeNode, int parentType, int treeId, std::vector<std::vector<int>>* componentRoots) {
	if (treeNode->isCutpoint) {
		std::cout << "-----------" << std::endl;
		std::cout << "cutpoint: " << treeNode->value << ", parentType: " << parentType << std::endl;
		for (int childType = 0; childType < 3; childType++) {
			for(TreeNode* componentNode: treeNode->getChildren(treeId, childType)) {
				std::cout << "figlio: " << componentNode->value << ", tipo: " << childType << ", parentType: " << parentType << std::endl;
				if (childType == parentType) {
					std::vector<int> newRoot{{componentNode->value, treeNode->value}};
					componentRoots->push_back(newRoot);
				}
			}
			for(TreeNode* componentNode: treeNode->getChildren(treeId, childType)) {
				findOtherRoots(componentNode, childType, treeId, componentRoots);
			}
		}
	}
	else {
		std::cout << "-----------" << std::endl;
		std::cout << "componente: " << treeNode->value << ", parentType: " << parentType << std::endl;
		for (int childType = 0; childType < 3; childType++) {
			for(TreeNode* cutpointNode: treeNode->getChildren(treeId, childType)) {
				std::cout << "figlio: " << cutpointNode->value << ", tipo: " << childType << ", parentType: " << parentType << std::endl;
			}
			for(TreeNode* cutpointNode: treeNode->getChildren(treeId, childType)) {
				findOtherRoots(cutpointNode, childType, treeId, componentRoots);
			}
		}
	}
}

void permute(std::vector<TreeNode*> elements, int start, std::vector<std::vector<TreeNode*>>* result) {
    if (start == elements.size() - 1) {
        result->push_back(elements);
        return;
    }

    for (int i = start; i < elements.size(); ++i) {
        std::swap(elements[start], elements[i]);
        permute(elements, start + 1, result);
        std::swap(elements[start], elements[i]);
    }
}

std::vector<std::vector<int>> findPermutationsOfChildren(TreeNode* treeNode, int treeId) {
    std::vector<std::queue<TreeNode*>> queues(1);
	std::vector<std::vector<int>> orders(1);

	queues[0].push(treeNode);

    while (!queues[0].empty()) {
		for (int i = 0; i < queues.size(); i++) {
			TreeNode* u = queues[i].front();
			queues[i].pop();

			if (!u->isCutpoint) {
				orders[i].push_back(u->value);
				for (int childType = 0; childType < 3; childType++) {
					for(TreeNode* child: treeNode->getChildren(treeId, childType)) {
						queues[i].push(child);
					}
				}
			}
			else {

				std::vector<TreeNode*> sourceChildren = u->getChildren(treeId, 0);
				std::vector<TreeNode*> sinkChildren = u->getChildren(treeId, 2);

				std::vector<std::queue<TreeNode*>> newQueues;

				if (sourceChildren.size() > 1) {
					std::vector<std::vector<TreeNode*>> permutations;
					permute(u->getChildren(treeId, 0), 0, &permutations);
					for (std::vector<TreeNode*> permutation: permutations){
						std::queue<TreeNode*> newQueue = queues[i];
						for (TreeNode* child: permutation) {
							newQueue.push(child);
						}
						newQueues.push_back(newQueue);
					}
				} else {
					newQueues.push_back(queues[i]);
					for (TreeNode* child: u->getChildren(treeId, 0)) {
						for (std::queue<TreeNode*> queue: newQueues) {
							queue.push(child);
						}
					}
				}

				for (TreeNode* child: u->getChildren(treeId, 1)) {
					for (std::queue<TreeNode*> queue: newQueues) {
						queue.push(child);
					}
				}

				std::vector<std::queue<TreeNode*>> newNewQueues;

				if (sinkChildren.size() > 1) {
					std::vector<std::vector<TreeNode*>> permutations;
					permute(u->getChildren(treeId, 2), 0, &permutations);
					for (std::queue<TreeNode*> queue: newQueues) {
						for (std::vector<TreeNode*> permutation: permutations){
							std::queue<TreeNode*> newQueue = queue;
							for (TreeNode* child: permutation) {
								queue.push(child);
							}
							newNewQueues.push_back(newQueue);
						}
					}
				} else {
					for (TreeNode* child: u->getChildren(treeId, 0)) {
						for (std::queue<TreeNode*> queue: newQueues) {
							queue.push(child);
						}
					}
					newNewQueues = newQueues;
				}

				queues = newNewQueues;
			}
		}
    }
	return orders;
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

Array<Graph, int> getBiconnectedComponents(Graph* G) {
	EdgeArray<int> edgeArray = EdgeArray<int>(*G);

	int numberOfBiconnectedComponents = biconnectedComponents(*G, edgeArray);
	std::cout << "il grafo ha " << numberOfBiconnectedComponents << " componenti biconnesse" << std::endl;

	Array<Graph, int> biconnectedComponentsGraphs(numberOfBiconnectedComponents);
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

Array<int, int> mergeLayouts(Array<Array<int, int>, int>* topologicalOrders, std::vector<int>* orderOfComponents, TreeNode* rootNode, int componentRoot, Array<Array<int, int>, int>* sourceAndSinkOfComponents) {
	Array<int, int> currentLayout = (*topologicalOrders)[(*orderOfComponents)[0]];

	// fusione dei layout delle componenti biconnesse
	for (int i = 1; i < orderOfComponents->size(); i++) {
		int component = (*orderOfComponents)[i];

		//std::cout << "layout attuale: ";
		//printArray(currentLayout);

		Array<int, int> orderToAdd = (*topologicalOrders)[component];
		int cutpoint = getParentCutpoint(rootNode, component, componentRoot);

		//std::cout << "la componente " << component << " è collegata al grafo G_" << i << " tramite il cutpoint " << cutpoint << std::endl;

		int newLayoutSize = currentLayout.size() + orderToAdd.size() - 1;
		Array<int, int> newLayout(newLayoutSize);

		if (cutpoint == (*sourceAndSinkOfComponents)[component][0]) {
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
		else if (cutpoint == (*sourceAndSinkOfComponents)[component][1]){
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
	return currentLayout;
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
	Array<Graph, int> biconnectedComponentsGraphs = getBiconnectedComponents(G);
	int numberOfBiconnectedComponents = biconnectedComponentsGraphs.size();

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

	// nodeOfComponent è una mappa che restituisce il TreeNode dal valore del TreeNode
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
	int componentRoot = 0;
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
				int childrenType = -1;
				if (neighbor->isCutpoint) {
					if (sourceAndSinkOfComponents[u->value][0] == neighbor->value) {
						childrenType = 0; // sorgente
					}
					else if (sourceAndSinkOfComponents[u->value][1] == neighbor->value) {
						childrenType = 2; // pozzo
					}
					else {
						childrenType = 1; // intermedio
					}
				}
				else {
					if (sourceAndSinkOfComponents[neighbor->value][0] == u->value) {
						childrenType = 0;
					}
					else if (sourceAndSinkOfComponents[neighbor->value][1] == u->value) {
						childrenType = 2;
					}
					else {
						childrenType = 1;
					}
				}
				u->addChild(componentRoot, neighbor, childrenType);
            }
        }
    }

	std::cout << "ordine delle componenti: ";
	printArray(orderOfComponents);

	// ricerca dei cutpoint ristretti
	std::unordered_map<int, bool> isCutpointRestricted;
	std::unordered_map<int, int> cutpointRestrictedInComponent;
	std::unordered_map<int, TreeNode*> restrictingComponentOfCutpoint;

	for(int i = numberOfBiconnectedComponents - 1; i >= 0; i--) {
		int component = orderOfComponents[i];
		int countRestricted = 0;
		cutpointRestrictedInComponent[component] = -1;

		for(int cutpoint: cutpointsOfComponent[component]) {
			std::cout << "analizzando cutpoint " << cutpoint << " nella componente " << component << std::endl;
			for(int j = numberOfBiconnectedComponents - 1; j > i; j--) {
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
						restrictingComponentOfCutpoint[cutpoint] = nodeOfComponent[otherComponent];
						std::cout << "il cutpoint " << cutpoint << " è ristretto, trovato con componente " << otherComponent << std::endl;
						break;
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

	std::cout << "CAUSE RESTRIZIONI:" << std::endl;
	for (int cp: cutpoints) {
		if (restrictingComponentOfCutpoint.find(cp) != restrictingComponentOfCutpoint.end()) {
			std::cout << "il cutpoint " << cp << " è ristretto a causa della componente " << restrictingComponentOfCutpoint[cp]->value << std::endl;
		}
	}

	printBCT(rootOfBCT, componentRoot);

	// contiene una lista di coppie [componente su cui si può radicare, cutpoint causa di tale scelta] se questo cutpoint poi risulta essere ristretto allora l'albero deve essere ignorato
	std::vector<std::vector<int>> componentRoots;
	findOtherRoots(rootOfBCT, -1, componentRoot, &componentRoots);

	std::cout << "POSSIBILI RADICI (oltre alla radice attuale = " << componentRoot << "): ";
	for (std::vector<int> root: componentRoots) {
		std::cout << "[" << root[0] << ", " << root[1] << "] ";
	}
	std::cout << std::endl;

	// radicazione degli alberi sulle possibili radici
	for (int i = 0; i < componentRoots.size(); i++) {
		int treeId = componentRoots[i][0];
		TreeNode* rootOfBCT = nodeOfComponent[treeId];

		std::unordered_map<int, bool> visited;
		std::queue<TreeNode*> queue;

		visited[rootOfBCT->id] = true;
		queue.push(rootOfBCT);

		// BST sul BCT non radicato per radicarlo
		while (!queue.empty()) {
			TreeNode* u = queue.front();
			queue.pop();

			// esplora tutti i nodi adiacenti e salva i figli del nodo (grado di libertà sull'ordine)
			for (TreeNode* neighbor : u->neighbors) {
				if (!visited[neighbor->id]) {
					visited[neighbor->id] = true;
					queue.push(neighbor);
					int childrenType = -1;
					if (neighbor->isCutpoint) {
						if (sourceAndSinkOfComponents[u->value][0] == neighbor->value) {
							childrenType = 0; // sorgente
						}
						else if (sourceAndSinkOfComponents[u->value][1] == neighbor->value) {
							childrenType = 2; // pozzo
						}
						else {
							childrenType = 1; // intermedio
						}
					}
					else {
						if (sourceAndSinkOfComponents[neighbor->value][0] == u->value) {
							childrenType = 0;
						}
						else if (sourceAndSinkOfComponents[neighbor->value][1] == u->value) {
							childrenType = 2;
						}
						else {
							childrenType = 1;
						}
					}
					u->addChild(treeId, neighbor, childrenType);
				}
			}
		}
	}

	std::cout << "################################### BCT RADICATO NELLA COMPONENTE " << componentRoot << std::endl;

	BCTPermutationIterator iterator(rootOfBCT, componentRoot, &restrictingComponentOfCutpoint);

    std::vector<int> order;
    
    int step = 1;
    while (iterator.hasNext()) {
        order = iterator.next();

        std::cout << "ordine " << step << " trovato: ";
        for (int component: order) {
            std::cout << component << " ";
        }
        std::cout << std::endl;

		Array<int, int> result = mergeLayouts(&topologicalOrders, &order, rootOfBCT, componentRoot, &sourceAndSinkOfComponents);

		std::cout << "RISULTATO " << step << ": ";
		for (int n: result) {
			std::cout << n << " ";
		}
		std::cout << std::endl;

		step++;
    }

	for (std::vector<int> rooting: componentRoots){
		componentRoot = rooting[0];
		int parentCutpoint = rooting[1];
		rootOfBCT = nodeOfComponent[componentRoot];

		bool hasNewResults = true;

		std::cout << "################################### BCT RADICATO NELLA COMPONENTE " << componentRoot << std::endl;

		std::unordered_map<int, bool> visited;
		std::queue<TreeNode*> queue;

		// ordine delle componenti
		Array<int, int> orderOfComponents(numberOfBiconnectedComponents);
		int i = 0;

		visited[rootOfBCT->id] = true;
		queue.push(rootOfBCT);

		// BST sul BCT non radicato per trovare l'ordine delle componenti
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
				}
			}
		}

		std::cout << "ordine delle componenti: ";
		printArray(orderOfComponents);

		// ricerca dei cutpoint ristretti
		std::unordered_map<int, bool> isCutpointRestricted;
		std::unordered_map<int, int> cutpointRestrictedInComponent;
		std::unordered_map<int, TreeNode*> restrictingComponentOfCutpoint;

		for(int i = numberOfBiconnectedComponents - 1; i >= 0; i--) {
			int component = orderOfComponents[i];
			int countRestricted = 0;
			cutpointRestrictedInComponent[component] = -1;

			for(int cutpoint: cutpointsOfComponent[component]) {
				std::cout << "analizzando cutpoint " << cutpoint << " nella componente " << component << std::endl;
				for(int j = numberOfBiconnectedComponents - 1; j > i; j--) {
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
							restrictingComponentOfCutpoint[cutpoint] = nodeOfComponent[otherComponent];
							std::cout << "il cutpoint " << cutpoint << " è ristretto, trovato con componente " << otherComponent << std::endl;
							if (cutpoint == parentCutpoint) {
								hasNewResults = false;
							}
							break;
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
		if (hasNewResults) {
			BCTPermutationIterator iterator(rootOfBCT, componentRoot, &restrictingComponentOfCutpoint);

			std::vector<int> order;
			
			while (iterator.hasNext()) {
				order = iterator.next();

				std::cout << "ordine " << step << " trovato: ";
				for (int component: order) {
					std::cout << component << " ";
				}
				std::cout << std::endl;

				Array<int, int> result = mergeLayouts(&topologicalOrders, &order, rootOfBCT, componentRoot, &sourceAndSinkOfComponents);

				std::cout << "RISULTATO " << step << ": ";
				for (int n: result) {
					std::cout << n << " ";
				}
				std::cout << std::endl;

				step++;
			}
		}
		else {
			std::cout << "IL RADICAMENTO IN " << componentRoot << " NON RESTITUISCE NUOVI RISULTATI" << std::endl;
		}
	}


	std::cout << "TERMINATO" << std::endl;

}

void test(Graph* G) {
	//std::cout << G->firstEdge()->graphOf() << "   " << G << std::endl;
	//std::cout << G->firstEdge()->graphOf()->numberOfEdges() << "   " << G->numberOfEdges() << std::endl;
	//std::cout << G->firstEdge()->graphOf()->numberOfNodes() << "   " << G->numberOfNodes() << std::endl;
	//std::cout << G->firstEdge()->graphOf()->firstNode()->index() << "   " << G->firstNode()->index() << std::endl;
	//std::cout << G->firstEdge()->graphOf()->firstEdge()->target()->index() << "   " << G->firstEdge()->target()->index() << std::endl;
	//std::cout << std::endl;
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

	draw(&G, "Graph.svg");
	//saveGraph(&G, "graph")

	return 0;
}