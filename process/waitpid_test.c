//
// Created by xuweilin on 22-11-3.
//
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/wait.h>

/**
 * 使用 waitpid 来回收子进程，下面的程序使用 for 循环创建出 5 个子进程，然后父进程使用 waitpid 回收某一个特定子进程
 */

int waitpid_test() {

    int i;
    pid_t pid, wpid, tmpid;

    for (i = 0; i < 5; i++) {
        if ((pid = fork()) == 0) {
            break;
        }
        if (i == 2) {
            tmpid = pid;
            printf("--------pid = %d\n", tmpid);
        }
    }

    if (5 == i) {
        sleep(5);

        // wait(NULL);   一次 wait/waitpid 函数调用只能回收一个子进程
        // wpid = waitpid(-1, NULL, WNOHANG); 回收任意子进程，没有结束的子进程，父进程直接返回 0
        // wpid = waitpid(tmpid, NULL, 0);  指定一个子进程，不阻塞等待
        wpid = waitpid(tmpid, NULL, WNOHANG);

        if (wpid == -1) {
            perror("waitpid error");
            exit(1);
        }

        printf("I'm a parent, wait a child finish: %d \n", wpid);
    } else {
        sleep(i);
        printf("I'm %dth child， pid=%d\n", i + 1, getpid());
    }

    return 0;
}