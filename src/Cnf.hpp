#ifndef CNF
#define CNF

#include <fstream>
#include <cstring>
#include <iostream>
#include <algorithm>
#include <cassert>
#include <climits>
#include <chrono>
#include <utility>
#include "step.hpp"
#include "myStack.hpp"
#include "vector.hpp"
#include "bitmap.hpp"

#ifndef SUCCESS
#define SUCCESS 0
#endif

#ifndef ERROR
#define ERROR -1
#endif

#define CNF_MEM_INCR 300

class Cnf {
public:
    // 静态成员变量
    static unsigned int countCases;                 // 记录实例数
    static unsigned int countDpllCalls;             // 记录Dpll()调用次数
    
    // 构造函数、析构函数
    Cnf();
    ~Cnf();
   
    // 功能函数
    int Add(Vector<int> &);                         // 添加子句
    int Read(std::string);                          // 读取CNF文件
    void Resize(int);                               // 重新分配内存
    int GetVariableNum(void) const;                 // 返回CNF文件的变元数信息
    bool HaveUnitClause(void) const;               // 判断子句集是否含有单子句
    bool Empty (void)const;                        // 判断子句集是否为空集
    bool HaveEmpty (void)const;                     // 判断子句集是否含有空子句
    int FindUnitClause (void)const;                 // 从最后一个子句找出一个单子句
    int Select (void)const;                         // 选择第一个子句的第一个文字
    bool Dpll(bool [], int);                        // 用DPLL算法求解SAT问题
    void Inverse(Step);                             // 按Step的内容进行逆操作
    void EnableClause(int);                         // 将一个子句标记为有效（恢复）
    void DisableClause(int);                        // 将一个子句标记为无效（删除）
    void EnableLiteralInClause(int, int);           // 将一个子句中特定文字标记为有效（恢复）
    void DisableLiteralInClause(int, int);          // 将一个子句中特定文字标记为无效（删除）
    bool GetClauseStatus(int) const;                      // 查询子句是否被满足
    bool GetLiteralStatus(int, int) const;          // 查询文字是否被删除
    int GetFirstLiteral(int) const;
    friend void threadInterface(Cnf &, bool [], int, bool &, double &);     // 线程入口函数                     
private:
    Vector<Vector<int>> clauses;                        // 子句域

    int length = 0;                                     // 子句数
    int unsat = 0;                                      // 未满足子句数量
    int clausesNum = 0;                                 // 初始子句数
    int variableNum = 0;                                // 初始变元数
    int clauseMaxLength = 0;                            // 向量最大长度
    int size = 0;                                       // 内存大小
    // int haveEmpty = -1;
    // int single = -1;
    unsigned char * LiteralBitmap = nullptr;           // 文字位图
    unsigned char * clauseBitmap = nullptr;             // 子句位图
    int * clauseLength = nullptr;                       // 各子句未满足文字数量
    Vector<Vector<std::pair<int, int>>> whereTheLiteralIs;             // 变元会出现在哪些子句
};


// 初始化静态成员变量
unsigned int Cnf::countCases = 0;
unsigned int Cnf::countDpllCalls = 0;


Cnf::Cnf() { 
    countCases++;                                          // 更新对象实例计数器
}


Cnf::~Cnf() {
    if(LiteralBitmap!=nullptr) delete[] LiteralBitmap;
    if(clauseBitmap!=nullptr) delete[] clauseBitmap;
    if(clauseLength!=nullptr) delete[] clauseLength;
}


void Cnf::Inverse(Step stp) {
    switch (stp.operation) {
    case 0:
        EnableClause(stp.clau);
        break;

    case 1:
        EnableLiteralInClause(stp.clau, stp.lit);
        break; 

    case 2:
        DisableClause(stp.clau);
        break;

    default:
        std::cout << "cnf.hpp : Cnf::Inverse() : Bad operation!";
        exit(-1);
        break;
    }
}


int Cnf::Add (Vector<int> & newClause) {
    if(length == size) Resize(size + CNF_MEM_INCR);
    // else if(size < length) {
    //     std::cout << "\nCnf.hpp : Cnf::Add() : 数据越界，堆损坏！";
    //     exit(-1);
    // }
    // clauses[length] = newClause; //2023.9.6 递归BUG：非法操作>错误长度>选取错误的子句中的决策文字>l == 0>无穷递归
    clauses.Add(newClause);
    int len = clauses[length].Length();
    // #pragma unroll 2
    for(int i = 0; i < len; i++){
        std::pair<int,int> posV2(length, i);
        whereTheLiteralIs[clauses[length][i]+variableNum].Add(posV2);  
    }
    clauseLength[length] = clauses[length].Length();
    length++;
    unsat++;
    return SUCCESS;
}


