//
// Created by xuweilin on 22-11-6.
//

#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <pthread.h>
#include <string.h>

/**
 * pthread_join 回收线程，线程返回值为结构体
 */

struct thrd {
    int var;
    char str[256];
};

void *tfn(void *argc) {

    struct thrd *tval;
    tval = malloc(sizeof(struct thrd));
    tval->var = 100;
    strcpy(tval->str, "hello thread");

    return (void *) tval;
}

int pthrd_join() {
    pthread_t tid;
    struct thrd *retval;

    int ret = pthread_create(&tid, NULL, tfn, NULL);
    if (ret != 0) {
        perror("pthread create error");
        exit(1);
    }

    ret = pthread_join(tid, (void **)&retval);
    if (ret != 0) {
        perror("pthread join error");
        exit(1);
    }

    printf("child thread exit with var = %d\n", retval->var);

    pthread_exit(NULL);
}