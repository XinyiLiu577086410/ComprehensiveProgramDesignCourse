#include "Vector.hpp"
#include <fstream>
#include <cstring>
#include <iostream>
#define MAX_CLAUSES 250000 // CNF范式允许的最多子句个数
// 如果太大会爆栈，覆盖函数入口产生段错误
#ifndef SUCCESS
#define SUCCESS 0
#endif
#ifndef ERROR
#define ERROR -1
#endif

class Cnf{
    private: 
        Vector * clauses = new Vector[MAX_CLAUSES];
        // Vector clauses[MAX_CLAUSES];
        int length = 0;
        int VariableNum, ClausesNum;
        int GetFirstLiteral(int index); // 返回顺序为index（从0开始）的子句的第一个文字
        int Delete(int index); // 从CNF中删除clause[index]（会将最后的子句填充到这里，然后将length减小1。）
        int Add(Vector clause); // 在末尾添加一个子句
        int Wash(int literal); // 删除所有含文字literal的clause
        int Reduce(int literal); // 在所有clause中删除literal
        bool HaveSingle(void); // 判断是否含单子句
        bool Empty(void); // 判空
        int FindSingle(void); // 找出一个单子句
        int Select(void); // 选择一种出现最多的文字
        bool HaveEmpty(void); // 如果CNF命题含子句返回true，此时CNF命题是不可满足的
    public:
        bool Verify(bool rslt[]){ 
            for(int i = 0; i < length; i++) 
                if(!clauses[i].Verify(rslt)){std::cout<<"\n**"<<i+1<<"**\n";  return false;}
            return true; 
        }
        int Read(std::string filename); // 从file中读取CNF范式的复合命题
        bool Dpll(bool solution[]); // DPLL算法求解CNF范式的SAT问题
        int GetVariableNum(void){ return VariableNum; }
        void operator = (Cnf & Obj){
            clauses = new Vector[MAX_CLAUSES];
            memcpy(clauses, Obj.clauses, MAX_CLAUSES * sizeof(Vector));
            length = Obj.length;
        }
};



int Cnf::Add (Vector clause){
    if(length >= MAX_CLAUSES) return ERROR;
    clauses[length] = clause;
    length++;
    return SUCCESS;
}

int Cnf::Delete (int index){
    if(index < 0 || index >= length) return ERROR;
    clauses[index] = clauses[length-1];
    length--;
    return SUCCESS;
}

bool Cnf::HaveSingle (void) {
    int i;
    for(i = length - 1; i >= 0; i--){
        if(clauses[i].IsSingle()) return true;
    }
    return false;
}

bool Cnf::HaveEmpty (void) {
    int i;
    for(i = length - 1; i >= 0; i--){
        if(clauses[i].Empty()) return true;
    }
    return false;
}

int Cnf::FindSingle (void) {
    int i;
    for(i = length - 1; i >= 0; i--){
        if(clauses[i].IsSingle()) return clauses[i].GetFirstLiteral();
    }
    return 0; // 命题的编号不会是0， 用0代表没有找到单子句。
}

int Cnf::GetFirstLiteral (int index) {
    if (index < 0 || index >= length) return 0;
    return clauses[index].GetFirstLiteral();
}

int Cnf::Wash (int literal) {
    for (int i = 0; i < length; ) 
        if(clauses[i].Find(literal) != ERROR) Delete(i); else i++;   
    return SUCCESS;
}

int Cnf::Reduce (int literal){
    for (int i = 0; i < length; i++) 
        if ( clauses[i].Find(literal) != ERROR ) {
            clauses[i].Delete(literal);
        }
    return SUCCESS;
}

bool Cnf::Empty (void) {
    return length == 0;
}

int Cnf::Read (std::string filename) {
    memset(clauses, 0, MAX_CLAUSES * sizeof(Vector));
    std::ifstream file;
    file.open(filename);
    char ch;
    while (file >> ch && ch != 'p') continue;
    // int VariableNum, ClausesNum;
    std::string type;
    file >> type >> VariableNum >> ClausesNum;
    for (int i = 0; i < ClausesNum; i++) {
        Vector tmp;
        int p;
        file >> p;
        while(p) {
            tmp.Add(p);
            file >> p;
        }
        Add(tmp);
    }
    return SUCCESS;
}

int Cnf::Select (void) {
    return GetFirstLiteral(0);
}

bool Cnf::Dpll (bool solution[]) {
    while(HaveSingle()){
        int literal = FindSingle();
        if(literal > 0) solution[literal] = true; else solution[-literal] = false;
        Wash(literal);
        if(Empty()) return true;
        else{
            Reduce(-literal);
            if (HaveEmpty()) return false;
        }
    }
    int p = Select();
    Cnf S1 = *this, S2 = *this;
    S1.Add(Vector(p));
    S2.Add(Vector(-p));
    if ( S1.Dpll(solution) ) {
        if(p > 0) solution[p] = true; else solution[-p] = false;
        return true;
    }
    else {
        if(p > 0) solution[p] = false; else solution[-p] = true;
        return S2.Dpll(solution);
    }
}