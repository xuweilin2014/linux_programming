//
// Created by xuweilin on 22-10-29.
//
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <pthread.h>
#include <sys/stat.h>

int stat_test() {
    struct stat sbuf;

    int ret = stat("../file_io/lseek.txt", &sbuf);
    // int ret = lstat("../file_io/lseek.txt", &sbuf);
    // stat 会穿透符号链接，而 lstat 不会

    if (ret == -1) {
        perror("stat error");
        exit(1);
    }

    if (S_ISREG(sbuf.st_mode)) {
        printf("It's a regular\n");
    } else if (S_ISDIR(sbuf.st_mode)) {
        printf("It's a dir\n");
    } else if (S_ISFIFO(sbuf.st_mode)) {
        printf("It's a pipe");
    } else if (S_ISLNK(sbuf.st_mode)) {
        printf("It's a sym link");
    }

    printf("file size:%ld\n", sbuf.st_size);
    return 0;
}