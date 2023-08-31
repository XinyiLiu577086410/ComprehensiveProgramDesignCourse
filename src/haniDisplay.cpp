#include "coordinate.hpp"
#include "cnf.hpp"
#include "vector.hpp"
#include <iostream>
#include <fstream>
#include <algorithm>
#include <chrono>
#include <thread>
void display(int resInXY[]) {
    if(resInXY[0] != 849) {
        std::cout << "\nhaniDisplay.cpp : display() : variable number is not 849, check your .res file!";
        exit(-1);
    }
    int positiveResInXY[100], cur = 0;
    for(int i = 1; i <= resInXY[0]; i++) {
        VectorXY xy;
        xy.x = resInXY[i] / 100;
        xy.y = (resInXY[i] / 10) % 10;
        if(resInXY[i] > 0 && InAreaXY(xy) && resInXY[i] % 10 != 0)
            positiveResInXY[++cur] = resInXY[i];
    }
    positiveResInXY[0] = cur;
    // std::cout << "\nPositive Variable Number : " << cur;
    if(cur != 61) {
        std::cout << "\nhaniDisplay.cpp : display() : positive variable number is not 61, check your .res file!";
        exit(-1);
    }
    for(int row  = 1; row <= 9; row++) {
        int inf = infRow[row], sup = supRow[row];
        std::cout << "\n";
        for(int i = 0; i < abs(5 - row); i++) {
            std::cout << " ";
        }
        for(int col = inf; col <= sup; col++) {
            bool flag = false;
            VectorRowColumn rc = {row, col};
            VectorXY xy = RowColumnToXY(rc);
            int numInf = xy.x * 100 + xy.y * 10, numSup = numInf + 10;
            for(int i = 1; i <= positiveResInXY[0]; i++) {
                if(positiveResInXY[i] < numSup && positiveResInXY[i] > numInf) {
                    std::cout << positiveResInXY[i] % 10 << " ";
                    flag = true;
                }
            }
            if(!flag) std::cout << "  ";
        }
    }
    std::cout << "\n";
}

int read(std::string filename, int res[]) {
    std::ifstream solution;
    solution.open(filename);
    if(!solution.is_open()) { 
        std::cout<<"\nhaniDisplay : main() : 打开文件失败！"; 
        exit(-1); 
    }
    int cur = 0;
    int p, returnValue;
    char ch;
    while(!solution.eof() && solution >> ch && ch != 's');
    solution >> returnValue;
    if(returnValue == 1){
        while(!solution.eof() && solution >> ch && ch != 'v');
        for(int i = 0; i < 849 ; i++) {
            solution >> res[++cur];
        };
        res[0] = cur; // 保存个数
    }
    else{
        std::cout << "\nhaniDispaly.cpp : read() : Bad .res file!";
        exit(-1);
    }
    solution.close();
    return returnValue;
}


int main(int argc, char * argv[]) {
    int res[250000];
    std::string resFileName = argv[1];
    read(resFileName, res);
    display(res);
    return 0;
}
