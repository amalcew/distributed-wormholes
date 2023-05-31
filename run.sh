#!/bin/bash

mkdir -p out
cd ./src
make
cd ../
mv ./src/main ./out/main
mpirun --oversubscribe -np 12 ./out/main