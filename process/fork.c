//
// Created by xuweilin on 22-10-31.
//
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

int fork_test() {
    printf("before fork-1\n");
    printf("before fork-2\n");
    printf("before fork-3\n");
    printf("before fork-4\n");

    pid_t pid = fork();
    if (pid == -1) {
        perror("fork error");
        exit(1);
    } else if(pid == 0) {
        printf("---child process: my pid is %d, and my parent is %d\n", getpid(), getppid());
    } else if(pid > 0) {
        printf("---parent process: my child is %d, and my pid is %d, my parent is %d\n", pid, getpid(), getppid());
    }

    printf("=============end of file\n");

    return 0;
}