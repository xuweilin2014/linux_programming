//
// Created by xuweilin on 22-10-31.
//
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/wait.h>

int zoom_test() {
    pid_t pid, wpid;
    pid = fork();

    int status;

    if (pid == 0) {
        printf("---child, my parent=%d, my=%d, going to sleep 10s\n", getppid(), getpid());
        sleep(15);
        printf("-------------child die-------------\n");
        return 73;
    } else if (pid > 0) {
        // wait(NULL); 不关心子进程终止的原因
        // 如果子进程未终止，父进程阻塞在这个函数上
        // 一次 wait/waitpid 函数调用，只能回收随机一个子进程
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

        printf("I am parent, pid = %d, myson = %d\n", getpid(), pid);
    } else {
        perror("fork");
        exit(1);
    }

    return 20;
}