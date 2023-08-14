#ifndef MYSTACK
#define MYSTACK
#include "vector.hpp"
#include <cassert>
#include <cstdlib>
class myStack
{
public:
    myStack(/* args */);
    ~myStack();
    void BoostSize(){
        Vector * newArr = new (std::nothrow) Vector[size + 10];
        assert(newArr != nullptr);
        if(arr){
            for(int i = 0; i < top; i++) {
                newArr[i] = arr[i];
            }
            delete[] arr;
        }
        size = size + 10;
        arr = newArr;
    }

    Vector & Pop() {
        if(Empty()) {
            std::cout << "\n 栈下溢！";
            exit(-1);
        }
        else top--;
        return arr[top];
    }

    void Push(Vector & V) { 
        if(top >= size) BoostSize();
        arr[top] = V;
        top++;
    }

    bool Empty(void) {
        return top == 0;
    }
private:
    Vector * arr;
    int top;
    int size;
};

myStack::myStack(/* args */)
{
    arr = nullptr;
    size = 0;
    top = 0;
}

myStack::~myStack()
{
    if(arr) delete[] arr;
}

#endif