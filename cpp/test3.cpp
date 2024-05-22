#include <iostream>
#include <vector>
#include <stack>
#include <algorithm>

// Struttura per rappresentare un grafo
class Graph {
public:
    int V;
    std::vector<std::vector<int>> adj;

    Graph(int V) {
        this->V = V;
        adj.resize(V);
    }

    void addEdge(int v, int w) {
        adj[v].push_back(w);
    }

    int findSource();
	int findSink();
};

int Graph::findSource()
{
	for
}


int main() {
    Graph g(5);
    g.addEdge(0, 1);
    g.addEdge(0, 2);
    g.addEdge(2, 3);
    g.addEdge(2, 4);
    g.addEdge(3, 4);
    g.addEdge(4, 1);

    g.findOuterFace();

    return 0;
}
