//
// Created by xuweilin on 22-11-7.
//

#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <pthread.h>
#include <string.h>

/**
 * LINUX 互斥锁 mutex 使用示例
 */

pthread_mutex_t mutex;

void *tfn5(void *argc) {
    srand(time(NULL));

    while (1) {

        pthread_mutex_lock(&mutex);
        printf("hello ");
        // 模拟长时间操作共享资源，导致 cpu 易主，产生与时间有关的错误
        sleep(rand() % 3);
        printf("world\n");
        pthread_mutex_unlock(&mutex);

        sleep(rand() % 3);
    }

    return NULL;
}

int pthrd_shared() {

    pthread_t tid;
    srand(time(NULL));
    // 初始化互斥锁，可以认为🔓的值为 1
    int ret = pthread_mutex_init(&mutex, NULL);
    if (ret != 0) {
        fprintf(stderr, "mutex init error: %s\n", strerror(ret));
        exit(1);
    }

    pthread_create(&tid, NULL, tfn5, NULL);
    while (1) {
        // 加锁可以认为锁的值减一
        pthread_mutex_lock(&mutex);
        printf("HELLO ");
        sleep(rand() % 3);
        printf("WORLD\n");
        // 解锁可以认为锁的值加一
        pthread_mutex_unlock(&mutex);

        sleep(rand() % 3);

    }

    ret = pthread_mutex_destroy(&mutex);
    if (ret != 0) {
        fprintf(stderr, "mutex init error: %s\n", strerror(ret));
        exit(1);
    }

    pthread_join(tid, NULL);

    return 0;
}