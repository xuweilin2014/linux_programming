#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <fcntl.h>

/**
 *
 * 总结 read 函数返回值：
 *
 * 1. 返回非零值： 实际 read 到的字节数
 * 2. 返回 -1：
 *      1）：errno != EAGAIN (或!= EWOULDBLOCK) read 出错
 *      2）：errno == EAGAIN (或== EWOULDBLOCK) 设置了非阻塞读，并且没有数据到达
 * 3. 返回 0：读到文件末尾
 *
 * 阻塞与非阻塞是对于文件而言的。而不是 read、write 等的属性。read 终端，默认阻塞读
 *
 * 下面的程序使用从终端非阻塞读取字符，如果没有读取到字符，那么就打印 try again，然后等待 2s，如果读取到字符，就将其
 * 写出到标准输出
 */

int nonblock_readtty() {
    char buf[10];
    int fd, n;

    fd = open("/dev/tty", O_RDONLY | O_NONBLOCK);
    if (fd < 0) {
	    perror("open /dev/tty");
        exit(1);
    }

    while ((n = read(fd, buf, 10)) < 0) {
	    if (errno != EAGAIN) {
	        perror("read /dev/tty");
            exit(1);
	    } else {
            write(STDOUT_FILENO, "try again\n", strlen("try again\n"));
            sleep(2);
        }
    }

    write(STDOUT_FILENO, buf, n);
    close(fd);

    return 0;
}    
