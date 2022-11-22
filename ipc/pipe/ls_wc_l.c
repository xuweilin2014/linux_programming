//
// Created by xuweilin on 22-11-3.
//
#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <string.h>

/**
 * pipe 管道可以用于父子进程之间的通信，下面的程序子进程将 ls 命令的结果写入到管道中，
 * 父进程从管道中读取结果执行 wc 命令
 */

int ls_wc_l() {
    int fd[2];
    int ret;
    pid_t pid;

    ret = pipe(fd);
    if (ret == -1) {
        perror("pipe error");
        exit(1);
    }

    pid = fork();
    if (pid == -1) {
        perror("fork error");
        exit(1);
    } else if(pid > 0) {
        close(fd[1]);
        // 将 STDIN 标准输入重定向到 fd[0]，也就是 wc 命令从管道中读取数据
        // fd[0] 为读端
        dup2(fd[0], STDIN_FILENO);
        execlp("wc", "wc", "-l", NULL);
        perror("execlp error");
        exit(1);
    } else if (pid == 0) {
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