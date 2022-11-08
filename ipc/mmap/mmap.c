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

int mmap_test() {
    char *p = NULL;
    int fd;

    fd = open("../dir/test_mmap", O_CREAT | O_TRUNC | O_RDWR, 0644);
    if (fd == -1) {
        perror("open error");
        exit(1);
    }

    // 将文件大小扩展为 20，并且需要文件的写权限才能对文件进行拓展
    ftruncate(fd, 20);
    // 使用 lseek 获取到文件的大小
    int len = lseek(fd, 0, SEEK_END);

    p = mmap(0, len, PROT_READ | PROT_WRITE, MAP_SHARED, fd, 0);
    if (p == MAP_FAILED) {
        perror("mmap error");
        exit(1);
    }

    strcpy(p, "hello mmap");
    printf("----%s-----\n", p);

    int ret = munmap(p, len);
    if (ret < 0) {
        perror("munmap error\n");
        exit(1);
    }

    return 0;
}