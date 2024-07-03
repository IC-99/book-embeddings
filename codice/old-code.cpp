#include <iostream>
#include <fstream>
#include <list>
#include <stack>
#include <queue>
#include <vector>
#include <memory>

#define NIL -1
using namespace std;

class Edge {
public:
	int u;
	int v;
	Edge(int u, int v);
};

Edge::Edge(int u, int v)
{
	this->u = u;
	this->v = v;
}

class TreeNode {
	int val;
	bool isComponent;
	vector<shared_ptr<TreeNode>> children;

public:
	TreeNode(int val, bool isComponent);
	int getVal();
	void addChild(shared_ptr<TreeNode> child);
	void printSubTree();
};

TreeNode::TreeNode(int val, bool isComponent)
{
	this->val = val;
	this->isComponent = isComponent;
	this->children = {};
}

int TreeNode::getVal()
{
	return val;
}

void TreeNode::addChild(shared_ptr<TreeNode> child)
{
	children.push_back(child);
}

void TreeNode::printSubTree()
{
	if (isComponent) {
		cout << "B_";
	}
	else {
		cout << "C_";
	}
	cout << val << " ha " << children.size() << " figli: ";
	for (shared_ptr<TreeNode> child : children) {

		cout << (child->isComponent ? "B_" : "C_") << child->val << " ";
	}
	cout << endl;
	for (shared_ptr<TreeNode> child : children) {
		child->printSubTree();
	}
}

// A class that represents an undirected graph
class Graph {
	int V; // No. of vertices
	int E; // No. of edges
	vector<int> cutpoints;
	vector<vector<bool>> B;
	vector<int> B_order;
	list<int>* adj; // A dynamic array of adjacency lists

	// A Recursive DFS based function used by BCC()
	void BCCUtil(int u, int disc[], int low[], list<Edge>* st, int parent[]);

public:
	Graph(int V);
	void addEdge(int u, int v);
	void BCC(); // initialize this->B and this->cutpoints
	void printStats();
	vector<vector<bool>> getComponents();
	shared_ptr<TreeNode> getBlockCutpointTree(int rootIndex);
	vector<int> getComponentsOrder();
};

Graph::Graph(int V)
{
	this->V = V;
	this->E = 0;
	this->B = {};
	this->B_order = {};
	adj = new list<int>[V];
}

void Graph::addEdge(int u, int v)
{
	adj[u].push_back(v);
	adj[v].push_back(u);
	E++;
}

void Graph::printStats()
{
	cout << "Above are " << B.size() << " biconnected components in graph" << endl;

	for (int i = 0; i < B.size(); i++) {
		cout << "component B_" << i << ": [";
		for (int j = 0; j < V; j++) {
			cout << B[i][j];
			if (j < V - 1) {
				cout << ", ";
			}
		}
		cout << "]" << endl;
	}

	cout << "cutpoints: [";
	for (int i = 0; i < cutpoints.size(); i++) {
		cout << cutpoints[i];
		if (i < cutpoints.size() - 1) {
			cout << ", ";
		}
	}
	cout << "]" << endl;
}

vector<vector<bool>> Graph::getComponents()
{
	return B;
}

// A recursive function that finds and prints strongly connected
// components using DFS traversal
// u --> The vertex to be visited next
// disc[] --> Stores discovery times of visited vertices
// low[] -- >> earliest visited vertex (the vertex with minimum
// discovery time) that can be reached from subtree
// rooted with current vertex
// *st -- >> To store visited edges
void Graph::BCCUtil(int u, int disc[], int low[], list<Edge>* st, int parent[])
{
	// A static variable is used for simplicity, we can avoid use
	// of static variable by passing a pointer.
	static int time = 0;

	// Initialize discovery time and low value
	disc[u] = low[u] = ++time;
	int children = 0;

	// Go through all vertices adjacent to this
	list<int>::iterator i;
	for (i = adj[u].begin(); i != adj[u].end(); ++i) {
		int v = *i; // v is current adjacent of 'u'

		// If v is not visited yet, then recur for it
		if (disc[v] == -1) {
			children++;
			parent[v] = u;
			// store the edge in stack
			st->push_back(Edge(u, v));
			BCCUtil(v, disc, low, st, parent);

			// Check if the subtree rooted with 'v' has a
			// connection to one of the ancestors of 'u'
			// Case 1 -- per Strongly Connected Components Article
			low[u] = min(low[u], low[v]);

			// If u is an articulation point, pop all edges from stack till u -- v
			if ((disc[u] == 1 && children > 1) || (disc[u] > 1 && low[v] >= disc[u])) {
				cout << "cutpoint: " << u << endl;
				cutpoints.push_back(u);
				vector<bool> row(V, false);
				while (st->back().u != u || st->back().v != v) {
					Edge edge = st->back();
					row[edge.u] = true;
					row[edge.v] = true;
					cout << st->back().u << "--" << st->back().v << " ";
					st->pop_back();
				}
				Edge edge = st->back();
				row[edge.u] = true;
				row[edge.v] = true;
				cout << st->back().u << "--" << st->back().v;
				st->pop_back();
				cout << endl;
				B.push_back(row);
			}
		}

		// Update low value of 'u' only of 'v' is still in stack
		// (i.e. it's a back edge, not cross edge).
		// Case 2 -- per Strongly Connected Components Article
		else if (v != parent[u]) {
			low[u] = min(low[u], disc[v]);
			if (disc[v] < disc[u]) {
				st->push_back(Edge(u, v));
			}
		}
	}
}

