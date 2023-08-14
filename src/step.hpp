#ifndef STEP
#define STEP

#include "vector.hpp"

class Step
{
private:

public:
    Vector v;
    char operation;
    Step();
    ~Step();
    Step & operator= (Step &);
};

Step::Step(){}

Step::~Step(){}

Step & Step::operator= (Step & obj) {
    if(this != &obj){
        v = obj.v;
        operation = obj.operation;
    }
    return *this;
}

#endif