//
// Created by xuweilin on 22-11-15.
//

#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <pthread.h>
#include <arpa/inet.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <string.h>
#include <ctype.h>
#include <fcntl.h>
#include <signal.h>
#include <errno.h>
#include <sys/epoll.h>
#include "wrap_socket.h"

#define MAX_EVENTS 1024
#define BUFLEN 4096
#define SERV_PORT 8080

void recvdata(int fd, int events, void *arg);
void senddata(int fd, int events, void *arg);

/**
 * 使用 reactor 模型 + epoll 函数来实现高性能服务器，epoll 的本质和观察者模型类似，服务器在 epoll 注册事件以及和事件对应
 * 的处理器函数，当事件被触发时，就回调处理器函数。比如注册的 lfd 的 EPOLLIN 对应的 accept_conn 函数，当有用户连接时，就
 * 会触发，调用 accept 函数，建立连接。然后把 cfd 也注册到 epoll 上，并且有 EPOLLIN 事件和对应的 recvdata 函数，然后
 * 接收到数据时，回调 recvdata，将 EPOLLIN->recvdata 替换成 EPOLLOUT->senddata，也就是待写时机成熟时，把接收到的数据
 * 返回给客户端。
 *
 * 下面实现了 echo server，就是将客户端发送过来的消息原封不动的转发回去
 */

struct myevent_s {
    // 要监听的文件描述符
    int fd;
    // 要监听的事件
    int events;
    // 泛型参数
    void *arg;
    // 回调函数
    void (*callback)(int fd, int events, void *arg);
    // 是否在监听:-1 -> 在红黑树上监听，0 -> 不在（红黑树）
    int status;
    char buf[BUFLEN];
    int len;
    // 记录每次加入红黑树 g_efd 的时间值
    long last_active;
};

// 全局变量，保存 epoll_create 返回的文件描述符
int g_efd;
// 自定义的结构体类型数组 +1 --> listenfd
struct myevent_s g_events[MAX_EVENTS + 1];

/* 初始化 myevent_s 结构体变量 */

void eventset(struct myevent_s *ev, int fd, void (*call_back)(int, int, void *), void *arg) {
    ev->fd = fd;
    ev->events = 0;
    ev->status = 0;
    ev->last_active = time(NULL);
    ev->callback = call_back;
    ev->arg = arg;
    ev->len = ((struct myevent_s *) arg)->len;
}

/* 向 epoll 监听的红黑树添加一个文件描述符 */

void eventadd(int efd, int events, struct myevent_s *ev) {

    int op;
    struct epoll_event epv = {0, {0}};
    epv.data.ptr = ev;

    // EPOLLIN 或者 EPOLLOUT
    epv.events = ev->events = events;

    // 当前文件描述符不在红黑树上
    if (ev->status == 0) {
        // 将其加入到红黑树中，并将 status 置为 1
        op = EPOLL_CTL_ADD;
        ev->status = 1;
    }

    if ((epoll_ctl(efd, op, ev->fd, &epv)) < 0) {
        printf("event add failed [fd=%d], events[%d]\n", ev->fd, events);
    } else {
        printf("event add ok [fd=%d], events[%d]\n", ev->fd, events);
    }
}

/* 从 epoll 监听的红黑树中删除一个文件描述符 */

void eventdel(int efd, struct myevent_s *ev) {

    struct epoll_event epv = {0, {0}};

    if (ev->status == 0)
        return;

    epv.data.ptr = NULL;
    ev->status = 0;
    epoll_ctl(efd, EPOLL_CTL_DEL, ev->fd, &epv);

}

void accept_conn(int lfd, int events, void *arg) {

    struct sockaddr_in cin;
    socklen_t cli_len = sizeof(cin);
    int cfd, i;

    if ((cfd = accept(lfd, (struct sockaddr *) &cin, &cli_len)) < 0) {
        if (errno != EAGAIN && errno != EWOULDBLOCK){
            /* 暂时不做出错处理 */
        }
        printf("%s: accept, %s\n", __func__ , strerror(errno));
        return;
    }

    /**
     * do while(0) 是用来模拟 goto，因为 while(0) 是不会循环的，但是可以使用 break，当触发某一个条件时，调用 break，
     * 则可以退出 while 循环，后面的语句就不会执行
     */
    do {
        // 从全局数组 g_events 中找到一个空闲的元素，也就是不在红黑树上的元素
        for (i = 0; i < MAX_EVENTS; ++i) {
            if (g_events[i].status == 0){
                break;
            }
        }

        if (i == MAX_EVENTS) {
            printf("");
            break;
        }

        // 将 cfd 设置为非阻塞
        if (fcntl(cfd, F_SETFL, O_NONBLOCK) < 0) {
            printf("%s: fcntl failed, %s", __func__ , strerror(errno));
            break;
        }

        //
        eventset(&g_events[i], cfd, recvdata, &g_events[i]);
        eventadd(g_efd, EPOLLIN, &g_events[i]);
    } while (0);

    printf("new connect [%s:%d][time:%ld], pos[%d]\n", 
           inet_ntoa(cin.sin_addr), ntohs(cin.sin_port), g_events[i].last_active, i);
    
}

