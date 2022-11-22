//
// Created by xuweilin on 22-11-21.
//

#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <fcntl.h>
#include <event2/bufferevent.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <event2/event.h>
#include <event2/listener.h>
#include <string.h>
#include <arpa/inet.h>

#define SERV_PORT 9876

/**
 * 使用 libevent 编写客户端代码的基本步骤：
 *
 * 1.创建 event_base
 * 2.使用 bufferevent_socket_new() 创建一个用跟服务器通信的 bufferevent 事件对象
 * 3.使用 bufferevent_socket_connect 连接服务器
 * 4.使用 bufferevent_setcb 给 bufferevent 对象的 read、write、event 设置回调
 * 5.设置 bufferevent 对象的读写缓冲区 enable / disable
 * 6.接受、发送数据 bufferevent_read/bufferevent_write
 * 7.释放资源
 */

// 接受服务器发送过来的数据
void read_callback(struct bufferevent *bev, void  *arg) {

    char buf[1024] = {0};
    bufferevent_read(bev, buf, sizeof(buf));
    printf("server say:%s\n", buf);
    sleep(1);

}

void write_callback(struct bufferevent *bev, void *arg) {

    printf("----------我是客户端的写回调函数\n");

}

void event_callback(struct bufferevent *bev, short events, void *arg) {

    if (events & BEV_EVENT_EOF) {
        printf("connection closed\n");
    } else if(events & BEV_EVENT_ERROR) {
        printf("some other error\n");
    } else if(events & BEV_EVENT_CONNECTED) {
        printf("已经连接服务器...\\(^o^)/...\n");
        return;
    }

    // 释放资源
    bufferevent_free(bev);

}

// 客户端与用户交互，从终端读取数据写给服务器
void read_terminal(evutil_socket_t fd, short what, void *arg) {

    // 读数据
    char buf[1024] = {0};
    int len = read(fd, buf, sizeof(buf));

    struct bufferevent* bev = (struct bufferevent*)arg;
    // bev 包含了服务器端的 fd，向服务器发送数据
    bufferevent_write(bev, buf, len+1);

}

int main() {

    struct event_base *base = event_base_new();
    int fd = socket(AF_INET, SOCK_STREAM, 0);

    struct bufferevent *bev = bufferevent_socket_new(base, fd, BEV_OPT_CLOSE_ON_FREE);

    struct sockaddr_in serv_addr;
    serv_addr.sin_family = AF_INET;
    serv_addr.sin_port = htons(SERV_PORT);
    inet_pton(AF_INET, "127.0.0.1", &serv_addr.sin_addr.s_addr);

    // 连接服务器
    bufferevent_socket_connect(bev, (struct sockaddr *) &serv_addr, sizeof(serv_addr));
    bufferevent_setcb(bev, read_callback, write_callback, event_callback, NULL);
    // 设置读缓冲区生效
    bufferevent_enable(bev, EV_READ);

    // 创建事件
    struct event *ev = event_new(base, STDIN_FILENO, EV_READ | EV_PERSIST, read_terminal, bev);

    // 添加事件
    event_add(ev, NULL);
    event_base_dispatch(base);

    event_free(ev);
    event_base_free(base);

    return 0;
}