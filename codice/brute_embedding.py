import itertools
from math import factorial

#G = {0: [1, 2 , 3], 1 : [2], 4: [3], 5: [3], 6: [2], 7: [2]}
#nodes = [0, 1, 2, 3, 4, 5, 6, 7]

#G = {0: [1, 2], 1: [2], 2: [4, 5], 3: [2, 5], 4: [5, 6, 7], 5: [8, 10], 9: [5, 10], 11: [5], 12: [5]}
#nodes = [0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12]

G = {0: [1, 2], 1: [2, 3], 4: [3], 5: [3, 9], 6: [3], 7: [3, 6], 8: [4], 10: [6]}
nodes = [0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10]

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
    