//
// Created by xuweilin on 22-11-6.
//
#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <pthread.h>

/**
 * 线程创建示例
 */

void *callback2(void *argc) {
    printf("main: pid = %d, tid = %lu\n", getpid(), pthread_self());

    return NULL;
}

int pthrd_creat() {

    pthread_t tid;

    int ret = pthread_create(&tid, NULL, callback2, NULL);
    if (ret != 0) {
        perror("pthread_create error");
    }

    printf("main: pid = %d, tid = %lu\n", getpid(), pthread_self());
    getchar();

    return 0;

}