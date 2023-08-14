#include "cnf.hpp"
#include <iostream>
#include <algorithm>
#include <cassert>
int main(int argc, char * argv[]){
    Cnf S;
    std::string cnfAddress;
    cnfAddress = argv[1];
    S.Read("./testset/"+cnfAddress);
    std::string solutionAddress;
    solutionAddress = argv[2];
    std::ifstream solution;
    solution.open("./output/"+solutionAddress);
    assert(solution.is_open());
    if(!solution.is_open()) { std::cout<<"\nverify : 打开文件失败！\n\n"; return 0; }
    int p;
    char ch;
    std::string value;
    bool rslt[250000];
    while(!solution.eof() && solution >> ch && ch != 's');
    solution >> p;
    if(p == 0) { std::cout << "\nCNF是不可满足的。\n\n"; return 0; }
    else if(p == -1) { std::cout << "\n求解程序未能在限定时间内完成求解。\n\n"; return 0; }
    while(!solution.eof() && solution >> ch && ch != 'v');
    while(!solution.eof() && solution >> p && p != 't') { //? 
        int abs_p = abs(p);
        if(p > 0) rslt[abs_p] = true;
        else if (p < 0) rslt[abs_p] = false;
    };
    if(S.Verify(rslt)) std::cout<<"\nCNF可满足性验证成功！\n\n"; else std::cout<<"\nCNF可满足性验证失败！\n\n ";
    solution.close();
    return 0;
}