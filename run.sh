#!/bin/zsh

# 运行脚本，用于批量测试CNF算例

if test -e "./executable"
then
else
    mkdir executable
#编译四个独立的功能程序
    c++ ./src/dpll.cpp  -o ./executable/dpll -std=c++11 -O3 -lpthread 
    # c++ ./src/verify.cpp -o ./executable/verify -std=c++11 -O3
    c++ ./src/haniCnfGen.cpp -o ./executable/haniCnfGen -std=c++11 -O3
    c++ ./src/haniDisplay.cpp -o ./executable/haniDisplay -std=c++11 -O3
    c++ ./src/haniGame.cpp -o ./executable/haniGame -std=c++11 -O3
    c++ ./src/gridGen.cpp -o ./executable/gridGen -std=c++11 -O3
fi
if test -e "./dpllOutput" 
then
    rm -rf ./dpllOutput
fi
mkdir dpllOutput

#开始
while 
do
    echo "\n\n\n基于SAT的蜂窝数独求解器\n提示：cnf文件请放在./testset下\n请按下相应键位选择功能：(a)SAT 批量求解\t(b)SAT 单个求解\t(c)蜂窝数独批量求解\t(d)蜂窝数独单个求解\t(e)蜂窝数独游戏\t(其他)退出"
    read option
    case ${option} in
    a)
        echo "请输入运行限时(秒数)："
        read timelimit
        echo "请输入变元策略（1~6）"
        read tag
        echo "\n求解快照："
        for file in `ls  ./testset` 
            do
            echo "\n\nrun.sh : 正在求解 ${file} : " 
            ./executable/dpll "./testset/${file}" "./dpllOutput/${file%.*}.res" ${timelimit} ${tag}
            ./verify "./testset/${file}" "./dpllOutput/${file%.*}.res"
            done
    ;;
    b)
        echo "请输入文件名（不含路径，必须在./testset中）："
        read cnfFile
        echo "请输入运行限时(秒数)："
        read timelimit
        echo "请输入变元策略（1~6）"
        read tag
        echo "\n\nrun.sh : 正在求解 ${cnfFile} : " 
        ./executable/dpll "./testset/${cnfFile}" "./dpllOutput/${cnfFile%.*}.res" ${timelimit} ${tag}
        ./verify "./testset/${cnfFile}" "./dpllOutput/${cnfFile%.*}.res"
    ;;
    c)
        echo "请输入初始格局文件（不含路径，必须在./hanidoku中）"
        read file
        echo "请输入运行限时(秒数)："
        read timelimit
        for line in `cat ./hanidoku/${file}`
        do
            ./executable/haniCnfGen ./hanidoku/hanidoku.cnf ${line}
            ./executable/dpll ./hanidoku/hanidoku.cnf ./hanidoku/hanidoku.res  ${timelimit} "1"
            ./executable/haniDisplay ./hanidoku/hanidoku.res 
        done
    ;;
    d)
        echo "请输入初始格局字符串"
        read grid
        echo "请输入运行限时(秒数)："
        read timelimit
        ./executable/haniCnfGen ./hanidoku/hanidoku.cnf ${grid}
        ./executable/dpll ./hanidoku/hanidoku.cnf ./hanidoku/hanidoku.res ${timelimit} "1"
        ./executable/haniDisplay ./hanidoku/hanidoku.res 
    ;;
    e)
        ./executable/gridGen
        ./executable/haniCnfGen ./hanidoku/standard.cnf null
        ./executable/haniGame ./hanidoku/grid.txt
        grid=`cat ./hanidoku/grid.txt` 
        ./executable/haniCnfGen ./hanidoku/hanidoku.cnf ${grid}
        ./executable/dpll ./hanidoku/hanidoku.cnf ./hanidoku/hanidoku.res  "30"  "1"
        echo "\n解如下"
        ./executable/haniDisplay ./hanidoku/hanidoku.res 
    ;;
    *)
        echo "程序结束"
        break
    ;;
    esac
done