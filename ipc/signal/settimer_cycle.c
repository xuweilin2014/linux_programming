//
// Created by xuweilin on 22-11-5.
//
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/wait.h>
#include <sys/time.h>

/**
 * 下面的 setitimer 设定的时间第一次隔 2s 后向当前进程发送 SIGALRM 信号，然后每隔 5s 向当前进程发送 SIGALRM 信号
 * 由于使用 signal 函数注册了 SIGALRM 信号的处理函数，相当于 myfunc 按照 setitimer 设定的时间间隔进行调用
 */

void myfunc(int signo) {
    printf("hello world\n");
}

int settimer_cycle() {
    struct itimerval it, oldit;

    signal(SIGALRM, myfunc);

    it.it_value.tv_sec = 2;
    it.it_value.tv_usec = 0;

    it.it_interval.tv_sec = 5;
    it.it_interval.tv_usec = 0;

    if (setitimer(ITIMER_REAL, &it, &oldit) == -1) {
        perror("setitimer error");
        return -1;
    }

    while (1);

    return 0;
}