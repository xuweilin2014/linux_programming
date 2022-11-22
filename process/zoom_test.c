//
// Created by xuweilin on 22-10-31.
//
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/wait.h>

/**
 * 下面程序使用 wait(&status) 捕获子进程异常终止的原因
 *
 * 获取子进程正常终止值：
 *
 * WIFEXITED(status) --》 为真 --》调用 WEXITSTATUS(status) --》 得到 子进程 退出值。
 *
 * 获取导致子进程异常终止信号：
 *
 * WIFSIGNALED(status) --》 为真 --》调用 WTERMSIG(status) --》 得到 导致子进程异常终止的信号编号。
 *
 */

int zoom_test() {
    pid_t pid, wpid;
    pid = fork();

    int status;

    if (pid == 0) {
        printf("---child, my parent=%d, my=%d, going to sleep 10s\n", getppid(), getpid());
        printf("-------------child die-------------\n");
        exit(73);
    } else if (pid > 0) {
        // wait(NULL); 不关心子进程终止的原因
        // 如果子进程未终止，父进程阻塞在这个函数上
        // 一次 wait/waitpid 函数调用，只能回收随机一个子进程
        sleep(5);
        // status 表明子进程终止的原因
        wpid = wait(&status);
        if (wpid == -1) {
            perror("wait err");
            exit(1);
        }

        // 为真，说明子进程正常终止
        if (WIFEXITED(status)) {
            printf("child exit with %d\n", WEXITSTATUS(status));
        }

        // 为真，说明子进程被信号终止
        if (WIFSIGNALED(status)) {
            printf("child kill with signal %d\n", WTERMSIG(status));
        }

        printf("I am parent, pid = %d, my son = %d\n", getpid(), pid);
    } else {
        perror("fork");
        exit(1);
    }

    return 20;
}