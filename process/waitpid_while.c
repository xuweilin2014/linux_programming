//
// Created by xuweilin on 22-11-3.
//
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/wait.h>

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

        while ((wpid = waitpid(-1, NULL, WNOHANG)) != -1) {
            if (wpid > 0) {
                printf("wait child %d\n", wpid);
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