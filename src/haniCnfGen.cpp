/*
    Generat .cnf file for search a solution for hanidoku in coordinates of XY
*/
#include <fstream>
#include <iostream>
#include <cassert>
#include <cstring>
#include "coordinate.hpp"
using namespace std;

int lineMustContain[][10] = {
    {-1}, // 0
    {-1}, // 1
    {-1}, // 2
    {-1}, // 3
    {-1}, // 4
    {1, 5}, // 5
    {3, 4, 5, 6}, // 6
    {5, 3, 4, 5, 6, 7}, // 7
    {7, 2, 3, 4, 5, 6, 7, 8}, // 8
    {9, 1, 2, 3, 4, 5, 6, 7, 8, 9}, // 9
};


int lineAlternative[][11][2]{
    {-1}, // 0
    {-1}, // 1
    {-1}, // 2
    {-1}, // 3
    {-1}, // 4
    {
        {10, },
        {1, 6},
        {2, 6},
        {2, 7},
        {3, 6},
        {3, 7},
        {3, 8},
        {4, 6},
        {4, 7},
        {4, 8},
        {4, 9}
    }, //5
    {
        {6,  },
        {3, 7},
        {3, 8},
        {3, 9},
        {2, 8},
        {2, 7},
        {1, 7},
    }, //6
    {
        {3, },
        {1,8},
        {2,8},
        {2,9}
    }, // 7
    {
        {1,  },
        {1, 9}
    }, // 8
    {
        {0, }
    }, // 9
};

bool isNum(char c) {
    return c >= '0' && c <= '9';
}


