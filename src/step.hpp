#ifndef STEP
#define STEP

#include "vector.hpp"

typedef struct {
    char operation; // 1 : Delete a clause ; 2 : Delete literals in the clause
    int cla;
    int lit;
} Step;

#endif