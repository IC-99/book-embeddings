#include <stdio.h>
#include <time.h>
#include <stdlib.h>

int main() {
    printf("Hello, WebAssembly!\n");
    srand(time(NULL));
    int r = rand();
    printf("numero casuale selezionato: %d\n", r);
    return 0;
}