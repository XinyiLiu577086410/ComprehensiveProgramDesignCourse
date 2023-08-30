#ifndef LITERAL
#define LITERAL
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
    return lit;
}


void literal::Write(int x) {
    lit = x;
    Enable();
}

void literal::Enable(void) {
    status = true;
}

void literal::Disable(void) {
    status = false;
}

#endif