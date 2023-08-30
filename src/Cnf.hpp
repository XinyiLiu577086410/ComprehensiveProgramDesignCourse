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

#ifndef SUCCESS
#define SUCCESS 0
#endif

#ifndef ERROR
#define ERROR -1
#endif

#ifndef CNF_MEM_INCR
#define CNF_MEM_INCR 300
#endif

class Cnf {
public:
    // 静态成员变量
    static unsigned int countCases;                 // 记录实例数
    static unsigned int countDpllCalls;             // 记录Dpll()调用次数
    
    // 构造函数、析构函数
    Cnf();
    ~Cnf();
   
    // 功能函数
    int Add(Vector &);                              // 添加子句
    int Read(std::string);                          // 读取CNF文件
    void Resize(int);                               // 重新分配内存
    int GetVariableNum(void) const;                 // 返回CNF文件的变元数信息
    int GetFirstLiteral ()const;                    // 返回第一个子句的第一个文字
    bool HaveUnitClause  (void)const;               // 判断子句集是否含有单子句
    bool Empty  (void)const;                        // 判断子句集是否为空集
    bool HaveEmpty (void)const;                     // 判断子句集是否含有空子句
    // bool Find (int) const;                          // 判断子句集是否含有指定的文字
    int FindUnitClause (void)const;                 // 从最后一个子句找出一个单子句
    int Select (void)const;                         // 选择第一个子句的第一个文字
    int Select (int)const;                          // 选择出现次数最多的变元
    bool Verify (bool [])const;                     // 验证求解正确性
    void Show (void)const;                          // 展示各个子句
    bool Dpll(bool [], int);                        // 用DPLL算法求解SAT问题
    void Inverse(Step);
    friend void threadInterface(Cnf &, bool [], int, bool &, double &);
                                                    // 线程接口函数
private:
    Vector * clauses;                               // 数据域
    int length, use;                                // 子句个数
    int clausesNum;                                 // CNF文件中子句数信息
    int variableNum;                                // CNF文件中变元数信息
    int size;                                       // 内存大小
    int * associationTable;                         // 变元关联表，统计每个变量与子句关联的次数
};


// 初始化静态成员变量
unsigned int Cnf::countCases = 0;
unsigned int Cnf::countDpllCalls = 0;


Cnf::Cnf() { 
    countCases++;                                   // 更新计数器
    clauses = nullptr; 
    length = 0;    
    use = 0; 
    clausesNum = 0;
    variableNum = 0; 
    size = 0;  
    associationTable = nullptr;
}


Cnf::~Cnf() { 
    if(clauses !=  nullptr) {
        delete[] clauses; 
        clauses = nullptr;
    }
    if(associationTable !=  nullptr) {
        delete[] associationTable; 
        associationTable = nullptr;
    }
}

void Cnf::Inverse(Step stp) {
    switch (stp.operation)
    {
    case 0:
        clauses[stp.cla].Enable();
        length++;
        break;
    case 1:
        clauses[stp.cla].EnableLiteral(stp.lit);
        break;
    case 2:
        clauses[stp.cla].Disable();
        length--;
        break;
    default:
        break;
    }
}


int Cnf::Add (Vector & newClause) {
    if(use == size) Resize(size + CNF_MEM_INCR);
    if(size < use) {
        std::cout << "\nCnf.hpp : Cnf::Add() : 数据越界，堆损坏！";
        exit(-1);
    }
    clauses[use] = newClause;
    int newClauseSize = newClause.GetSize();
    for(int i = 0; i < newClauseSize; i++) {
        if(newClause[i].GetStatus())
            associationTable[newClause[i].GetLiteral() + variableNum]++;
    }
    length++;
    use++;
    return SUCCESS;
}


