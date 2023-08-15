#ifndef CNF
#define CNF

#include <fstream>
#include <cstring>
#include <iostream>
#include <algorithm>
#include <cassert>
#include <climits>
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
    Cnf & operator= (const Cnf & );                 // 深拷贝

    // 基本功能函数，可以添加元素，删除元素，操作关联表
    int Add(const Vector &);                              // 添加子句
    int Delete(int);                                // 删除子句

    // 高级功能函数
    int Reduce(void);                               // 子句集预处理
    int Read(std::string);                    // 读取CNF文件
    int DeleteDesignatedClause(const Vector&);      // 删除指定子句
    void Resize(int);                               // 重新分配内存
    int GetVariableNum(void) const;                 // 返回CNF文件的变元数信息
    int GetFirstLiteral (int)const;                 // 返回顺序为指定位序子句的第一个文字
    bool HaveSingle  (void)const;                   // 判断子句集是否含有单子句
    bool Empty  (void)const;                        // 判断子句集是否为空集
    bool HaveEmpty (void)const;                     // 判断子句集是否含有空子句
    bool Find (int) const;                          // 判断子句集是否含有指定的文字
    int FindSingle (void)const;                     // 从最后一个子句找出一个单子句
    int Select (void)const;                         // 基准分支变元选择函数，返回第一个子句的第一个文字
    int Select (int)const;                          // 重载分支变元选择函数
    bool Verify (bool [])const;                     // 验证求解正确性
    void Show (void)const;                          // 展示各个子句
    bool Dpll(bool [], int);                        // 用DPLL算法求解SAT问题
    friend void threadInterface(Cnf &, bool [], int, bool&);
private:
    Vector * clauses;                               // 注意深拷贝
    int length;                                     // 子句个数
    int clausesNum;                                 // CNF文件中子句数信息
    int variableNum;                                // CNF文件中变元数信息
    int size;                                       // 内存大小
    int * associationTable;                         // 变元关联表，统计每个变量与子句关联的次数
    int maxLength, minLength;
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
    if(size == length) Resize(size + 10);
    if(size < length) {
        std::cout<<"\nCnf::Add() : size < length detected, heap is damaged!";
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
            std::cout << "\nCnf::Delete() : associationTable underflow!";
            exit(-1);
        }
    }
    clauses[index] = clauses[length-1];
    length--;
    return SUCCESS;
}


int Cnf::Reduce(void){
    int count = 0;
    if(maxLength == minLength) {
        std::cout << "\nCnf::Reduce() : maxLength == minLength. I'm returning!";
        return 0;
    }
    if(variableNum <= 50) {
        std::cout << "\nCnf::Reduce() : variableNum <= 50. I'm returning!";
        return 0;
    }
    for(int i = 0; i < length; i++) {
        int lengthPivot = clauses[i].GetLength();
        for(int j = 0; j < length; ) {
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
    return count;
};


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


int Cnf::GetFirstLiteral (int index) const { 
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


bool Cnf::Find(int target) const {
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
    return 0; // 用0代表没有找到单子句。
}

int Cnf::Select (int tag) const {
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


void Cnf::Show (void) const {
    std::cout<<"\nShowing cnf class: ";
    std::cout<<"\nlength : "<<length;
    uint64_t hashcode1 = 1, hashcode2 = 0;
    for(int i = 0; i < length; i++){
        std::cout << "\nClause NO." << i + 1 << " : ";
        clauses[i].Show();
        u_int64_t hhashcode1 = 1;
        u_int64_t hhashcode2 = 0;
        int clauseLength = clauses[i].GetLength();
        for (int j = 0; j < clauseLength; j++)
        {
            hhashcode1 *= clauses[i][j] + clauseLength;
            hhashcode2 += clauses[i][j] * clauseLength;
        }
        hashcode1 += hhashcode2;
        hashcode2 += hhashcode1;

    }
    std::cout << "\nHash code : " << hashcode1 * hashcode2;
    std::cout << "\nEnd of Showing\n";
}


bool Cnf::Verify (bool rslt[]) const {
    for(int i = 0; i < length; i++)
        if(!clauses[i].Verify(rslt)){
            std::cout<<"\nCnf::Verify() : Clause unsatisfied : "<<"**"<<i+1<<"**";  return false;
        }
    return true;
}

// Dpll 的辅助变量
Vector replaceVector, appendVector;
int literal, i;
Step singleStep;
bool error = false;
bool Cnf::Dpll (bool solution[], int deepth) {
    if(deepth > variableNum) {
        std::cout << "\ndeep == " << deepth <<" . too DEEP the recursion  is. ";
        error = true;   // 检查程序出错，如果递归
        return false;   // 过深，由(*error)处原路返回
    }
    if(deepth == 0) {         // 在Dpll() 入口处
        int count = Reduce(); // 进行子句集预处理
        std::cout << "\nReduced " << count << " clauses";
    }
    countDpllCalls++;
    myStack toInverse;  // 反演栈，Dpll所有添加和删除操作
                        // 都会被封装为Step类压入栈中
    // 运用单子句规则进行化简
    while(HaveSingle()) {
        literal = FindSingle();
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
            std::cout << "\nFind solution at the deepth of " << deepth <<" !!";
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


void threadInterface(Cnf & obj, bool solution[], int deepth, bool & returnValue, int & miniSecTime) {
    /* 计时开始 */    std::chrono::steady_clock::time_point t0 = std::chrono::steady_clock::now();       
    returnValue = obj.Dpll(solution, deepth);
    /* 计时结束 */   std::chrono::steady_clock::time_point t1 = std::chrono::steady_clock::now();        
    auto duration = std::chrono::duration_cast< std::chrono::duration<double> >(t1 - t0);
    miniSecTime =  duration.count() * 1000;
}



#endif