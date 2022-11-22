//
// Created by xuweilin on 22-11-6.
//

#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <pthread.h>
#include <string.h>

/**
 * 函数 pthread_join(pthread_t __th, void **retval) 等待由 thread 标识的线程终止。
 *
 * 若 retval 为一非空指针，将会保存线程终止时返回值的拷贝，该返回值亦即线程调用 return 或 pthred_exit() 时所指定的值。
 *
 */

void *tfn2(void *argc) {

    return (void *) 74;
}

int pthrd_join2() {
    pthread_t tid;
    int* val;

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