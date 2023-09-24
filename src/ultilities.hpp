#include <cassert>
#include <cstdlib>
#include <iostream>
#include <cstring>
#include <algorithm>


#ifndef BITMAP
#define BITMAP
constexpr unsigned char bit0 = 0x01;
constexpr unsigned char bit1 = bit0 << 1;
constexpr unsigned char bit2 = bit1 << 1;
constexpr unsigned char bit3 = bit2 << 1;
constexpr unsigned char bit4 = bit3 << 1;
constexpr unsigned char bit5 = bit4 << 1;
constexpr unsigned char bit6 = bit5 << 1;
constexpr unsigned char bit7 = bit6 << 1;
unsigned char masks[8] = { 
    bit0, bit1, bit2, bit3,
    bit4, bit5, bit6, bit7
};
#endif


#ifndef COORDINATE
#define COORDINATE
/*
      RANGES:
ROW          COLUMN
1           [1,2,...,5]       
2           [1,2,...,6]
3           [1,2,...,7]
4           [1,2,...,8]
5           [1,2,...,9]
6           [1,2,...,8]
7           [1,2,...,7]
8           [1,2,...,6]
9           [1,2,...,5]

COLUMN      ROW
1           [1,2,...,9]
2           [1,2,...,9]
3           [1,2,...,9]
4           [1,2,...,9]
5           [1,2,...,9]
6           [2,3,...,8]
7           [3,4,5,6,7]
8           [ 4, 5, 6 ]
9           [    5    ]

X(Y)         Y(X)
0           [4,5,...,8]
1           [3,4,...,8]
2           [2,3,...,8]
3           [1,2,...,8]
4           [0,1,...,8]
5           [0,1,...,7]
6           [0,1,...,6]
7           [0,1,...,5]
8           [0,1,...,4]

*/
int infXY[] = {4, 3, 2, 1, 0, 0, 0, 0, 0};
int supXY[] = {8, 8, 8, 8, 8, 7, 6, 5, 4};
int infRow[] = {0, 1, 1, 1, 1, 1, 1, 1, 1, 1};
int supRow[] = {0, 5, 6, 7, 8, 9, 8, 7, 6, 5};

struct VectorXY{
    int x, y;  
};


struct VectorRowColumn{
    int row, col;  
};


bool InAreaXY(int x, int y);
bool InAreaXY(VectorXY xy);
bool InAreaRowColumn(VectorRowColumn rowCol);
VectorXY RowColumnToXY(VectorRowColumn);
VectorRowColumn XYtoRowColunm(VectorXY);


bool InAreaXY(int x, int y) {
    return x >= 0 && y >= 0 && x <= 8 && y <= 8 && x + y >= 4 && x + y <= 12; 
}


bool InAreaXY(VectorXY xy) {
    return InAreaXY(xy.x, xy.y); 
}


bool InAreaRowColumn(VectorRowColumn rowCol) {
    return InAreaXY(RowColumnToXY(rowCol));
}

// x + y = row + 3
// upper: y + 1 = col
// under: col = 9 - x


VectorXY RowColumnToXY(VectorRowColumn rowCol) {
    VectorXY xy;
    if(rowCol.row <= 5) {
        xy.y = rowCol.col - 1;
        xy.x = rowCol.row + 3 - xy.y;
    }
    else
    {
        xy.x = 9 - rowCol.col;
        xy.y = rowCol.row + 3 - xy.x;
    }
    return xy;
}


VectorRowColumn XYtoRowColunm(VectorXY xy){
    VectorRowColumn rowCol;
    rowCol.row = xy.x + xy.y - 3; 
    if(rowCol.row <= 5) {
        rowCol.col = xy.y + 1;
    }
    else
    {
        rowCol.col = 9 - xy.x;
    }
    return rowCol;
};

#endif


#ifndef MYSTACK
#define MYSTACK
/*
    自己实现的stack类，用于子句集的回溯
*/
#ifndef STK_MEM_INCR
#define STK_MEM_INCR 250
#endif

// typedef Step typeS;
template <typename typeS> 
class MyStack {
public:
    // 构造函数、析构函数
    MyStack();
    ~MyStack();
    // 基本功能函数
    void Resize(int newSize);
    typeS Pop(void);         // 出栈
    void Push(typeS V);      // 进栈
    bool Empty(void);        // 判空
private:
    typeS * arr;
    int top;
    int size;
};

template <typename typeS> 
MyStack<typeS>::MyStack() {
    arr = nullptr;
    size = 0;
    top = 0;
}

