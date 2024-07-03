#! /bin/bash

g++ -I/home/ivan/OGDF/include -I/home/ivan/OGDF/src -o file.o -c $1
g++ -o binary -L/home/ivan/OGDF file.o -lOGDF -lCOIN -lgcov
./binary