#include <cstring>
#ifndef SUCCESS
#define SUCCESS 0
#endif
#ifndef ERROR
#define ERROR -1
#endif
#define MAX_LITERALS 15 // 子句的最大长度（文字个数）
class Vector{
    public:
        int Add(int x);
        int Delete(int x);
        int Find(int x); // return index([0,1,....999]) of x 
        bool IsSingle();
        int GetFirstLiteral(void);
        bool Empty(void);
        Vector(int x);
        Vector(void);
        bool Verify(bool rslt[]){
            for(int i = 0; i < length; i++ ){
                if(a[i] > 0 && rslt[a[i]] || a[i] < 0 && !rslt[-a[i]]) return true;
            }
            return false;
        }
    private:
        int a[MAX_LITERALS];
        int length = 0;
};

Vector::Vector(int x){
    Add(x);
}

Vector::Vector (void){
    ;
}

int Vector::Find (int x) {
    int i;
    for (i = length - 1; i >= 0; i--) {
        if(a[i] == x) break;
    }
    return i; // 返回-1（ERROR）表示没有找到
}

int Vector::Add (int x) {
    if(length >= MAX_LITERALS) return ERROR;
    a[length] = x;
    length++;
    return SUCCESS;
}

int Vector::Delete (int x) {
    int pos = Find(x);
    if(pos != ERROR){
        while (pos != ERROR) {
            a[pos] = a[length-1];
            length--;
            pos = Find(x);
        }
        return SUCCESS;        
    }else return ERROR;
}

bool Vector::IsSingle (void) {
    return length == 1;
}
bool Vector::Empty (void) {
    return length == 0;
}
int Vector::GetFirstLiteral (void) {
    if (Empty()) return 0; // 没有找到
    return a[0];
}