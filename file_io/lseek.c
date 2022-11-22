//
// Created by xuweilin on 22-10-29.
//
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <fcntl.h>

/**
 * 改变文件偏移量：lseek()
 *
 * 对于每个打开的文件，系统内核会记录其文件偏移量，有时也将文件偏移量称为读写偏移量或指针。文件偏移量是指执行下一个 read()
 * 或 write()操作的文件起始位置，会以相对于文件头部起始点的文件当前位置来表示。文件第一个字节的偏移量为 0
 *
 * off_t lseek(int fd, off_t offset, int whence);
 *
 * whence 参数则表明应参照哪个基点来解释 offset 参数，应为下列其中之一：
 *
 * SEEK_SET：将文件偏移量设置为从文件头部起始点开始的 offset 个字节
 * SEEK_CUR：相对于当前文件偏移量，将文件偏移量调整 offset 个字节
 * SEEK_END：将文件偏移量设置为起始于文件尾部的 offset 个字节，也就是说，offset 参数应该从文件最后一个字节之后的下一个字节算起
 *
 * 下面的程序，写一个句子到空白文件，完事使用 lseek 函数调整光标位置，读取刚才写那个文件。如果不使用 lseek 调整位置的话，
 * 那么 read 函数读取到的内容为空，因为读写文件共用一个偏移位置
 */

int lseek_test() {
    int fd, n;
    char msg[] = "It's a test for lseek";
    char ch;

    fd = open("../file_io/lseek.txt", O_RDWR | O_CREAT, 0664);
    if (fd < 0) {
        perror("open lseek.txt error");
        exit(1);
    }

    // 使用 fd 对打开的文件进行写操作，之后读位置位于文件结尾处
    // 读写文件共用一个偏移位置
    write(fd, msg, strlen(msg));

    // 修改文件读写指针位置，位于文件头
    lseek(fd, 0, SEEK_SET);

    while ((n = read(fd, &ch, 1))) {
        if (n < 0) {
            perror("read error");
            exit(1);
        }
        write(STDOUT_FILENO, &ch, n);
    }

    close(fd);

    return 0;
}

// 使用 lseek 获取文件大小
int lseek_file_size() {
    int fd = open("../file_io/lseek.txt", O_RDWR);
    if (fd == -1) {
        perror("open error");
        exit(1);
    }

    int length = lseek(fd, 0, SEEK_END);
    printf("file size:%d\n", length);

    return 0;
}