#include "cnf.hpp"
#include <iostream>
#include <algorithm>
#include <cstdlib>
#include <cassert>
int main(int argc, char * argv[]){
    Cnf S;
    std::string cnfAddress;
    cnfAddress = argv[1];
    S.Read(cnfAddress);
    std::string solutionFile;
    solutionFile = argv[2];
    std::ifstream solution;
    solution.open(solutionFile);
    if(!solution.is_open()) { 
        std::cout << "\nverify : main() : 打开文件失败 ：" << solutionFile; 
        exit(-1); 
    }
    int p;
    char ch;
    bool rslt[250000];
    while(!solution.eof() && solution >> ch && ch != 's');
    solution >> p;
    if(p == 0) { 
        std::cout << "\nverify : main() : CNF是不可满足的。"; 
        return 0; 
    }
    else if(p == -1) { 
        std::cout << "\nverify : main() : 求解程序未能在限定时间内完成求解。"; 
        return 0; 
    }
    while(!solution.eof() && solution >> ch && ch != 'v');
    while(!solution.eof() && solution >> p && p != 't') { 
        int abs_p = abs(p);
        if(p > 0) 
            rslt[abs_p] = true;
        else if (p < 0) 
            rslt[abs_p] = false;
    };
    if(S.Verify(rslt)) 
        std::cout<<"\nverify : main() : CNF可满足性验证成功！"; 
    else 
        std::cout<<"\nverify : main() : CNF可满足性验证失败！";
    solution.close();
    return 0;
}