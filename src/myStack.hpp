/*
    自己实现的stack类，用于子句集的回溯
*/
#ifndef MYSTACK
#define MYSTACK

#include <cassert>
#include <cstdlib>

#include "step.hpp"
#include "vector.hpp"

#ifndef STK_MEM_INCR
#define STK_MEM_INCR 100
#endif

typedef Step type;

class myStack {
public:
    // 构造函数、析构函数
    myStack();
    ~myStack();
    // 基本功能函数
    void Resize(int newSize);
    const type & Pop(void);         // 出栈 // 加const是只允许拷贝
    void Push(const type & V);      // 进栈
    bool Empty(void);               // 判空
private:
    type * arr;
    int top;
    int size;
};


myStack::myStack() {
    arr = nullptr;
    size = 0;
    top = 0;
}


myStack::~myStack() {
    if(arr != nullptr) {
        delete[] arr;
        arr = 0;
    }
}


void myStack::Resize(int newSize) {
    if(newSize <= 0) {
        std::cout<<"\nmyStack::Resize() : Bad resize, the new size is zero or negative.";
        exit(-1);// 及时结束程序，防止Segmentation fault覆盖错误位置信息
    }
    if(newSize < top) { 
        std::cout<<"\nmyStack::Resize() : Bad resize, the new size is too small.";  
        exit(-1);
    }
    type * newArr = new (std::nothrow) type[newSize];
    assert(newArr != nullptr);
    if(arr) {
        for(int i = 0; i < top; i++) {
            newArr[i] = arr[i];
        }
        delete[] arr;
        arr = nullptr;
    }
    size = newSize;
    arr = newArr;
}


const type & myStack::Pop() {
    if(Empty()) {
        std::cout << "\nmyStack::Pop() : stack underflow!";
        exit(-1);
    }
    else top--;
    return arr[top];
}


void myStack::Push(const type & V) { 
    if(top == size) Resize(size + STK_MEM_INCR);
    if(size < top) {
        std::cout<<"\nmyStack::Push() : size < top detected, heap is damaged!";
        exit(-1);
    }
    arr[top] = V;
    top++;
}


bool myStack::Empty(void) {
    return top == 0;
}


#endif