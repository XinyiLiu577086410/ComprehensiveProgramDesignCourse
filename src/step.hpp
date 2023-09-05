#ifndef STEP
#define STEP

#include "vector.hpp"

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