#include "Cnf.hpp"
#include <iostream>
int main(int argc, char * argv[]){
    Cnf S;
    std::string name;
    // std::cout << "请输入待验证的CNF文件名\n";
    // std::cin >> name;
    name = argv[1];
    S.Read(name);
    std::ifstream solution;
    solution.open("./solution.txt");
    int p;
    std::string value;
    bool rslt[250000];
    while(!solution.eof() && solution >> p >> value){
        if(value == "true") rslt[p] = true;
        else if (value == "false") rslt[p] = false;
    };
    if(S.Verify(rslt)) std::cout<<"CNF可满足性验证成功！\n"; else std::cout<<"CNF可满足性验证失败！ \n";
    solution.close();
    return 0;
}