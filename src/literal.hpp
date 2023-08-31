#ifndef LITERAL
#define LITERAL
#include <iostream>
#include <cstdlib>
class Literal {
private:
    int lit;
    bool status;
public:
    Literal();
    ~Literal();
    bool GetStatus(void);
    int GetLiteral(void);
    void Write(int);
    void Enable(void);
    void Disable(void);
    void SetStatus(bool);
};

Literal::Literal() {
    lit = 0;
    status = false;
}

Literal::~Literal(){
}

bool Literal::GetStatus(void) {
    return status;
}

int Literal::GetLiteral(void) {
    // if(!GetStatus()) {
    //     std::cout << "\nliteral.hpp : Literal::GetLiteral() : try to get the value of a deleted Literal!";
    //     exit(-1);
    // }    
    return lit;
}


void Literal::Write(int x) {
    lit = x;
}

void Literal::Enable(void) {
    if(GetStatus()) {
        std::cout << "\nliteral.hpp : Literal::Enable() : try to enable a enabled literal!";
        exit(-1);
    }   
    status = true;
}

void Literal::Disable(void) {
     if(!GetStatus()) {
        std::cout << "\nliteral.hpp : Literal::Disable() : try to disable a disabled literal!";
        exit(-1);
    }   
    status = false;
}

void Literal::SetStatus(bool t) { 
    status = t;
}


#endif