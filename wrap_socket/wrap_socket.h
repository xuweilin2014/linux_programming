//
// Created by xuweilin on 22-10-23.
//

#ifndef NETWORK_PROGRAMMING_WRAP_SOCKET_H
#define NETWORK_PROGRAMMING_WRAP_SOCKET_H

#include "sys/socket.h"

/**
 * const 数据类型 指针变量; 常量指针即这是个指向常量的指针
 *
 * 常量指针指向的对象不能通过这个指针来修改，可是仍然可以通过原来的声明修改；
 * 常量指针可以被赋值为变量的地址，之所以叫常量指针，是限制了通过这个指针修改变量的值；
 * 指针还可以指向别处，因为指针本身只是个变量，可以指向任意地址。
 *
 * @param s
 */
void perr_exit(const char *s);
int Accept(int fd, struct sockaddr *sa, socklen_t *salenptr);
int Bind(int fd, const struct sockaddr *sa,socklen_t salen);
int Connect(int fd, const struct sockaddr *sa, socklen_t salen);
int Listen(int fd, int backlog);
int Socket(int family, int type, int protocol);
ssize_t Read(int fd, void *ptr, size_t nbytes);
ssize_t Write(int fd, const void* ptr, size_t nbytes);
int Close(int fd);
ssize_t Readn(int fd, void *vptr, size_t n);
ssize_t Writen(int fd, const void *vptr, size_t n);
static ssize_t my_read(int fd, char *ptr);
ssize_t Readline(int fd, void *vptr, size_t maxlen);

#endif //NETWORK_PROGRAMMING_WRAP_SOCKET_H
