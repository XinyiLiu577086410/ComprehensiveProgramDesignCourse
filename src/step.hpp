#ifndef STEP
#define STEP

#include "vector.hpp"

class Step
{
private:

public:
    Vector frame;
    int operation;
    Step();                                 // 构造函数
    ~Step();                                // 析构函数
    Step & operator= (const Step &);        // 赋值重载
};

Step::Step(){}

Step::~Step(){}

Step & Step::operator= (const Step & obj) {
    if(this != &obj){
        frame = obj.frame;
        operation = obj.operation;
    }
    return *this;
}

#endif