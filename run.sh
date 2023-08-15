#!/bin/zsh

# 运行脚本，用于批量测试CNF算例

if test -e "./executable"
then
    rm -rf ./executable
fi

if test -e "./output" 
then
    rm -rf ./output
fi

mkdir executable
mkdir output

g++ ./src/main.cpp -o ./executable/dpll -std=c++11 -Ofast 
g++ ./src/verify.cpp -o ./executable/verify -std=c++11 -O1

echo "\n\nSnapshot:"
for file in `ls  ./testset` 
    do
    echo "\nSolving ${file}" 
    ./executable/dpll "${file}"
    ./executable/verify "${file}" "${file%.*}.res"
    done
