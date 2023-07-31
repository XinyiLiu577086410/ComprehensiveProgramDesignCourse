#include <iostream>
#include <string>
#include "Cnf.hpp"
#define SUCCESS 0
#define ERROR -1

int main(void){
    std::string name;
    size_t pos = 0;
    std::cout << "\n请输入CNF文件名\n";
    std::cin >> name;
    Cnf S;
    S.Read(name);
    std::cout << S.Dpll();
    return 0;
}