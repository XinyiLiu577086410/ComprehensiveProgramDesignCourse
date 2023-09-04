#!/bin/zsh
# 测试hanidoku求解功能
g++ ./src/haniCnfGen.cpp -o ./executable/haniCnfGen -std=c++11 -O3
g++ ./src/dpll.cpp -o ./executable/dpll -std=c++11 -O3 -lpthread
g++ ./src/haniDisplay.cpp -o ./executable/haniDisplay -std=c++11 -O3
echo "请输入初始格局文件"
read file
for line in `cat ${file}`
do
    ./executable/haniCnfGen ./hanidoku/hanidoku.cnf ${line}
    # echo "\n\n\nhani.sh : 正在求解 ./hanidoku/hanidoku.cnf : " 
    ./executable/dpll ./hanidoku/hanidoku.cnf ./hanidoku/hanidoku.res "30"
    # ./executable/verify ./hanidoku/hanidoku.cnf ./hanidoku/hanidoku.res
    ./executable/haniDisplay ./hanidoku/hanidoku.res 
done