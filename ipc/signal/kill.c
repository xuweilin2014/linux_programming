//
// Created by xuweilin on 22-11-5.
//
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/wait.h>

/**
 * kill 函数发送指定信号给对应进程
 *
 * int kill（pid_t pid, int signum）
 *
 * pid: > 0 发送信号给指定进程
 *      = 0 发送信号给调用 kill 函数的那个进程处于同一进程组的进程
 *      < -1 取绝对值，发送信号给该绝对值对应的进程组所有组员
 *      = -1 发送信号给有权限发送的所有进程
 *
 * 在下面的程序当中，子进程发送信号 kill 父进程
 */

int kill_test() {
    pid_t pid = fork();

    if (pid > 0) {
        printf("i'm parent, pid = %d\n", getpid());
        while (1);
    } else if (pid == 0) {
        printf("i'm child, pid = %d, parent pid =%d\n", getpid(), getppid());
        sleep(2);
        kill(getppid(), SIGKILL);
    }

    return 0;
}