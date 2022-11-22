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

#define SERV_PORT 9876
#define BACKLOG 128

/**
 * 使用 libevent 编写服务器端代码的基本步骤：
 *
 * 1. 创建 event_base
 * 2. 创建服务器连接监听器 evconnlistener_new_bind()
 * 3. 在 evconnlistener_new_bind 的回调函数中，处理接受连接后的操作
 * 4. 回调函数被调用，说明有一个新客户端连接上来。会得到一个新的 fd，用于和客户端通信（读，写）
 * 5. 使用 bufferevent_socket_new 创建一个新的 bufferevent 事件，将 fd 封装到这个事件对象中
 * 6. 使用 bufferevent_setcb 给这个事件对象的 read、write、event 设置回调
 * 7. 设置 bufferevent 的读写缓冲区 enable / disable
 * 8. 接受、发送数据 bufferevent_read() / bufferevent_write()
 * 9. 启动循环监听
 * 10.释放资源
 */

// 事件
void event_callback(struct bufferevent *bev, short what, void *ctx) {
    if (what & BEV_EVENT_EOF) {
        printf("connection closed\n");
    } else if (what & BEV_EVENT_ERROR) {
        printf("connection error\n");
    }

    bufferevent_free(bev);
    printf("bufferevent 资源已经被释放...\n");
}

void write_callback(struct bufferevent *bev, void *ctx) {
    printf("I'm  服务器，成功写数据给客户端，写缓冲区回调函数被回调\n");
}

// 当客户端发送过来数据时，就会触发 read_callback 回调，将一段固定的信息返回
void read_callback(struct bufferevent *bev, void *ctx) {
    char buf[1024] = {0};

    bufferevent_read(bev, buf, sizeof(buf));
    printf("client say: %s", buf);

    char *p = "我是服务器，已经成功收到你发送过来的消息";
    bufferevent_write(bev, p, strlen(p) + 1);

    sleep(1);
}

// 当有客户端新连接建立时，listener_callback 会被回调
void listener_callback(struct evconnlistener *listener, evutil_socket_t fd,
        struct sockaddr *cli_addr, int socklen, void *arg) {

    struct event_base *base = (struct event_base *) arg;
    printf("new connection\n");

    struct bufferevent *bev = bufferevent_socket_new(base, fd, BEV_OPT_CLOSE_ON_FREE);

    // 在客户端连接 fd 上设置 read_callback 和 write_callback 回调，分别处理 socket 上的读写事件
    bufferevent_setcb(bev, read_callback, write_callback, event_callback, NULL);
    // enable 读缓冲区
    bufferevent_enable(bev, EV_READ);
}

int main() {

    struct sockaddr_in serv_addr;

    memset(&serv_addr, 0, sizeof(serv_addr));
    serv_addr.sin_family = AF_INET;
    serv_addr.sin_port = htons(SERV_PORT);
    serv_addr.sin_addr.s_addr = htonl(INADDR_ANY);

    struct event_base *base = event_base_new();
    struct evconnlistener *listener;

    // evconnlistener_new_bind：socket -> bind -> listen -> accept
    // evconnlistener_new_bind 主要的作用是设置监听，以及监听回调，当对方和服务器建立好连接时，就会触发 listener_callback 方法，
    // 在客户端连接 fd 上创建 bufferevent，并且注册 read_callback 和 write_callback 处理函数，处理连接上的读写事件
    listener = evconnlistener_new_bind(base, listener_callback, base, LEV_OPT_CLOSE_ON_FREE | LEV_OPT_REUSEABLE,
                                       BACKLOG,(struct sockaddr *) &serv_addr,sizeof(serv_addr));

    event_base_dispatch(base);

    event_base_free(base);
    evconnlistener_free(listener);

    return 0;
}