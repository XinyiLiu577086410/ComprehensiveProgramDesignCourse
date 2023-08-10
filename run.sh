#!/bin/zsh
mkdir executable
mkdir output

g++ ./src/main.cpp -o ./executable/dpll -std=c++11 -O3 
# g++ ./src/Verify.cpp -o ./executable/verify -std=c++11

for file in `ls ./testset` 
    do
    echo Solving ${file}
    ./executable/dpll "${file}"
    # ./executable/verify "./testset/${file}" >> ./executable/output.txt
    done
