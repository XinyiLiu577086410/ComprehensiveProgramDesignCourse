#ifndef CNF
#define CNF
#include <fstream>
#include <cstring>
#include <iostream>
#include <algorithm>
#include "myStack.hpp"
#include "vector.hpp"
/*
#define MAX_CLAUSES 250000 // CNF范式允许的最多子句个数
// 如果太大会爆栈，覆盖函数入口产生段错误
// 这个版本采用了递降的递归内存分配方法，如果递归入口函数占了很多内存，允许的递归深度会变浅。
*/
#ifndef SUCCESS
#define SUCCESS 0
#endif
#ifndef ERROR
#define ERROR -1
#endif

class Cnf{
public:
    static unsigned int countCases;
    static unsigned int countDPLLCalls;
    Cnf(){      
        countCases++;  
        length = 0;
        clauses = nullptr; 
    }
    Cnf(int size){
        countCases++;  
        length = 0;
        clauses = new Vector[size];
    }
    ~Cnf() { 
        if(clauses !=  nullptr) delete[] clauses; 
        if(assoiciationTable !=  nullptr) delete[] assoiciationTable; 
    }
    void Resize(const int size);  // 重新分配CNF数据的内存，原有数据会被清空
    // O(N*M)
    Cnf & operator= (const Cnf & Obj){
        if(this != &Obj){
            length = Obj.length;
            size = Obj.size;
            variableNum = Obj.variableNum;
            
            if(assoiciationTable != nullptr) delete[] assoiciationTable;
            assoiciationTable = new int[Obj.variableNum * 2 + 1];
            memcpy(assoiciationTable, Obj.assoiciationTable, sizeof(int) * ( variableNum * 2 + 1 ) );

            if(clauses != nullptr) delete[] clauses;
            clauses = new Vector[Obj.size];
            for(int i = 0; i < Obj.length; i++) clauses[i] = Obj.clauses[i];
        }
        return * this;
    }
    bool Verify (bool rslt[])const;
    int Read(std::string filename); // 从file中读取CNF范式的复合命题
    bool Dpll(bool solution[]); // DPLL算法求解CNF范式的SAT问题
    int GetVariableNum(void) const{ return variableNum; }
    
    void Show  (void)const; // 展示各个子句
    int GetFirstLiteral (const int index)const; // 返回顺序为index（从0开始）的子句的第一个文字
    int Delete(const int index); // 从CNF中删除clause[index]（会将最后的子句填充到这里，然后将length减小1。）
    int Add(Vector & clause); // 在末尾添加一个子句 //  这里必须引用传递， 值传递是浅拷贝
    int Wash(const int literal); // 删除所有含文字literal的clause
    int Reduce(const int literal); // 在所有clause中删除literal
    bool HaveSingle  (void)const; // 判断是否含单子句
    bool Empty  (void)const; // 判空
    int FindSingle (void)const; // 找出一个单子句
    int Select (void)const; // 选择第一个子句的第一个文字
    int Select (int)const; // 重载，传递任何一个无意义整型常数即可调用，参数无实际意义
    bool HaveEmpty (void)const; // 如果CNF命题含子句返回true，此时CNF命题是不可满足的
    bool Find (const int) const; // CNF命题中查找参数
    void DeleteDesignatedClause(const Vector&);
    // static unsigned long long int CountCases(void) { return countCases; }
private:
    Vector * clauses; // 含动态分配内存的类对象的赋值可能会造成正确性错误，浅拷贝没有完成副本的赋值相当于原地工作。
    // Vector clauses[MAX_CLAUSES]; //  栈里的数组会带来段错误，尤其是在输入数据比较大的时候
    int length = 0; //子句的个数
    int clausesNum = 0; //只能在调用DPLL算法之前用到
    int variableNum = 0; //变量的总数
    int size = 0;
    int * assoiciationTable = nullptr; //统计每个变量出现次数，调用Read()时动态分配空间
};

unsigned int Cnf::countCases = 0;
unsigned int Cnf::countDPLLCalls = 0;

bool Cnf::Find(const int target) const{
    for(int i = 0; i < length; i++)if(clauses[i].Find(target) != ERROR) return true;
    return false;
}

