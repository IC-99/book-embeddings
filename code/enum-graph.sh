#!/bin/bash

# Controlla che un argomento sia passato allo script
if [ -z "$1" ]; then
    echo "Uso: $0 <file.cpp>"
    exit 1
fi

# Compila il file sorgente in un file oggetto
g++ -I/home/ivan/OGDF/include -I/home/ivan/OGDF/src -o file1.o -c main_enum.cpp || { echo "Compilazione fallita"; exit 1; }

# Link del file oggetto in un eseguibile
g++ -o binary -L/home/ivan/OGDF file1.o -lOGDF -lCOIN -lgcov || { echo "Link fallito"; exit 1; }

# Esegue il binario
./binary "$(cat $1)" || { echo "Esecuzione fallita"; exit 1; }