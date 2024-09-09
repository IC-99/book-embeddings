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

void test(Graph* G) {
	//std::cout << G->firstEdge()->graphOf() << "   " << G << std::endl;
	//std::cout << G->firstEdge()->graphOf()->numberOfEdges() << "   " << G->numberOfEdges() << std::endl;
	//std::cout << G->firstEdge()->graphOf()->numberOfNodes() << "   " << G->numberOfNodes() << std::endl;
	//std::cout << G->firstEdge()->graphOf()->firstNode()->index() << "   " << G->firstNode()->index() << std::endl;
	//std::cout << G->firstEdge()->graphOf()->firstEdge()->target()->index() << "   " << G->firstEdge()->target()->index() << std::endl;
	//std::cout << std::endl;
}

void saveGraph(Graph* G, string fileName)
{
	GraphIO::write(*G, fileName + ".gml", GraphIO::writeGML);
}

void readGraph(Graph* G, string fileName)
{
	GraphIO::read(*G, fileName + ".gml", GraphIO::readGML);
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