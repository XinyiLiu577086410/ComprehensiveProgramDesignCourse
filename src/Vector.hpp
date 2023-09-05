/*
    自己实现的vector类，用来存储子句
*/

#ifndef VECTOR
#define VECTOR

#include <cstring>
#include <algorithm>
#include <iostream>
#include <cassert>
// #include "literal.hpp"

#ifndef SUCCESS
#define SUCCESS 0
#endif

#ifndef ERROR
#define ERROR -1
#endif


#ifndef VCT_MEM_INCR
#define VCT_MEM_INCR 20
#endif

// typedef Literal typeV;
template <typename typeV>
class Vector{
    public:
        // 构造函数、析构函数、下标重载
        Vector();                    // 默认构造函数
        ~Vector();                   // 析构函数
        typeV & operator[](int x);
        Vector & operator=(Vector & );
        // 功能函数
        int Add(typeV);              // 添加元素
        void Resize(int);            // 重新分配内存
        bool IsUnit(void) const;     // 判断是否是单子句
        bool Empty(void) const;      // 判空
        int Length(void) const;      // 向量长度
        int Size(void) const;        // 可用空间
        int Find(typeV x) const;       //  
        // int Used(void);              //   
        // bool Status(void) const;
        // void SetStatus(bool);
        // bool Verify(const bool []) const;   // 验证解的正确性
        // void Show(void) const;          // 展示向量
        // int GetFirstLiteral(void) const;// 返回第一个文字
        // int Add(Literal);               // 添加坐标对
        // int Next(int);                  // 迭代器 
        // void Enable(void);              // 标记为无效
        // void Disable(void);             // 标记为有效
        // void EnableLiteral(int x);      // 标记文字x为有效    
        // void DisableLiteral(int x);     // 标记文字x为无效
        // void EnablePos(int pos);          // 标记文字[pos]为有效    
        // void DisablePos(int pos);         // 标记文字[pos]为无效
    private:
        typeV * elem;                   // 数据域
        int length;                     // 向量长度
        int size;                       // 数据域内存大小
        // int use;                        // 已有数据（包括标记为被删除的）数量
        // bool status;                    // 删除标志位
};


// template <typename typeV>
// void Vector<typeV>::SetStatus(bool t) {
//     status = t; 
// }


// template <typename typeV>
// int Vector<typeV>::GetUse(void) {
//     return use;
// }

template <typename typeV>
Vector<typeV>::Vector(){
    elem = nullptr;
    size = 0;
    length = 0; 
    // use = 0;
    // status = false;
}


template <typename typeV>
Vector<typeV>::~Vector() { 
    if(elem != nullptr) {
        delete[] elem; 
        elem = nullptr;
    }
}


template <typename typeV>
typeV & Vector<typeV>::operator[](int x) {
    return elem[x];
}


template <typename typeV>
int Vector<typeV>::Find(typeV x) const {
    for (int i = 0; i < length; i++) {
        if(elem == x) {
            return i;
        }
    }
    return ERROR;
}


// template <typename typeV>
// bool Vector<typeV>::GetStatus(void) const {
//     return status;
// }


template <typename typeV>
int Vector<typeV>::Size(void) const {
    return size;
}


template <typename typeV>
Vector<typeV> & Vector<typeV>::operator=(Vector & obj) {
    if(this != &obj) {
        length = obj.GetLength();
        size = obj.GetSize();
        // status = obj.GetStatus();
        // use = obj.GetUse();
        if(elem) delete[] elem;
        elem = new (std::nothrow) typeV[size];
        assert(elem != nullptr);
        for (int i = 0; i < length; i++) {
            // elem[i].Write(obj[i].GetLiteral());
            // elem[i].SetStatus(obj[i].GetStatus());
            elem[i] = obj[i];
        }
    }
    return *this;
}


// template <typename typeV>
// int Vector<typeV>::Add (int x) {
//     if(size == use) Resize(size + VCT_MEM_INCR);
//     if(size < use) {
//         std::cout<<"\nvector.hpp : Vector<typeV>::Add() : 数据越界，堆损坏！";
//         exit(-1);
//     }
//     elem[use].Write(x);
//     elem[use].Enable();
//     length++;
//     use++;
//     return SUCCESS;
// }


