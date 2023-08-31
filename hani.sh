#!/bin/zsh

# 测试hanidoku求解功能
rm ./hanidoku/*
g++ ./src/haniCnfGen.cpp -o ./executable/haniCnfGen -std=c++11 -O3
g++ ./src/dpll.cpp -o ./executable/dpll -std=c++11 -O3 -lpthread
g++ ./src/haniDisplay.cpp -o ./executable/haniDisplay -std=c++11 -O3
./executable/haniCnfGen ./hanidoku/hanidoku.cnf HQV1G0000000000000000000000000000000000000000000000020530000403500
echo "\nhani.sh : 正在求解 ./hanidoku/hanidoku.cnf : " 
./executable/dpll ./hanidoku/hanidoku.cnf ./hanidoku/hanidoku.res "80"
./executable/verify ./hanidoku/hanidoku.cnf ./hanidoku/hanidoku.res
./executable/haniDisplay ./hanidoku/hanidoku.res
