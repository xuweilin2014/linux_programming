//
// Created by xuweilin on 22-10-31.
//
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

/**
 * 父子进程共享
 *
 * 父子进程共享：文件描述符、mmap映射区
 * 父子进程不共享：进程 id、返回值、各自的父进程、进程创建时间、闹钟、未决信号集、全局变量（读时共享，写时复制）
 *
 * 下面的程序中，全局变量 var 父进程和子进程更改之后，值不一样
 */

int var = 100;

int fork_shared() {
    pid_t pid = fork();

    if (pid == -1) {
        perror("fork error");
        exit(1);
    } else if (pid > 0) {
        var = 288;
        printf("parent, var = %d\n", var);
        printf("I'm parent pid=%d, getppid=%d\n", getpid(), getppid());
    } else if (pid == 0) {
        sleep(5);
        var = 120;
        printf("child, var = %d\n", var);
        printf("I'm child pid=%d, getppid=%d\n", getpid(), getppid());
    }

    getchar();
    return 0;
}