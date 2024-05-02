#include <stdio.h>
#include <time.h>
#include <stdlib.h>

int main() {
    printf("Hello, WebAssembly!\n");
    srand(time(NULL));   // Initialization, should only be called once.
    int r = rand();      // Returns a pseudo-random integer between 0 and RAND_MAX.
    printf("numero casuale selezionato: %d\n", r);
    return 0;
}