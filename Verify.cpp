#include "Cnf.hpp"
#include <iostream>
int main(void){
    Cnf S;
    std::string name;
    std::cout << "请输入待验证的CNF文件名\n";
    std::cin >> name;
    S.Read(name);
    std::ifstream solution;
    solution.open("solution");
    int p;
    std::string value;
    bool rslt[60000];
    while(!solution.eof() && solution >> p >> value){
        if(value == "true") rslt[p] = true;
        else if (value == "false") rslt[p] = false;
    };
    if(S.Verify(rslt)) std::cout<<"验证成功！\n"; else std::cout<<"验证失败! \n";
    return 0;
}