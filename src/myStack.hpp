/*
    自己实现的stack类，用于子句集的回溯
*/
#ifndef MYSTACK
#define MYSTACK

#include <cassert>
#include <cstdlib>

// #include "step.hpp"
// #include "vector.hpp"

#ifndef STK_MEM_INCR
#define STK_MEM_INCR 250
#endif

// typedef Step typeS;
template <typename typeS> 
class MyStack {
public:
    // 构造函数、析构函数
    MyStack();
    ~MyStack();
    // 基本功能函数
    void Resize(int newSize);
    typeS Pop(void);         // 出栈
    void Push(typeS V);      // 进栈
    bool Empty(void);        // 判空
private:
    typeS * arr;
    int top;
    int size;
};

template <typename typeS> 
MyStack<typeS>::MyStack() {
    arr = nullptr;
    size = 0;
    top = 0;
}

template <typename typeS> 
MyStack<typeS>::~MyStack() {
    if(arr != nullptr) {
        delete[] arr;
        arr = nullptr;
    }
}

template <typename typeS> 
void MyStack<typeS>::Resize(int newSize) {
    if(newSize <= 0) {
        std::cout << "\nmyStack.hpp : myStack::Resize() : 重新分配内存失败，内存大小不能小于等于0！";
        exit(-1);// 及时结束程序，防止Segmentation fault覆盖错误位置信息
    }
    if(newSize < top) { 
        std::cout << "\nmyStack.hpp : myStack::Resize() : 重新分配内存失败，内存大小不能小于已有有效数据所占大小！";  
        exit(-1);
    }
    typeS * newArr = new (std::nothrow) typeS[newSize];
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

template <typename typeS> 
typeS MyStack<typeS>::Pop() {
    if(Empty()) {
        std::cout << "\nmyStack.hpp : myStack::Pop() : 出栈失败，栈下溢！";
        exit(-1);
    }
    else top--;
    return arr[top];
}

template <typename typeS> 
void MyStack<typeS>::Push(typeS elem) { 
    if(top == size) Resize(size + STK_MEM_INCR);
    if(size < top) {
        std::cout<<"\nmyStack.hpp : myStack::Push() : 入栈失败，有效数据溢出内存区域，堆已经损坏！";
        exit(-1);
    }
    arr[top] = elem;
    top++;
}

template <typename typeS> 
bool MyStack<typeS>::Empty(void) {
    return top == 0;
}


#endif