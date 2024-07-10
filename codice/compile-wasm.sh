#!/bin/bash

# Controlla che un argomento sia passato allo script
if [ -z "$1" ]; then
    echo "Uso: $0 <file.cpp>"
    exit 1
fi

# Compila il file sorgente in un file oggetto
em++ -O3 -I/home/ivan/OGDF/include -I/home/ivan/OGDF/src -o file.o -c "$1" || { echo "Compilazione fallita"; exit 1; }

# Link del file oggetto in un eseguibile
em++ -O3 -o binary.js -L/home/ivan/OGDFWA/build file.o -lOGDF -lCOIN || { echo "Link fallito"; exit 1; }