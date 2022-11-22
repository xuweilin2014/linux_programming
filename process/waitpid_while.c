//
// Created by xuweilin on 22-11-3.
//
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/wait.h>

/**
 * wait 和 waitpid 一次只能回收一个子进程，要回收多个子进程，必须将 waitpid 和 while 循环结合使用。
 *
 * 下面的程序使用 waitpid 和 while 回收多个创建出来的子进程
 */

int waitpid_while() {

    int i;
    pid_t pid, wpid, tmpid;

    for (i = 0; i < 5; i++) {
        if (fork() == 0) {
            break;
        }
    }

    if (5 == i) {
        // 使用阻塞的方式回收子进程
        /*while ((wpid = waitpid(-1, NULL, 0)) != -1) {
            printf("wait child %d\n", wpid);
        }*/

        // -1 表示任意子进程，WNOHANG 表示使用非阻塞的方式回收子进程
        while ((wpid = waitpid(-1, NULL, WNOHANG)) != -1) {
            if (wpid > 0) {
                printf("wait child %d\n", wpid);

            // waitpid 函数调用时， 参 3 指定了WNOHANG，如果返回 0， 表明没有子进程结束。
            } else if (wpid == 0) {
                sleep(1);
            }
        }
    } else {
        sleep(i);
        printf("I'm %dth child， pid=%d\n", i + 1, getpid());
    }

    return 0;
}