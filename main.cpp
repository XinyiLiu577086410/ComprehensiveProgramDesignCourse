#include <iostream>
#include <cstdio>
#include "VectorInt.hpp"
#include "Cnf.hpp"
int main(void){
    char name[30], s[100000];
    size_t pos = 0;
    std::cout << "请输入CNF文件名\n";
    std::cin >> name;
    FILE * fp = fopen(name, "r");

    while(!feof(fp)) {
        s[pos++] = fgetc(fp);
    }
    s[pos] = '\0';
    printf("%s", s);
    return 0;
}