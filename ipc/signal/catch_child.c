//
// Created by xuweilin on 22-11-5.
//
#include <unistd.h>
#include <stdlib.h>
#include <stdio.h>
#include <errno.h>
#include <signal.h>
#include <sys/wait.h>

/**
 * 本程序使用信号让父进程回收多个子进程
 * @param signo
 */

void catch_child(int signo) {
    pid_t wpid;
    int status;

    // 由于信号处理函数不具有排队性，因此当多个子进程结束，发送 SIG_CHLD 信号时，如果注册函数正在执行，
    // 那么执行完毕之后就只会处理其中一个信号，再调用一次 catch_child 方法，因此需要循环执行 wait/waitpid
    // 处理已经死亡的多个子进程
    // if ((wpid = wait(&status)) != -1) {
    while ((wpid = waitpid(-1, &status, 0)) != -1) {
        if (WIFEXITED(status)) {
            printf("catch child id %d, ret = %d\n", wpid, WEXITSTATUS(status));
        }
    }
}

int catch_child_signo() {
    pid_t pid;

    // 在父进程注册信号处理函数之前，屏蔽掉 SIG_CHLD 信号，这是因为，如果父进程在注册信号处理函数之前，子进程就运行完毕时，
    // 会发送一个 SIG_CHLD 信号给父进程，此时注册还没完成，因此父进程会直接忽略掉信号。因此屏蔽掉此信号之后，当再有 SIG_CHLD
    // 信号发送过来时，直接会把对应未决信号表中对应的位置为 1，当父进程注册完成之后，解除屏蔽，就会触发父进程的信号处理函数，循环
    // 调用 wait 回收子进程
    sigset_t set;
    sigemptyset(&set);
    sigaddset(&set, SIGCHLD);
    sigprocmask(SIG_BLOCK, &set, NULL);

    int i;
    for (i = 0; i < 15; i++) {
        if ((pid = fork()) == 0) {
            break;
        }
    }

    if (i == 15) {
        struct sigaction act;
        act.sa_handler = catch_child;
        sigemptyset(&act.sa_mask);
        act.sa_flags = 0;
        // 注册信号处理函数
        sigaction(SIGCHLD, &act, NULL);
        // 解除屏蔽
        sigprocmask(SIG_UNBLOCK, &set, NULL);
        printf("I'm parent, pid = %d\n", getpid());
        while (1);
    } else {
        printf("I'm child, pid = %d\n", getpid());
        exit(i);
    }

    return 0;
}