template <typename typeS> 
MyStack<typeS>::~MyStack() {
    if(arr != nullptr) 
        delete[] arr;
}

template <typename typeS> 
void MyStack<typeS>::Resize(int newSize) {
    // if(newSize <= 0) {
    //     std::cout << "\nmyStack.hpp : myStack::Resize() : 重新分配内存失败，内存大小不能小于等于0！";
    //     exit(-1);// 及时结束程序，防止Segmentation fault覆盖错误位置信息
    // }
    // if(newSize < top) { 
    //     std::cout << "\nmyStack.hpp : myStack::Resize() : 重新分配内存失败，内存大小不能小于已有有效数据所占大小！";  
    //     exit(-1);
    // }
    typeS * newArr = new (std::nothrow) typeS[newSize];
    assert(newArr != nullptr);
    if(arr) {
        for(int i = 0; i < top; i++) {
            newArr[i] = arr[i];
        }
        delete[] arr;
        arr = nullptr;
    }
    size = newSize;
    arr = newArr;
}

template <typename typeS> 
typeS MyStack<typeS>::Pop() {
    // if(Empty()) {
    //     std::cout << "\nmyStack.hpp : myStack::Pop() : 出栈失败，栈下溢！";
    //     exit(-1);
    // }
    // else
    top--;
    return arr[top];
}

template <typename typeS> 
void MyStack<typeS>::Push(typeS elem) { 
    if(top == size) Resize(size + STK_MEM_INCR);
    // if(size < top) {
    //     std::cout<<"\nmyStack.hpp : myStack::Push() : 入栈失败，有效数据溢出内存区域，堆已经损坏！";
    //     exit(-1);
    // }
    arr[top] = elem;
    top++;
}

template <typename typeS> 
bool MyStack<typeS>::Empty(void) {
    return top == 0;
}


#endif


#ifndef STEP
#define STEP

/*
    Step 三元组
    operation：操作：
        0：删除子句
        1：删除子句中的文字
        2：添加单子句
    cla：子句索引（下标）
    lit：被删除的文字
*/
typedef struct {
    uint8_t operation; // 0 : Delete a clause ; 1 : Delete a literal in the clause 2 : add an unit clause
    int clau;
    int lit;
} Step; // Step 三元组

#endif


#ifndef VECTOR
#define VECTOR
/*
    自己实现的vector类，用来存储子句
*/

#ifndef SUCCESS
#define SUCCESS 0
#endif

#ifndef ERROR
#define ERROR -1
#endif


#ifndef VCT_MEM_INCR
#define VCT_MEM_INCR 50
#endif

template <typename typeV>
class Vector{
    public:
        Vector();                    // 默认构造函数
        ~Vector();                   // 析构函数
        typeV & operator[](int x) const;
        Vector<typeV> & operator=(Vector<typeV> && ); // 这个交换指针域的赋值，右值清空
        // 定义移动赋值函数后拷贝构造函数会被隐式删除
        // 功能函数
        void Add(typeV&);            // 添加元素，typeV == Vector 时 > operator=() > SwapElem，参数向量会被清空
        void Resize(int);            // 重新分配内存
        bool Empty(void) const;      // 判空
        int Length(void) const;      // 向量长度
        int Size(void) const;        // 可用空间
        // int Find(typeV & x) const;   // 寻找元素 
        typeV * SwapElem();          // 交换指针域
    private:
        typeV * elem;                // 数据域
        int length;                  // 向量长度
        int size;                    // 数据域内存大小
};


template <typename typeV>
Vector<typeV>::Vector(){
    elem = nullptr;
    size = 0;
    length = 0; 
}


template <typename typeV>
Vector<typeV>::~Vector() { 
    if(elem != nullptr) delete[] elem;
}

template <typename typeV>
void Vector<typeV>::Add( typeV & v) {
    if(length == size) 
        Resize(size+VCT_MEM_INCR);
    elem[length++] = std::move(v);
}     

template <typename typeV>
typeV & Vector<typeV>::operator[](int x) const{
    return elem[x];
}


template <typename typeV>
int Vector<typeV>::Size(void) const {
    return size;
}


template <typename typeV>
typeV * Vector<typeV>::SwapElem() {
    typeV * tmp = elem;
    elem = nullptr;
    length = 0;
    size = 0;
    return tmp;
}


