#!/bin/zsh
mkdir executable
mkdir output

g++ ./src/main.cpp -o ./executable/dpll -std=c++11 -O0 
g++ ./src/Verify.cpp -o ./executable/verify -std=c++11
echo Snapshot:
for file in `ls ./testset` 
    do
    echo "\nSolving ${file}" 
    # ./executable/dpll "./testset/${file}" 输入格式错误
    ./executable/dpll "${file}"
    ./executable/verify "./testset/${file}" "./output/${file%.*}.res"
    break
    done
