//
// Created by xuweilin on 22-11-3.
//
#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <string.h>
#include <sys/wait.h>

/**
 * pipe 管道可以用于两个兄弟进程之间的通信，下面的程序 1# 进程将 ls 命令的结果写入到管道中，
 * 2# 进程从管道中读取结果执行 wc 命令
 */

int pipe_broth() {
    int fd[2];
    int ret, i;
    pid_t pid;

    ret = pipe(fd);
    if (ret == -1) {
        perror("pipe error");
        exit(1);
    }

    for (i = 0; i < 2; i++) {
        pid = fork();
        if (pid == -1) {
            perror("fork error");
            exit(1);
        }
        if (pid == 0)
            break;
    }

    if (i == 2) {
        // 父进程不使用管道，所以一定要关闭父进程的管道，保证数据单向流动
        // close(fd[0]);
        close(fd[1]);

        // 父进程回收两个子进程
        wait(NULL);
        wait(NULL);
    } else if(i == 0) {
        close(fd[1]);
        // 将 STDIN 标准输入重定向到 fd[0]，也就是 wc 命令从管道中读取数据
        // fd[0] 为读端
        dup2(fd[0], STDIN_FILENO);
        execlp("wc", "wc", "-l", NULL);
        perror("execlp error");
        exit(1);
    } else if (i == 1) {
        close(fd[0]);
        // 将 STDOUT 标准输出重定向到 fd[1]，也就是 ls 命令的结果输出到管道中
        // fd[1] 为写端
        dup2(fd[1], STDOUT_FILENO);
        execlp("ls", "ls", NULL);
        perror("execlp error");
        exit(1);
    }

    return 0;
}