//
// Created by xuweilin on 22-11-6.
//

#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <pthread.h>

/**
 * 循环创建多个线程
 */

void *callback(void *argc) {

    int i = (int) argc;
    sleep(i);
    printf("I'm %dth thread: pid = %d, tid = %lu\n",i + 1, getpid(), pthread_self());

    return NULL;
}

int pthread_loop() {

    int i;
    int ret;
    pthread_t tid;

    for (i = 0; i < 5; i++) {
        ret = pthread_create(&tid, NULL, callback, (void*) i);
        if (ret != 0) {
            perror("pthread_create error");
        }
    }

    sleep(i);
    printf("main: I'm Main, pid = %d, tid = %lu\n", getpid(), pthread_self());

    return 0;
}