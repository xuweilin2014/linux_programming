#include <unistd.h>
#include <fcntl.h>
#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define MSG_TRY "try again\n"

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
