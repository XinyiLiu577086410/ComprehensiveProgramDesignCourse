#ifndef LITERAL
#define LITERAL
#include <iostream>
#include <cstdlib>
class literal {
private:
    int lit;
    bool status;
public:
    literal();
    ~literal();
    bool GetStatus(void);
    int GetLiteral(void);
    void Write(int);
    void Enable(void);
    void Disable(void);
    void SetStatus(bool);
};

literal::literal() {
    lit = 0;
    status = false;
}

literal::~literal(){
}

bool literal::GetStatus(void) {
    return status;
}

int literal::GetLiteral(void) {
    if(!GetStatus()) {
        std::cout << "literal.hpp : literal::GetLiteral() : try to get the value of a deleted literal!";
        exit(-1);
    }    
    return lit;
}


void literal::Write(int x) {
    lit = x;
}

void literal::Enable(void) {
    if(GetStatus()) {
        std::cout << "literal.hpp : literal::Enable() : try to enable a enabled literal!";
        exit(-1);
    }   
    status = true;
}

void literal::Disable(void) {
     if(!GetStatus()) {
        std::cout << "literal.hpp : literal::Disable() : try to disable a disabled literal!";
        exit(-1);
    }   
    status = false;
}

void literal::SetStatus(bool t) { 
    status = t;
}


#endif