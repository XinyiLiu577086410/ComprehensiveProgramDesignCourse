#ifndef STEP
#define STEP

#include "vector.hpp"

/*
    Step 三元组
    operation：操作：
        0：伪删除子句
        1：伪删除子句中的文字
        2：添加单子句
    cla：子句索引（下标）
    lit：被删除的文字
*/
typedef struct {
    char operation; // 1 : Delete a clause ; 2 : Delete literals in the clause
    int cla;
    int lit;
} Step; // Step 三元组

#endif