//
// Created by xuweilin on 22-10-30.
//
#include <unistd.h>
#include <fcntl.h>
#include <stdlib.h>
#include <string.h>
#include <stdio.h>

/**
 * 在下面的程序中加入段错误成分，段错误在 unlink 之前，由于发生段错误，程序后续删除 temp.txt 的 dentry 部分就不会再执行，
 * temp.txt 就保留了下来，这是不科学的。
 *
 * 解决办法是检测 fd 有效性后，立即释放 temp.txt，由于进程未结束，虽然 temp.txt 的硬链接数已经为 0，但还不会立即释放，仍然存在，
 * 要等到程序执行完才会释放。这样就能避免程序出错导致临时文件保留下来。因为文件创建后，硬链接数立马减为0，即使程序异常退出，这个文件也
 * 会被清理掉。这时候的内容是写在内核空间的缓冲区。
 *
 * 隐式回收：当进程结束运行时，所有进程打开的文件会被关闭，申请的内存空间会被释放。系统的这一特性称之为隐式回收系统资源。

 */

int unlink_test(void) {
    int fd;
    char *p = "test of unlink.\n";
    char *p2 = "after write something\n";
    int ret = 0;

    fd = open("../file_io/temp.txt", O_RDWR | O_CREAT | O_TRUNC, 0644);
    if (fd < 0) {
        perror("open temp error");
        exit(1);
    }

    ret = write(fd, p, strlen(p));
    if (ret == -1) {
        perror("-----write error");
    }

    printf("hi! I'm printf\n");
    printf("Enter anykey continue\n");
    getchar();

    ret = unlink("../file_io/temp.txt");
    if (ret < 0) {
        perror("unlink error");
        exit(1);
    }

    ret = write(fd, p2, strlen(p2));
    if (ret == -1) {
        perror("-----write error");
    }

    close(fd);
    return 0;
}