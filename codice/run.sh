#!/bin/bash

# Controlla che un argomento sia passato allo script
if [ -z "$1" ]; then
    echo "Uso: $0 <file.cpp>"
    exit 1
fi

# Compila il file sorgente in un file oggetto
g++ -I/home/ivan/OGDF/include -I/home/ivan/OGDF/src -o file.o -c "$1" || { echo "Compilazione fallita"; exit 1; }

# Link del file oggetto in un eseguibile
g++ -o binary -L/home/ivan/OGDF file.o -lOGDF -lCOIN -lgcov || { echo "Link fallito"; exit 1; }

# Esegue il binario
./binary || { echo "Esecuzione fallita"; exit 1; }

echo "Script eseguito con successo"