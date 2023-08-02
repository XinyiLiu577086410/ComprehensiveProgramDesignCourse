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
int main(void){    
    Cnf S;
    std::string name;
    std::cout << "请输入CNF文件名\n";
    std::cin >> name;
    

    /*  计时开始  */
    std::chrono::steady_clock::time_point  t0 = std::chrono::steady_clock::now();

    // 输入
    S.Read(name); 
    int VarNum = S.GetVariableNum(); 
    bool sat;
    sat = S.Dpll(solution);
    
    // 输出
    if(sat) { 
        std::cout << "CNF命题是可满足的\n";     
        std::cout << "成真赋值的解在solution文件中。\n";
    }
    else std::cout << "CNF命题是不可满足的\n";

    std::ofstream out;
    out.open("solution");
    if(sat) for (int i = 1; i <= VarNum; i++) {
        out << i << ' ' << (solution[i] ? "true\n" : "false\n");
    }
    else;

    /*  计时结束  */
    std::chrono::steady_clock::time_point  t1 = std::chrono::steady_clock::now();


    std::chrono::duration<double> time = std::chrono::duration_cast< std::chrono::duration<double> >(t1 - t0);
    std::cout << "Time used: " << time.count() * 1000 << " ms.\n";
    return 0;
}