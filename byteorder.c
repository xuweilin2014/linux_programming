//
// Created by xuweilin on 22-10-22.
//
#include <stdio.h>

// 判断机器的字节序
void byteorder() {
    union {
        short value;
        char union_bytes[sizeof(short)]
    } test;

    test.value = 0x0102;

    // 大端字节序（整数的低位字节保存到内存的高位地址处）
    if(test.union_bytes[0] == 1 && test.union_bytes[1] == 2) {
        printf("big endian\n");
    // 小端字节序（整数的低位字节保存到内存的低地址处）
    } else if(test.union_bytes[0] == 2 && test.union_bytes[1] == 1) {
        printf("small endian\n");
    } else {
        printf("unknown..");
    }
}
