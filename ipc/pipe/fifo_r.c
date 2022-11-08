//
// Created by xuweilin on 22-11-4.
//
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <string.h>

int main() {
    int fd, len;
    char buf[4096];

    fd = open("../ipc/testfifo", O_RDONLY);
    if (fd < 0) {
        perror("open error");
        exit(1);
    }

    while (1) {
        len = read(fd, buf, sizeof(buf));
        write(STDOUT_FILENO, buf, len);
        sleep(3);
    }

    close(fd);

    return 0;
}