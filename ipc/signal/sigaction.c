//
// Created by xuweilin on 22-11-5.
//
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/wait.h>
#include <sys/time.h>

/**
 * 下面的程序演示了 sigaction 函数的使用，当某一个信号触发时，sigaction 中指定的回调函数会被调用
 *
 * 下述实例中，调用 sigaction 注册了两个信号，SIGINT 和 SIGQUIT，当触发了 SIGINT 时，再次触发 SIGINT
 * 不会引起act的调用；同理，当调用 SIGQUIT 时，再次触发 SIGQUIT，也不会引起 act 的调用
 */

void sig_catch(int signo) {
    // SIGINT ==> (CTRL + C)
    if (signo == SIGINT) {
        printf("catch you!! %d\n", signo);
    // SIGQUIT ==> (CTRL + \)
    } else if (signo == SIGQUIT) {
        printf("---------catch you!!%d\n", signo);
    }
}

int sigaction_test() {
    struct sigaction act, old_act;

    act.sa_handler = sig_catch;

    /**
     * sa_mask 字段定义了一组信号，在调用由 sa_handler 所定义的处理器程序时将阻塞该组信号。当调用信号处理器程序时，
     * 会在调用信号处理器之前，将 sa_mask 定义的信号中未处于当前进程掩码之列的任何信号自动添加到进程掩码中。这些信号将保留在进程掩码中，
     * 直至信号处理器函数返回，届时将自动删除这些信号。利用 sa_mask 字段可指定一组信号，不允许它们中断此处理器程序的执行。
     * 此外，引发对处理器程序调用的信号将自动添加到进程信号掩码中。这意味着，当正在执行处理器程序时，如果同一个信号实例第二次
     * 抵达，信号处理器程序将不会递归中断自己。
     */
    sigemptyset(&act.sa_mask);
    act.sa_flags = 0;

    int ret = sigaction(SIGINT, &act, &old_act);
    if (ret == -1) {
        perror("sigaction error");
        exit(1);
    }

    ret = sigaction(SIGQUIT, &act, &old_act);
    if (ret == -1) {
        perror("sigaction error");
        exit(1);
    }

    while (1);

    return 0;
}