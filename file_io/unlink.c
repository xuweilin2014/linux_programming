//
// Created by xuweilin on 22-10-30.
//
#include <unistd.h>
#include <fcntl.h>
#include <stdlib.h>
#include <string.h>
#include <stdio.h>

int unlink_test(void) {
    int fd;
    char *p = "test of unlink.\n";
    char *p2 = "after write something\n";

    fd = open("../file_io/temp.txt", O_RDWR | O_CREAT | O_TRUNC, 0644);
    if (fd < 0) {
        perror("open temp error");
        exit(1);
    }

    int ret = unlink("../file_io/temp.txt");
    if (ret < 0) {
        perror("unlink error");
        exit(1);
    }

    ret = write(fd, p, strlen(p));
    if (ret == -1) {
        perror("-----write error");
    }

    printf("hi! I'm printf\n");
    ret = write(fd, p2, strlen(p2));
    if (ret == -1) {
        perror("-----write error");
    }

    printf("Enter anykey continue\n");
    getchar();

    close(fd);
    return 0;
}