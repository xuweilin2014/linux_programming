//
// Created by xuweilin on 22-11-4.
//
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <errno.h>
#include <fcntl.h>
#include <sys/mman.h>
#include <string.h>
#include <sys/wait.h>

/**
 * 父子进程使用 mmap 进程间通信，父进程先创建映射区，open(O_RDWR) mmap(MAP_SHARED)
 * 然后指定 MAP_SHARED 权限。
 *
 * 下面的代码，父子进程使用 mmap 进行通信，共享内存大小是一个 int 变量，子进程往里面写入变量值，父进程会读取出对应的值，
 * 实现共享，但是 var_global 不共享
 */

int var_global = 20;

int fork_mmap() {
    int *p;
    pid_t pid;

    int fd;
    fd = open("../dir/temp", O_RDWR|O_CREAT|O_TRUNC, 0644);
    if (fd < 0) {
        perror("open error");
        exit(1);
    }
    ftruncate(fd, 4);

    p = (int*) mmap(NULL, 4, PROT_WRITE|PROT_READ, MAP_SHARED, fd, 0);
    if (p == MAP_FAILED) {
        perror("mmap error");
        exit(1);
    }
    close(fd);

    pid = fork();
    if (pid == 0) {
        *p = 2000;
        var_global = 1000;
        printf("child, *p=%d, var=%d\n", *p, var_global);
    } else {
        sleep(1);
        printf("parent, *p=%d, var=%d\n", *p, var_global);
        wait(NULL);

        int ret = munmap(p, 4);
        if (ret == -1) {
            perror("munmap error");
            exit(1);
        }
    }

    return 0;
}