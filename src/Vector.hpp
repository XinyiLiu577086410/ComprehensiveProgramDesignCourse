#ifndef VECTOR
#define VECTOR
#include <cstring>
#include <algorithm>
#include <iostream>
#ifndef SUCCESS
#define SUCCESS 0
#endif
#ifndef ERROR
#define ERROR -1
#endif
class Vector{
    public:
        int Add(int); //添加文字
        int Delete(int); // 删除文字，用末尾文字覆盖x
        int Find(int) const; // 返回参数的索引
        bool IsSingle(void) const; // 判断是否是单子句
        int GetFirstLiteral(void) const; // 返回第一个文字
        bool Empty(void) const; // 判空
        
        Vector(); // 默认构造函数
        Vector(int, int); // 设定数据域大小，添加一个x
        Vector(int); // 设定数据域大小
        ~Vector() { if(a != nullptr) delete[] a; } // 析构函数
        void Resize(int);
        Vector & operator= (const Vector & Obj) { // 重载函数，深度拷贝
            if(&Obj != this) {
                length = Obj.length;
                size = Obj.size;
                if(a != nullptr) delete[] a;
                a = new int[Obj.size];
                memcpy(a, Obj.a, Obj.length * sizeof(int));
            }
            return *this;
        }
        bool operator== (Vector & v) const {
            if(length != v.GetLength()) return false;
            for(int i = 0; i < length; i++) { 
                if(v.Find(a[i]) == ERROR) return false;
            }
            return true;
        }
        int operator[] (int x) {
            if(x < 0 || x >= length) { std::cout << "class Vector::operater[]：索引越界!" << std::endl; return 0; }
            else return a[x];
        }
        bool Verify(bool []) const; // 验证
        void Show(void) const; // 展示Vector
        int GetLength(void) const {return length;}
    private:
        int * a;
        int length;
        int size;
};

void Vector::Resize(int newsize) {
    if(newsize < size){
        if(a) delete[] a;
        if(newsize == 0) a = nullptr;
        else a = new int[newsize];
        length = 0;
    }
    else {
        int * tmp = new int[newsize];
        if(a) {
            memcpy(tmp, a, length * sizeof(int));
            delete[] a;
        }   
        a = tmp;
    }
    size = newsize;
}

void Vector::Show(void) const{
    for(int i = 0; i < length; i++) std::cout << a[i] << ' ';
}


bool Vector::Verify(bool result[]) const{
    for(int i = 0; i < length; i++ )
        if((a[i] > 0 && result[a[i]]) || (a[i] < 0 && !result[-a[i]]))  return true;
    return false;
}

Vector::Vector(){
    a = nullptr;
    size = 0;
    length = 0; 
}

Vector::Vector (int newsize, int x){
    a = new int[newsize];
    size = newsize;
    length = 0;
    Add(x);
}

Vector::Vector (int newsize){
    a = new int[newsize];
    size = newsize;
    length = 0;
}

int Vector::Find (int x) const{
    int i;
    for (i = length - 1; i >= 0; i--) {
        if(a[i] == x) break;
    }
    return i; // 返回-1（ERROR）表示没有找到
}

int Vector::Add (int x) {
    if(size <= length) Resize(2 * (length + 1));
    a[length] = x;
    length++;
    return SUCCESS;
}

int Vector::Delete (int x) {
    int pos = Find(x);
    if(pos != ERROR){
        while (pos != ERROR) {
            a[pos] = a[length-1];
            length--;
            pos = Find(x);
        }
        return SUCCESS;
    }else return ERROR;
}

bool Vector::IsSingle (void) const{
    return length == 1;
}
bool Vector::Empty (void) const{
    return length == 0;
}
int Vector::GetFirstLiteral (void) const{
    if (Empty()) return 0; // 没有找到
    return a[0];
}
#endif