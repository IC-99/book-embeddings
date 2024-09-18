import subprocess
import random
import itertools
from math import factorial

def is_book_embedding(graph, order):
    # Mappa i nodi all'indice del loro ordinamento
    index_map = {node: index for index, node in enumerate(order)}
    
    # Ottieni tutti gli archi del grafo
    edges = []
    for u in graph:
        for v in graph[u]:
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

def brute_embedding(graph, nodes, stats = False):
    solutions = []
    step = 0
    percentage = 0
    steps = factorial(len(nodes))
    for order in itertools.permutations(nodes):
        if int(step / steps * 10) > percentage:
            percentage = int(step / steps * 10)
            if stats:
                print(f'{percentage}0%')
        if is_book_embedding(graph, list(order)):
            solutions.append(list(order))
        step += 1
    return solutions

def esegui_comando(comando):
    # Esegue il comando di shell e cattura l'output
    risultato = subprocess.run(comando, shell=True, capture_output=True, text=True)
    return risultato.stdout, risultato.stderr

def n_embeddings(stdout: str, graph_size: int):
    words = stdout.split(" ")
    read = -1000000000
    embeddings = []
    embedding = []
    for i in range(len(words)):
        if "\nRESULT" in words[i]:
            embedding = []
            read = 0
        if read > 0:
            embedding.append(int(words[i]))
            if read == graph_size:
                embeddings.append(embedding)
                read = -1000000000
        read += 1
    return embeddings

def random_graph(n):
    nodes = list(range(n))
    dag = {i: set() for i in range(n)}
    
    # Ordine casuale dei nodi per evitare cicli
    nodi = list(range(n))
    random.shuffle(nodi)
    
    # Crea un albero diretto per garantire che il DAG sia connesso
    for i in range(1, n):
        nodo_precedente = nodi[random.randint(0, i - 1)]
        nodo_attuale = nodi[i]
        
        # Aggiunge un arco diretto dal nodo precedente a quello attuale
        dag[nodo_precedente].add(nodo_attuale)
    
    # Aggiungi ulteriori archi casuali rispettando la direzione per mantenere il DAG aciclico
    for _ in range(random.randint(n, n * (n - 1) // 2)):
        nodo1 = random.randint(0, n-2)
        nodo2 = random.randint(nodo1 + 1, n - 1)
        if random.random() < 0.15:
            dag[nodi[nodo1]].add(nodi[nodo2])
    
    return dag, nodes

def graph_to_string(graph, nodes):
    graph_str = ''
    for node in nodes:
        graph_str += str(node) + '\n'
    for source_node in graph:
        for target_node in graph[source_node]:
            graph_str += str(source_node) + ',' + str(target_node) + '\n'
    return graph_str

def equals_embeddings(embs1, embs2):
    if len(embs1) != len(embs2):
        return False
    for emb2 in embs2:
        if not emb2 in embs1:
            return False
    return True

def main():
    count = 0
    n_test = 1000
    graph_size = 8
    for step in range(n_test):
        graph, nodes = random_graph(graph_size)
        comando = "./binary '" + graph_to_string(graph, nodes) + "'"
        stdout, stderr = esegui_comando(comando)

        res = n_embeddings(stdout, graph_size)
        brute_res = brute_embedding(graph, nodes)
        if equals_embeddings(res, brute_res):
            count += 1
            print("TEST SUPERATO", len(res), "==", len(brute_res))
        else:
            print("########TEST NON SUPERATO########", len(res), "!=", len(brute_res))
            print("con il grafo")
            print(graph)
    print("l'algoritmo ha superato con successo", count, "test su", n_test)

if __name__ == "__main__":
    main()