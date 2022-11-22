//
// Created by xuweilin on 22-11-3.
//
#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <string.h>

/**
 * 使用 pipe 实现最基本的进程间通信，创建打开管道之后，fd[0] 为读端，fd[1] 为写端，
 * 管道 pipe 只能在父子进程之间使用，或者兄弟进程之间使用，不能在无血缘关系之间的进程使用
 *
 * 下面的程序，父进程往管道中写入数据，而子进程从中读出数据，然后子进程再显示到屏幕
 */

int pipe_test() {
    int ret;
    int fd[2];
    pid_t pid;

    char *str = "hello pipe\n";
    char buf[1024];

    ret = pipe(fd);
    if (ret == -1) {
        perror("pipe error");
        exit(1);
    }

    pid = fork();
    if (pid > 0) {
        // 父进程往管道中写入数据，因此需要关闭管道的读端 fd[0]
        close(fd[0]);
        write(fd[1], str, strlen(str));
        close(fd[1]);
    } else if (pid == 0){
        // 子进程往管道中读出数据，因此需要关闭管道的写端 fd[1]
        close(fd[1]);
        ret = read(fd[0], buf, sizeof(buf));
        write(STDOUT_FILENO, buf, ret);
        close(fd[0]);
    }

    return 0;
}