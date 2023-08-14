#include <iostream>
#include <string>
#include <fstream>
#include <cstdio>
#include <chrono>

#include "cnf.hpp"
#ifndef SUCCESS
#define SUCCESS 0
#endif
#ifndef ERROR
#define ERROR -1
#endif
int main(int argc, char * argv[]){
    Cnf S;
    std::string inputFileName;
    inputFileName = argv[1];

    /* 输入 */ 
    S.Read("./testset/" + inputFileName);
    int VarNum = S.GetVariableNum();
    bool sat = -1;
    bool solution[VarNum + 1];

    /* 计时开始 */    std::chrono::steady_clock::time_point  t0 = std::chrono::steady_clock::now();
    sat = S.Dpll(solution);
    /* 计时结束 */   std::chrono::steady_clock::time_point  t1 = std::chrono::steady_clock::now();
    std::chrono::duration<double> time = std::chrono::duration_cast< std::chrono::duration<double> >(t1 - t0);

    /* 输出 */ 
    char tmp[512];
    // 截去inputFileName的 ".cnf"
    int i = inputFileName.length();
    while(inputFileName[i+1] != '.' && i > 0) { --i; }
    int j = 0;
    while(j <= i) { tmp[j] = inputFileName[j]; j++; } 
    tmp[j] = '\0';
    std::string name = tmp;
    std::ofstream outFile;

    outFile.open("./output/" + name + ".res");
    if(!outFile.is_open()) std::cout<<"\nmain : 创建输出文件"<<"./output/" + name + ".res"<<"失败！\n";
    else std::cout<<"\ndpll : 创建输出文件"<<"./output/" + name + ".res"<<"成功！\n";

    outFile << "s " << sat ;
    if(sat == 1) {
        outFile << "\nv ";
        for (int i = 1; i <= VarNum; i++) {
            outFile << i * (solution[i] ? +1 : -1) << ' ';
        }
    }
    outFile << "\nt " << time.count() * 1000 ;
    std::cout << "Time used : " << time.count() * 1000 << "ms";
    std::cout << "\nClass Cnf case created : " << S.countCases << " times.";
    std::cout << "\nDpll() is called : " << S.countDpllCalls << " times.";
    outFile.close();
    return 0;
}
