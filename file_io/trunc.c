//
// Created by xuweilin on 22-10-29.
//
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

/*
 * 传入参数：
 *      1.指针作为函数参数
 *      2.通常有 const 关键字修饰
 *      3.指针指向有效区域，在函数内部做读操作
 * 传出参数：
 *      1.指针作为函数参数
 *      2.在函数调用之前，指针指向的空间可以无意义，但是必须有效
 *      3.在函数内部，做写操作
 *      4.函数调用结束之后，充当函数返回值
 * 传入传出参数：
 *      1.指针作为函数参数
 *      2.在函数调用之前，指针指向的空间可必须有实际意义
 *      3.在函数内部，先做读操作，后做写操作
 *      4.函数调用结束之后，充当函数返回值
 */

// 使用 truncate 扩展文件大小
int trunc_test() {
    // open/lseek(fd, 249, SEEK_END)/write(fd, "\0", 1);
    // 使用 lseek 扩展文件如上所是，并且 lseek 扩展文件最后必须要有 io 操作才能真正扩展

    int ret = truncate("../file_io/lseek.txt", 700);

    printf("ret = %d\n", ret);

    return 0;
}