// O(1) or O(N*M)
void Cnf::Resize(const int newsize){
    if(newsize < size) {
        if(clauses) delete[] clauses;
        if(newsize == 0) clauses = nullptr;
        else clauses = new Vector[newsize];
        length = 0;
    }
    else {
        Vector * tmp = new Vector[newsize];
        if(clauses) {
            for(int i = 0; i < length; i++) tmp[i] = clauses[i];
            delete[] clauses;
        }
        clauses = tmp;
    }
    size = newsize;
}

//O(N*M)
void Cnf::Show (void) const {
    std::cout<<"\n\nlength:"<<length;
    for(int i = 0; i < length; i++){
        std::cout << "\nClauses " << i << " : ";
        clauses[i].Show();
    }
}
//O(N*M)
bool Cnf::Verify (bool rslt[]) const {
    for(int i = 0; i < length; i++)
        if(!clauses[i].Verify(rslt)){
            std::cout<<"不满足的子句："<<"\n**"<<i+1<<"**\n";  return false;
        }
    return true;
}

// O(Resize) + O(M) + O(1)
int Cnf::Add (Vector & newClause){
    if( size <= length ) Resize(2 * length + 10);
    clauses[length] = newClause;
    for(int i = 0; i < newClause.GetLength(); i++) {
        assoiciationTable[newClause[i] + variableNum]++;
    }
    length++;
    return SUCCESS;
}
// O(M)
int Cnf::Delete (const int index){
    if(index < 0 || index >= length) return ERROR;
    int vectorLength = clauses[index].GetLength();
    for(int i = 0; i < vectorLength; i++) {
        assoiciationTable[clauses[index][i]+variableNum]--;
    }
    // length++;  commit:ffa24b3的问题所在
    clauses[index] = clauses[length-1];
    length--;
    return SUCCESS;
}
// O(N)
bool Cnf::HaveSingle (void) const{
    int i;
    for(i = length - 1; i >= 0; i--){
        if(clauses[i].IsSingle()) return true;
    }
    return false;
}
//O(N)
bool Cnf::HaveEmpty (void) const{
    int i;
    for(i = length - 1; i >= 0; i--){
        if(clauses[i].Empty()) return true;
    }
    return false;
}
//O(N)
int Cnf::FindSingle (void) const{
    int i;
    for(i = length - 1; i >= 0; i--){
        if(clauses[i].IsSingle()) return clauses[i].GetFirstLiteral();
    }
    return 0; // 命题的编号不会是0， 用0代表没有找到单子句。
}
//O(1)
int Cnf::GetFirstLiteral (const int index) const{
    if (index < 0 || index >= length) return 0;
    return clauses[index].GetFirstLiteral();
}
//O(N*M)
int Cnf::Wash (const int literal) {
    for (int i = 0; i < length; ){
        if(clauses[i].Find(literal) != ERROR) {
            Delete(i); 
        }
        else i++;
    }
    return SUCCESS;
}
//O(N*M)
int Cnf::Reduce (const int literal){
    for (int i = 0; i < length; i++)
        if ( clauses[i].Find(literal) != ERROR ) {
            clauses[i].Delete(literal);
            assoiciationTable[literal + variableNum]--;
        }
    return SUCCESS;
}
//O(1)
bool Cnf::Empty (void) const{
    return length == 0;
}
//O(N*M)
int Cnf::Read (std::string filename) {
    std::ifstream file;
    file.open(filename);
    char ch;
    while(file >> ch && ch == 'c') {
        char buff[1024];
        file.getline(buff, 1024, '\n');
    }
    std::string type;
    file >> type >> variableNum >> clausesNum;
    Resize(clausesNum);
    assoiciationTable = new int[2 * variableNum + 1];
    memset(assoiciationTable, 0, sizeof(int) * (variableNum * 2 + 1));
    int p[100000];
    for (int i = 0; i < clausesNum; i++) {
        int j = 0;
        file >> p[j];
        while (p[j++]) file >> p[j];
        Vector tmp(j);
        for(int k = 0; k < j - 1; k++) {
            tmp.Add(p[k]);
        }
        Add(tmp); // Add() 的值传递参数作为一个类会被析构
    }
    Show();
    return SUCCESS;
}
//O(variableNum)
int Cnf::Select (const int tag) const{
    int mostFrequentLiteral = GetFirstLiteral(0); // 缺省值
    for(int i = -variableNum; i <= variableNum; i++) {
        if(/*O(n*m) Find(i) && */ assoiciationTable[i + variableNum] > assoiciationTable[mostFrequentLiteral + variableNum]) {
            mostFrequentLiteral = i;
        }
    }
    return mostFrequentLiteral;
}
//O(1)
int Cnf::Select (void) const{
    return GetFirstLiteral(length/2);
}
//  这个函数在不可满足时证伪时效率很低
//  正确性完成
//  大数据集 效率很低

