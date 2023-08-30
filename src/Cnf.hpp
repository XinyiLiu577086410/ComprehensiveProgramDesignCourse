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
   
    // 运算符重载
    Cnf & operator= (const Cnf & );                 // 深拷贝

    // 基本功能函数，可以添加元素，删除元素，操作关联表
    int Add(const Vector &);                        // 添加子句
    int Delete(int);                                // 删除子句

    // 高级功能函数
    int Reduce(int);                                // 子句集预处理
    int Read(std::string);                          // 读取CNF文件
    int DeleteDesignatedClause(const Vector&);      // 删除指定子句
    void Resize(int);                               // 重新分配内存
    int GetVariableNum(void) const;                 // 返回CNF文件的变元数信息
    int GetFirstLiteral (int)const;                 // 返回顺序为指定位序子句的第一个文字
    bool HaveUnitClause  (void)const;               // 判断子句集是否含有单子句
    bool Empty  (void)const;                        // 判断子句集是否为空集
    bool HaveEmpty (void)const;                     // 判断子句集是否含有空子句
    bool Find (int) const;                          // 判断子句集是否含有指定的文字
    int FindUnitClause (void)const;                 // 从最后一个子句找出一个单子句
    int Select (void)const;                         // 选择第一个子句的第一个文字
    int Select (int)const;                          // 选择出现次数最多的变元
    int Select (int, int)const;                     // 选择最短子句的第一个变元
    bool Verify (bool [])const;                     // 验证求解正确性
    void Show (void)const;                          // 展示各个子句
    bool Dpll(bool [], int);                        // 用DPLL算法求解SAT问题
    friend void threadInterface(Cnf &, bool [], int, bool &, double &);
                                                    // 线程接口函数
private:
    Vector * clauses;                               // 数据域
    int length;                                     // 子句个数
    int clausesNum;                                 // CNF文件中子句数信息
    int variableNum;                                // CNF文件中变元数信息
    int size;                                       // 内存大小
    int * associationTable;                         // 变元关联表，统计每个变量与子句关联的次数
    int maxLength, minLength;                       // 最大最小长度
};


// 初始化静态成员变量
unsigned int Cnf::countCases = 0;
unsigned int Cnf::countDpllCalls = 0;


