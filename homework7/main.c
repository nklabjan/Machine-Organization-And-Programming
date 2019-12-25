#include <stdio.h>

#include <string.h>

typedef union {
    double d;
    char c[20];
}U;

int main() {
    U temp;
    temp.d = 11.5;
    strcpy(temp.c,"CS 354");
    printf("%zu, %f, %s\n", sizeof(temp), temp.d, temp.c);
    return 0;
}