//
// Created by xuweilin on 22-11-5.
//
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/wait.h>

/**
 * unsigned int alarm(unsigned int seconds)
 *
 * 在 seconds 秒过后，发送 SIGALARM 信号给当前调用 alarm 函数的进程，当前进程会终止运行
 *
 * 下面的程序统计计算机 1s 之内能够计数多少
 */

int alarm_test() {

    int i = 0;
    alarm(1);

    while (1) {
        printf("%d\n", i++);
    }

}