#include <iostream>
#include <vector>
#include <map>
#include <set>

using namespace std;

vector<pair<int, int>> find_outerface_edges(const map<int, vector<int>> &adj_list, int start_node) {
    vector<pair<int, int>> outerface_edges;
    set<pair<int, int>> visited_edges;
    int current_node = start_node;

    while (true) {
        bool found_edge = false;

        for (int neighbor : adj_list.at(current_node)) {
            pair<int, int> edge = {current_node, neighbor};

            // Continue if the edge has already been visited
            if (visited_edges.find(edge) != visited_edges.end()) {
                continue;
            }

            // Add edge to outerface and mark it visited
            visited_edges.insert(edge);
            outerface_edges.push_back(edge);

            // Move to the next node in the outerface
            current_node = neighbor;
            found_edge = true;
            break;
        }

        // If no unvisited edge is found, break the loop
        if (!found_edge) {
            break;
        }

        // If we return to the start node, we've completed the outerface cycle
        if (current_node == start_node) {
            break;
        }
    }

    return outerface_edges;
}

int main() {
    // Example directed outerplanar graph represented as an adjacency list
    /*
    map<int, vector<int>> adj_list = {
        {0, {}},
        {1, {4, 0}},
        {3, {1, 4}},
        {4, {0}}
    };*/

    map<int, vector<int>> adj_list = {
        {0, {1, 5}},
        {5, {3, 4}},
        {3, {4}},
        {4, {1}},
        {1, {}}
    };

    int start_node = 0; // Start from node 3, which is part of the outerface

    vector<pair<int, int>> outerface_edges = find_outerface_edges(adj_list, start_node);

    cout << "Outerface edges: ";
    for (const auto &edge : outerface_edges) {
        cout << "(" << edge.first << ", " << edge.second << ") ";
    }
    cout << endl;

    return 0;
}
