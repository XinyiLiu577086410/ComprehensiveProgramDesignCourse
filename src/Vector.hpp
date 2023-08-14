#ifndef VECTOR
#define VECTOR
#include <cstring>
#include <algorithm>
#include <iostream>
#include <cassert>
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
        void clear(void) { length = 0; }
        Vector(); // 默认构造函数
        Vector(int, int); // 设定数据域大小，添加一个元素
        Vector(int); // 设定数据域大小
        ~Vector() { if(a != nullptr) delete[] a; } // 析构函数
        void Resize(int);
        Vector & operator= (const Vector & Obj) { // 深拷贝
            if(&Obj != this) {
                length = Obj.length;
                size = Obj.size;
                if(a != nullptr) delete[] a;
                if(size){
                    a = new (std::nothrow) int[Obj.size];
                    assert(a != nullptr);
                    memcpy(a, Obj.a, Obj.length * sizeof(int));
                }
            }
            return *this;
        }
        bool operator== (Vector & V) const {
            if(length != V.GetLength()) return false;
            if(length==0 && V.length==0) return true;
            for(int i = 0; i < length; i++) { 
                if(V.Find(a[i]) == ERROR) return false;
            }
            return true;
        }
        int operator[] (int x) {
            if(x < 0 || x >= length) { std::cout << "class Vector::operater[]：索引越界!" << std::endl; return 0; }
            else return a[x];
        }
        bool Verify(bool []) const; // 验证
        void Show(void) const; // 展示Vector
        int GetLength(void) const { return length; }
    private:
        int * a;
        int length;
        int size;
};

void Vector::Resize(int newsize) {
    if(newsize < size){
        if(a) delete[] a;
        a = nullptr;
        if(newsize > 0) {
            a = new (std::nothrow) int[newsize];
            assert(a != nullptr);
        }
        length = 0;
    }
    else {
        int * tmp = new (std::nothrow) int[newsize];
        assert(tmp != nullptr);
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
    if(newsize == 0) return;
    a = new (std::nothrow) int[newsize];
    assert(a != nullptr);
    size = newsize;
    length = 0;
    Add(x);
}

Vector::Vector (int newsize){
    if(newsize == 0) return;
    a = new (std::nothrow) int[newsize];
    assert(a != nullptr);
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

int Vector::Add (int V) {
    if(size <= length) Resize(length + 10);
    a[length] = V;
    length++;
    return SUCCESS;
}

int Vector::Delete (int x) {
    int pos = Find(x);
    if(pos != ERROR){
        while (pos != ERROR) {
            a[pos] = a[length-1]; // 赋值时左值动态分配的内存被释放
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