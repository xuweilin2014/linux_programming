#include <unistd.h>
#include <fcntl.h>
#include <stdio.h>
#include <errno.h>
#include <string.h>

/**
 * open() 调用既能打开一个业已存在的文件，也能创建并打开一个新文件
 */

int open_test() {
    int fd;
    fd = open("./dict.c", O_RDONLY | O_TRUNC);
    printf("fd = %d, errno=%d:%s\n", fd, errno, strerror(errno));
    close(fd);

    return 0;
}    
