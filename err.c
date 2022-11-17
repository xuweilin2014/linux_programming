//
// Created by xuweilin on 22-10-22.
//
#include <stdio.h>
#include <stdlib.h>

void sys_err(const char* str) {
    perror(str);
    exit(1);
}