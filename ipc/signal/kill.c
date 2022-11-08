//
// Created by xuweilin on 22-11-5.
//
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/wait.h>

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