bool Cnf::Dpll (bool solution[]) {
    Show();
    countDPLLCalls++;
    myStack toDelete, toAdd;
    // Show();
    while(HaveSingle()){
        int literal = FindSingle();
        //记录结果
        if(literal > 0) solution[literal] = true; else solution[-literal] = false;
        //Wash()
            for (int i = 0; i < length; ){
                if(clauses[i].Find(literal) != ERROR) {
                    toAdd.Push(clauses[i]);
                    Delete(i); 
                }
                else i++;
            }
        //end Wash()
        //Reduce()
            for (int i = 0; i < length; i++)
                if ( clauses[i].Find(-literal) != ERROR ) {
                    toAdd.Push(clauses[i]);
                    // Vector replaceVector = clauses[i];  commit 402027b3648a07cd70cf6684a23675b02ebe8830 的错误。
                    Vector replaceVector;
                    replaceVector = clauses[i];
                    replaceVector.Delete(-literal);
                    toDelete.Push(replaceVector);
                    Add(replaceVector);
                    Delete(i);
                    assoiciationTable[-literal + variableNum]--;
                }
        //End Reduce()
        // Show();
        if(Empty()) return true;
        if (HaveEmpty()) return true;
    }
    int p = Select(1);
    /*
    Cnf S1(length+1), S2(length+1);
    S1 = *this;
    S2 = *this;
    */
    Vector V1(1, p), V2(1,-p);
    Add(V1);
    // Show();
    if (Dpll(solution)) {
        toDelete.Push(V1);
        while (!toAdd.Empty()) Add(toAdd.Pop());
        while (!toDelete.Empty()) DeleteDesignatedClause(toDelete.Pop());
        // while(!toAdd.Empty()) {Vector tmp; tmp = toAdd.Pop(); Add(tmp);}
        // while(!toDelete.Empty()) {Vector tmp; tmp = toDelete.Pop(); DeleteDesignatedClause(tmp);}
        return true;
    }
    else { 
        DeleteDesignatedClause(V1);
        toDelete.Push(V2);
        Add(V2);
        // Show();
        bool sat = Dpll(solution);
        while (!toAdd.Empty()) Add(toAdd.Pop());
        while (!toDelete.Empty()) DeleteDesignatedClause(toDelete.Pop());
        // while(!toAdd.Empty()) {Vector tmp; tmp = toAdd.Pop(); Add(tmp);}
        // while(!toDelete.Empty()) {Vector tmp; tmp = toDelete.Pop(); DeleteDesignatedClause(tmp);}
        return sat;
    }
}

void Cnf::DeleteDesignatedClause(const Vector & target) {
    for(int i = 0; i < length; i++) {
        if(target == clauses[i]) Delete(i);
    }
};

/*
 CNF_SAT(76264,0x1ee68a080) malloc: *** error for object 0x60000000c000: pointer being freed was not allocated
 CNF_SAT(76264,0x1ee68a080) malloc: *** set a breakpoint in malloc_error_break to debug
 
 悬挂指针。。
 */

/*
test.请输入CNF文件名
cnf

length now is1

length:1
Clauses 0 : 50 59 89 
length now is2
length now is2CNF命题是可满足的
成真赋值的解在solution文件中。
Time used: 0.681542 ms.

*/


// 只有在打了一个通宵游戏后才会做出用size_t和longlong去替代int的事情
#endif