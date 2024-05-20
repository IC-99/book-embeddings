#include <stdio.h>
#include <stdlib.h>

#define MAX_V 1000

// Struttura per rappresentare un nodo nella lista di adiacenza
typedef struct AdjListNode {
    int dest;
    struct AdjListNode* next;
} AdjListNode;

// Struttura per rappresentare una lista di adiacenza
typedef struct AdjList {
    AdjListNode* head;
} AdjList;

// Struttura per rappresentare un grafo
typedef struct Graph {
    int V;
    AdjList* array;
} Graph;

// Struttura per rappresentare uno stack
typedef struct Stack {
    int* array;
    int top;
    int capacity;
} Stack;

typedef struct BiConnectedComponent {
    int* edges;
    int size;
} BiConnectedComponent;

AdjListNode* newAdjListNode(int dest) {
    AdjListNode* newNode = (AdjListNode*)malloc(sizeof(AdjListNode));
    newNode->dest = dest;
    newNode->next = NULL;
    return newNode;
}

Graph* createGraph(int V) {
    Graph* graph = (Graph*)malloc(sizeof(Graph));
    graph->V = V;
    graph->array = (AdjList*)malloc(V * sizeof(AdjList));

    for (int i = 0; i < V; ++i) {
        graph->array[i].head = NULL;
    }
    
    return graph;
}

void addEdge(Graph* graph, int src, int dest) {
    AdjListNode* newNode = newAdjListNode(dest);
    newNode->next = graph->array[src].head;
    graph->array[src].head = newNode;
}

void addUndirectedEdge(Graph* graph, int src, int dest) {
    addEdge(graph, src, dest);
    addEdge(graph, dest, src);
}

Stack* createStack(int capacity) {
    Stack* stack = (Stack*)malloc(sizeof(Stack));
    stack->capacity = capacity;
    stack->top = -1;
    stack->array = (int*)malloc(stack->capacity * sizeof(int));
    return stack;
}

int isStackEmpty(Stack* stack) {
    return stack->top == -1;
}

void push(Stack* stack, int item) {
    stack->array[++stack->top] = item;
}

int pop(Stack* stack) {
    if (isStackEmpty(stack))
        return -1;
    return stack->array[stack->top--];
}

void findBCCUtil(Graph* graph, int u, int disc[], int low[], Stack* stack, int parent[], BiConnectedComponent* bcc, int* currentTime) {
    disc[u] = low[u] = ++(*currentTime);
    int children = 0;
    AdjListNode* pCrawl = graph->array[u].head;

    while (pCrawl != NULL) {
        int v = pCrawl->dest;
        if (disc[v] == -1) {
            children++;
            parent[v] = u;
            push(stack, u * graph->V + v);
            findBCCUtil(graph, v, disc, low, stack, parent, bcc, currentTime);

            low[u] = (low[u] < low[v]) ? low[u] : low[v];

            if ((parent[u] == -1 && children > 1) || (parent[u] != -1 && low[v] >= disc[u])) {
                BiConnectedComponent component;
                component.edges = (int*)malloc(stack->capacity * sizeof(int));
                component.size = 0;

                while (1) {
                    int edge = pop(stack);
                    component.edges[component.size++] = edge;
                    if (edge == u * graph->V + v) break;
                }
                
                bcc[(*currentTime) - 1] = component;
            }
        } else if (v != parent[u] && disc[v] < disc[u]) {
            low[u] = (low[u] < disc[v]) ? low[u] : disc[v];
            push(stack, u * graph->V + v);
        }

        pCrawl = pCrawl->next;
    }
}

BiConnectedComponent* findBCC(Graph* graph, int* numBCC) {
    if (graph == NULL || graph->V <= 0) {
        printf("Grafo non valido.\n");
        return NULL;
    }

    int *disc = (int*)malloc(graph->V * sizeof(int));
    int *low = (int*)malloc(graph->V * sizeof(int));
    int *parent = (int*)malloc(graph->V * sizeof(int));

    if (disc == NULL || low == NULL || parent == NULL) {
        printf("Errore di allocazione di memoria.\n");
        return NULL;
    }

    Stack* stack = createStack(graph->V * graph->V);
    if (stack == NULL) {
        printf("Errore di allocazione di memoria per lo stack.\n");
        free(disc);
        free(low);
        free(parent);
        return NULL;
    }

    *numBCC = 0;
    BiConnectedComponent* bcc = (BiConnectedComponent*)malloc(graph->V * sizeof(BiConnectedComponent));

    if (bcc == NULL) {
        printf("Errore di allocazione di memoria per le componenti biconnesse.\n");
        free(disc);
        free(low);
        free(parent);
        free(stack->array);
        free(stack);
        return NULL;
    }

    for (int i = 0; i < graph->V; i++) {
        disc[i] = -1;
        low[i] = -1;
        parent[i] = -1;
    }

    int currentTime = 0;

    for (int i = 0; i < graph->V; i++) {
        if (disc[i] == -1) {
            findBCCUtil(graph, i, disc, low, stack, parent, bcc, &currentTime);
            (*numBCC)++;
        }
    }

    free(disc);
    free(low);
    free(parent);
    free(stack->array);
    free(stack);

    return bcc;
}

int main() {
    Graph* graph = createGraph(5);
    addUndirectedEdge(graph, 0, 1);
    addUndirectedEdge(graph, 0, 2);
    addUndirectedEdge(graph, 1, 2);
    addUndirectedEdge(graph, 1, 3);
    addUndirectedEdge(graph, 3, 4);

    int numBCC = 0;
    BiConnectedComponent* bcc = findBCC(graph, &numBCC);

    printf("Numero di componenti biconnesse: %d\n", numBCC);

    for (int i = 0; i < numBCC; i++) {
        printf("Componente biconnessa %d: [", i+1);
        for (int j = 0; j < bcc[i].size; j++) {
            printf("(%d -- %d)", bcc[i].nodes[j].u, bcc[i].nodes[j].v);
            if (j < bcc[i].size - 1) {
                printf(", ");
            }
        }
        printf("]\n");
    }

    // Libera la memoria delle componenti biconnesse
    for (int i = 0; i < numBCC; i++) {
        free(bcc[i].nodes);
    }
    free(bcc);

    // Libera la memoria del grafo
    for (int v = 0; v < graph->V; v++) {
        AdjListNode* curr = graph->array[v].head;
        while (curr != NULL) {
            AdjListNode* temp = curr;
            curr = curr->next;
            free(temp);
        }
    }
    free(graph->array);
    free(graph);

    return 0;
}