void Cnf::Resize(int newSize) {
    // if(newSize <= 0) {
    //     std::cout << "\nCnf.hpp : Cnf::Resize() : 重新分配内存失败，内存大小不能小于等于0！";  
    //     exit(-1);
    // }
    // if(newSize <= length) {
    //     std::cout << "\nCnf.hpp : Cnf::Resize() : 重新分配内存失败，内存大小不能小于已有有效数据所占大小！";  
    //     exit(-1);
    // }
    clauses.Resize(newSize);
    size = newSize;

    // unsigned char * LiteralBitmap
    unsigned char * tmp1 = new unsigned char[size*clauseMaxLength/8+100];
    memset(tmp1, 0xff, sizeof(unsigned char)*(size*clauseMaxLength/8+100));
    if(LiteralBitmap != nullptr) {
        memcpy(tmp1, LiteralBitmap, sizeof(unsigned char)*(length*clauseMaxLength/8+1));
        delete[] LiteralBitmap;
    }
    LiteralBitmap = tmp1;

    // unsigned char * clauseBitmap
    unsigned char * tmp2 = new unsigned char[size/8+100];
    memset(tmp2, 0xff, sizeof(unsigned char)*(size/8+100));
    if(clauseBitmap != nullptr){
        memcpy(tmp2, clauseBitmap, sizeof(unsigned char)*(length/8+1));
        delete[] clauseBitmap;
    }
    clauseBitmap = tmp2;

    // int * clauseLength
    int * tmp3 = new int[size+100];
    memset(tmp3, 0, sizeof(unsigned char)*(size+100));
    if(clauseLength) {    
        memcpy(tmp3, clauseLength, sizeof(int)*(length));
        delete[] clauseLength;
    }
    clauseLength = tmp3;
}


int Cnf::GetVariableNum(void) const {
    return variableNum; 
}


bool Cnf::Empty (void) const {
    return unsat == 0;
}


bool Cnf::HaveEmpty (void) const {
    // #pragma unroll 8 
    for(int i = 0; i < length; i++) {
        if(GetClauseStatus(i) && clauseLength[i] == 0) {
            return true;
        }

    }
    return false;
}


int Cnf::FindUnitClause (void) const {
    // #pragma unroll 8
    for(int i = 0; i < length; i++)
        if(GetClauseStatus(i) && clauseLength[i] == 1) {
            if(clauses[i].Empty()){
                std::cout << "\nclauseLength[] damaged!  clauseLength[i] == " << clauseLength[i] << " i == " << i <<" clauses[i].size == "<<clauses[i].Size()<<"clauses[i].length == "<<clauses[i].Length()<<"clauses[i].elem == "<< clauses[i].SwapElem();
                exit(-1);
            }
            return GetFirstLiteral(i);
        }
    return 0;
    // 不是return -1
}

int Cnf::GetFirstLiteral(int pos) const{
    if(clauses[pos].Empty()) {
        std::cout << "cnf.hpp:Cnf::GetFirstLiteral():子句[pos]为空！";
        exit(-1);
    }
    if(clauseLength[pos] == 0) {
        std::cout << "cnf.hpp:Cnf::GetFirstLiteral():子句[pos]的文字全被删除！";
        exit(-1);
    }
    if(!GetClauseStatus(pos)) {
        std::cout << "\nCan't get first literal from a disabled clause!";
        exit(-1);
    }
    // #pragma unroll 8
    for(int i = 0; i < clauses[pos].Length(); i++) 
        if(GetLiteralStatus(pos, i))
            return clauses[pos][i];
    std::cout << "\ncnf.hpp:Cnf::GetFirstLiteral():找不到未被删除的文字。";
    exit(-1);
    // return 0;
}

int Cnf::Select (void) const {
    // #pragma unroll 8
    for(int i = 0; i < length; i++)
        if(GetClauseStatus(i) && clauseLength[i] != 0)
            return GetFirstLiteral(i);
    return 0;
}

inline void Cnf::EnableClause(int clau) {
    // std::cout << "\nEnableClauses() : I am called!!";
    // if(GetClauseStatus(clau)){
    //     std::cout << "\ncnf.hpp : Cnf::EnableClauses() : Bad status";
    //     exit(-1);
    // }
    // if(clau < 0 || clau >= length) {
    //     std::cout << "\ncnf.hpp : Cnf::EnableClauses() : Bad Index : " << clau << "\n"; 
    //     exit(-1);
    // }
    // std::cout << "\nBefore EnableClause(): clauseBitmap[clau/8] = "<< (int) clauseBitmap[clau/8]<<" And clau == "<<clau;
    clauseBitmap[clau/8] |= masks[clau % 8];  
    // std::cout << "\nAfter : clauseBitmap[clau/8] = "<< (int) clauseBitmap[clau/8];
    unsat++;
}


