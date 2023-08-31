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
    int Add(Vector &);                              // 添加子句
    int Read(std::string);                          // 读取CNF文件
    void Resize(int);                               // 重新分配内存
    int GetVariableNum(void) const;                 // 返回CNF文件的变元数信息
    int GetFirstLiteral ()const;                    // 返回第一个子句的第一个文字
    bool HaveUnitClause  (void)const;               // 判断子句集是否含有单子句
    bool Empty  (void)const;                        // 判断子句集是否为空集
    bool HaveEmpty (void)const;                     // 判断子句集是否含有空子句
    // bool Find (int) const;                           // 判断子句集是否含有指定的文字
    int FindUnitClause (void)const;                 // 从最后一个子句找出一个单子句
    int Select (void)const;                         // 选择第一个子句的第一个文字
    // int Select (int)const;                          // 选择出现次数最多的变元
    bool Verify (bool [])const;                     // 验证求解正确性
    void Show (void)const;                          // 展示各个子句
    bool Dpll(bool [], int);                        // 用DPLL算法求解SAT问题
    void Inverse(Step);
    void EnableClause(int);
    void DisableClause(int);
    void EnableLiteralInClause(int, int);
    void DisableLiteralInClause(int, int);
    Literal Find(int lit);
    friend void threadInterface(Cnf &, bool [], int, bool &, double &);
                                                    // 线程接口函数
private:
    Vector * clauses;                               // 数据域
    int length, use;                                // 子句个数, 子句总数（含标记为被删除的子句）
    int clausesNum;                                 // CNF文件中子句数信息
    int variableNum;                                // CNF文件中变元数信息
    int size;                                       // 内存大小
    Vector * relateTable;
    // int * associationTable;                         // 变元关联表，统计每个变量与子句关联的次数
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
    // associationTable = nullptr;
    relateTable = nullptr;
}


Cnf::~Cnf() { 
    if(clauses !=  nullptr) {
        delete[] clauses; 
        clauses = nullptr;
    }
    // if(associationTable !=  nullptr) {
    //     delete[] associationTable; 
    //     associationTable = nullptr;
    // }
    if(relateTable !=  nullptr) {
        delete[] relateTable; 
        relateTable = nullptr;
    }
}

