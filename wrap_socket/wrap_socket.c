//
// Created by xuweilin on 22-10-23.
//

#include "wrap_socket.h"
#include "stdlib.h"
#include "stdio.h"
#include "unistd.h"
#include "errno.h"
#include "sys/socket.h"

/**
 * 出错处理封装函数
 */

void perr_exit(const char *s) {
    perror(s);
    exit(-1);
}

int Accept(int fd, struct sockaddr *sa, socklen_t *salenptr) {
    int n;
    while ((n = accept(fd, sa, salenptr)) < 0) {
        if ((errno == ECONNABORTED) || (errno == EINTR)) {
            continue;
        } else {
            perr_exit("accept error");
        }
    }

    return n;
}

int Bind(int fd, const struct sockaddr *sa,socklen_t salen) {
    int n;

    if ((n = bind(fd, sa, salen)) < 0) {
        perr_exit("bind error");
    }

    return n;
}

int Connect(int fd, const struct sockaddr *sa, socklen_t salen) {
    int n;
    n = connect(fd, sa, salen);
    if (n < 0) {
        perr_exit("connect error");
    }

    return 0;
}

int Listen(int fd, int backlog) {
    int n;
    if ((n = listen(fd, backlog)) < 0) {
        perr_exit("listen error");
    }
    return n;
}

int Socket(int family, int type, int protocol) {
    int n;
    if ((n = socket(family, type, protocol)) < 0) {
        perr_exit("socket error");
    }

    return n;
}

ssize_t Read(int fd, void *ptr, size_t nbytes) {
    ssize_t n;

    while ((n = read(fd, ptr, nbytes)) == -1) {
        if (errno == EINTR) {
            continue;
        } else {
            return -1;
        }
    }

    return n;
}

ssize_t Write(int fd, const void *ptr, size_t nbytes) {
    ssize_t n;

    while ((n = write(fd, ptr, nbytes)) == -1) {
        if (errno == EINTR) {
            continue;
        } else {
            return -1;
        }
    }

    return n;
}

int Close(int fd) {
    int n;
    if ((n = close(fd)) == -1) {
        perr_exit("close error");
    }

    return n;
}

// 从 fd 文件描述符代表的缓冲区中读取 n 个字节到 vptr 代表的 buf 中
ssize_t Readn(int fd, void *vptr, size_t n) {
    size_t nread = 0;   // int 实际读取到的字节数
    size_t nleft = n;   // unsigned int 剩余未读取的字节数
    char *ptr = vptr;

    while (nleft > 0) {
        if ((nread = read(fd, ptr, n)) < 0) {
            if (errno == EINTR)
                nread = 0;
            else
                return -1;
        } else if (nread == 0){
            break;
        }

        nleft -= nread;
        ptr += nread;
    }

    // 返回读取到的字节数
    return n - nleft;
}

// 把 vptr 代表的 buf 中的 n 个字节写入到 fd 代表的写缓冲区中
ssize_t Writen(int fd, const void *vptr, size_t n) {
    size_t nleft = n;
    size_t nwritten = 0;
    // ptr 代表 buf 不应该被更改（buf 中已经装了数据）
    const char *ptr = vptr;

    while (nleft > 0) {
        if((nwritten = write(fd, ptr, nleft)) < 0) {
            if (errno == EINTR) {
                nwritten = 0;
            } else {
                return -1;
            }
        } else if (nwritten == 0) {
            break;
        }

        nleft -= nwritten;
        ptr += nwritten;
    }

    // 返回实际写入的字节数
    return n - nleft;
}

static ssize_t my_read(int fd, char *ptr) {

    static int read_cnt = 0;
    static char *read_ptr;
    static char read_buf[100];

    // 从 fd 读取数据一部分数据之后，把读取到的字节数赋值给 read_cnt，之后每次调用 my_read 方法之后，
    // 下面的 while 循环不会执行，而是直接 read_cnt--，把 read_ptr 指向的字符赋值给 *ptr

    while (read_cnt <= 0) {
        if ((read_cnt = read(fd, read_buf, sizeof(read_buf))) < 0) {
            if (errno == EINTR)
                continue;
            else
                return -1;
        } else if (read_cnt == 0) {
            return 0;
        }

        read_ptr = read_buf;
    }

    read_cnt--;
    // *read_ptr++ 先对 read_ptr 进行间接引用，然后再对 read_ptr 指针进行自增
    *ptr = *read_ptr++;

    return 1;

}

/* readline -- fgets */
// 传出参数 vptr
ssize_t Readline(int fd, void *vptr, size_t maxlen) {

    ssize_t n, rc;
    char c, *ptr = vptr;

    for (n = 1; n < maxlen; n++) {
        if ((rc = my_read(fd, &c)) == 1) {
            *ptr++ = c;
            if (c == '\n') {
                break;
            }
        } else if(rc == 0) {
            *ptr = 0;
            return n - 1;
        } else {
            return -1;
        }
    }

    *ptr = 0;
    return n;
}