inline void Cnf::DisableClause(int clau) {
    // std::cout << "\nDisableClauses() : I am called!!";
    // if(!GetClauseStatus(clau)){
    //     std::cout << "\ncnf.hpp : Cnf::DisableClauses() : Bad status";
    //     exit(-1);
    // }
    // if(clau < 0 || clau >= length) {
    //     std::cout << "\ncnf.hpp : Cnf::DisableClause() : Bad Index : " << clau << "\n"; 
    //     exit(-1);
    // }
    // std::cout << "\nBefore DisableClause(): clauseBitmap[clau/8] = "<< (int) clauseBitmap[clau/8]<<" And clau == "<<clau;
    clauseBitmap[clau/8] &= ~masks[clau % 8];  
    // std::cout << "\nAfter : clauseBitmap[clau/8] = "<< (int) clauseBitmap[clau/8];
    unsat--;
}


inline void Cnf::EnableLiteralInClause(int clau, int lit) {
    //  if(!GetClauseStatus(clau)||GetLiteralStatus(clau, lit)){
    //     std::cout << "\ncnf.hpp : Cnf::EnableLiteralInClause() : Bad status";
    //     exit(-1);
    // }
    // if(clau < 0 || clau >= length) {
    //     std::cout << "\ncnf.hpp : Cnf::EnableLiteralInClause() : Bad Index : " << clau << "\n"; 
    //     exit(-1);
    // }
    int pos = (clau*clauseMaxLength+lit);
    LiteralBitmap[pos/8] |= masks[pos % 8];
    clauseLength[clau]++;
}


inline void Cnf::DisableLiteralInClause(int clau, int lit) {
    // if(!GetClauseStatus(clau)||!GetLiteralStatus(clau, lit)){
    //     std::cout << "\ncnf.hpp : Cnf::DisableLiteralInClause() : Bad status";
    //     exit(-1);
    // }
    // if(clau < 0 || clau >= length) {
    //     std::cout << "\ncnf.hpp : Cnf::DisableLiteralInClause() : Bad Index : " << clau << "\n"; 
    //     exit(-1);
    // }
    int pos = (clau*clauseMaxLength+lit);
    LiteralBitmap[pos/8] &= ~masks[pos % 8];  
    clauseLength[clau]--;
}


inline bool Cnf::GetClauseStatus(int clau) const{
    // if(clau < 0 || clau >= length) {
    //     std::cout << "\ncnf.hpp : Cnf::GetClauseStatus() : Bad Index : " << clau << "\n"; 
    //     exit(-1);
    // }
    if(clauseBitmap[clau/8] & masks[clau%8]) 
        return true;
    else 
        return false;
}


inline bool Cnf::GetLiteralStatus(int clau, int lit) const{
    // if(!GetClauseStatus(clau)) {
    //     std::cout << "\ncnf.hpp : Cnf::GetLiteralStatus() : Bad Clause Status";
    //     exit(-1);
    // // }
    // if(clau < 0 || clau >= length) {
    //     std::cout << "\ncnf.hpp : Cnf::GetLiteralStatus() : Bad Index : " << clau << "\n"; 
    //     exit(-1);
    // }
    int pos = (clau*clauseMaxLength+lit);
    if(LiteralBitmap[pos/8] & masks[pos%8])
        return true;
    else 
        return false;
}


int Cnf::Read (std::string filename) {
    std::ifstream file;
    file.open(filename);
    if(!file.is_open()) {
        std::cout << "\nCnf.hpp : Cnf::Read() : 打开输入文件失败：" << filename;
        exit(-1);
    } 
    char ch;
    while(file >> ch && ch == 'c') {
        char buff[1024];
        file.getline(buff, 1024, '\n'); // 过滤注释
    }
    std::string type;
    file >> type >> variableNum >> clausesNum;
    whereTheLiteralIs.Resize(variableNum*2+1);
    int buf[clausesNum][100];
    for (int i = 0; i < clausesNum; i++) {
        int j = 0;
        file >> buf[i][j];
        while (j<100 && buf[i][j++]) file >> buf[i][j];
        int len = j - 1;
        if(len > clauseMaxLength)
            clauseMaxLength = len;
    }
    Resize(clausesNum + CNF_MEM_INCR);      // 注意和clauseMaxNum的次序
    for (int i = 0; i < clausesNum; i++) {
        int j = 0;
        while (j<100 && buf[i][j++]);
        Vector<int> vectorToAppend;
        for(int k = 0; k <= j - 2; k++) {
            vectorToAppend.Add(buf[i][k]);
        }
        Add(vectorToAppend);
    }
    return SUCCESS;
}


