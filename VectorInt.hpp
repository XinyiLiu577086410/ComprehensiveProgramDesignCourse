#define SUCCESS 0
#define ERROR -1
class VectorInt{
     public:
        int Add(int x);
        int Delete(int x);
        int Find(int x); // return index([0,1,....9999]) of x 
    private:
        int a[10000];
        int length = 0;
};
int VectorInt::Add(int x) {
    if(length >= 10000) return ERROR;
    a[length] = x;
    length++;
    return SUCCESS;

}
int VectorInt::Delete(int x){
    int pos = vectorInt::Find(x);
    if(pos >= 0){
        a[pos] = a[length-1];
        length--;
        return SUCCESS;        
    }else return ERROR;
}