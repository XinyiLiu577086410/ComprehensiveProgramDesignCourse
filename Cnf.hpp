#include "Vector.hpp"
#include <fstream>
#include <cstring>
#include <iostream>
/*
#define MAX_CLAUSES 250000 // CNF范式允许的最多子句个数
// 如果太大会爆栈，覆盖函数入口产生段错误
// 这个版本采用了递降的递归内存分配方法，如果递归入口函数占了很多内存，递归深度会变浅。
*/
#ifndef SUCCESS
#define SUCCESS 0
#endif
#ifndef ERROR
#define ERROR -1
#endif

class Cnf{
    public:
        Cnf(){        
            length = 0;
            clauses = nullptr; 
        }
        Cnf(int size){
            length = 0;
            clauses = new Vector[size];
        }
        ~Cnf() { if(clauses !=  nullptr) delete[] clauses; }
        void Resize(int size);  // 重新分配CNF数据的内存，原有数据会被清空
        Cnf & operator= (const Cnf & Obj){
            if(this != &Obj){
                length = Obj.length;
                size = Obj.size;
                if(clauses != nullptr) delete[] clauses;
                clauses = new Vector[Obj.size];
                for(int i = 0; i < Obj.length; i++) clauses[i] = Obj.clauses[i];
            }
            return * this;
        }
        bool Verify(bool rslt[]);
        int Read(std::string filename); // 从file中读取CNF范式的复合命题
        bool Dpll(bool solution[]); // DPLL算法求解CNF范式的SAT问题
        int GetVariableNum(void){ return VariableNum; }
        
        void Show(void); // 展示各个子句
        int GetFirstLiteral(int index); // 返回顺序为index（从0开始）的子句的第一个文字
        int Delete(int index); // 从CNF中删除clause[index]（会将最后的子句填充到这里，然后将length减小1。）
        int Add(Vector & clause); // 在末尾添加一个子句 //  这里必须引用传递， 值传递是浅拷贝
        int Wash(int literal); // 删除所有含文字literal的clause
        int Reduce(int literal); // 在所有clause中删除literal
        bool HaveSingle(void); // 判断是否含单子句
        bool Empty(void); // 判空
        int FindSingle(void); // 找出一个单子句
        int Select(void); // 选择一种出现最多的文字
        int Select(int); // 重载
        bool HaveEmpty(void); // 如果CNF命题含子句返回true，此时CNF命题是不可满足的
    private:
        Vector * clauses; // 含动态分配内存的类对象的赋值可能会造成正确性错误，浅拷贝没有完成副本的赋值相当于原地工作。
        // Vector clauses[MAX_CLAUSES]; //  栈里的数组会带来段错误，尤其是在输入数据比较大的时候
        int length = 0; //子句的个数
        int VariableNum = 0, ClausesNum = 0; //这两个变量只在调用DPLL算法之前用到
        int size = 0;
};

void Cnf::Resize(int newsize){
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

 void Cnf::Show(void){
    std::cout<<"\n\nlength:"<<length;
    for(int i = 0; i < length; i++){
        std::cout << "\nClauses " << i << " : ";
        clauses[i].Show();
    }
}

bool Cnf::Verify(bool rslt[]){
    for(int i = 0; i < length; i++)
        if(!clauses[i].Verify(rslt)){
            std::cout<<"\n验证失败的子句："<<"\n**"<<i+1<<"**\n";  return false;
        }
    return true;
}


int Cnf::Add (Vector & clause){
    if( size <= length ) Resize(2 * length + 10);
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
    for (int i = 0; i < length; ){
        if(clauses[i].Find(literal) != ERROR) {
            Delete(i); 
        }
        else i++;
    }
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
    std::ifstream file;
    file.open(filename);
    char ch;
    while(file >> ch && ch == 'c') {
        char buff[1024];
        file.getline(buff, 1024, '\n');
    }
    std::string type;
    file >> type >> VariableNum >> ClausesNum;
    Resize(ClausesNum);
    int p[100000];
    for (int i = 0; i < ClausesNum; i++) {
        int j = 0;
        file >> p[j];
        while (p[j++]) file >> p[j];
        Vector tmp(j);
        for(int k = 0; k < j - 1; k++) tmp.Add(p[k]);
        Add(tmp); // Add() 的值传递参数作为一个类会被析构
    }
    return SUCCESS;
}

int Cnf::Select (int tag) {
    int table[200000] = {0,};
    for(int i = 0; i < length / 10; i++){
        clauses[i].Static(table);
    }
    int index = 0;
    for(int i = 1; i < 200000; i++){
        if(table[i] && table[i] > table[index]) index = i;
    }
    return index - 100000;
}

int Cnf::Select (void) {
    return GetFirstLiteral(0);
}
//  这个函数在不可满足时证伪时效率很低
//  正确性完成
//  大数据集 效率很低
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
    Cnf S1(length+1), S2(length+1);
    S1 = *this;
    S2 = *this;
    Vector V1(1, p), V2(1,-p);
    S1.Add(V1);
    S2.Add(V2);
    if ( S1.Dpll(solution) )  return true;
    else  return S2.Dpll(solution);
}


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