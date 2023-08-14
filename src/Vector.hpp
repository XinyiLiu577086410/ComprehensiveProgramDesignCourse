/*
    自己实现的vector类，用来存储子句
*/

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
        // 构造函数、析构函数
        Vector();                       // 默认构造函数
        Vector(int, int);               // 设定数据域大小，添加一个元素
        Vector(int);                    // 设定数据域大小
        ~Vector();                      // 析构函数

        // 运算符重载
        Vector & operator= (const Vector & Obj);
        bool operator== (Vector & V) const;
        int operator[] (int x);         // 下标引用重载

        // 功能函数
        int Add(int);                   // 添加文字
        int Delete(int);                // 删除文字，用末尾文字覆盖x
        int Find(int) const;            // 返回参数的索引
        bool IsSingle(void) const;      // 判断是否是单子句
        int GetFirstLiteral(void) const;// 返回第一个文字
        bool Empty(void) const;         // 判空
        void Clear(void);               // 清空向量
        void Resize(int);               // 重新分配内存
        int GetLength(void) const;      // 返回向量中元素的个数
        bool Verify(bool []) const;     // 验证解的正确性
        void Show(void) const;          // 展示向量
    private:
        int * a;                        // 数据域
        int length;                     // 元素个数
        int size;                       // 数据域内存大小
};


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


Vector::~Vector() { 
    if(a != nullptr) 
        delete[] a; 
}


Vector & Vector::operator= (const Vector & Obj) {   // 深拷贝
    if(&Obj != this) {
        length = Obj.length;
        size = Obj.size;
        if(a != nullptr) delete[] a;                // 释放内存
        if(size){
            a = new (std::nothrow) int[Obj.size];
            assert(a != nullptr);
            memcpy(a, Obj.a, Obj.length * sizeof(int));
        }
    }
    return *this;
}


bool Vector::operator== (Vector & V) const {
    if(length != V.GetLength()) return false;
    if(length==0 && V.length==0) return true;
    for(int i = 0; i < length; i++) { 
        if(V.Find(a[i]) == ERROR) return false;
    }
    return true;
}


int Vector::operator[] (int x) {
    if(x < 0 || x >= length) { 
        std::cout << "class Vector::operater[]：索引越界!" << std::endl; 
        return 0; 
    }
    else return a[x];
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
            a[pos] = a[length-1];
            length--;
            pos = Find(x);
        }
        if(length * 3 < size) Resize(length + 3);
        return SUCCESS;
    }else return ERROR;
}


int Vector::Find (int x) const{
    int i;
    for (i = length - 1; i >= 0; i--) {
        if(a[i] == x) break;
    }
    return i;                   // 返回-1（ERROR）时表示没有找到
}


bool Vector::IsSingle (void) const{
    return length == 1;
}


int Vector::GetFirstLiteral (void) const{
    if (Empty()) return 0;      // 返回0没有找到
    return a[0];
}


bool Vector::Empty (void) const{
    return length == 0;
}


void Vector::Clear(void) { 
    length = 0; 
}


void Vector::Resize(int newSize) {
    if(newSize == 0) {
        std::cout<<"Vector::Resize() : Bad resize, the new size is  zero.";  
        exit(-1);
    }
    if(newSize < length){
        std::cout<<"Vector::Resize() : Bad resize, the new size is too small.";  
        exit(-1);
    }
    int * newSpace = new (std::nothrow) int[newSize];
    assert(newSpace != nullptr);
    if(a) {
        memcpy(newSpace, a, length * sizeof(int));
        delete[] a;
    }   
    a = newSpace;
    size = newSize;
}


int Vector::GetLength(void) const {
    return length; 
}


bool Vector::Verify(bool result[]) const{
    for(int i = 0; i < length; i++ )
        if((a[i] > 0 && result[a[i]]) || (a[i] < 0 && !result[-a[i]]))  return true;
    return false;
}


void Vector::Show(void) const{
    for(int i = 0; i < length; i++) std::cout << a[i] << ' ';
}


#endif