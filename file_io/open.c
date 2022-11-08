#include <unistd.h>
#include <fcntl.h>
#include <stdio.h>
#include <errno.h>
#include <string.h>

int open_test() {
    int fd;
    fd = open("./dict.c", O_RDONLY | O_TRUNC);
    printf("fd = %d, errno=%d:%s\n", fd, errno, strerror(errno));
    close(fd);

    return 0;
}    
