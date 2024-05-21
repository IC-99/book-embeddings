// A C++ program to find biconnected components in a given undirected graph
#include <iostream>
#include <list>
#include <stack>
#include <vector>
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

// A class that represents an directed graph
class Graph {
	int V; // No. of vertices
	int E; // No. of edges
    int count;
	vector<vector<bool>> B;
	list<int>* adj; // A dynamic array of adjacency lists

	// A Recursive DFS based function used by BCC()
	void BCCUtil(int u, int disc[], int low[],
				list<Edge>* st, int parent[]);

public:
	Graph(int V); // Constructor
	void addEdge(int v, int w); // function to add an edge to graph
	void addUndirectedEdge(int v, int w);
	void BCC(); // prints strongly connected components
    int getCount();
	vector<vector<bool>> getComponents();
};

Graph::Graph(int V)
{
	this->V = V;
	this->E = 0;
    this->count = 0;
	this->B = {};
	adj = new list<int>[V];
}

void Graph::addEdge(int v, int w)
{
	adj[v].push_back(w);
	E++;
}

void Graph::addUndirectedEdge(int v, int w)
{
	addEdge(v, w);
	addEdge(w, v);
}

int Graph::getCount()
{
    return count;
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
void Graph::BCCUtil(int u, int disc[], int low[], list<Edge>* st,
					int parent[])
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
				count++;
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
	list<Edge>* st = new list<Edge>[E];

	// Initialize disc and low, and parent arrays
	for (int i = 0; i < V; i++) {
		disc[i] = NIL;
		low[i] = NIL;
		parent[i] = NIL;
	}

	vector<bool> row(V, false);

	for (int i = 0; i < V; i++) {
		if (disc[i] == NIL)
			BCCUtil(i, disc, low, st, parent);

		int j = 0;
		// If stack is not empty, pop all edges from stack
		if (!st->empty()) {
			while (st->size() > 0) {
				Edge edge = st->back();
				row[edge.u] = true;
				row[edge.v] = true;
				j = 1;
				cout << edge.u << "--" << edge.v << " ";
				st->pop_back();
			}
		}
		if (j == 1) {
			cout << endl;
			count++;
		}
	}
	if (j == 1){
		B.push_back(row);
	}
}

// Driver program to test above function
int main()
{
	Graph dag(12);
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

	Graph g(12);
	g.addUndirectedEdge(0, 1);
	g.addUndirectedEdge(0, 5);
	g.addUndirectedEdge(1, 2);
	g.addUndirectedEdge(3, 4);
	g.addUndirectedEdge(4, 1);
	g.addUndirectedEdge(4, 10);
	g.addUndirectedEdge(4, 11);
	g.addUndirectedEdge(5, 3);
	g.addUndirectedEdge(5, 4);
	g.addUndirectedEdge(6, 5);
	g.addUndirectedEdge(7, 5);
	g.addUndirectedEdge(7, 6);
	g.addUndirectedEdge(7, 8);
	g.addUndirectedEdge(8, 6);
	g.addUndirectedEdge(9, 1);
	g.addUndirectedEdge(9, 2);
	g.addUndirectedEdge(10, 11);

	g.BCC();
	cout << "Above are " << g.getCount() << " biconnected components in graph" << endl;

	vector<vector<bool>> B = g.getComponents();
	for (int i = 0; i < g.getCount(); i++) {
		cout << "component B_" << i << ": [";
		for (int j = 0; j < 12; j++) {
			cout << B[i][j] << " ";
		}
		cout << "]" << endl;
	}
	return 0;
}