void Cnf::Inverse(Step stp) {
    switch (stp.operation)
    {           // stp.operation 表示已经发生的操作
    case 0:     // 伪删除了一个子句， 索引为stp.cla
        EnableClause(stp.cla);
        break;
    case 1:     // 伪删除索引stp.cla子句的[stp.lit]元素
        // clauses[stp.cla].DisablePos(stp.lit); 这句话会重复删除Literal
        clauses[stp.cla].EnablePos(stp.lit); 
        break;
    case 2:     // 添加了索引stp.cla的单子句
        // DisableClause(stp.lit); 这是出现cnf.hpp : Cnf::DisableClause() : trying to disable a disabled clause的原因。8.31.2023 15:39 Really BAD.
        DisableClause(stp.cla);
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
    clauses[use].Enable();
    int newClauseSize = newClause.GetSize();
    for(int i = 0; i < newClauseSize; i++) {
        relateTable[newClause[i].GetLiteral() + variableNum].Add(Literal(use, i));
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
        file.getline(buff, 1024, '\n'); // 过滤注释
    }
    std::string type;
    file >> type >> variableNum >> clausesNum;
    Resize(clausesNum);

    relateTable = new (std::nothrow) Vector[2 * variableNum + 1];
    assert(relateTable != nullptr);
   
    int p[100000];
    for (int i = 0; i < clausesNum; i++) {
        int j = 0;
        file >> p[j];
        while (p[j++]) {
            file >> p[j];
        }
        Vector vectorToAppend;
        for(int k = 0; k <= j - 2; k++) {
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
        for(int i = 0; i < use; i++){ 
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
    for(int i = 0; i < use; i++)
        if(clauses[i].GetStatus() && !clauses[i].Empty())
            return clauses[i].GetFirstLiteral();
    return 0;
}


bool Cnf::HaveUnitClause (void) const {
    int i;
    for(i = 0; i < use; i++){
        if(clauses[i].GetStatus() && clauses[i].IsUnit()) return true;
    }
    return false;
}


bool Cnf::Empty (void) const {
    return length == 0;
}


bool Cnf::HaveEmpty (void) const {
    int i;
    for(i = 0; i < use; i++){
        if(clauses[i].GetStatus() && clauses[i].Empty()) return true;
    }
    return false;
}


int Cnf::FindUnitClause (void) const {
    int i;
    for(i = 0; i < use; i++){
        if(clauses[i].GetStatus() && clauses[i].IsUnit()) {
            return clauses[i].GetFirstLiteral();
        }
    }
    return 0; // 0不会代表任何文字，用0代表没有找到单子句。
}


// int Cnf::Select (int a) const {
//     int mostFrequentLiteral = Select(); // 缺省值
//     int mostFrequency = associationTable[mostFrequentLiteral + variableNum];
//     for(int i = -variableNum; i <= variableNum; i++) {
//         if(i == 0) continue;
//         if(associationTable[i + variableNum] > mostFrequency) {
//             mostFrequentLiteral = i;
//             mostFrequency = associationTable[mostFrequentLiteral + variableNum];
//         }
//     }
//     return mostFrequentLiteral; 
// }


int Cnf::Select (void) const {
    int unit = GetFirstLiteral();
    if(unit == 0) {
        std::cout << "\ncnf.hpp : Cnf::Select() : unit == 0 detected!";
        exit(-1);
    }
    return unit;
}


void Cnf::Show (void) const {
    std::cout << "\nCnf.hpp : Cnf::Show() : 显示Cnf结构：";
    std::cout << "\nCnf.hpp : Cnf::Show() : 子句总数：" << length << "\n";
    // uint64_t hashcode1 = 0, hashcode2 = 0;
    for(int i = 0; i < use; i++){
        if(clauses[i].GetStatus()) {
            clauses[i].Show();
            // u_int64_t hhashcode1 = 1;
            // u_int64_t hhashcode2 = 0;
            // int clauseSize = clauses[i].GetSize();
            // for (int j = 0; j < clauseSize; j++)
            // {
            //     if(clauses[i][j].GetStatus()) {
            //         hhashcode1 *= clauses[i][j].GetLiteral() + clauseSize;
            //         hhashcode2 += clauses[i][j].GetLiteral() * clauseSize;
            //     }
            // }
            // hashcode1 += hhashcode1;
            // hashcode2 += hhashcode2;
        }
    }
    // std::cout << "\nCnf.hpp : Cnf::Show() : Cnf散列值：" << hashcode1 * hashcode2;
    std::cout << "\nCnf.hpp : Cnf::Show() : 显示结束\n";
}


bool Cnf::Verify (bool result[]) const {
    for(int i = 0; i < use; i++)
        if(clauses[i].GetStatus() && !clauses[i].Verify(result)){
            //输出的序号从1开始
            // std::cout << "\nCnf.hpp : Cnf::Verify() : 不满足的子句：" << "**NO." << i+1 << "** : ";  // 待修改
            clauses[i].Show();
            return false;
        }
    return true;
}


void Cnf::EnableClause(int pos) {
    if(pos > use) {
        std::cout << "\ncnf.hpp : Cnf::EnableClause() : pos > use detected!";
        exit(-1);
    }
    if(clauses[pos].GetStatus()) {
        std::cout << "\ncnf.hpp : Cnf::EnableClause() : trying to enable a enabled clause";
        exit(-1);
    }
    
    clauses[pos].Enable();
    // int used = clauses[pos].GetUse();
    // for(int i = 0; i < used; i++) {
    //     if(clauses[pos][i].GetStatus())
    //         associationTable[clauses[pos][i].GetLiteral() + variableNum]++;
    // }
    length++;
} 


void Cnf::DisableClause(int pos) {
    if(pos > use) {
        std::cout << "\ncnf.hpp : Cnf::DisableClause() : pos > use detected!";
        exit(-1);
    }
    if(!clauses[pos].GetStatus()) {
        std::cout << "\ncnf.hpp : Cnf::DisableClause() : trying to disable a disabled clause";
        exit(-1);
    }
    clauses[pos].Disable();
    // int used = clauses[pos].GetUse();
    // for(int i = 0; i < used; i++) {
    //     if(clauses[pos][i].GetStatus())
    //         associationTable[clauses[pos][i].GetLiteral() + variableNum]--;
    // }
    length--;
}


void Cnf::EnableLiteralInClause(int pos, int lit) {
    if(pos > use) {
        std::cout << "\ncnf.hpp : Cnf::EnableLiteralInClause() : pos > use detected!";
        exit(-1);
    }
    clauses[pos].EnableLiteral(lit);
    // int used = clauses[pos].GetUse();
    // for(int i = 0; i < used; i++) {
    //     if(clauses[pos][i].GetLiteral() == lit)
    //         associationTable[lit + variableNum]++;
    // }
}


void Cnf::DisableLiteralInClause(int pos, int lit) {
    if(pos > use) {
        std::cout << "\ncnf.hpp : Cnf::DisableLiteralInClause() : pos > use detected!";
        exit(-1);
    }
    clauses[pos].DisableLiteral(lit);
    // int used = clauses[pos].GetUse();
    // for(int i = 0; i < used; i++) {
    //     if(clauses[pos][i].GetLiteral() == lit)
    //         associationTable[lit + variableNum]--;
    // }
}


// Dpll() 的辅助变量
bool error = false;

bool Cnf::Dpll (bool solution[], int deepth) {
    if(deepth > variableNum) {
        std::cout << "\nCnf.hpp : Cnf::dpll() : (ERROR)现在深度是" << deepth <<" ， 递归深度过深，大于变元数，程序终止！";
        error = true;   // 检查程序出错，如果递归过深，由(*error)处原路返回
        return false;   
    }
    countDpllCalls++;
    MyStack toInverse;  // 反演栈，利用栈的FILO特性实现回溯（即操作反演：逆序进行逆操作）
    // 单子句规则
    int unit;
    while((unit = FindUnitClause()) != 0) {
        if(unit > 0) 
            solution[unit] = true;
        else 
            solution[-unit] = false;
        // 化简正文字（literal）
        int relateTableLength1 = relateTable[unit + variableNum].GetLength();
        for (int i = 0; i < relateTableLength1; i++) {
            Literal position = relateTable[unit + variableNum][i];
            if(clauses[position.clausePos].GetStatus() && clauses[position.clausePos][position.literalPos].GetStatus()) {
                if(clauses[position.clausePos][position.literalPos].GetLiteral() != unit) {
                    std::cout << "\ndpll : Cnf::Dpll() : relateTable damaged!!";
                    exit(-1);
                }
                DisableClause(position.clausePos);
                Step stp = {0, position.clausePos, 0};
                toInverse.Push(stp);
            }
        }
        // 化简负文字（-literal）
        int relateTableLength2 = relateTable[-unit + variableNum].GetLength();
        for (int i = 0; i < relateTableLength2; i++){
            Literal position = relateTable[-unit + variableNum][i];
            if(clauses[position.clausePos].GetStatus() && clauses[position.clausePos][position.literalPos].GetStatus()) {
                // DisableLiteralInClause(i, -unit);
                if(clauses[position.clausePos][position.literalPos].GetLiteral() != -unit) {
                    std::cout << "\ndpll : Cnf::Dpll() : relateTable damaged!!";
                    exit(-1);
                }
                clauses[position.clausePos].DisablePos(position.literalPos);
                Step stp = {1, position.clausePos, position.literalPos};
                toInverse.Push(stp);
            }
        }
        if(Empty()) {
            // 回溯
            // while (!toInverse.Empty()) {
            //     Inverse(toInverse.Pop());
            // }
            std::cout << "\nCnf.hpp : Cnf::dpll() : 找到解时的递归深度：" << deepth;
            return true; // 递归终点
        }
        if(HaveEmpty()) { 
            while (!toInverse.Empty()) {
                Inverse(toInverse.Pop()); 
            }
            return false; // 递归终点
        }
    }
    // 选取分支变元
    int p = Select();
    Vector v1, v2;
    // 构造单子句、添加单子句{p}
    v1.Add(p);
    Step stp = {2, use, 0};
    toInverse.Push(stp);
    Add(v1);
    // 求解分支 S + {p}
    if (Dpll(solution, deepth + 1)) {
        // 回溯
        // while (!toInverse.Empty()) {
        //     Inverse(toInverse.Pop());
        // }
        return true;
    }
    else { 
        // 返回false一律检查error
        if(error) {  // (*error)
           return false;
        }
        // 删除单子句{p}
        Inverse(toInverse.Pop());
        // 构造单子句{-p}、添加单子句{-p}
        v2.Add(-p);
        Step stp = {2, use, -p};    // 在调用Cnf::Add()之前压栈，才能保存正确的use
        toInverse.Push(stp);
        Add(v2);                    // 在这个函数中use++
        // 求解S + {-p}
        bool sat = Dpll(solution, deepth + 1);
        // 回溯并返回
        if(sat == false) {
            while (!toInverse.Empty()) {
                Inverse(toInverse.Pop());
            }  
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