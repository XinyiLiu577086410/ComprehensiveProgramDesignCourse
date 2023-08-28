#!/bin/zsh

# 运行脚本，用于批量测试CNF算例

if test -e "./executable"
then
else
    mkdir executable
fi

if test -e "./dpllOutput" 
then
    rm -rf ./dpllOutput
fi

mkdir dpllOutput

g++ ./src/dpll.cpp -o ./executable/dpll -std=c++11 -Ofast -lpthread
g++ ./src/verify.cpp -o ./executable/verify -std=c++11 -O1

echo "\n\nrun.sh : 求解快照："
for file in `ls  ./testset` 
    do
    echo "\n\n\nrun.sh : 正在求解 ${file} : " 
    ./executable/dpll "./testset/${file}" "./dpllOutput/${file%.*}.res"
    ./executable/verify "./testset/${file}" "./dpllOutput/${file%.*}.res"
    done
