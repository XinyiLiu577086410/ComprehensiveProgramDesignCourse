#include <iostream>
#include <fstream>
#include <cstdlib>
#include <cstdio>
#include "ultilities.hpp"
#include "cnf.hpp"


bool isNum(char c) {
    return c >= '0' && c <= '9';
}


void display(int intGrid[]) {
    int cur = 0;
    for(int row = 1; row <= 9; row++) {
        int inf = infRow[row], sup = supRow[row];
        std::cout << "\n";
        for(int i = 0; i < abs(5-row); i++)
            std::cout << " ";
        for(int col = inf; col <= sup; col++, cur++)
            if(intGrid[cur] == 0) 
                std::cout << "* ";
            else
                // std::cout << intGrid[cur] << " ";
                printf("%d ", intGrid[cur]);

    }
}

int main(int argc, char *argv[]){
    std::string charGrid;
    std::ifstream inFile;
    typedef struct {
        int a,b,c;
    } mod; // modify
    
    MyStack<mod> op;

    if(argc != 2) {
        std::cout << "\nhaniGame : 参数数量错误";
        exit(-1);
    }
    inFile.open(argv[1]);
    if(!inFile.is_open()) {
        std::cout << "\nhaniGame : 打开文件失败:" << argv[1];
        exit(-1);
    }
    inFile >> charGrid;
    int intGrid[61];
    int start = charGrid.length()/*, gridLen = charGrid.length()*/;
    while(start != 0 && isNum(charGrid[start-1])) 
        start--;   // 过滤前缀，定位起点
    int i,j;
    for(i = 0, j = start; j < charGrid.length(); i++, j++)
        intGrid[i] = charGrid[j] - '0'; 
    if(i != 61) {
        std::cout << "\nhaniGame : 初始格局错误！";
        exit(-1);
    }
    std::cout << "\nhaniGame : 游戏开始:";
    display(intGrid);
    while(1) {
        int in = 0, num = 0, pos = 0;
        char discord;
        std::cout << "\n输入：1）修改格局 2）验证 3)撤消 4）提示 其他）查看答案并退出\n";
        std::cin >> in;
        switch (in) {
        case 1:{
            VectorRowColumn rc;
            std::cout << "\n输入格式：<行> <列> <数>\n";
            std::cin >> rc.row >> rc.col >> num;
            while(!InAreaRowColumn(rc) || num > 9 || num < 0) {
                std::cout << "\n行号和列号非法，请再次输入<行> <列> <数>\n";
                // 清空非法输入
                std::cin.clear(); 
                while(std::cin.get()!='\n')
                    continue;
                std::cin >> rc.row >> rc.col >> num;
            }
            pos = 0;
            for(int row = 1; row < rc.row; row++) {
                pos += supRow[row];
            }
            pos += rc.col - 1;
            op.Push({rc.row, rc.col, intGrid[pos]});
            intGrid[pos] = num;
            display(intGrid);
            break;
        }
        case 2:{
            Cnf S;
            S.Read("./hanidoku/standard.cnf");
            pos = 0;
            for(int row = 1; row <= 9; row++) {
                for(int col = infRow[row]; col <= supRow[row]; col++) {
                    if(intGrid[pos] != 0) {
                        VectorXY xy = RowColumnToXY({row,col});
                        int varBool = xy.x * 100 + xy.y * 10 + intGrid[pos];
                        Vector<int> v;
                        v.Add(varBool);
                        S.Add(v);
                    }
                    pos++;
                }
            }
            // bool solution[61*9+100]; 
            bool solution[849];
            bool sat = S.Dpll(solution, 0);
            if(sat) {
                std::cout << "没有发现错误";
            }
            else {
                std::cout << "格局中有相冲突的数字，请检查";
            }
            display(intGrid);
            break;
        }
        case 3:{
            if(op.Empty()){
                std::cout<<"\n没有操作可以撤回！";
            }
            else{
                auto last = op.Pop();
                int num = last.c;
                VectorRowColumn rc = {last.a, last.b};
                pos = 0;
                for(int row = 1; row < rc.row; row++) {
                    pos += supRow[row];
                }
                pos += rc.col - 1;
                intGrid[pos] = num;
                display(intGrid);
            }
            break;
        }
        case 4:{
            VectorRowColumn rc;
            std::cout << "\n输入你想看的格，格式：<行> <列>\n";
            std::cin >> rc.row >> rc.col;
            while(!InAreaRowColumn(rc)) {
                std::cout << "\n行号和列号非法，请再次输入<行> <列>\n";
                // 清空非法输入
                std::cin.clear(); 
                while(std::cin.get()!='\n')
                    continue;
                std::cin >> rc.row >> rc.col >> num;
            }
            auto xy = RowColumnToXY(rc);
            int varInf = xy.x*100+xy.y*10+1;
            int varSup = xy.x*100+xy.y*10+9;
            Cnf S;
            S.Read("./hanidoku/standard.cnf");
            pos = 0;
            for(int row = 1; row <= 9; row++) {
                for(int col = infRow[row]; col <= supRow[row]; col++) {
                    if(intGrid[pos] != 0) {
                        VectorXY xy = RowColumnToXY({row,col});
                        int varBool = xy.x * 100 + xy.y * 10 + intGrid[pos];
                        Vector<int> v;
                        v.Add(varBool);
                        S.Add(v);
                    }
                    pos++;
                }
            }
            // bool solution[61*9+100]; 
            bool solution[849];
            bool sat = S.Dpll(solution, 0);
            for(int i = varInf; i<=varSup; i++){
                if(solution[i]) std::cout << "提示：填入" << i % 10;
            }
            display(intGrid);
            break;
        }
        default:
            inFile.close();
            std::ofstream outFile;
            outFile.open(argv[1], std::ios_base::ate);
            for(int i = 0; i < 61; i++)
                outFile << intGrid[i];
            outFile.close();
            return 0;
            // break;
        }
    }
    return 0;
}
