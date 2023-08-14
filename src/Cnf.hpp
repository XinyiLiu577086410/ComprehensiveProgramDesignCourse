#ifndef CNF
#define CNF

#include <fstream>
#include <cstring>
#include <iostream>
#include <algorithm>
#include <cassert>

#include "step.hpp"
#include "myStack.hpp"
#include "vector.hpp"

#ifndef SUCCESS
#define SUCCESS 0
#endif

#ifndef ERROR
#define ERROR -1
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
    Cnf & operator= (const Cnf & );              // 深拷贝

    // 功能函数
    int Read(std::string);                 // 读取CNF文件
    int Delete(const int);                    // 删除子句
    int DeleteDesignatedClause(const Vector&);     // 删除指定子句
    int Add(Vector &);                       // 添加子句
    void Resize(const int);                    // 重新分配内存
    int GetVariableNum(void) const;                 // 返回CNF文件的变元数信息
    int GetFirstLiteral (const int)const;     // 返回顺序为指定位序子句的第一个文字
    bool HaveSingle  (void)const;                   // 判断子句集是否含有单子句
    bool Empty  (void)const;                        // 判断子句集是否为空集
    bool HaveEmpty (void)const;                     // 判断子句集是否含有空子句
    bool Find (const int) const;                    // 判断子句集是否含有指定的文字
    int FindSingle (void)const;                     // 从最后一个子句找出一个单子句
    int Select (void)const;                         // 基准分支变元选择函数，返回第一个子句的第一个文字
    int Select (int)const;                          // 重载分支变元选择函数
    int Wash(const int);                    // 删除所有含指定文字的子句
    int Reduce(const int);                  // 在所有子句中寻找并删除文字
    bool Verify (bool [])const;                 // 验证求解正确性
    void Show (void)const;                          // 展示各个子句
    bool Dpll(bool []);                     // 用DPLL算法求解SAT问题

private:
    Vector * clauses;                               // 注意深拷贝
    int length;                                     // 子句个数
    int clausesNum;                                 // CNF文件中子句数信息
    int variableNum;                                // CNF文件中变元数信息
    int size;                                       // 内存大小
    int * associationTable;                         // 统计每个变量出现的次数，空间在调用Read()时动态分配
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


