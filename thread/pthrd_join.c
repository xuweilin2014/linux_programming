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

    // 使用 pthread_join 回收 tid 指定的线程，另外 retval 为 tfn 函数 return 指定的值
    // 即 retval = tval，即 retval 被赋值为 struct thrd 结构体的指针
    ret = pthread_join(tid, (void **)&retval);
    if (ret != 0) {
        perror("pthread join error");
        exit(1);
    }

    printf("child thread exit with var = %d\n", retval->var);

    pthread_exit(NULL);
}