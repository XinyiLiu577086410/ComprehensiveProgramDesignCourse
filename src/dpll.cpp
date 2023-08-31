#include <iostream>
#include <string>
#include <fstream>
#include <cstdio>
#include <chrono>
#include <thread>
#include <functional>
#include <iomanip>
#include "cnf.hpp"

#ifndef SUCCESS
#define SUCCESS 0
#endif
#ifndef ERROR
#define ERROR -1
#endif

#define TIME_LIMIT 30.0

int main(int argc, char * argv[]){
    if(argc != 3) {
        std::cout << "\ndpll : main() : 参数数量错误！";
        exit(-1);
    }
    Cnf S;
    std::string inputFileName, outputFileName;
    inputFileName = argv[1];
    outputFileName = argv[2];
    /* 输入 */ 
    S.Read(inputFileName);
    int VarNum = S.GetVariableNum();
    bool sat;
    bool * solution = new bool[VarNum + 1];
    double ms;
    bool taskFinished = false;
    std::thread tt(threadInterface, std::ref(S), solution,  0,  std::ref(sat), std::ref(ms), std::ref(taskFinished));   
    std::chrono::steady_clock::time_point timeStart = std::chrono::steady_clock::now();       
    while(1) {
        std::this_thread::sleep_for(std::chrono::milliseconds(100));
        auto timeElapsed = std::chrono::duration_cast< std::chrono::duration<double> > ( std::chrono::steady_clock::now() - timeStart );       
        double secElapsed = timeElapsed.count();
        if(taskFinished){
            tt.join();   
            break;
        }         
        else if(secElapsed >= TIME_LIMIT) {
            std::cout << "\ndpll : main() : 线程超时"<< TIME_LIMIT <<"s";
            std::ofstream outFile;
            outFile.open(outputFileName);
            outFile << "s " << -1;
            outFile << "\nt "<< secElapsed * 1000;
            tt.detach();
            outFile.close();
            return 0;
        }   
        else continue;           
    }
    std::ofstream outFile;
    outFile.open(outputFileName);
    if(!outFile.is_open()) std::cout << "\ndpll : main() : 创建文件失败：" << outputFileName;
    else std::cout << "\ndpll : main() : 创建输出文件成功：" << outputFileName;
    outFile << "s " << sat ;
    if(sat == 1) {
        outFile << "\nv ";
        for (int i = 1; i <= VarNum; i++) {
            outFile << i * (solution[i] ? +1 : -1) << ' ';
        }
        outFile << "0";
    }
    outFile << "\nt " << std::fixed << std::setprecision(2) << ms ;
    std::cout << "\ndpll : main() : 用时：" << std::fixed << std::setprecision(2) << ms << "ms";
    std::cout << "\ndpll : main() : Cnf实例数：" << S.countCases;
    std::cout << "\ndpll : main() : Cnf::Dpll()的调用次数：" << S.countDpllCalls;
    outFile.close();
    delete[] solution;
    return 0;
}
