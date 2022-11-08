//
// Created by xuweilin on 22-11-4.
//
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/stat.h>


int mkfifo_pipe() {
    int ret = mkfifo("../ipc/testfifo", 0664);
    if (ret == -1) {
        perror("mkfifo error");
        exit(1);
    }

    return 0;
}