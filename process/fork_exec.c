//
// Created by xuweilin on 22-10-31.
//
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

/**
 * execl 和 execlp 函数的使用，调用系统程序
 */

int fork_exec() {
    pid_t pid = fork();
    if (pid == -1) {
        perror("fork error");
        exit(1);
    } else if (pid == 0) {
        // execlp("ls", "-l", "-h", NULL); 错误写法
        // 参数1：要加载的程序名字，该函数需要配合 PATH 环境变量来使用，当 PATH 所有目录搜素后没有参数1则返回出错。
        // 该函数通常用来调用系统程序。如 ls、date、cp、cat 命令。
        // execlp("ls", "ls", "-l", "-h", "-a", NULL);
        // 和 execlp 不同的是，第一个参数是路径，不是文件名。
        execl("/bin/ls", "ls", "-l", "-h", "-a", NULL);
        perror("exec error");
        exit(1);
    } else if (pid > 0) {
        sleep(1);
        printf("I'm parent: %d\n", getpid());
    }

    return 0;
}