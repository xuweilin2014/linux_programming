//
// Created by xuweilin on 22-10-30.
//
#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <sys/stat.h>
#include <fcntl.h>

/*
 * int dup(int fd);
 * int dup2(int fd, int fd 2);
 *
 * 这两个函数主要进行函数重定向，比如 dup 返回 dup_fd 被重定向到 fd 这个文件描述符指向的文件，当往 dup_fd 中写入内容时，最终会写入到 fd 中
 * dup2 则把 fd2 重定向到 fd，如果往 fd2 中写入数据，最终会写入到 fd 中。比如把 STDOUT_FILENO 重定向到某一个文件的 fd 中，那么通过 printf
 * 方法本来应该打印到屏幕上的字符串，就会被写入到文件中。
 *
 * 关于 dup 函数，当我们调用它的时候，dup 会返回一个新的描述符，这个描述一定是当前可用文件描述符中的最小值。我们知道，一般的0，1，2描述符分别被标准输入、
 * 输出、错误占用，所以在程序中如果 close 掉标准输出1后，调用 dup 函数，此时返回的描述符就是 1。对于 dup2，可以用 fd2 指定新描述符的值，如果 fd2
 * 本身已经打开了，则会先将其关闭。如果 fd 等于 fd2，则返回 fd2，并不关闭它。这两个函数返回的描述符与 fd 描述符所指向的文件共享同一文件表项。
 */

int dup_test() {

    // mode & ~umask = 666 & ~002 = 666 & 775 = 664 (rw-rw-r--)
    int fd = open("../file_io/lseek.txt", O_CREAT | O_APPEND | O_RDWR, 0666);
    if (fd < 0) {
        printf("open error\n");
        return 0;
    }

    if (write(fd, "hello world\n", 12) != 12) {
        printf("write fd error\n");
    }

    int dup_fd = dup2(fd, STDOUT_FILENO);
    if (dup_fd != STDOUT_FILENO) {
        printf("error dup2\n");
        return 0;
    }
    close(fd);

    char buf[1024];
    int n = 0;
    while ((n = read(STDIN_FILENO, buf, 1024)) != 0) {
        if (n < 0) {
            printf("read error\n");
            return 0;
        }

        printf("%s", buf);
        fflush(stdout);
        sleep(1);
    }

    close(dup_fd);
    return 0;
}