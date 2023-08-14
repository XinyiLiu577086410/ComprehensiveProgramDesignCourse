#!/bin/zsh
rm -rf ./executable
rm -rf ./output
mkdir executable
mkdir output
read
g++ ./src/main.cpp -o ./executable/dpll -std=c++11 -O0 -Wl,-stack_size -Wl,0x20000000 
g++ ./src/verify.cpp -o ./executable/verify -std=c++11
echo "\n\nSnapshot:"
for file in `ls  ./testset` 
    do
    echo "\nSolving ${file}" 
    ./executable/dpll "${file}"
    ./executable/verify "${file}" "${file%.*}.res"
    done
