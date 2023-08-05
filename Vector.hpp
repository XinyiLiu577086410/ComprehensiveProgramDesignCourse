#include <cstring>
#include <algorithm>
#include <iostream>
#ifndef SUCCESS
#define SUCCESS 0
#endif
#ifndef ERROR
#define ERROR -1
#endif
class Vector{
    public:
        int Add(int); //添加文字
        int Delete(int); // 删除文字，用末尾文字覆盖x
        int Find(int); // 返回x的索引
        bool IsSingle(void); // 判断是否是单子句
        int GetFirstLiteral(void); // 返回第一个文字
        bool Empty(void); // 判空
        
        Vector(); // 默认构造函数
        Vector(int, int); // 设定数据域大小，添加一个x
        Vector(int); // 设定数据域大小
        ~Vector() { if(a != nullptr) delete[] a; } // 析构函数
        void Resize(int);
        Vector & operator= (const Vector & Obj) {
            if(&Obj != this) {
                length = Obj.length;
                size = Obj.size;
                if(a != nullptr) delete[] a;
                // a = nullptr; //  可能是这里出现了double free？
                a = new int[Obj.size];
                memcpy(a, Obj.a, Obj.length * sizeof(int));
            }
            return *this;
        }

        bool Verify(bool []); // 验证
        void Show(void); // 展示Vector
        void Static(int []); //
    private:
        int * a;
        int length;
        int size;
};

void Vector::Resize(int newsize) {
    if(newsize < size){
        if(a) delete[] a;
        if(newsize == 0) a = nullptr;
        else a = new int[newsize];
        length = 0;
    }
    else {
        int * tmp = new int[newsize];
        if(a) {
            memcpy(tmp, a, length * sizeof(int));
            delete[] a;
        }   
        a = tmp;
    }
    size = newsize;
}


void Vector::Static(int table[]) {
    for (int i = 0; i < length; i++) table[a[i] + 100000]++;
}


void Vector::Show(void) {
    for(int i = 0; i < length; i++) std::cout << a[i] << ' ';
}


bool Vector::Verify(bool result[]) {
    for(int i = 0; i < length; i++ )
        if((a[i] > 0 && result[a[i]]) || (a[i] < 0 && !result[-a[i]]))  return true;
    return false;
}

Vector::Vector(){
    a = nullptr;
    size = 0;
    length = 0; 
}

Vector::Vector (int newsize, int x){
    a = new int[newsize];
    size = newsize;
    length = 0;
    Add(x);
}

Vector::Vector (int newsize){
    a = new int[newsize];
    size = newsize;
    length = 0;
}

int Vector::Find (int x) {
    int i;
    for (i = length - 1; i >= 0; i--) {
        if(a[i] == x) break;
    }
    return i; // 返回-1（ERROR）表示没有找到
}

int Vector::Add (int x) {
    if(size <= length) Resize(2 * (length + 1));
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
