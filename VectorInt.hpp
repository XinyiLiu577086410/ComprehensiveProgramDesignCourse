#define SUCCESS 0
#define ERROR -1
#define MAX_LITERALS 1000
class VectorInt{
    public:
        int Add(int x);
        int Delete(int x);
        int Find(int x); // return index([0,1,....999]) of x 
        bool IsSingle();
        int GetFirstLiteral(void);
        bool Empty(void);
        VectorInt(int x);
        VectorInt(void);
    private:
        int a[MAX_LITERALS] = {0,};
        int length = 0;
};

VectorInt::VectorInt(int x){
    Add(x);
}

VectorInt::VectorInt (void){}

int VectorInt::Find (int x) {
    int i;
    for (i = length - 1; i >= 0; i--) {
        if(a[i] == x) break;
    }
    return i; // 返回-1（ERROR）表示没有找到
}

int VectorInt::Add (int x) {
    if(length >= MAX_LITERALS) return ERROR;
    a[length] = x;
    length++;
    return SUCCESS;
}

int VectorInt::Delete (int x) {
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

bool VectorInt::IsSingle (void) {
    return length == 1;
}
bool VectorInt::Empty (void) {
    return length == 0;
}
int VectorInt::GetFirstLiteral (void) {
    if (Empty()) return 0; // 没有找到
    return a[0];
}