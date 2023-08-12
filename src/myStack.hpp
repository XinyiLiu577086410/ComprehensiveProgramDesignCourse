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
        if(arr){
            for(int i = 0; i < top; i++) {
                newArr[i] = arr[i];
            }
            delete[] arr;
        }
        size = size * 2 + 10;
        arr = newArr; // commit 402027b3648a07cd70cf6684a23675b02ebe8830 出错的地方，原来没有这一句
    }

    Vector & Pop() {
        if(Empty()) std::cout << "\n 栈下溢！";
        top--;
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
};

myStack::myStack(/* args */)
{
}

myStack::~myStack()
{
    if(arr) delete[] arr;
}