template <typename typeV>
Vector<typeV> & Vector<typeV>::operator=(Vector<typeV> && obj) {
    if(this != &obj) {
        length = obj.Length();
        size = obj.Size();
        if(elem != nullptr) delete[] elem;
        elem = obj.SwapElem();
    }
    return *this;
}


template <typename typeV>
void Vector<typeV>::Resize(int newSize) {
    // if(newSize <= 0) {
    //     std::cout<<"\nvector.hpp : Vector<typeV>::Resize() : 重新分配内存失败，内存大小不能小于等于0！";  
    //     exit(-1);
    // }
    // if(newSize <= size) {
    //     std::cout<<"\nvector.hpp : Vector<typeV>::Resize() : 重新分配内存失败，内存大小不能小于等于已有大小！";  
    //     exit(-1);
    // }
    typeV * newSpace = new (std::nothrow) typeV[newSize];
    assert(newSpace != nullptr);
    if(elem) {
        // memcpy(newSpace, elem, size * sizeof(typeV));
        #pragma unroll 3
        for(int i = 0; i < length; i++){
            newSpace[i] = std::move(elem[i]);
        }
        delete[] elem;
        elem = nullptr;
    }   
    elem = newSpace;
    size = newSize;
}


// template <typename typeV>
// bool Vector<typeV>::IsUnit (void) const{
//     return length == 1;
// }


template <typename typeV>
bool Vector<typeV>::Empty (void) const{
    return length == 0;
}


template <typename typeV>
int Vector<typeV>::Length(void) const {
    return length; 
}

#endif


#ifndef MYQUEUE
#define MYQUEUE
#define QUU_MEM_INCR 50
// template<typename typeQ>
typedef  std::pair<int, int> typeQ;
class MyQueue
{
private:
    typeQ * elem = nullptr;
    int size = 0;
    int length = 0;
    int front = 0;
    int back = 0; // 约定back指向队尾后一个位置
public:
    MyQueue(){}
    ~MyQueue() {
        if(elem != nullptr) delete elem;
    };
    void Resize(int n) {
        typeQ * tmp = new (std::nothrow) typeQ[n];
        assert(tmp != nullptr);
        if(elem) {
            if(back >= front) {
                #pragma unroll 8
                for(int i = front, j = 0; i < back; i++, j++){
                    tmp[j] = elem[i];
                }
                back = back - front;
                front = 0;
            }
            else{
                int j = 0;
                #pragma unroll 8
                for(int i = front; i < size; i++, j++){
                    tmp[j] = elem[i];
                }
                #pragma unroll 8
                for(int i = 0; i < back; i++, j++){
                    tmp[j] = elem[i];
                }
                back = length;
                front = 0;
            }
            delete[] elem;
        }
        size = n; // capacity = n-1
        elem = tmp;
    }
    typeQ Pop(void) {
        typeQ tmp = elem[front];
        front = (front+1)%size;
        length--;
        return tmp;
    }
    // typeQ Top(void) {
    //     return elem[front];
    // }
    void Push(typeQ & x) {
        if(length >= size - 1) 
            Resize(size+QUU_MEM_INCR);
        elem[back] = x;
        back = (back+1)%size;
        length++;
    }
    bool Empty(void) const{
        return length == 0;
    }
    int Length(void) const{
        return length;
    }
    void Delete1(int clausePos) {
        if(back >= front) {
            #pragma unroll 8
            for(int i = front, j = 0; i < back; i++, j++){
                if(elem[i].first == clausePos){
                    elem[i].first = -1;
                    break;
                }
            }
        }
        else{
            #pragma unroll 8
            for(int i = front; i < size; i++){
                if(elem[i].first == clausePos){
                    elem[i].first = -1;
                    break;
                }
            }
            #pragma unroll 8
            for(int i = 0; i < back; i++){
                if(elem[i].first == clausePos){
                    elem[i].first = -1;
                    break;
                }
            }
        }    
    };

    // void Delete2(int literal) {
    //     if(back >= front) {
    //         #pragma unroll 8
    //         for(int i = front, j = 0; i < back; i++, j++){
    //             if(elem[i].second == literal)
    //                 elem[i].first = -1;
    //         }
    //     }
    //     else{
    //         #pragma unroll 8
    //         for(int i = front; i < size; i++){
    //             if(elem[i].second == literal)
    //                 elem[i].first = -1;
    //         }
    //         #pragma unroll 8
    //         for(int i = 0; i < back; i++){
    //             if(elem[i].second == literal)
    //                 elem[i].first = -1;
    //         }
    //     }    
    // };
};


#endif