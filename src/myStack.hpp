#include "vector.hpp"
class myStack
{
private:
    Vector * arr = nullptr;
    int top = 0;
    int size = 0;
public:
    myStack(/* args */);
    ~myStack();
    void BoostSize(){
        Vector * newArr = new Vector[size * 2 + 10];
        for(int i = 0; i < top; i++) {
            newArr[i] = arr[i];
        }
        size = size * 2 + 1;
        if(arr != nullptr) delete[] arr;
    }
    Vector & Pop() {
        top--;
        return arr[top];
    }
    void Push(Vector & v) { 
        if(top >= size) BoostSize();
        arr[top] = v;
        top++;
    }
    bool Empty(void) {
        return top == 0;
    }
};

myStack::myStack(/* args */)
{
}

myStack::~myStack()
{
    if(arr) delete[] arr;
}
