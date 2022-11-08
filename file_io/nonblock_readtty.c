#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <fcntl.h>

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
