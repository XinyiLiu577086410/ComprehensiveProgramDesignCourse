#include "cnf.hpp"
#include <iostream>
#include <algorithm>
int main(int argc, char * argv[]){
    Cnf S;
    std::string cnfAddress;
    cnfAddress = argv[1];
    S.Read(cnfAddress);
    std::string solutionAddress;
    solutionAddress = argv[2];
    std::ifstream solution;
    solution.open(solutionAddress);
    int p;
    char ch;
    std::string value;
    bool rslt[250000];
    while(!solution.eof() && solution >> ch && ch != 's');
    solution >> p;
    if(p != 1) {std::cout << "\nCNF是不可满足的，或求解程序未能在限定时间内完成求解。"; return 0;}
    while(!solution.eof() && solution >> ch && ch != 'v');
    while(!solution.eof() && solution >> p && p != 't') { //? 
        int abs_p = abs(p);
        if(p > 0) rslt[abs_p] = true;
        else if (p < 0) rslt[abs_p] = false;
    };
    if(S.Verify(rslt)) std::cout<<"\nCNF可满足性验证成功！"; else std::cout<<"\nCNF可满足性验证失败！ ";
    solution.close();
    return 0;
}