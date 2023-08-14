/*
    自己实现的stack类，用于子句集的回溯
*/
#ifndef MYSTACK
#define MYSTACK

#include <cassert>
#include <cstdlib>

#include "vector.hpp"

class myStack {
public:
    myStack();
    ~myStack();
    void Resize(int newSize);
    Vector & Pop(void);
    void Push(Vector & V);
    bool Empty(void);
private:
    Vector * arr;
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
        exit(-1);                   // 防衍生错误覆盖关键错误信息
    }
    if(newSize < top) { 
        std::cout<<"\nmyStack::Resize() : Bad resize, the new size is too small.";  
        exit(-1);
    }
    Vector * newArr = new (std::nothrow) Vector[newSize];
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


Vector & myStack::Pop() {
    if(Empty()) {
        std::cout << "\nmyStack::Pop() : stack underflow!";
        exit(-1);
    }
    else top--;
    if(top * 2 < size) Resize(top + 3);
    return arr[top];
}


void myStack::Push(Vector & V) { 
    if(top == size) Resize(size + 10);
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