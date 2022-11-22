//
// Created by xuweilin on 22-11-20.
//

#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <fcntl.h>
#include <event2/event.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <event.h>

#define FIFO_ADDR "my_fifo"

zengj/**
 * libevent 库的安装和使用：
 *
 * ./configure       检查安装环境 生成 makefile 文件
 * make              生成 .o 和可执行文件
 * sudo make install 将必要的资源拷贝到系统指定目录
 *
 */

// 写操作回调函数
void read_cb(evutil_socket_t fd, short what, void *argc) {

    // 读管道
    char buf[1024] = {0};

    int len = read(fd, buf, sizeof(buf));

    printf("read event: %s \n", what & EV_READ ? "Yes" : "No");
    printf("data len = %d, buf = %s\n", len, buf);

}

int main() {

    unlink(FIFO_ADDR);
    // 创建有名管道
    mkfifo(FIFO_ADDR, 0664);

    int fd = open(FIFO_ADDR, O_RDONLY);
    if (fd == -1) {
        perror("open error");
        exit(1);
    }

    // 创建一个 event_base
    struct event_base *base = NULL;
    // 创建事件
    struct event* ev;

    base = event_base_new();
    // 监听读事件，同时事件一直持续
    ev = event_new(base, fd, EV_READ | EV_PERSIST, read_cb, base);

    // 添加事件
    event_add(ev, NULL);
    // 开启事件循环
    event_base_dispatch(base);

    // 释放资源
    event_free(ev);
    event_base_free(base);

    close(fd);

    return 0;
}