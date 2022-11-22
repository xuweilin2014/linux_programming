#include <unistd.h>
#include <fcntl.h>
#include <stdlib.h>
#include <stdio.h>
#include <errno.h>
#include <string.h>

#define MSG_TRY "try again\n"
#define MSG_TIMEOUT "time out\n"

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
 * 下面的程序和 nonblock_readtty.c 类似，都是从终端非阻塞读取字符，如果没有读取到字符，那么就打印 try again，
 * 然后等待 2s，如果读取到字符，就将其写出到标准输出。但是下面的程序只会尝试 5 次，如果还没有读取到任何字符，那么就
 * 打印出 time out 信息
 */

int nonblock_timeout() {
    char buf[10];
    int fd, n, i;

    fd = open("/dev/tty", O_RDONLY | O_NONBLOCK);
    if (fd < 0) {
	    perror("open /dev/tty");
        exit(1);
    }

    printf("open /dev/tty ok... %d\n", fd);
    
    for(i = 0; i < 5; i++) {
	    n = read(fd, buf, 10);
        if(n > 0) {
            break;
	    }
        if (errno != EAGAIN) {
	        perror("read /dev/tty");
            exit(1);
        } else {
            write(STDOUT_FILENO, MSG_TRY, strlen(MSG_TRY));
            sleep(2);
	    }
    }	
    
    if(i == 5) {
	    write(STDOUT_FILENO, MSG_TIMEOUT, strlen(MSG_TIMEOUT));
    } else {
        write(STDOUT_FILENO, buf, n);
    }

    close(fd);
    return 0;
}    
