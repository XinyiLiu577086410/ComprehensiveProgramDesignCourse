#!/bin/zsh

# 测试hanidoku求解功能
rm ./hanidoku/*
g++ ./src/haniCnfGen.cpp -o ./executable/haniCnfGen -std=c++11
g++ ./src/dpll.cpp -o ./executable/dpll -std=c++11 -Ofast -lpthread
g++ ./src/haniDisplay.cpp -o ./executable/haniDispaly -std=c++11
./executable/haniCnfGen ./hanidoku/hanidoku.cnf
echo "\n\n\nrun.sh : 正在求解 ./hanidoku/hanidoku.cnf : " 
./executable/dpll ./hanidoku/hanidoku.cnf ./hanidoku/hanidoku.res
./executable/verify ./hanidoku/hanidoku.cnf ./hanidoku/hanidoku.res
