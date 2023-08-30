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


#ifndef VCT_MEM_INCR
#define VCT_MEM_INCR 10
#endif

class Vector{
    public:
        // 构造函数、析构函数
        Vector();                       // 默认构造函数
        ~Vector();                      // 析构函数

        // 运算符重载
        Vector & operator= (const Vector & );
        bool operator== (const Vector & V) const;
        int operator[] (int x) const;         // 下标引用重载

        // 基本功能函数
        int Add(int);                   // 添加文字
        int Delete(int);                // 删除文字，用末尾文字覆盖x
        void Clear(void);               // 清空向量
        void Resize(int);               // 重新分配内存

        // 高级功能函数
        int Find(int) const;            // 返回参数的索引
        bool IsSingle(void) const;      // 判断是否是单子句
        int GetFirstLiteral(void) const;// 返回第一个文字
        bool Empty(void) const;         // 判空
        int GetLength(void) const;      // 返回向量中元素的个数
        bool Verify(const bool []) const;     // 验证解的正确性
        void Show(void) const;          // 展示向量
        bool Contain(const Vector &) const;         // 判断集合包含
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


Vector::~Vector() { 
    if(a != nullptr) {
        delete[] a; 
        a = nullptr;
    }
}


Vector & Vector::operator= (const Vector & obj) {   // 深拷贝
    if(&obj != this) {
        length = obj.length;
        size = obj.size;
        if(a != nullptr) {
            delete[] a;                             // 释放内存
            a = nullptr;                            // 防止 double free
        }
        if(size){
            a = new (std::nothrow) int[size];       // 防止 new int[0]
            assert(a != nullptr);
            memcpy(a, obj.a, obj.length * sizeof(int));
        }
    }
    return *this;
}


bool Vector::operator== (const Vector & V) const {
    return Contain(V) && V.Contain(*this);
}


int Vector::operator[] (int x) const{
    if(x < 0 || x >= length) { 
        std::cout << "\nvector.hpp : Vector::operater[] : 索引越界!" << std::endl; 
        exit(-1);
    }
    else return a[x];
}


int Vector::Add (int V) {
    if(size == length) Resize(length + VCT_MEM_INCR);
    if(size < length) {
        std::cout<<"\nvector.hpp : Vector::Add() : 数据越界，堆损坏！";
        exit(-1);
    }
    a[length] = V;
    length++;
    return SUCCESS;
}

int Vector::Delete (int x) {
    int count = 0;
    int pos = Find(x);
    if(pos != ERROR){
        while (pos != ERROR) {
            a[pos] = a[length-1];
            length--;
            count++;
            pos = Find(x);
        }
        return count;// 返回删除的个数
    }else return ERROR;
}


int Vector::Find (int x) const{
    int i;
    for (i = length - 1; i >= 0; i--) {
        if(a[i] == x) break;
    }
    return i;// 返回-1（ERROR）时表示没有找到
}


void Vector::Clear(void) { 
    length = 0; 
}


void Vector::Resize(int newSize) {
    if(newSize <= 0) {
        std::cout<<"\nvector.hpp : Vector::Resize() : 重新分配内存失败，内存大小不能小于等于0！";  
        exit(-1);
    }
    if(newSize < length){
        std::cout<<"\nvector.hpp : Vector::Resize() : 重新分配内存失败，内存大小不能小于已有有效数据所占大小！";  
        exit(-1);
    }
    int * newSpace = new (std::nothrow) int[newSize];
    assert(newSpace != nullptr);
    if(a) {
        memcpy(newSpace, a, length * sizeof(int));
        delete[] a;
        a = nullptr;
        // 这里修改之后 Segmentation fault 消失了
    }   
    a = newSpace;
    size = newSize;
}


bool Vector::IsSingle (void) const{
    return length == 1;
}


int Vector::GetFirstLiteral (void) const{
    if (Empty()) return 0;      // 返回0表示没有找到
    return a[0];
}


bool Vector::Empty (void) const{
    return length == 0;
}


int Vector::GetLength(void) const {
    return length; 
}


bool Vector::Verify(const bool result[]) const{
    for(int i = 0; i < length; i++ )
        if((a[i] > 0 && result[a[i]]) || (a[i] < 0 && !result[-a[i]]))  return true;
    std::cout << "\n变元取值：";
    for(int i = 0; i < length; i++ ) std::cout << abs(a[i]) << " : " << (result[abs(a[i])] ? "true " : "false ");
    return false;
}


void Vector::Show(void) const{
    for(int i = 0; i < length; i++) std::cout << a[i] << ' ';
}


bool Vector::Contain(const Vector & V) const{
    for(int i = 0; i < V.GetLength(); i++) { 
        if(Find(V[i]) == ERROR) return false;
    }
    return true;
}


#endif