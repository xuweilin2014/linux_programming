//
// Created by xuweilin on 22-10-31.
//
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <pthread.h>

/**
 * 循环创建子进程
 */

int loop_fork() {
    int i;
    pid_t pid;

    for (i = 0; i < 5; i++) {
        if (fork() == 0) {
            break;
        }
    }

    if (5 == i) {
        sleep(i + 1);
        printf("I'm parent\n");
    } else {
        sleep(i);
        printf("I'm %dth child\n", i + 1);
    }

    return 0;
}