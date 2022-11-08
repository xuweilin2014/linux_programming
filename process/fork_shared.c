//
// Created by xuweilin on 22-10-31.
//
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

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