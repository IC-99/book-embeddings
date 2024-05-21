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
    AdjList* adjLists;
} Graph;

// Struttura per rappresentare uno stack
typedef struct Stack {
    int* array;
    int top;
    int capacity;
} Stack;

AdjListNode* newAdjListNode(int dest) {
    AdjListNode* newNode = (AdjListNode*)malloc(sizeof(AdjListNode));
    newNode->dest = dest;
    newNode->next = NULL;
    return newNode;
}

Graph* createGraph(int V) {
    Graph* graph = (Graph*)malloc(sizeof(Graph));
    graph->V = V;
    graph->adjLists = (AdjList*)malloc(V * sizeof(AdjList));

    for (int i = 0; i < V; ++i) {
        graph->adjLists[i].head = NULL;
    }
    
    return graph;
}

void addEdge(Graph* graph, int src, int dest) {
    // Aggiungi un arco da src a dest
    AdjListNode* newNode = newAdjListNode(dest);
    newNode->next = graph->adjLists[src].head;
    graph->adjLists[src].head = newNode;
}

void addUndirectedEdge(Graph* graph, int src, int dest) {
    addEdge(graph, src, dest);
    addEdge(graph, dest, src);
}

void printGraph(Graph* graph) {
    for (int v = 0; v < graph->V; ++v) {
        AdjListNode* listNode = graph->adjLists[v].head;
        printf("Lista di adiacenza del vertice %d: [", v);
        while (listNode) {
            printf("%d", listNode->dest);
            if (listNode->next)
                printf(", ");
            listNode = listNode->next;
        }
        printf("]\n");
    }
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

void findBCCUtil(Graph* graph, int u, int parent[], int low[], int disc[], Stack* stack) {
    static int currentTime = 0;
    
    int children = 0;

    disc[u] = currentTime;
    low[u] = currentTime;

    currentTime++;

    AdjListNode* neighbor = graph->adjLists[u].head;

    while (neighbor != NULL) {
        int v = neighbor->dest;
        if (disc[v] == -1) {
            children++;
            parent[v] = u;
            push(stack, u * graph->V + v); // strano
            findBCCUtil(graph, v, parent, low, disc, stack, currentTime);

            low[u] = (low[u] < low[v]) ? low[u] : low[v];

            if ((parent[u] == -1 && children > 1) || (parent[u] != -1 && low[v] >= disc[u])) {
                while (stack->array[stack->top] != u * graph->V + v) {
                    int edge = pop(stack);
                    printf("(%d -- %d) ", edge / graph->V, edge % graph->V);
                }
                int edge = pop(stack);
                printf("(%d -- %d)\n", edge / graph->V, edge % graph->V);
            }
        } else if (v != parent[u] && disc[v] < disc[u]) {
            low[u] = (low[u] < disc[v]) ? low[u] : disc[v];
            push(stack, u * graph->V + v);
        }
        pCrawl = pCrawl->next;
    }
}


void findBCC(Graph* graph) {
    int *disc = (int*)malloc(graph->V * sizeof(int));
    int *low = (int*)malloc(graph->V * sizeof(int));
    int *parent = (int*)malloc(graph->V * sizeof(int));

    Stack* stack = createStack(graph->V * graph->V);

    int count = 0;
    int currentTime;

    for (int i = 0; i < graph->V; i++) {
        disc[i] = -1;
        low[i] = -1;
        parent[i] = -1;
    }

    int currentTime = 0;

    for (int i = 0; i < graph->V; i++) {
        if (disc[i] == -1) {
            findBCCUtil(graph, i, parent, low, disc, stack, currentTime);
        }
        if (!isStackEmpty(stack)) {
            count++;
            while (!isStackEmpty(stack)) {
                int w = pop(stack);
                printf("%d ", w);
            }
            printf("\n");
        }
    }

    free(disc);
    free(low);
    free(parent);
    free(stack->array);
    free(stack);
}

int main() {
    int V = 12;
    Graph* dag = createGraph(V);
    Graph* graph = createGraph(V);

    addEdge(dag, 0, 1);
    addEdge(dag, 0, 5);
    addEdge(dag, 1, 2);
    addEdge(dag, 3, 4);
    addEdge(dag, 4, 1);
    addEdge(dag, 4, 10);
    addEdge(dag, 4, 11);
    addEdge(dag, 5, 3);
    addEdge(dag, 5, 4);
    addEdge(dag, 6, 5);
    addEdge(dag, 7, 5);
    addEdge(dag, 7, 6);
    addEdge(dag, 7, 8);
    addEdge(dag, 8, 6);
    addEdge(dag, 9, 1);
    addEdge(dag, 9, 2);
    addEdge(dag, 10, 11);

    addUndirectedEdge(graph, 0, 1);
    addUndirectedEdge(graph, 0, 5);
    addUndirectedEdge(graph, 1, 2);
    addUndirectedEdge(graph, 3, 4);
    addUndirectedEdge(graph, 4, 1);
    addUndirectedEdge(graph, 4, 10);
    addUndirectedEdge(graph, 4, 11);
    addUndirectedEdge(graph, 5, 3);
    addUndirectedEdge(graph, 5, 4);
    addUndirectedEdge(graph, 6, 5);
    addUndirectedEdge(graph, 7, 5);
    addUndirectedEdge(graph, 7, 6);
    addUndirectedEdge(graph, 7, 8);
    addUndirectedEdge(graph, 8, 6);
    addUndirectedEdge(graph, 9, 1);
    addUndirectedEdge(graph, 9, 2);
    addUndirectedEdge(graph, 10, 11);

    printGraph(dag);

    printf("Componenti biconnesse nel grafo dato:\n");

    findBCC(graph);

    return 0;
}