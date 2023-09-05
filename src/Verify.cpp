// #include "cnf.hpp"
// #include <iostream>
// #include <algorithm>
// #include <cstdlib>
// #include <cassert>
// int main(int argc, char * argv[]){
//     std::cout << "\nverify : main() : 正在验证解的正确性。";
//     Cnf S;
//     std::string cnfAddress;
//     cnfAddress = argv[1];
//     S.Read(cnfAddress);
//     int varNum = S.GetVariableNum();
//     std::string solutionFile;
//     solutionFile = argv[2];
//     std::ifstream solution;
//     solution.open(solutionFile);
//     if(!solution.is_open()) { 
//         std::cout << "\nverify : main() : 打开文件失败 ：" << solutionFile; 
//         exit(-1); 
//     }
//     int p;
//     char ch;
//     bool result[250000];
//     while(!solution.eof() && solution >> ch && ch != 's');
//     solution >> p;
//     if(p == 0) { 
//         std::cout << "\nverify : main() : CNF是不可满足的。"; 
//         return 0; 
//     }
//     else if(p == -1) { 
//         std::cout << "\nverify : main() : 求解程序未能在限定时间内完成求解。"; 
//         return 0; 
//     }
//     while(!solution.eof() && solution >> ch && ch != 'v') continue;
//     // while(!solution.eof() && solution >> p && p != 't') {  //ascii(t) == 116 
//     // char buff[50000];
//     // solution.getline(buff, 50000, '\n');
//     for(int i = 0; i < varNum; i++) { 
//         // sscanf(buff, "%d", &p);
//         solution >> p;
//         int abs_p = abs(p);
//         if(p > 0) 
//             result[abs_p] = true;
//         else if (p < 0) 
//             result[abs_p] = false;
//     };
//     if(S.Verify(result)) 
//         std::cout << "\nverify : main() : CNF可满足性验证成功！"; 
//     else 
//         std::cout << "\nverify : main() : CNF可满足性验证失败！";
//     solution.close();
//     return 0;
// }