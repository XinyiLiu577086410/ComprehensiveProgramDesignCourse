#include <iostream>
#include <string>
#include <fstream>
#include <cstdio>
#include "Cnf.hpp"
#ifndef SUCCESS
#define SUCCESS 0
#endif
#ifndef ERROR
#define ERROR -1
#endif
bool solution[250000];
int main(int argc, char * argv[]){
    Cnf S;
    std::string inputFileName;
    inputFileName = argv[1];

    /* 输入 */ 
    S.Read("./testset/" + inputFileName);
    int VarNum = S.GetVariableNum();
    bool sat = -1;

    /* 计时开始 */    std::chrono::steady_clock::time_point  t0 = std::chrono::steady_clock::now();
    sat = S.Dpll(solution);
    /* 计时结束 */   std::chrono::steady_clock::time_point  t1 = std::chrono::steady_clock::now();
    std::chrono::duration<double> time = std::chrono::duration_cast< std::chrono::duration<double> >(t1 - t0);
   
    /* 输出 */ 
    char tmp[128];
    int i = 0;
    // 截去inputFileName的 ".cnf"
    while(inputFileName[i] != '.' && i < 127) { tmp[i] = inputFileName[i]; ++i; }
    tmp[i] = '\0';
    std::string name = tmp;
    std::ofstream outFile;
    outFile.open("./output/" + name + ".res");
    outFile << "s " << sat ;
    if(sat == 1) {
        outFile << "\nv ";
        for (int i = 1; i <= VarNum; i++) {
            outFile << i * (solution[i] ? +1 : -1) << ' ';
        }
    }
    outFile << "\nt " << time.count() * 1000 ;
    outFile.close();
    return 0;
}