// The function to do DFS traversal. It uses BCCUtil()
void Graph::BCC()
{
	int* disc = new int[V];
	int* low = new int[V];
	int* parent = new int[V];
	list<Edge>* stack = new list<Edge>[E];

	// Initialize disc and low, and parent arrays
	for (int i = 0; i < V; i++) {
		disc[i] = NIL;
		low[i] = NIL;
		parent[i] = NIL;
	}

	vector<bool> row(V, false);
	
	for (int i = 0; i < V; i++) {
		if (disc[i] == NIL)
			BCCUtil(i, disc, low, stack, parent);

		int j = 0;
		// If stack is not empty, pop all edges from stack
		if (!stack->empty()) {
			while (stack->size() > 0) {
				Edge edge = stack->back();
				row[edge.u] = true;
				row[edge.v] = true;
				j = 1;
				cout << edge.u << "--" << edge.v << " ";
				stack->pop_back();
			}
		}
		if (j == 1) {
			cout << endl;
			B.push_back(row);
		}
	}
}

shared_ptr<TreeNode> Graph::getBlockCutpointTree(int rootIndex)
{
	vector<bool> visitedC(cutpoints.size(), false);
	vector<bool> visitedB(B.size(), false);
	B_order = {};

	auto rootNode = make_shared<TreeNode>(rootIndex, true);

	queue<shared_ptr<TreeNode>> queueB;
	queueB.push(rootNode);
	queue<shared_ptr<TreeNode>> queueC;

	while (!queueB.empty()) {
		auto currentBNode = queueB.front();
		queueB.pop();
		int currentBIndex = currentBNode->getVal();
		
		visitedB[currentBIndex] = true;
		B_order.push_back(currentBIndex);

		for (int i = 0; i < cutpoints.size(); i++) {
			if (!visitedC[i]) {
				if (B[currentBIndex][cutpoints[i]]) {
					auto child = make_shared<TreeNode>(i, false);
					currentBNode->addChild(child);
					queueC.push(child);
				}
			}
		}
		while (!queueC.empty()) {
			auto currentCNode = queueC.front();
			queueC.pop();
			int currentCIndex = currentCNode->getVal();

			visitedC[currentCIndex] = true;

			for (int i = 0; i < B.size(); i++) {
				if (!visitedB[i]) {
					if (B[i][cutpoints[currentCIndex]]) {
						auto child = make_shared<TreeNode>(i, true);
						currentCNode->addChild(child);
						queueB.push(child);
					}
				}
			}
		}
	}
	return rootNode;
}

vector<int> Graph::getComponentsOrder()
{
	return B_order;
}

class Dag {
	int V;
	int E;
	list<int>* adj;

	list<int> findHamiltonianPathHelper(vector<bool> component, int start, int target, list<int> path);

public:
	Dag(int V);
	void addEdge(int u, int v);
	Graph getGraph();
	void printDag();
	int findComponentSource(vector<bool> component);
	int findComponentSink(vector<bool> component);
	list<int> findHamiltonianPath(vector<bool> component);
};

Dag::Dag(int V)
{
	this->V = V;
	adj = new list<int>[V];
}

void Dag::addEdge(int u, int v)
{
	adj[u].push_back(v);
	E++;
}

void Dag::printDag()
{
	cout << "DAG:" << endl;
	for (int u = 0; u < V; ++u) {
        cout << u << ":";
        for (int adjVertex : adj[u]) {
            cout << " " << adjVertex;
        }
        cout << endl;
    }
	cout << endl;
}

Graph Dag::getGraph()
{
	Graph g = Graph(V);
	for (int u = 0; u < V; ++u) {
        for (int v : adj[u]) {
            g.addEdge(u, v);
        }
    }
	return g;
}

int Dag::findComponentSource(vector<bool> component)
{
	for (int u = 0; u < V; ++u) {
		if (component[u]) {
			for (int v: adj[u]) {
				component[v] = false;
			}
		}
	}
	for (int u = 0; u < V; ++u) {
		if (component[u]) {
			return u;
		}
	}
	return -1;
}

int Dag::findComponentSink(vector<bool> component)
{
	for (int u = 0; u < V; ++u) {
		bool isSink = true;
		if (component[u]) {
			for (int v: adj[u]) {
				if (component[v]) {
					isSink = false;
					break;
				}
			}
			if (isSink) {
				return u;
			}
		}
	}
	return -1;
}

list<int> Dag::findHamiltonianPathHelper(vector<bool> component, int start, int target, list<int> path)
{
	component[start] = false;
	if (start == target) {
		for (int i = 0; i < component.size(); i++) {
			if (component[i]) {
				return {};
			}
		}
		return {start};
	}
	for (int neighbor : adj[start]) {
		if (component[neighbor]) {
			list<int> newPath = path;
			list<int> result = findHamiltonianPathHelper(component, neighbor, target, path);
			if (!result.empty()) {
				result.push_front(start);
				return result;
			}
		}
	}
	return {};
}

list<int> Dag::findHamiltonianPath(vector<bool> component)
{
	int start = findComponentSource(component);
	int target = findComponentSink(component);
	list<int> path = findHamiltonianPathHelper(component, start, target, {});
	return path;
}

int main()
{
	Dag dag(12);
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

	dag.printDag();

	Graph g = dag.getGraph();

	g.BCC();

	g.printStats();

	cout << endl;

	auto T = g.getBlockCutpointTree(1);
	T->printSubTree();

	cout << endl;
	vector<vector<bool>> components = g.getComponents();
	vector<int> componentsOrder = g.getComponentsOrder();
	for (int i = 0; i < componentsOrder.size(); i++) {
		list<int> path = dag.findHamiltonianPath(components[componentsOrder[i]]);
		cout << "component B_" << i << " has hamiltonian path: ";
		for (int num : path) {
			cout << num << " ";
		}
		cout << endl;
	}
	
	return 0;
}