int main(int argc, char * argv[]) {
    if(argc != 3) {
        std::cout << "\nhaniCnfGen : main() : 参数个数错误！\n usage: ./haniCnfGen <output file> <initial grid>";
        exit(0);
    }
    ofstream outFile;
    std::string outputFileName = argv[1];
    outFile.open(outputFileName);
    assert(outFile.is_open());
    char initGrid[100];
    strcpy(initGrid , argv[2]);
    if(strcmp("null", initGrid) != 0) {
        int start =  strlen(initGrid) - 1, count = 0, i = 0;
        while(start != 0 && isNum(initGrid[start-1])) 
            start--;   // 过滤前缀，定位起点
        for(i = start; initGrid[i]!='\0'; i++) {
            if(initGrid[i] != '0') count++;
        }
        outFile << "\np cnf " << 849 <<" "<< 7407 + count; 
        i = start;
        for(int row = 1; row <= 9; row++) {
            int inf = infRow[row], sup = supRow[row];
            for (int col = inf; col <= sup; col++) {
                int k = initGrid[i++] - '0';
                if(k != 0){
                    VectorRowColumn rc = {row, col};
                    VectorXY xy = RowColumnToXY(rc);
                    outFile << "\n" << xy.x << xy.y << k << " 0";
                }
            }
        }
    }
    else{
        outFile << "\np cnf " << 849 <<" "<< 7407; 
    }
    /*  格约束  */
    /*   每一格至少一个变元为真   */
    // for(int x = 0; x <= 8; x++){
    //     int sup = supXY[x], inf = infXY[x]; 
    //     for (int y = inf; y <= sup; y++) {
    //         outFile << "\n";
    //         for(int k = 1; k <= 9; k++){
    //             outFile << x << y << k << " ";
    //         }
    //         outFile << " 0";
    //     }  
    // }

    /*   每一格至多一个变元为真   */
    for(int x = 0; x <= 8; x++){
        int sup = supXY[x], inf = infXY[x]; 
        for (int y = inf; y <= sup; y++) {
            for(int k = 1; k <= 9; k++){
                for(int kk = k + 1; kk <= 9; kk++) {
                    outFile << "\n-" << x << y << k << " " << "-" << x << y << kk << " 0";
                }
            }
        }
    }

    /*   线约束   */
    /*  下降对角线不重复  */
    for(int k = 1; k <= 9; k++) {
        for(int x = 0; x <= 8; x++) {
            int sup = supXY[x], inf = infXY[x]; 
            for(int y = inf; y <= sup; y++) {
                for(int yy = y + 1; yy <= sup; yy++) {
                    outFile << "\n-" << x << y << k << " " << "-" << x << yy << k << " 0";
                }
            }
        }
    }

    /*  上升对角线不重复  */
    for(int k = 1; k <= 9; k++) {
        for(int y = 0; y <= 8; y++) {
            int sup = supXY[y], inf = infXY[y]; 
            for(int x = inf; x <= sup; x++) {
                for(int xx = x + 1; xx <= sup; xx++) {
                        outFile << "\n-" << x << y << k << " " << "-" << xx << y << k << " 0";
                }
            }
        }
    }

    /*   行不重复   */
    for(int k = 1; k <= 9; k++) {
        for(int row = 1; row <= 9; row++) {
            int inf = infRow[row], sup = supRow[row];
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

    /*   下降对角线连续   */
    for(int x = 0; x <= 8; x++) {
        int sup = supXY[x], inf = infXY[x]; 
        int len = sup - inf + 1;
        for(int i = 1; i <= lineMustContain[len][0]; i++) {
            int numberMustContain = lineMustContain[len][i];
            outFile << "\n";
            for(int cur = inf; cur <= sup; cur++) {
                outFile << x << cur << numberMustContain <<" ";
            }
            outFile << "0";
        }
    }
    for(int x = 0; x <= 8; x++) {
        int sup = supXY[x], inf = infXY[x]; 
        int len = sup - inf + 1;
        for(int j = 1; j <= lineAlternative[len][0][0]; j++) {
            outFile << "\n";
            for(int cur = inf; cur <= sup; cur++) {
                outFile << x << cur << lineAlternative[len][j][0] <<" ";
                outFile << x << cur << lineAlternative[len][j][1] <<" ";                
            }
            outFile << "0";            
        }
    }

    /*   上升对角线连续   */
    for(int y = 0; y <= 8; y++) {
        int sup = supXY[y], inf = infXY[y]; 
        int len = sup - inf + 1;
        for(int i = 1; i <= lineMustContain[len][0]; i++) {
            int numberMustContain = lineMustContain[len][i];
            outFile << "\n";
            for(int cur = inf; cur <= sup; cur++) {
                outFile << cur << y << numberMustContain <<" ";
            }
            outFile << "0";
        }
    }
    for(int y = 0; y <= 8; y++) {
        int sup = supXY[y], inf = infXY[y]; 
        int len = sup - inf + 1;
        for(int j = 1; j <= lineAlternative[len][0][0]; j++) {
            outFile << "\n";
            for(int cur = inf; cur <= sup; cur++) {
                outFile << cur << y << lineAlternative[len][j][0] <<" ";
                outFile << cur << y << lineAlternative[len][j][1] <<" ";                
            }
            outFile << "0";            
        }
    }

    /*  行连续  */
    for(int row = 1; row <= 9; row++) {
        int inf = infRow[row], sup = supRow[row];
        int len = sup - inf + 1;
        for(int i = 1; i <= lineMustContain[len][0]; i++) {
            int numberMustContain = lineMustContain[len][i];
            outFile << "\n";
            for (int col = inf; col <= sup; col++) {
                VectorRowColumn rc = {row,col};
                VectorXY xy = RowColumnToXY(rc);
                outFile << xy.x <<  xy.y << numberMustContain <<" ";
            }
            outFile << "0";            

        }
    }
    for(int row = 1; row <= 9; row++) {
        int inf = infRow[row], sup = supRow[row];
        int len = sup - inf + 1;
        for(int j = 1; j <= lineAlternative[len][0][0]; j++) {
            outFile << "\n";
            for (int col = inf; col <= sup; col++) {
                VectorRowColumn rc = {row,col};
                VectorXY xy = RowColumnToXY(rc);
                outFile << xy.x << xy.y << lineAlternative[len][j][0] <<" ";
                outFile << xy.x << xy.y << lineAlternative[len][j][1] <<" ";  
            }
            outFile << "0";            
        }
    }

    return 0;
}