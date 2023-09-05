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
        Vector();                    // 默认构造函数
        ~Vector();                   // 析构函数
        typeV & operator[](int x) const;
        Vector<typeV> & operator=(Vector<typeV> & ); // 交换指针域的赋值，右值清空
        // 功能函数
        void Add(const typeV&);      // 添加元素
        void Resize(int);            // 重新分配内存
        bool Empty(void) const;      // 判空
        int Length(void) const;      // 向量长度
        int Size(void) const;        // 可用空间
        int Find(typeV & x) const;   // 寻找元素 
        typeV * SwapElem();          // 交换指针域
    private:
        typeV * elem;                // 数据域
        int length;                  // 向量长度
        int size;                    // 数据域内存大小
};


template <typename typeV>
Vector<typeV>::Vector(){
    elem = nullptr;
    size = 0;
    length = 0; 
}


template <typename typeV>
Vector<typeV>::~Vector() { 
    if(elem != nullptr) delete[] elem;
}

template <typename typeV>
void Vector<typeV>::Add(const typeV & v) {
    if(length == size) 
        Resize(size+VCT_MEM_INCR);
    elem[length++] = v;
}     

template <typename typeV>
typeV & Vector<typeV>::operator[](int x) const{
    return elem[x];
}


template <typename typeV>
int Vector<typeV>::Find(typeV & x) const {
    for (int i = 0; i < length; i++) {
        if(elem == x) {
            return i;
        }
    }
    return ERROR;
}

template <typename typeV>
int Vector<typeV>::Size(void) const {
    return size;
}


template <typename typeV>
typeV * Vector<typeV>::SwapElem() {
    typeV * tmp = elem;
    elem = nullptr;
    return tmp;
}


template <typename typeV>
Vector<typeV> & Vector<typeV>::operator=(Vector<typeV> & obj) {
    if(this != &obj) {
        length = obj.Length();
        size = obj.Size();
        if(elem != nullptr) delete[] elem;
        elem = obj.SwapElem();
    }
    return *this;
}


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
        for(int i = 0; i < length; i++) newSpace[i] = elem[i];
        delete[] elem;
        elem = nullptr;
    }   
    elem = newSpace;
    size = newSize;
}


// template <typename typeV>
// bool Vector<typeV>::IsUnit (void) const{
//     return length == 1;
// }


template <typename typeV>
bool Vector<typeV>::Empty (void) const{
    return length == 0;
}


template <typename typeV>
int Vector<typeV>::Length(void) const {
    return length; 
}




#endif