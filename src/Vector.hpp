/*
    自己实现的vector类，用来存储子句
*/

#ifndef VECTOR
#define VECTOR

#include <cstring>
#include <algorithm>
#include <iostream>
#include <cassert>
#include "literal.hpp"

#ifndef SUCCESS
#define SUCCESS 0
#endif

#ifndef ERROR
#define ERROR -1
#endif


#ifndef VCT_MEM_INCR
#define VCT_MEM_INCR 10
#endif

typedef literal typeV;

class Vector{
    public:
        // 构造函数、析构函数、下标重载
        Vector();                       // 默认构造函数
        ~Vector();                      // 析构函数
        typeV operator[](int x);
        Vector & operator=(Vector & );
        // 功能函数
        int Add(int);                   // 添加文字
        void Resize(int);               // 重新分配内存
        void Enable(void);              // 标记为无效
        void Disable(void);             // 标记为有效
        void EnableLiteral(int x);      // 标记文字x为有效    
        void DisableLiteral(int x);     // 标记文字x为无效
        bool IsUnit(void) const;        // 判断是否是单子句
        int GetFirstLiteral(void) const;// 返回第一个文字
        bool Empty(void) const;         // 判空
        int GetLength(void) const;      // 返回向量中元素的个数
        bool Verify(const bool []) const;   // 验证解的正确性
        void Show(void) const;          // 展示向量
        int GetSize(void) const;
        bool GetStatus(void) const;
        void SetStatus(bool);
        int GetUse(void);
        int Find(int x) const;
    private:
        typeV * literals;               // 数据域
        int length;                     // 有效元素个数
        int use;                        // 已有数据（包括标记为被删除的）数量
        int size;                       // 数据域内存大小
        bool status;                    // 删除标志位
};


void Vector::SetStatus(bool t) {
    status = t; 
}


int Vector::GetUse(void) {
    return use;
}


int Vector::Find(int x) const {
    for (int i = 0; i < size; i++) {
        if(literals[i].GetStatus() && literals[i].GetLiteral() == x) {
            return i;
        }
    }
    return ERROR;
}


bool Vector::GetStatus(void) const {
    return status;
}


int Vector::GetSize(void) const {
    return size;
}

Vector & Vector::operator=(Vector & obj) {
    if(this != &obj) {
        length = obj.GetLength();
        size = obj.GetSize();
        status = obj.GetStatus();
        use = obj.GetUse();
        if(literals) delete[] literals;
        literals = new (std::nothrow) typeV[size];
        assert(literals != nullptr);
        for (int i = 0; i < size; i++) {
            literals[i].Write(obj[i].GetLiteral());
            literals[i].SetStatus(obj[i].GetStatus());
        }
    }
    return *this;
}


Vector::Vector(){
    literals = nullptr;
    size = 0;
    length = 0; 
    use = 0;
    status = false;
}


Vector::~Vector() { 
    if(literals != nullptr) {
        delete[] literals; 
        literals = nullptr;
    }
}

typeV Vector::operator[](int x) {
    return literals[x];
}


int Vector::Add (int x) {
    if(size == use) Resize(size + VCT_MEM_INCR);
    if(size < use) {
        std::cout<<"\nvector.hpp : Vector::Add() : 数据越界，堆损坏！";
        exit(-1);
    }
    literals[use].Write(x);
    literals[use].Enable();
    length++;
    use++;
    return SUCCESS;
}


void Vector::Resize(int newSize) {
    if(newSize <= 0) {
        std::cout<<"\nvector.hpp : Vector::Resize() : 重新分配内存失败，内存大小不能小于等于0！";  
        exit(-1);
    }
    if(newSize <= size) {
        std::cout<<"\nvector.hpp : Vector::Resize() : 重新分配内存失败，内存大小不能小于等于已有大小！";  
        exit(-1);
    }
    typeV * newSpace = new (std::nothrow) typeV[newSize];
    assert(newSpace != nullptr);
    if(literals) {
        memcpy(newSpace, literals, size * sizeof(typeV));
        delete[] literals;
        literals = nullptr;
    }   
    literals = newSpace;
    size = newSize;
}


bool Vector::IsUnit (void) const{
    return length == 1;
}


bool Vector::Empty (void) const{
    return length == 0;
}


int Vector::GetLength(void) const {
    return length; 
}


int Vector::GetFirstLiteral(void) const {   // 返回第一个文字
    for(int i = 0; i < size; i++)
        if(literals[i].GetStatus())
            return literals[i].GetLiteral();
    return 0;
}

bool Vector::Verify(const bool result[]) const{
    for(int i = 0; i < size; i++ )
        if(literals[i].GetStatus())
            if(
                (literals[i].GetLiteral() > 0 && result[literals[i].GetLiteral()]) 
                ||
                (literals[i].GetLiteral() < 0 && !result[-literals[i].GetLiteral()])
            )  
                return true;
    // std::cout << "\n变元取值：";
    // for(int i = 0; i < length; i++ ) std::cout << abs(literals[i].GetLiteral()) << " : " << (result[abs(literals[i].GetLiteral())] ? "true " : "false ");
    return false;
}


void Vector::Show(void) const{
    for(int i = 0; i < size; i++) if(literals[i].GetStatus()) std::cout << literals[i].GetLiteral() << ' ';
}


void Vector::Disable(void) {
    status = false;
}


void Vector::Enable(void) {
    status = true;
}


void Vector::EnableLiteral(int x) {
    for(int i = 0; i < size; i++) {
        if(literals[i].GetLiteral() == x) {  //这里有错，没有确认status
            literals[i].Enable();
            length++;
        }
    }
}


void Vector::DisableLiteral(int x) {
    for(int i = 0; i < size; i++) {
        if(literals[i].GetLiteral() == x) {
            literals[i].Disable();
            length--;
        }   
    }
}

#endif