int Cnf::Read (std::string filename) {
    std::ifstream file;
    file.open(filename);
    assert(file.is_open());
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


int Cnf::Delete (const int index) {
    if(index < 0 || index >= length) return ERROR;
    int vectorLength = clauses[index].GetLength();
    for(int i = 0; i < vectorLength; i++) {
        associationTable[clauses[index][i]+variableNum]--;
    }
    clauses[index] = clauses[length-1];
    length--;
    if(length * 2 < size) Resize(length + 3);
    return SUCCESS;
}


int Cnf::DeleteDesignatedClause(const Vector & target) {
    int count = 0;
    for(int i = 0; i < length; i++) {
        if(target == clauses[i]) {
            Delete(i);
            return 1;
        }
    }
    return 0;
}


int Cnf::Add (Vector & newClause) {
    if(size == length) Resize(size + 10);
    if(size < length) {
        std::cout<<"\nCnf::Add() : size < length detected, heap is damaged!";
        exit(-1);
    }
    clauses[length] = newClause;
    for(int i = 0; i < newClause.GetLength(); i++) {
        associationTable[newClause[i] + variableNum]++;
    }
    length++;
    return SUCCESS;
}


void Cnf::Resize(const int newSize) {
    if(newSize <= 0) {
        std::cout<<"Cnf::Resize() : Bad resize, the new size is zero or negative.";  
        exit(-1);
    }
    if(newSize < length) {
        std::cout<<"Cnf::Resize() : Bad resize, the new size is too small.";  
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


int Cnf::GetFirstLiteral (const int index) const { 
    if (index < 0 || index >= length) return 0;
    return clauses[index].GetFirstLiteral();
}


bool Cnf::HaveSingle (void) const {
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


bool Cnf::Find(const int target) const {
    for(int i = 0; i < length; i++){
        if(clauses[i].Find(target) != ERROR){ 
            return true;
        }
    }
    return false;
}


int Cnf::FindSingle (void) const {
    int i;
    for(i = length - 1; i >= 0; i--){
        if(clauses[i].IsSingle()) {
            return clauses[i].GetFirstLiteral();
        }
    }
    return 0; // 命题的编号不会是0， 用0代表没有找到单子句。
}


int Cnf::Select (const int tag) const {
    int mostFrequentLiteral = GetFirstLiteral(0); // 缺省值
    for(int i = -variableNum; i <= variableNum; i++) {
        if(associationTable[i + variableNum] > associationTable[mostFrequentLiteral + variableNum]) {
            mostFrequentLiteral = i;
        }
    }
    return mostFrequentLiteral;
}


int Cnf::Select (void) const {
    return GetFirstLiteral(length/2);
}


int Cnf::Wash (const int literal) {
    for (int i = 0; i < length; ){
        if(clauses[i].Find(literal) != ERROR) {
            Delete(i); 
        }
        else i++;
    }
    return SUCCESS;
}


int Cnf::Reduce (const int literal) {
    for (int i = 0; i < length; i++)
        if ( clauses[i].Find(literal) != ERROR ) {
            clauses[i].Delete(literal);
            associationTable[literal + variableNum]--;
        }
    return SUCCESS;
}


void Cnf::Show (void) const {
    std::cout<<"\nShowing cnf class: ";
    std::cout<<"\nlength : "<<length;
    for(int i = 0; i < length; i++){
        std::cout << "\nClause NO." << i << " : ";
        clauses[i].Show();
    }
    std::cout<<"\nEnd of Showing\n";
}


bool Cnf::Verify (bool rslt[]) const {
    for(int i = 0; i < length; i++)
        if(!clauses[i].Verify(rslt)){
            std::cout<<"\nCnf::Verify() : Clause unsatisfied : "<<"**"<<i+1<<"**";  return false;
        }
    return true;
}

// Dpll 的辅助变量（可以放在全局区）
Vector replaceVector, V;
int literal;
Step singleStep;

bool Cnf::Dpll (bool solution[]) {
    countDpllCalls++;
    myStack toInverse; // 反演栈
    std::cout<<"\ncountDpllCalls : "<<countDpllCalls;
    Show();
    // 运用单子句规则进行化简
    while(HaveSingle()) {
        literal = FindSingle();
        //记录结果在solution布尔数组里
        if(literal > 0) solution[literal] = true; else solution[-literal] = false;
        // 化简正文字
        for (int i = 0; i < length; ) {
            if(clauses[i].Find(literal) != ERROR) {
                // 删除子句
                singleStep.operation = -1;
                singleStep.v = clauses[i];
                toInverse.Push(singleStep);
                Delete(i); // 尾部子句被写到 clauses[i]，原地停留 
            }
            else i++;
        }
        // 化简负文字
        for (int i = 0; i < length; i++){
            if ( clauses[i].Find(-literal) != ERROR ) {
                // 构造新子句
                replaceVector = clauses[i];
                replaceVector.Delete(-literal);
                // 添加新子句
                singleStep.operation = +1;
                singleStep.v = replaceVector;
                toInverse.Push(singleStep);
                Add(replaceVector);
                // 删除旧子句
                singleStep.operation = -1;
                singleStep.v = clauses[i];
                toInverse.Push(singleStep);
                Delete(i); // replaceVector 被 Delete() 从尾部写回原处，不必原地停留
                // 更新变元关联表
                associationTable[-literal + variableNum]--;
            }
        }
        if(Empty()) {
            while (!toInverse.Empty()) {
                singleStep = toInverse.Pop();
                if(singleStep.operation == +1) {
                    DeleteDesignatedClause(singleStep.v);
                }
                else if(singleStep.operation == -1){
                    Add(singleStep.v);
                }
            }
            std::cout<<"\ncountDpllCalls : "<<countDpllCalls;
            Show();
            return true; // 递归终点
        }
        if(HaveEmpty()) { 
            while (!toInverse.Empty()) {
                singleStep = toInverse.Pop();
                if(singleStep.operation == +1) {
                    DeleteDesignatedClause(singleStep.v);
                }
                else if(singleStep.operation == -1){
                    Add(singleStep.v);
                }
            }        
            std::cout<<"\ncountDpllCalls : "<<countDpllCalls;
            Show();
            return false; // 递归终点
        }
    }
    // 选取分支变元
    int p = Select(0);
    // 构造单子句
    V.Clear();
    V.Add(p);
    // 添加单子句{p}
    singleStep.operation = +1;
    singleStep.v = V;
    toInverse.Push(singleStep);
    Add(V);
    // 求解分支 S + {p}
    if (Dpll(solution)) {
        while (!toInverse.Empty()) {
            singleStep = toInverse.Pop();
            if(singleStep.operation == +1) {
                DeleteDesignatedClause(singleStep.v);
            }
            else if(singleStep.operation == -1){
                Add(singleStep.v);
            }
        }    
        std::cout<<"\ncountDpllCalls : "<<countDpllCalls;
        Show();
        return true;
    }
    else { 
        // 删除单子句{p}
        singleStep = toInverse.Pop();
        if(singleStep.operation == +1) {
            DeleteDesignatedClause(singleStep.v);
        }
        else if(singleStep.operation == -1){
            Add(singleStep.v);
        }        
        // 构造单子句{-p}
        V.Clear();
        V.Add(-p);
        // 添加单子句{-p}
        singleStep.operation = +1;
        singleStep.v = V;
        toInverse.Push(singleStep);
        Add(V);
        // 求解S + {-p}
        bool sat = Dpll(solution);
        // 回溯并返回
        while (!toInverse.Empty()) {
            singleStep = toInverse.Pop();
            if(singleStep.operation == +1) {
                DeleteDesignatedClause(singleStep.v);
            }
            else if(singleStep.operation == -1){
                Add(singleStep.v);
            }
        }  
        std::cout<<"\ncountDpllCalls : "<<countDpllCalls;
        Show();
        return sat;
    }
}

#endif