// template <typename typeV>
// int Vector<typeV>::Add(Literal newLit) {
//     if(size == use) Resize(size + VCT_MEM_INCR);
//     if(size < use) {
//         std::cout<<"\nvector.hpp : Vector<typeV>::Add() : 数据越界，堆损坏！";
//         exit(-1);
//     }
//     elem[use] = newLit;
//     length++;
//     use++;
//     return SUCCESS;
// }


template <typename typeV>
void Vector<typeV>::Resize(int newSize) {
    if(newSize <= 0) {
        std::cout<<"\nvector.hpp : Vector<typeV>::Resize() : 重新分配内存失败，内存大小不能小于等于0！";  
        exit(-1);
    }
    if(newSize <= size) {
        std::cout<<"\nvector.hpp : Vector<typeV>::Resize() : 重新分配内存失败，内存大小不能小于等于已有大小！";  
        exit(-1);
    }
    typeV * newSpace = new (std::nothrow) typeV[newSize];
    assert(newSpace != nullptr);
    if(elem) {
        // memcpy(newSpace, elem, size * sizeof(typeV));
        for(int i = 0;i < length; i++) newSpace[i] = elem[i];
        delete[] elem;
        elem = nullptr;
    }   
    elem = newSpace;
    size = newSize;
}


template <typename typeV>
bool Vector<typeV>::IsUnit (void) const{
    return length == 1;
}


template <typename typeV>
bool Vector<typeV>::Empty (void) const{
    return length == 0;
}


template <typename typeV>
int Vector<typeV>::Length(void) const {
    return length; 
}


// template <typename typeV>
// int Vector<typeV>::GetFirstLiteral(void) const {   // 返回第一个文字
//     for(int i = 0; i < use; i++)
//         if(elem[i].GetStatus())
//             return elem[i].GetLiteral();
//     return 0;
// }


// template <typename typeV>
// bool Vector<typeV>::Verify(const bool result[]) const{
//     for(int i = 0; i < use; i++ )
//         if(elem[i].GetStatus())
//             if(
//                 (elem[i].GetLiteral() > 0 && result[elem[i].GetLiteral()]) 
//                 ||
//                 (elem[i].GetLiteral() < 0 && !result[-elem[i].GetLiteral()])
//             )  
//                 return true;
//     // std::cout << "\n变元取值：";
//     // for(int i = 0; i < length; i++ ) std::cout << abs(elem[i].GetLiteral()) << " : " << (result[abs(elem[i].GetLiteral())] ? "true " : "false ");
//     return false;
// }


// template <typename typeV>
// void Vector<typeV>::Show(void) const{
//     std::cout << "\n";
//     for(int i = 0; i < use; i++) 
//         if(elem[i].GetStatus()) 
//             std::cout << elem[i].GetLiteral() << ' ';
// }


// template <typename typeV>
// void Vector<typeV>::Disable(void) {
//     if(status == false){
//         std::cout << "\n vector.hpp : Vector<typeV>::Disable() : try to disable a disabled Vector!";
//         exit(-1);
//     }
//     status = false;
// }


// template <typename typeV>
// void Vector<typeV>::Enable(void) {
//     if(status == true){
//         std::cout << "\n vector.hpp : Vector<typeV>::Enable() : try to enable a enabled Vector!";
//         exit(-1);
//     }
//     status = true;
// }


// template <typename typeV>
// void Vector<typeV>::EnableLiteral(int x) {
//     for(int i = 0; i < use; i++) {
//         if(!elem[i].GetStatus() && elem[i].GetLiteral() == x) {  //这里有错，没有确认status
//             elem[i].Enable();
//             length++;
//         }
//     }
// }


// template <typename typeV>
// void Vector<typeV>::DisableLiteral(int x) {
//     for(int i = 0; i < use; i++) {
//         if(elem[i].GetStatus() && elem[i].GetLiteral() == x) {
//             elem[i].Disable();
//             length--;
//         }   
//     }
// }


// template <typename typeV>
// void Vector<typeV>::EnablePos(int pos) {
//     if(pos >= use || pos < 0) {
//         std::cout << "\nvector.cpp : Vector<typeV>::EnablePos() : BAD pos!";
//         exit(-1);
//     }
//     elem[pos].Enable();
//     length++;
// }


// template <typename typeV>
// void Vector<typeV>::DisablePos(int pos) {
//     if(pos >= use || pos < 0) {
//         std::cout << "\nvector.cpp : Vector<typeV>::DisablePos() : BAD pos!";
//         exit(-1);
//     }
//     elem[pos].Disable();
//     length--;
// }


#endif