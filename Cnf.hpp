#include "VectorInt.hpp"
#include <fstream>
#define MAX_CLAUSES 1000
#define SUCCESS 0
#define ERROR -1
class Cnf{
    private: 
        VectorInt clauses[MAX_CLAUSES] = {0,};
        int length = 0;
        int GetFirstLiteral(int index); // 返回顺序为index（从0开始）的子句的第一个文字
        int Delete(int index); // 从CNF中删除clause[index]（会将最后的子句填充到这里，然后将length减小1。）
        int Add(VectorInt clause); // 在末尾添加一个子句
        int Wash(int literal); // 删除所有含文字literal的clause
        int Reduce(int literal); // 在所有clause中删除literal
        bool HaveSingle(void); // 判断是否含单子句
        bool Empty(void); // 判空
        int FindSingle(void); // 找出一个单子句
        int Select(void); // 选择一种出现最多的文字
    public:
        int Read(std::string filename); // 从file中读取CNF范式的复合命题
        int Dpll(void); // DPLL算法求解CNF范式的SAT问题

};

int Cnf::Add (VectorInt clause){
    if(length >= MAX_CLAUSES) return ERROR;
    clauses[length] = clause;
    length++;
    return SUCCESS;
}

int Cnf::Delete (int index){
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

int Cnf::FindSingle (void) {
    int i;
    for(i = length - 1; i >= 0; i--){
        if(clauses[i].IsSingle()) return clauses[i].GetFirstLiteral();
    }
    return 0; // 命题的编号不会是0， 用0代表没有找到单子句。
}

int Cnf::GetFirstLiteral (int index) {
    if (index < 0) return 0;
    return clauses[index].GetFirstLiteral();
}

int Cnf::Wash (int literal) {
    for (int i = 0; i < length; ) 
        if(clauses[i].Find(literal) != ERROR) Delete(i); else i++;   
    return SUCCESS;
}

int Cnf::Reduce (int literal){
    for (int i = 0; i < length; ) 
        if ( clauses[i].Find(literal) != ERROR ) {
            clauses[i].Delete(literal);
            if(clauses[i].Empty()) Delete(i); else ++i;
        }
    return SUCCESS;
}

bool Cnf::Empty (void) {
    return length == 0;
}

int Cnf::Read (std::string filename) {
    std::ifstream file;
    file.open(filename);
    char ch;
    while (file >> ch && ch != 'p') continue;
    int VariableNum, ClausesNum;
    std::string type;
    file >> type >> VariableNum >> ClausesNum;
    for (int i = 0; i < ClausesNum; i++) {
        VectorInt tmp;
        int p;
        file >> p;
        while(p) {
            tmp.Add(p);
            file >> p;
        }
        Add(tmp);
    }
    std::cout<<"succcess!";
    return SUCCESS;
}

int Cnf::Select (void) {
    return GetFirstLiteral(0);
}

int Cnf::Dpll (void) {
    while(HaveSingle()){
        int literal = FindSingle();
        Wash(literal);
        if(Empty()) return 1;
        else{
            Reduce(-literal);
        }
    }
    int p = Select();
    Cnf S1 = *this, S2 = *this;
    S1.Add(VectorInt(p));
    S2.Add(VectorInt(-p));
    if(S1.Dpll()) return 1; else return S2.Dpll();
}