void recvdata(int fd, int events, void *arg) {
    
    struct myevent_s *ev = (struct myevent_s *) arg;
    ssize_t len;
    
    len = recv(fd, ev->buf, sizeof(ev->buf), 0);

    // 读取到数据之后，写出不能直接写，而要 epoll_wait 判断写出的时机满足之后（比如 kernel buffer 有空），才能写返回数据
    eventdel(g_efd, ev);
    
    if (len > 0) {
        ev->len = len;
        ev->buf[len] = '\0';

        printf("[%d]:%s\n", fd, ev->buf);
        // 设置 fd 对应的回调函数为 senddata，当可以写出数据时，回调 senddata
        eventset(ev, fd, senddata, ev);
        eventadd(g_efd, EPOLLOUT, ev);
    } else if (len == 0) {
        close(fd);
        printf("[fd=%d] pos[%ld] closed\n", fd, ev - g_events);
    } else {
        close(fd);
        printf("recv[fd=%d] error[%d]:%s\n", fd, errno, strerror(errno));
    }
    
}

void senddata(int fd, int events, void *arg)
{
    struct myevent_s *ev = (struct myevent_s *)arg;
    int len;

    // 直接将数据 回写给客户端。未作处理
    len = send(fd, ev->buf, ev->len, 0);
    // 从红黑树 g_efd 中移除
    eventdel(g_efd, ev);

    if (len > 0) {

        printf("send[fd=%d], [%d]%s\n", fd, len, ev->buf);
        // 将该 fd 的 回调函数改为 recvdata
        eventset(ev, fd, recvdata, ev);
        // 从新添加到红黑树上， 设为监听读事件
        eventadd(g_efd, EPOLLIN, ev);

    } else {
        // 关闭链接
        close(ev->fd);
        printf("send[fd=%d] error %s\n", fd, strerror(errno));
    }

    return ;
}

/* 创建 socket, 初始化 lfd */

void init_listen_socket(int efd, short port){

    struct sockaddr_in sin;

    int lfd = socket(AF_INET, SOCK_STREAM, 0);
    // 将 socket 设为非阻塞
    fcntl(lfd, F_SETFL, O_NONBLOCK);

    memset(&sin, 0, sizeof(sin));
    sin.sin_family = AF_INET;
    sin.sin_addr.s_addr = INADDR_ANY;
    sin.sin_port = htons(port);

    bind(lfd, (struct sockaddr *)&sin, sizeof(sin));
    listen(lfd, 20);

    int opt = 1;
    setsockopt(lfd, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt));

    eventset(&g_events[MAX_EVENTS], lfd, accept_conn, &g_events[MAX_EVENTS]);

    /* void eventadd(int efd, int events, struct myevent_s *ev) */
    eventadd(efd, EPOLLIN, &g_events[MAX_EVENTS]);

}

int main() {

    unsigned short port = SERV_PORT;
    // 创建红黑树,返回给全局 g_efd
    g_efd = epoll_create(MAX_EVENTS + 1);
    
    if (g_efd <= 0)
        printf("create efd in %s err %s\n", __func__, strerror(errno));

    // 初始化监听 socket，并且将监听文件描述符添加到红黑树上，并且注册 accept_conn 回调函数
    init_listen_socket(g_efd, port);
    // 保存已经满足就绪事件的文件描述符数组
    struct epoll_event events[MAX_EVENTS+1];
    printf("server running:port[%d]\n", port);

    int checkpos = 0, i;

    while (1) {
        /* 超时验证，每次测试 100 个链接，不测试 listenfd 当客户端 60 秒内没有和服务器通信，则关闭此客户端接 */
        // 当前时间
        long now = time(NULL);

        // 一次循环检测 100 个。 使用 checkpos 控制检测对象
        for (i = 0; i < 100; i++, checkpos++) {
            if (checkpos == MAX_EVENTS)
                checkpos = 0;

            // 不在红黑树 g_efd 上
            if (g_events[checkpos].status != 1)
                continue;

            // 客户端不活跃的世间
            long duration = now - g_events[checkpos].last_active;

            if (duration >= 60) {
                // 关闭与该客户端链接
                close(g_events[checkpos].fd);
                printf("[fd=%d] timeout\n", g_events[checkpos].fd);
                // 将该客户端 从红黑树 g_efd 移除
                eventdel(g_efd, &g_events[checkpos]);
            }
        }

        /* 监听红黑树 g_efd, 将满足的事件的文件描述符加至 events 数组中, 1 秒没有事件满足, 返回 0 */
        int nfd = epoll_wait(g_efd, events, MAX_EVENTS + 1, 1000);
        if (nfd < 0) {
            printf("epoll_wait error, exit\n");
            break;
        }

        for (i = 0; i < nfd; i++) {
            /* 使用自定义结构体 myevent_s 类型指针, 接收 联合体 data 的 void *ptr 成员 */
            struct myevent_s *ev = (struct myevent_s *)events[i].data.ptr;

            if ((events[i].events & EPOLLIN) && (ev->events & EPOLLIN)) {           //读就绪事件
                ev->callback(ev->fd, events[i].events, ev->arg);
            }

            if ((events[i].events & EPOLLOUT) && (ev->events & EPOLLOUT)) {         //写就绪事件
                ev->callback(ev->fd, events[i].events, ev->arg);
            }
        }
    }

    return 0;
}