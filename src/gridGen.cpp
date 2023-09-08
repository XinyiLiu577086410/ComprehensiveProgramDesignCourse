#include <fstream>
#include <cstdlib>
#include <iostream>
#include "cnf.hpp"
#include "ultilities.hpp"
VectorXY posToXy(int pos) {
    if(pos < 0 || pos > 60) {
        std::cout << "gridGen : posToXy() : Bad Pos!";
        exit(-1);
    }
    int count = 1, row, col;
    for(row = 1; row <= 9; row++) {
        for(col = 1; col <= supRow[row]; col++, count++) {
            if(count == pos) break;
        }
    }
    return RowColumnToXY({row,col});
}

int main(void) { 
    int grid[61] = {0,};
    // 从完整合法填充开始，基于挖洞法生成
    char seed[8][61+10] = {
        "5632438465747251366387241524519876357861324637241548653735246",
        "4563263847554721632638975437915428625789634463217554876363452",
        "5674334526747935866287935423698417554821763357628461543242356",
        "5236464572335798464798623523649851768231745451723637586456423",
        "6475347638575382645382147638569714264178253274153646231553624",
        "5637461542374631524389726535928174627594863486732563857456432",
        "5624374356243721566251873437862941554398762687495375683443526",
        "6534742756336984755781263424573918636298475438576261425352634",
    };
    int selectSeed = arc4random() % 8;
    int initNum;
    std::cout << "\ngridGen.cpp : 请输入初始格局提示数数量(5~10)：";
    std::cin >> initNum;
    while(initNum>10 || initNum<5) {
        std::cout << "\n请再次输入合适的提示数数量：";
            std::cin >> initNum;
    }
    for(int count = 0; count < initNum; count++) {
        int pos = arc4random() % 61 + 1;
        while(grid[pos] != 0) pos = arc4random() % 61 + 1;
        // auto xy = posToXy(pos);
        // int xyv = xy.x * 100 + xy.y * 10;
        // for(int i = xyv + 1; i <= xyv + 9; i++) 
        //     if(solution[i]) 
        //         grid[pos] = i - xyv;
        grid[pos] = seed[selectSeed][pos-1] - '0';
    }
    std::ofstream outFile;
    outFile.open("./hanidoku/grid.txt", std::ios_base::ate);
    for(int i = 1; i <= 61; i++) {
        outFile << grid[i];
    }
    outFile.close();
    return 0;
}


