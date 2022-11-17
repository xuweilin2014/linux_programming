//
// Created by xuweilin on 22-11-7.
//

#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <pthread.h>

/**
 * LINUX 读写锁示例程序
 */

int counter = 0;
pthread_rwlock_t rwlock;

void *th_write(void *argc) {
    int t;
    int i = (int) argc;

    while (1) {
        pthread_rwlock_wrlock(&rwlock);
        t = counter;
        usleep(1000);
        printf("========write %d: %lu: counter=%d ++counter=%d\n", i, pthread_self(), t, ++counter);
        pthread_rwlock_unlock(&rwlock);
        usleep(1000);
    }

    return NULL;
}

void *th_read(void *argc) {
    int i = (int) argc;

    while (1) {
        pthread_rwlock_rdlock(&rwlock);
        printf("========read %d: %lu: %d\n", i, pthread_self(), counter);
        pthread_rwlock_unlock(&rwlock);
        usleep(2000);
    }

    return NULL;
}

int rwlock_test() {

    int i;
    pthread_t tid[8];
    pthread_rwlock_init(&rwlock, NULL);

    // 创建三个写线程
    for (i = 0; i < 3; i++) {
        pthread_create(&tid[i], NULL, th_write, (void *)i);
    }

    // 创建五个读线程
    for (i = 0; i < 5; i++) {
        pthread_create(&tid[i], NULL, th_read, (void *) i);
    }

    for (i = 0; i < 8; i++) {
        pthread_join(tid[i], NULL);
    }

    pthread_rwlock_destroy(&rwlock);

    return 0;
}