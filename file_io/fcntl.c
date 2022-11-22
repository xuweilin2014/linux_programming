#include <unistd.h>
#include <fcntl.h>
#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define MSG_TRY "try again\n"

/**
 * fcntl 用来改变一个【已经打开】的文件的 访问控制属性
 *
 * 获取文件状态： F_GETFL
 * 设置文件状态： F_SETFL
 *
 * 下面的程序使用 fcntl 设置 STDIN_FILENO 文件描述符的访问属性，将其设置为 O_NONBLOCK，实现的效果类似于
 * nonblock_readtty.c，如果不能从输入中获取到字符，那么就不停打印 try again，而不会阻塞
 */

int fcntl_test(void) {
    char buf[10];
    int flags, n;

    // 获取 stdin 属性信息
    flags = fcntl(STDIN_FILENO, F_GETFL);
    if (flags == -1) {
        perror("fcntl error");
        exit(1);
    }

    flags |= O_NONBLOCK;
    int ret = fcntl(STDIN_FILENO, F_SETFL, flags);
    if (ret == -1) {
       perror("fcntl error");
       exit(1);
    }

    while((n = read(STDIN_FILENO, buf, 10)) < 0) {
	    if (errno != EAGAIN) {
	        perror("read /dev/tty");
            exit(1);
	    }
        sleep(3);
        write(STDOUT_FILENO, MSG_TRY, strlen(MSG_TRY));
    }

    write(STDOUT_FILENO, buf, n);

    return 0;
}    
