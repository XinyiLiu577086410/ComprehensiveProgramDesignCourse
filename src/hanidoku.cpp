/*
    Generate .cnf file for search a solution for hanidoku in coordinates of XY
*/
#include <fstream>
#include <iostream>
#include <cassert>
#include "coordinate.hpp"
using namespace std;
int main(void) {
    ofstream outFile;
    outFile.open("./hanidoku/hanidoku.cnf");
    assert(outFile.is_open());
    outFile << "\np cnf " << 849 <<" "<< 3312; 
    /*  |literal| : [041, 849] / {between [041, 849]}     */
    /*  行约束  */
    for(int k = 1; k <= 9; k++) {
        for(int x = 0; x <= 8; x++) {
            int sup = supXY[x], inf = infXY[x]; 
            for(int i = inf; i <= sup; i++) {
                for(int j = i + 1; j <= sup; j++) {
                    outFile << "\n-" << x << i << k << " " << "-" << x << j << k << " 0";
                }
            }
        }
    }
    for(int k = 1; k <= 9; k++) {
        for(int y = 0; y <= 8; y++) {
            int sup = supXY[y], inf = infXY[y]; 
            for(int i = inf; i <= sup; i++) {
                for(int j = i + 1; j <= sup; j++) {
                        outFile << "\n-" << i << y << k << " " << "-" << j << y << k << " 0";
                }
            }
        }
    }
    
    for(int k = 1; k <= 9; k++) {
        for(int row = 1; row <= 9; row++) {
            int inf = infRow[1], sup = supRow[1];
            for(int i = inf; i <= sup; i++) {
                for(int j = i + 1; j <= sup; j++) {
                    VectorRowColumn rci = {row, i};
                    VectorRowColumn rcj = {row, j};
                    VectorXY xyi = RowColumnToXY(rci);
                    VectorXY xyj = RowColumnToXY(rcj);
                    outFile << "\n-" << xyi.x << xyi.y << k << " " << "-" << xyj.x << xyj.y << k << " 0";
                }
            }
        }
    }

    return 0;
}