int Cnf::Read (std::string filename) {
    std::ifstream file;
    file.open(filename);
    if(!file.is_open()) {
        std::cout << "\nCnf.hpp : Cnf::Read() : 打开输入文件失败：" << filename;
        exit(-1);
    }
    else {
        std::cout << "\nCnf.hpp : Cnf::Read() : 打开输入文件成功：" << filename;
    }   
    char ch;
    while(file >> ch && ch == 'c') {
        char buff[1024];
        file.getline(buff, 1024, '\n');
    }
    std::string type;
    file >> type >> variableNum >> clausesNum;
    Resize(clausesNum);

    associationTable = new (std::nothrow) int[2 * variableNum + 1];
    assert(associationTable != nullptr);
    memset(associationTable, 0, sizeof(int) * (variableNum * 2 + 1));
   

    int p[100000];
    for (int i = 0; i < clausesNum; i++) {
        int j = 0;
        file >> p[j];
        while (p[j++]) {
            file >> p[j];
        }
        Vector vectorToAppend;
        for(int k = 0; k < j - 1; k++) {
            vectorToAppend.Add(p[k]);
        }
        Add(vectorToAppend);
    }
    return SUCCESS;
}


void Cnf::Resize(int newSize) {
    if(newSize <= 0) {
        std::cout << "\nCnf.hpp : Cnf::Resize() : 重新分配内存失败，内存大小不能小于等于0！";  
        exit(-1);
    }
    if(newSize <= use) {
        std::cout << "\nCnf.hpp : Cnf::Resize() : 重新分配内存失败，内存大小不能小于已有有效数据所占大小！";  
        exit(-1);
    }
    Vector * newSpace = new (std::nothrow) Vector[newSize];
    assert(newSpace != nullptr);
    if(clauses) {
        for(int i = 0; i < length; i++){ 
            newSpace[i] = clauses[i];
        }
        delete[] clauses;
        clauses = nullptr;
    }
    clauses = newSpace;
    size = newSize;
}


int Cnf::GetVariableNum(void) const {
    return variableNum; 
}


int Cnf::GetFirstLiteral () const { 
    for(int i = 0; i < size; i++)
        if(clauses[i].GetStatus() && !clauses[i].Empty())
            return clauses[i].GetFirstLiteral();
    return 0;
}


bool Cnf::HaveUnitClause (void) const {
    int i;
    for(i = length - 1; i >= 0; i--){
        if(clauses[i].IsUnit()) return true;
    }
    return false;
}


bool Cnf::Empty (void) const {
    return length == 0;
}


bool Cnf::HaveEmpty (void) const {
    int i;
    for(i = length - 1; i >= 0; i--){
        if(clauses[i].Empty()) return true;
    }
    return false;
}


int Cnf::FindUnitClause (void) const {
    int i;
    for(i = length - 1; i >= 0; i--){
        if(clauses[i].IsUnit()) {
            return clauses[i].GetFirstLiteral();
        }
    }
    return 0; // 用0代表没有找到单子句。
}


int Cnf::Select (int a) const {
    int mostFrequentLiteral = Select(); // 缺省值
    for(int i = -variableNum; i <= variableNum; i++) {
        if(i == 0) continue;
        if(associationTable[i + variableNum] > associationTable[mostFrequentLiteral + variableNum]) {
            mostFrequentLiteral = i;
        }
    }
    return mostFrequentLiteral; 
}


int Cnf::Select (void) const {
    return GetFirstLiteral();
}


