//
// Created by xuweilin on 22-11-6.
//

#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <pthread.h>
#include <string.h>

/**
 * pthread_join 回收线程，线程返回值为整形
 */

void *tfn2(void *argc) {

    return (void *) 74;
}

int pthrd_join2() {
    pthread_t tid;
    int val;

    int ret = pthread_create(&tid, NULL, tfn2, NULL);
    if (ret != 0) {
        perror("pthread create error");
        exit(1);
    }

    ret = pthread_join(tid, (void **)&val);
    if (ret != 0) {
        perror("pthread join error");
        exit(1);
    }

    printf("child thread exit with var = %d\n", val);

    pthread_exit(NULL);
}