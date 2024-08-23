#include <iostream>
#include <vector>
#include <map>
#include <queue>
#include <algorithm>
#include <unordered_map>

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
    bool sawAll = false;

    BCTPermutationIterator(TreeNode* rootNode, int treeIdentifier, std::unordered_map<int, TreeNode*>* restrictingComponentOfCutpoint) {
        root = rootNode;
        treeId = treeIdentifier;
        restrictions = restrictingComponentOfCutpoint;
        _initialize();
    }

    bool hasNext() {
        return false;
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
                    queue.push(child);
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
            sawAll = true;
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

int main() {
	std::cout << "INIZIO ESECUZIONE" << std::endl;
    // Supponiamo di avere un BCT rappresentato come una mappa

    int treeId = 7;
    
    TreeNode* b0 = new TreeNode(0, 7, false);
    TreeNode* c0 = new TreeNode(1, 1, true);
    TreeNode* b1 = new TreeNode(2, 6, false);
    TreeNode* c1 = new TreeNode(3, 3, true);
    TreeNode* b2 = new TreeNode(4, 1, false);
    TreeNode* b3 = new TreeNode(5, 3, false);
    TreeNode* b4 = new TreeNode(6, 5, false);
    TreeNode* c2 = new TreeNode(7, 4, true);
    TreeNode* c3 = new TreeNode(8, 5, true);
    TreeNode* c4 = new TreeNode(9, 6, true);
    TreeNode* b5 = new TreeNode(10, 0, false);
    TreeNode* b6 = new TreeNode(11, 2, false);
    TreeNode* b7 = new TreeNode(12, 4, false);

    TreeNode* b8 = new TreeNode(13, 8, false);
    c0->addChild(treeId, b8, 0);

    b0->addChild(treeId, c0, 1);
    c0->addChild(treeId, b1, 0);
    b1->addChild(treeId, c1, 2);
    c1->addChild(treeId, b2, 2);
    c1->addChild(treeId, b3, 2);
    c1->addChild(treeId, b4, 2);
    b2->addChild(treeId, c2, 0);
    b3->addChild(treeId, c3, 0);
    b4->addChild(treeId, c4, 1);
    c2->addChild(treeId, b5, 2);
    c3->addChild(treeId, b6, 0);
    c4->addChild(treeId, b7, 2);

    std::unordered_map<int, TreeNode*> restrictingComponentOfCutpoint;

    BCTPermutationIterator iterator(b0, treeId, &restrictingComponentOfCutpoint);

    std::vector<int> result;
    
    int i = 1;
    while (!iterator.sawAll) {
        result = iterator.next();

        std::cout << "ordine " << i << " trovato: ";
        for (int component: result) {
            std::cout << component << " ";
        }
        std::cout << std::endl;
        i++;
    }

    return 0;
}