void Cnf::Show (void) const {
    std::cout << "\nCnf.hpp : Cnf::Show() : 显示Cnf结构：";
    std::cout << "\nCnf.hpp : Cnf::Show() : 子句总数："<<length;
    uint64_t hashcode1 = 0, hashcode2 = 0;
    for(int i = 0; i < size; i++){
        std::cout << "\nCnf.hpp : Cnf::Show() : NO." << i + 1 << " : ";
        if(clauses[i].GetStatus()) {
            clauses[i].Show();
            u_int64_t hhashcode1 = 1;
            u_int64_t hhashcode2 = 0;
            int clauseLength = clauses[i].GetLength();
            for (int j = 0; j < clauseLength; j++)
            {
                hhashcode1 *= clauses[i][j].GetLiteral() + clauseLength;
                hhashcode2 += clauses[i][j].GetLiteral() * clauseLength;
            }
            hashcode1 += hhashcode1;
            hashcode2 += hhashcode2;
        }
    }
    std::cout << "\nCnf.hpp : Cnf::Show() : Cnf散列值：" << hashcode1 * hashcode2;
    std::cout << "\nCnf.hpp : Cnf::Show() : 显示结束\n";
}


bool Cnf::Verify (bool result[]) const {
    for(int i = 0; i < length; i++)
        if(clauses[i].GetStatus() && !clauses[i].Verify(result)){
            //输出的序号从1开始
            std::cout << "\nCnf.hpp : Cnf::Verify() : 不满足的子句：" << "**NO." << i+1 << "** : "; 
            clauses[i].Show();
            return false;
        }
    return true;
}


// Dpll 的辅助变量
bool error = false;

bool Cnf::Dpll (bool solution[], int deepth) {
    if(deepth > variableNum) {
        std::cout << "\nCnf.hpp : Cnf::dpll() : 现在深度是" << deepth <<" ， 递归深度过深，大于变元数，程序终止！";
        error = true;   // 检查程序出错，如果递归过深，由(*error)处原路返回
        return false;   
    }
    countDpllCalls++;
    MyStack toInverse;  // 反演栈
    // 单子句规则
    int unit;
    while((unit = FindUnitClause()) != 0) {
        if(unit > 0) solution[unit] = true; else solution[-unit] = false;
        // 化简正文字（literal）
        for (int i = 0; i < size; i++) {
            if(clauses[i].GetStatus() && clauses[i].Find(unit) != ERROR) {
                clauses[i].Disable(); // 删除子句
                length--;
                Step stp = {0, i, 0};
                toInverse.Push(stp);
            }
        }
        // 化简负文字（-literal）
        for (int i = 0; i < size; i++){
            if (clauses[i].GetStatus() && clauses[i].Find(-unit) != ERROR ) {
                clauses[i].DisableLiteral(-unit);
                Step stp = {1, i, -unit};
                toInverse.Push(stp);
            }
        }
        if(Empty()) {
            // 反演操作进行回溯
            while (!toInverse.Empty()) {
                Inverse(toInverse.Pop());
            }
            std::cout << "\nCnf.hpp : Cnf::dpll() : 找到解时的递归深度：" << deepth;
            return true; // 递归终点
        }
        if(HaveEmpty()) { 
            // 回溯
            while (!toInverse.Empty()) {
                Inverse(toInverse.Pop());
            }
            return false; // 递归终点
        }
    }
    // 选取分支变元
    int p = Select(0);
    Vector v1, v2;
    v1.Enable();
    v2.Enable();
    // 构造单子句
    v1.Add(p);
    // 添加单子句{p}
    Step stp = {2, use, p};
    toInverse.Push(stp);
    Add(v1);
    // 求解分支 S + {p}
    if (Dpll(solution, deepth + 1)) {
        // 回溯
        while (!toInverse.Empty()) {
            Inverse(toInverse.Pop());
        }
        return true;
    }
    else { 
        // 返回false一律检查error
        if(error) {  // (*error)
           return false;
        }
        // 删除单子句{p}
        Inverse(toInverse.Pop());
        // 构造单子句{-p}
        v2.Add(-p);
        // 添加单子句{-p}
        Add(v2);
        Step stp = {2, use, -p};
        toInverse.Push(stp);
        // 求解S + {-p}
        bool sat = Dpll(solution, deepth + 1);
        // 回溯并返回
        while (!toInverse.Empty()) {
            Inverse(toInverse.Pop());
        }  
        return sat;
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