Cnf::Cnf() { 
    countCases++;                                   // 更新计数器
    clauses = nullptr; 
    length = 0;     
    clausesNum = 0;
    variableNum = 0; 
    size = 0;  
    associationTable = nullptr;
    maxLength = 0;
    minLength = INT_MAX;
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


Cnf & Cnf::operator= (const Cnf & obj) {
    if(this != &obj) {
        length = obj.length;
        size = obj.size;
        variableNum = obj.variableNum;
        
        if(associationTable != nullptr) { 
            delete[] associationTable; 
            associationTable = nullptr;
        }
        associationTable = new (std::nothrow) int[variableNum * 2 + 1];
        assert(associationTable != nullptr);
        memcpy(associationTable, obj.associationTable, sizeof(int) * ( variableNum * 2 + 1 ) );

        if(clauses != nullptr) { 
            delete[] clauses; 
            clauses = nullptr; 
        }
        clauses = new (std::nothrow) Vector[obj.size];
        assert(clauses != nullptr);
        for(int i = 0; i < obj.length; i++) { 
            clauses[i] = obj.clauses[i];
        }
    }
    return * this;
}


int Cnf::Add (const Vector & newClause) {
    if(size == length) Resize(size + CNF_MEM_INCR);
    if(size < length) {
        std::cout << "\nCnf.hpp : Cnf::Add() : 数据越界，堆损坏！";
        exit(-1);
    }
    clauses[length] = newClause;
    int newClauseLength = newClause.GetLength();
    for(int i = 0; i < newClauseLength; i++) {
        associationTable[newClause[i] + variableNum]++;
        if(newClauseLength > maxLength) maxLength = newClauseLength;
        if(newClauseLength < minLength) minLength = newClauseLength;
    }
    length++;
    return SUCCESS;
}


int Cnf::Delete (int index) {
    if(index < 0 || index >= length) return ERROR;
    int vectorLength = clauses[index].GetLength();
    for(int i = 0; i < vectorLength; i++) {
        associationTable[clauses[index][i]+variableNum]--;
        if(associationTable[clauses[index][i]+variableNum] < 0 || associationTable[-clauses[index][i]+variableNum] < 0) {
            std::cout << "\nCnf.hpp : Cnf::Delete() : 关联表数据值小于零，发生下溢！";
            exit(-1);
        }
    }
    clauses[index] = clauses[length-1];
    length--;
    return SUCCESS;
}


int Cnf::Reduce(int flag){
    int count = 0, total = 0;
    if(maxLength == minLength) {
        std::cout << "\nCnf.hpp : Cnf::Reduce() : 子句等长，不做化简!";
        if(flag) std::cout << "\nCnf.hpp : Cnf::Reduce() : 化简子句数：" << count;    
        return 0;
    }
    if(variableNum <= 50) {
        std::cout << "\nCnf.hpp : Cnf::Reduce() : 变量数小于等于50，不做化简！";
        if(flag) std::cout << "\nCnf.hpp : Cnf::Reduce() : 化简子句数：" << count;
        return 0;
    }
    for(int i = 0; i < length; i++) {
        int lengthPivot = clauses[i].GetLength();
        for(int j = 0; j < length; ) {
            total++;
            if(i == j) { 
                j++; 
                continue; 
            }else{
                // 避免多余运算，先判断长度是否符合条件
                if( lengthPivot <= clauses[j].GetLength() && clauses[j].Contain(clauses[i]) ){
                    Delete(j);
                    count++;
                    continue;
                }
                else j++;
            }
        }
    }
    if(flag) std::cout << "\nCnf.hpp : Cnf::Reduce() : 化简子句数：" << count;
    return count;
};


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


int Cnf::DeleteDesignatedClause(const Vector & target) {
    for(int i = 0; i < length; i++) {
        if(target == clauses[i]) {
            Delete(i);
            return 1;
        }
    }
    return 0;
}


void Cnf::Resize(int newSize) {
    if(newSize <= 0) {
        std::cout << "\nCnf.hpp : Cnf::Resize() : 重新分配内存失败，内存大小不能小于等于0！";  
        exit(-1);
    }
    if(newSize < length) {
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


int Cnf::GetFirstLiteral (int index) const { 
    if (index < 0 || index >= length) return 0;
    return clauses[index].GetFirstLiteral();
}


bool Cnf::HaveUnitClause (void) const {
    int i;
    for(i = length - 1; i >= 0; i--){
        if(clauses[i].IsSingle()) return true;
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


bool Cnf::Find(int target) const {
    for(int i = 0; i < length; i++){
        if(clauses[i].Find(target) != ERROR){ 
            return true;
        }
    }
    return false;
}


int Cnf::FindUnitClause (void) const {
    int i;
    for(i = length - 1; i >= 0; i--){
        if(clauses[i].IsSingle()) {
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
    return GetFirstLiteral(length / 2);
}


int Cnf::Select (int a, int b) const {
    int minLenPos = 0;
    for(int i = 1; i < length; i++) {
        if(clauses[minLenPos].GetLength() > clauses[i].GetLength())
            minLenPos = i;
    }
    return clauses[minLenPos].GetFirstLiteral();
}


void Cnf::Show (void) const {
    std::cout << "\nCnf.hpp : Cnf::Show() : 显示Cnf结构：";
    std::cout << "\nCnf.hpp : Cnf::Show() : 子句总数："<<length;
    uint64_t hashcode1 = 0, hashcode2 = 0;
    for(int i = 0; i < length; i++){
        std::cout << "\nCnf.hpp : Cnf::Show() : NO." << i + 1 << " : ";
        clauses[i].Show();
        u_int64_t hhashcode1 = 1;
        u_int64_t hhashcode2 = 0;
        int clauseLength = clauses[i].GetLength();
        for (int j = 0; j < clauseLength; j++)
        {
            hhashcode1 *= clauses[i][j] + clauseLength;
            hhashcode2 += clauses[i][j] * clauseLength;
        }
        hashcode1 += hhashcode1;
        hashcode2 += hhashcode2;

    }
    std::cout << "\nCnf.hpp : Cnf::Show() : Cnf散列值：" << hashcode1 * hashcode2;
    std::cout << "\nCnf.hpp : Cnf::Show() : 显示结束\n";
}


bool Cnf::Verify (bool result[]) const {
    for(int i = 0; i < length; i++)
        if(!clauses[i].Verify(result)){
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
    Vector replaceVector, appendVector;
    int literal, i;
    Step singleStep;
    if(deepth > variableNum) {
        std::cout << "\nCnf.hpp : Cnf::dpll() : 现在深度是" << deepth <<" ， 递归深度过深，大于变元数，程序终止！";
        error = true;   // 检查程序出错，如果递归
        return false;   // 过深，由(*error)处原路返回
    }
    if(deepth == 0) {   // 在Dpll() 入口处，
        // Reduce(1);      // 进行子句集预处理
    }
    countDpllCalls++;
    MyStack toInverse;  // 反演栈，Dpll所有添加和删除操作
                        // 都会被封装为Step类压入栈中
    // 运用单子句规则进行化简
    while((literal = FindUnitClause()) != 0) {
        if(literal > 0) solution[literal] = true; else solution[-literal] = false;
        // 化简正文字（literal）
        for (i = 0; i < length; ) {
            if(clauses[i].Find(literal) != ERROR) {
                // 删除子句
                singleStep.operation = -1;
                singleStep.frame = clauses[i];
                toInverse.Push(singleStep);
                Delete(i); // 尾部子句被写到 clauses[i]，原地停留 
            }
            else i++;
        }
        // 化简负文字（-literal）
        for (i = 0; i < length; i++){
            if ( clauses[i].Find(-literal) != ERROR ) {
                /* 以下操作等价于在每一个子句中删除 -literal */
                // 1.构造新子句
                replaceVector = clauses[i];
                replaceVector.Delete(-literal); //  删除 -literal
                // 2.添加新子句
                singleStep.operation = +1;
                singleStep.frame = replaceVector;
                toInverse.Push(singleStep);
                Add(replaceVector);
                // 3.删除旧子句
                singleStep.operation = -1;
                singleStep.frame = clauses[i];
                toInverse.Push(singleStep);
                Delete(i); // replaceVector 被 Delete() 从末尾写回原处，不必原地停留
            }
        }
        if(Empty()) {
            // 反演操作进行回溯
            while (!toInverse.Empty()) {
                singleStep = toInverse.Pop();
                if(singleStep.operation == +1) {
                    DeleteDesignatedClause(singleStep.frame);
                }
                else if(singleStep.operation == -1){
                    Add(singleStep.frame);
                }
            }
            std::cout << "\nCnf.hpp : Cnf::dpll() : 找到解时的递归深度：" << deepth;
            return true; // 递归终点
        }
        if(HaveEmpty()) { 
            // 回溯
            while (!toInverse.Empty()) {
                singleStep = toInverse.Pop();
                if(singleStep.operation == +1) {
                    DeleteDesignatedClause(singleStep.frame);
                }
                else if(singleStep.operation == -1){
                    Add(singleStep.frame);
                }
            }        
            return false; // 递归终点
        }
    }
    // 选取分支变元
    int p = Select(0);
    // 构造单子句
    appendVector.Clear();
    appendVector.Add(p);
    // 添加单子句{p}
    singleStep.operation = +1;
    singleStep.frame = appendVector;
    toInverse.Push(singleStep);
    Add(appendVector);
    // 求解分支 S + {p}
    if (Dpll(solution, deepth + 1)) {
        // 回溯
        while (!toInverse.Empty()) {
            singleStep = toInverse.Pop();
            if(singleStep.operation == +1) {
                DeleteDesignatedClause(singleStep.frame);
            }
            else if(singleStep.operation == -1){
                Add(singleStep.frame);
            }
        }    
        return true;
    }
    else { 
        // 返回false一律检查error
        if(error) {  // (*error)
            while (!toInverse.Empty()) {
                singleStep = toInverse.Pop();
                if(singleStep.operation == +1) {
                    DeleteDesignatedClause(singleStep.frame);
                }
                else if(singleStep.operation == -1){
                    Add(singleStep.frame);
                }
            }
            return false;
        }
        // 删除单子句{p}
        singleStep = toInverse.Pop();
        if(singleStep.operation == +1) {
            DeleteDesignatedClause(singleStep.frame);
        }
        else if(singleStep.operation == -1){
            Add(singleStep.frame);
        }        
        // 构造单子句{-p}
        appendVector.Clear();
        appendVector.Add(-p);
        // 添加单子句{-p}
        singleStep.operation = +1;
        singleStep.frame = appendVector;
        toInverse.Push(singleStep);
        Add(appendVector);
        // 求解S + {-p}
        bool sat = Dpll(solution, deepth + 1);
        // 回溯并返回
        while (!toInverse.Empty()) {
            singleStep = toInverse.Pop();
            if(singleStep.operation == +1) {
                DeleteDesignatedClause(singleStep.frame);
            }
            else if(singleStep.operation == -1){
                Add(singleStep.frame);
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