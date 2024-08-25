import itertools
from math import factorial

G = {0: [1, 2, 3], 1 : [2], 4: [3], 5: [3], 6: [2], 7: [2]}
nodes = [0, 1, 2, 3, 4, 5, 6, 7]

#graph
G1 = {0: [1, 5], 1: [2], 3: [4], 4: [1, 10, 11], 5: [3, 4], 6: [5], 7: [5, 6, 8], 8: [6], 9: [1, 2], 10: [11]}
nodes1 = [0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11]

#graph2
G2 = {0: [2, 4, 5], 1: [5], 2: [3, 4], 4: [5], 6: [4]}
nodes2 = [0, 1, 2, 3, 4, 5, 6]

#graph3
G3 = {0: [1, 2, 3, 4, 5]}
nodes3 = [0, 1, 2, 3, 4, 5]

#graph4
G4 = {0: [1, 2], 1: [2], 2: [4, 5], 3: [2, 5], 4: [5, 6, 7], 5: [8, 10], 9: [5, 10], 11: [5], 12: [5]}
nodes4 = [0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12]

#graph5
G5 = {0: [1, 2], 1: [2, 3], 4: [3], 5: [3, 9], 6: [3], 7: [3, 6], 8: [4], 10: [6]}
nodes5 = [0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10]

#graph6
G6 = {0: [2], 1: [0, 2, 3], 4: [3], 5: [3, 9], 6: [3], 7: [3, 6], 8: [4], 10: [6]}
nodes6 = [0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10]

#graph7
G7 = {0: [2], 1: [0, 2]}
nodes7 = [0, 1, 2]

#graph8
G8 = {0: [1, 2], 1: [3], 2: [1], 4: [3], 5: [3, 9], 6: [3], 7: [3, 6], 8: [4], 10: [6], 11: [8, 13], 12: [11, 13]}
nodes8 = [0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13]

#G = G4
#nodes = nodes4

def is_book_embedding(order):
    # Mappa i nodi all'indice del loro ordinamento
    index_map = {node: index for index, node in enumerate(order)}
    
    # Ottieni tutti gli archi del grafo
    edges = []
    for u in G:
        for v in G[u]:
            edges.append((u, v))
    
    # Controlla ogni coppia di archi
    for i in range(len(edges)):
        u1, v1 = edges[i]
        idx_u1, idx_v1 = index_map[u1], index_map[v1]
        
        # Verifica la direzionalità dell'arco (u1, v1)
        if idx_u1 >= idx_v1:
            return False
        
        for j in range(i + 1, len(edges)):
            u2, v2 = edges[j]
            idx_u2, idx_v2 = index_map[u2], index_map[v2]
            
            # Verifica la direzionalità dell'arco (u2, v2)
            if idx_u2 >= idx_v2:
                return False
            
            # Controlla se gli archi si incrociano
            if (idx_u1 < idx_u2 < idx_v1 < idx_v2) or (idx_u2 < idx_u1 < idx_v2 < idx_v1):
                return False
    
    return True

solutions = []
step = 0
percentage = 0
steps = factorial(len(nodes))
for order in itertools.permutations(nodes):
    if int(step / steps * 100) > percentage:
        percentage = int(step / steps * 100)
        print(f'{percentage}%')
    if is_book_embedding(list(order)):
        print(order)
        solutions.append(order)
    step += 1

print('trovati', len(solutions), 'embedding:')

for embedding in solutions:
    print(embedding)
    