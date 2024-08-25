#include <stdio.h>
#include <stdlib.h>

#define NODES 14
#define MAXNCHILDREN 3

int G[NODES][MAXNCHILDREN] = {
    {1, 2, -1},    // Nodi collegati al nodo 0
    {3, -1, -1},   // Nodi collegati al nodo 1
    {1, -1, -1},   // Nodi collegati al nodo 2
    {-1, -1, -1},  // Nodo 3 non ha uscite
    {3, -1, -1},   // Nodi collegati al nodo 4
    {3, 9, -1},    // Nodi collegati al nodo 5
    {3, -1, -1},   // Nodi collegati al nodo 6
    {3, 6, -1},    // Nodi collegati al nodo 7
    {4, -1, -1},   // Nodi collegati al nodo 8
    {-1, -1, -1},  // Nodo 9 non ha uscite
    {6, -1, -1},   // Nodi collegati al nodo 10
    {8, 13, -1},   // Nodi collegati al nodo 11
    {11, 13, -1},  // Nodi collegati al nodo 12
    {-1, -1, -1}   // Nodo 13 non ha uscite
};

// Funzione per calcolare il fattoriale
int factorial(int n) {
    if (n == 0 || n == 1) return 1;
    return n * factorial(n - 1);
}

// Funzione per scambiare due valori in un array
void swap(int *a, int *b) {
    int temp = *a;
    *a = *b;
    *b = temp;
}

// Funzione per invertire una porzione di array
void reverse(int *arr, int start, int end) {
    while (start < end) {
        swap(&arr[start], &arr[end]);
        start++;
        end--;
    }
}

// Funzione per generare la prossima permutazione
int next_permutation(int *arr, int n) {
    int i = n - 2;
    while (i >= 0 && arr[i] >= arr[i + 1]) i--;
    if (i < 0) return 0;
    
    int j = n - 1;
    while (arr[j] <= arr[i]) j--;
    swap(&arr[i], &arr[j]);
    reverse(arr, i + 1, n - 1);
    return 1;
}

// Funzione per verificare se un embedding è valido
int is_book_embedding(int *order, int n) {
    int index_map[NODES];
    for (int i = 0; i < n; i++) {
        index_map[order[i]] = i;
    }

    for (int u = 0; u < NODES; u++) {
        for (int v_idx = 0; v_idx < MAXNCHILDREN && G[u][v_idx] != -1; v_idx++) {
            int v = G[u][v_idx];
            if (index_map[u] >= index_map[v]) return 0;

            for (int u2 = u + 1; u2 < NODES; u2++) {
                for (int v2_idx = 0; v2_idx < MAXNCHILDREN && G[u2][v2_idx] != -1; v2_idx++) {
                    int v2 = G[u2][v2_idx];
                    if (index_map[u2] >= index_map[v2]) return 0;

                    if ((index_map[u] < index_map[u2] && index_map[u2] < index_map[v] && index_map[v] < index_map[v2]) ||
                        (index_map[u2] < index_map[u] && index_map[u] < index_map[v2] && index_map[v2] < index_map[v])) {
                        return 0;
                    }
                }
            }
        }
    }

    return 1;
}

int main() {
    int nodes[NODES] = {0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13};
    int n = NODES;
    int step = 0;
    int percentage = 0;
    int steps = factorial(n);
    int solutions = 0;

    do {
        if (step * 100 / steps > percentage) {
            percentage = step * 100 / steps;
            printf("%d%%\n", percentage);
        }
        if (is_book_embedding(nodes, n)) {
            printf("Found a solution: ");
            for (int i = 0; i < n; i++) {
                printf("%d ", nodes[i]);
            }
            printf("\n");
            solutions++;
        }
        step++;
    } while (next_permutation(nodes, n));

    printf("Found %d embeddings.\n", solutions);

    return 0;
}