// Dpll() 的辅助变量
bool error = false, outOfTime = false;
bool Cnf::Dpll (bool solution[], int deepth = 0) {
    if(outOfTime) exit(-1);
    if(deepth > variableNum) {
        std::cout << "\nCnf.hpp : Cnf::dpll() : (ERROR)现在深度是" << deepth <<" ， 递归深度过深，大于变元数，程序终止！";
        error = true;   // 检查程序出错，如果递归过深（往往由回溯部分的错误引起），打开错误标记
        return false;   
    }
    countDpllCalls++;
    if(length != clauses.Length()){
        std::cout << "\nlength != clauses.Length(), length == " << length <<" , clauses.Length() == "<<clauses.Length();
    }
    for(int i = 0; i < length; i++){
        if(clauseLength[i] != clauses[i].Length()){
            std::cout << "\nclauseLength[i] != clauses[i].Length(), i == "<< i << " , clauseLength[i] == "<<clauseLength[i]<<" , clauses[i].Length()" << clauses[i].Length();
    }
    }
    MyStack<Step> toInverse;  //    反演栈，利用栈的FILO特性实现回溯（即操作反演：逆序进行逆操作）
    /*  单子句规则  */
    int unit;
    std::cout << "\nEntering dpll, deepth:" << deepth;
    while((unit = FindUnitClause()) != 0) {
        //  记录赋值
        std::cout << "\n Before, unit == " << unit;
        if(unit > 0) 
            solution[unit] = true;
        else 
            solution[-unit] = false;

        //  化简正文字（literal）
        int len1 = whereTheLiteralIs[unit+variableNum].Length();
        for(int i = 0; i < len1; i++) {
            std::pair<int, int> where = whereTheLiteralIs[unit+variableNum][i];
            if(GetClauseStatus(where.first)     &&
                        GetLiteralStatus(where.first, where.second)) 
            {
                DisableClause(where.first);
                toInverse.Push({0, where.first, -1});
            }
        }

        //  化简负文字（-literal）
        int len2 = whereTheLiteralIs[-unit+variableNum].Length();
        for(int i = 0; i < len2; i++) {
            std::pair<int, int> where = whereTheLiteralIs[-unit+variableNum][i];
            if(GetClauseStatus(where.first) 
                        &&  GetLiteralStatus(where.first, where.second)
            ) {
                DisableLiteralInClause(where.first, where.second);
                toInverse.Push({1, where.first, where.second});
            }

        }
        std::cout << "\nAfter, unsat == " << unsat;
        if(Empty()) 
            return true; // 递归终点

        if(HaveEmpty()) { 
            while (!toInverse.Empty()) {
                Inverse(toInverse.Pop()); 
            }
            return false; // 递归终点
        }
    }

    Vector<int> v1, v2;         // 新的单子句
    int l = abs(Select());      // 选取分支变元
    if(l == 0) { 
        std::cout << "\ncnf.hpp : Cnf::dpll() : Bad l!";
        exit(-1);
    }
    std::cout << "\nselected l == " << l ;
    v1.Add(l);                  // 构造单子句
    Step stp = {2, length, -1};  // 构造栈帧
    toInverse.Push(stp);        // 压栈
    Add(v1);                    // 添加单子句{l}

    /*  分裂规则  */
    if (Dpll(solution, deepth + 1))    // 求解分支 S + {l}
        return true;                   // 递归终点
    else { 
        if(error) return false;        // 返回false一律检查error（检查是否递归过深）
        Inverse(toInverse.Pop());      // 删除单子句{l}
        v2.Add(-l);                    // 构造单子句{-l}
        Step stp = {2, length, -1};    // 在调用Cnf::Add()之前压栈，才能保存正确的length
        toInverse.Push(stp);
        Add(v2);                       // 添加单子句{-l}
        bool sat = Dpll(solution, deepth + 1);                      // 求解S + {-l}
        if(sat == false) {        
            while (!toInverse.Empty()) Inverse(toInverse.Pop());    // 仅当返回false回溯
        }
        return sat;                     // 递归终点
    }
}


void threadInterface(Cnf & obj, bool solution[], int deepth, bool & returnValue, double & miniSecTime, bool & taskFinished) {
    taskFinished = false;
    /* 计时开始 */    std::chrono::steady_clock::time_point t0 = std::chrono::steady_clock::now();       
    returnValue = obj.Dpll(solution, deepth);
    /* 计时结束 */   std::chrono::steady_clock::time_point t1 = std::chrono::steady_clock::now();        
    auto duration = std::chrono::duration_cast< std::chrono::duration<double> >(t1 - t0);
    miniSecTime =  duration.count() * 1000;
    taskFinished = true;
}

#endif