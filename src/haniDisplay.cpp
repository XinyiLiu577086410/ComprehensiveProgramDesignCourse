#include "coordinate.hpp"
#include "cnf.hpp"
#include "vector.hpp"
#include <iostream>
#include <fstream>
#include <algorithm>
#include <chrono>
#include <thread>
void display(int resInXY[]) {
    int positiveResInXY[100], cur = 0;
    for(int i = 1; i <= resInXY[0]; i++) {
        if(resInXY[i] > 0) positiveResInXY[++cur] = resInXY[i];
    }
    positiveResInXY[0] = cur;
    std::cout << "\n" << cur;
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
}

int read(std::string filename, int res[]) {
    std::ifstream solution;
    solution.open(filename);
    if(!solution.is_open()) { std::cout<<"\nhaniDisplay : main() : 打开文件失败！"; exit(-1); }
    int cur = 0;
    int p, returnValue;
    char ch;
    while(!solution.eof() && solution >> ch && ch != 's');
    solution >> returnValue;
    if(returnValue == 1){
        while(!solution.eof() && solution >> ch && ch != 'v');
        while(!solution.eof() && solution >> p && p != 't') { 
            res[++cur] = p;
        };
        res[0] = cur; // 保存个数
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
