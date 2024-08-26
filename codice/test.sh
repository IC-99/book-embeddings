#!/bin/bash

# Controlla che un argomento sia passato allo script
if [ -z "$1" ]; then
    echo "Uso: $0 <file.cpp>"
    exit 1
fi

# Compila il file sorgente in un file oggetto
g++ -o test.o -c "$1" || { echo "Compilazione fallita"; exit 1; }

# Link del file oggetto in un eseguibile
g++ -o test_binary test.o || { echo "Link fallito"; exit 1; }

# Esegue il binario
./test_binary || { echo "Esecuzione fallita"; exit 1; }