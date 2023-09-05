#ifndef CNF
#define CNF

#include <fstream>
#include <cstring>
#include <iostream>
#include <algorithm>
#include <cassert>
#include <climits>
#include <chrono>
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
    bool HaveUnitClause  (void)const;               // 判断子句集是否含有单子句
    bool Empty  (void)const;                        // 判断子句集是否为空集
    bool HaveEmpty (void)const;                     // 判断子句集是否含有空子句
    int FindUnitClause (void)const;                 // 从最后一个子句找出一个单子句
    int Select (void)const;                         // 选择第一个子句的第一个文字
    bool Dpll(bool [], int);                        // 用DPLL算法求解SAT问题
    void Inverse(Step);                             // 按Step的内容进行逆操作
    void EnableClause(int);                         // 将一个子句标记为有效（恢复）
    void DisableClause(int);                        // 将一个子句标记为无效（删除）
    void EnableLiteralInClause(int, int);           // 将一个子句中特定文字标记为有效（恢复）
    void DisableLiteralInClause(int, int);          // 将一个子句中特定文字标记为无效（删除）
    bool GetClauseStatus(int);
    bool GetClauseLiteralStatus(int, int);
    friend void threadInterface(Cnf &, bool [], int, bool &, double &);     // 线程入口函数                     
private:
    Vector<Vector<int>> clauses;                        // 子句域

    int length = 0;                                     // 子句数
    int unsat = 0;                                      // 未满足子句数量
    int clausesNum = 0;                                 // 初始子句数
    int variableNum = 0;                                // 初始变元数
    int clauseMaxLength = 0;                            // 向量最大长度
    int size = 0;                                       // 内存大小

    unsigned char * variableBitmap = nullptr;           // 文字位图
    unsigned char * clauseBitmap = nullptr;             // 子句位图
    int * clauseLength = nullptr;                       // 各子句未满足文字数量
};


// 初始化静态成员变量
unsigned int Cnf::countCases = 0;
unsigned int Cnf::countDpllCalls = 0;


Cnf::Cnf() { 
    countCases++;                                          // 更新对象实例计数器
}


Cnf::~Cnf() {}


void Cnf::Inverse(Step stp) {

}


int Cnf::Add (Vector<int> & newClause) {
    if(length == size) Resize(size + CNF_MEM_INCR);
    else if(size < length) {
        std::cout << "\nCnf.hpp : Cnf::Add() : 数据越界，堆损坏！";
        exit(-1);
    }
    clauses[length] = newClause; // newClause 拥有的内存被交换过来并设newClause为nullptr
    EnableClause(length);
    length++;
    return SUCCESS;
}


void Cnf::Resize(int newSize) {
    if(newSize <= 0) {
        std::cout << "\nCnf.hpp : Cnf::Resize() : 重新分配内存失败，内存大小不能小于等于0！";  
        exit(-1);
    }
    if(newSize <= length) {
        std::cout << "\nCnf.hpp : Cnf::Resize() : 重新分配内存失败，内存大小不能小于已有有效数据所占大小！";  
        exit(-1);
    }
    clauses.Resize(newSize);
    size = newSize;
   
    // unsigned char * variableBitmap
    unsigned char * tmp1 = new unsigned char[size*clauseMaxLength/8+1];
    memcpy(tmp1, variableBitmap, sizeof(unsigned char)*(length*clauseMaxLength/8+1));
    delete[] variableBitmap;
    variableBitmap = tmp1;
    // unsigned char * clauseBitmap
    unsigned char * tmp2 = new unsigned char[size/8+1];
    memcpy(tmp2, clauseBitmap, sizeof(unsigned char)*(length/8+1));
    delete[] clauseBitmap;
    clauseBitmap = tmp2;
    // int * clausesUnsat
    int * tmp3 = new int[size];
    memcpy(tmp3, clauseLength, sizeof(int)*(length));
    delete[] clauseLength;
    clauseLength = tmp3;
}


int Cnf::GetVariableNum(void) const {
    return variableNum; 
}


bool Cnf::Empty (void) const {
    return length == 0;
}


