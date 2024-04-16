# grafo leveled-planar
G = {1: [2, 6],
     2: [3],
     3: [],
     4: [5],
     5: [],
     6: [9, 5],
     7: [6, 8],
     8: [9],
     9: []}

# grafo non leveled-planar (secondo me)
G2 = {1: [2, 6],
      2: [3],
      3: [],
      4: [5],
      5: [],
      6: [9, 5],
      7: [6, 8, 4],
      8: [9],
      9: []}

def get_induced_subgraph(G, vertices):
    subgraph = {}
    for vertex in vertices:
        subgraph[vertex] = []
        for neighbor in G[vertex]:
            if neighbor in vertices:
                subgraph[vertex].append(neighbor)
    return subgraph

print(get_induced_subgraph(G, [7,6,9]))
print(len(G))

for j in range(1, len(G) + 1):
    print(f'V_{j}:', get_induced_subgraph(G, list(range(1, j + 1))))
