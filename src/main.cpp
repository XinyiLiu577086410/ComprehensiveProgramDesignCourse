#include <iostream>
#include <string>
#include <fstream>
#include <cstdio>
#include <chrono>
#include <thread>
#include <functional>

#include "cnf.hpp"
#ifndef SUCCESS
#define SUCCESS 0
#endif
#ifndef ERROR
#define ERROR -1
#endif


void foo(int x){
    return;
}


int main(int argc, char * argv[]){
    Cnf S;
    std::string inputFileName;
    inputFileName = argv[1];

    /* 输入 */ 
    S.Read("./testset/" + inputFileName);
    int VarNum = S.GetVariableNum();
    bool sat;
    bool * solution = new bool[VarNum + 1];
    double ms;
    std::thread tt(threadInterface, std::ref(S), solution,  0,  std::ref(sat), std::ref(ms));   
    tt.join();                           
    /* 输出 */ 
    char tmp[512];
    /* 截去inputFileName的 ".cnf"拓展名，并拼接输出文件名 */
    int i = inputFileName.length();
    while(inputFileName[i+1] != '.' && i > 0) { --i; }
    int j = 0;
    while(j <= i) { tmp[j] = inputFileName[j]; j++; } 
    tmp[j] = '\0';
    std::string name = tmp;
    std::ofstream outFile;

    outFile.open("./output/" + name + ".res");
    if(!outFile.is_open()) std::cout<<"\ndpll : main() : FAIL to create " << "./output/" + name + ".res";
    else std::cout<<"\ndpll : main() : Created output file " << "./output/" + name + ".res" << " SUCCESSFULLY!\n";

    outFile << "s " << sat ;
    if(sat == 1) {
        outFile << "\nv ";
        for (int i = 1; i <= VarNum; i++) {
            outFile << i * (solution[i] ? +1 : -1) << ' ';
        }
    }
    outFile << "\nt " << std::fixed << std::setprecision(2) << ms ;
    std::cout << "Time used : " << std::fixed << std::setprecision(2) << ms << "ms";
    std::cout << "\nClass Cnf case created : " << S.countCases << " times.";
    std::cout << "\nDpll() is called : " << S.countDpllCalls << " times.";
    outFile.close();
    delete[] solution;
    return 0;
}