bool Cnf::HaveEmpty (void) const {
    for(int i = 0; i < length; i++)
        if(GetClauseStatus(i) && clauseLength[i]) {
            return true;
        }
    return false;
}


int Cnf::FindUnitClause (void) const {

}


int Cnf::Select (void) const {
    
}

inline void Cnf::EnableClause(int pos) {
    clauseBitmap[pos/8] |= masks[pos % 8];  
    length++;
}


inline void Cnf::DisableClause(int pos) {
    clauseBitmap[pos/8] &= ~masks[pos % 8];  
    length--;
}


inline void Cnf::EnableLiteralInClause(int clau, int lit) {
    variableBitmap[(clau * clauseMaxLength + lit) / 8] |= masks[lit % 8];
    clauseLength[clau]++;
}


inline void Cnf::DisableLiteralInClause(int clau, int lit) {
    variableBitmap[(clau * clauseMaxLength + lit) / 8] &= ~masks[lit % 8];  
    clauseLength[clau]--;
}


inline bool Cnf::GetClauseStatus(int pos) {
    if(clauseBitmap[pos/8] && masks[pos%8]) 
        return true;
    else 
        return false;
}


inline bool Cnf::GetClauseLiteralStatus(int clau, int lit) {
    if(clauseBitmap[(clau*clauseMaxLength+lit)/8] && masks[lit%8]) 
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
    else {
        // std::cout << "\nCnf.hpp : Cnf::Read() : 打开输入文件成功：" << filename;
    }   
    char ch;
    while(file >> ch && ch == 'c') {
        char buff[1024];
        file.getline(buff, 1024, '\n'); // 过滤注释
    }
    std::string type;
    file >> type >> variableNum >> clausesNum;
    Resize(clausesNum);

    clausesUnsat = new int[clausesNum];

    // relateTable = new (std::nothrow) Vector<int>[2 * variableNum + 1];
    // assert(relateTable != nullptr);
   
    int p[100000];
    for (int i = 0; i < clausesNum; i++) {
        int j = 0;
        file >> p[j];
        while (p[j++]) {
            file >> p[j];
        }
        Vector<int> vectorToAppend;
        for(int k = 0; k <= j - 2; k++) {
            vectorToAppend.Add(p[k]);
        }
        int len;
        if((len = vectorToAppend.Length()) > clauseMaxLength)
            clauseMaxLength = len;
        Add(vectorToAppend);
    }
    variableBitmap = new unsigned char[clauseMaxLength*size/8+1];
    clauseBitmap = new unsigned char[size/8+1];
    memset(variableBitmap, 0xFF, (clauseMaxLength*clausesNum/8+1)*sizeof(unsigned char));
    memset(clauseBitmap, 0xFF, (clausesNum/8+1)*sizeof(unsigned char));
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
    
    MyStack<Step> toInverse;  //    反演栈，利用栈的FILO特性实现回溯（即操作反演：逆序进行逆操作）
    /*  单子句规则  */
    int unit;
    while((unit = FindUnitClause()) != 0) {
        //  记录赋值
        if(unit > 0) 
            solution[unit] = true;
        else 
            solution[-unit] = false;
        //  化简正文字（literal）
        
        //  化简负文字（-literal）
        
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
    int p = abs(Select());      // 选取分支变元
    v1.Add(p);                  // 构造单子句
    Step stp = {2, length, 0};  // 构造栈帧
    toInverse.Push(stp);        // 压栈
    Add(v1);                    // 添加单子句{p}

    /*  分裂规则  */
    if (Dpll(solution, deepth + 1))    // 求解分支 S + {p}
        return true;                   // 递归终点
    else { 
        if(error) return false;        // 返回false一律检查error（检查是否递归过深）
        Inverse(toInverse.Pop());      // 删除单子句{p}
        v2.Add(-p);                    // 构造单子句{-p}
        Step stp = {2, length, -p};    // 在调用Cnf::Add()之前压栈，才能保存正确的use
        toInverse.Push(stp);
        Add(v2);                       // 添加单子句{-p}
       
        bool sat = Dpll(solution, deepth + 1);                      // 求解S + {-p}
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