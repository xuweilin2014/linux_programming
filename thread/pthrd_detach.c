//
// Created by xuweilin on 22-11-6.
//

#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <pthread.h>
#include <string.h>

/**
 * 线程分离示例，使用 int pthread_detach(pthread_t pid); 设置线程分离，其中 pid 表示线程号，
 * 线程分离后，其资源在结束后会自动进行回收。
 *
 * 下面由于子线程已经被回收，父线程调用 pthread_join 方法获取到子线程时可能会获取到无效值
 */

void *call4(void *argc) {
    printf("thread: pid = %d, tid = %lu\n", getpid(), pthread_self());

    return NULL;
}

int pthrd_detach() {

    pthread_t tid;

    int ret = pthread_create(&tid, NULL, call4, NULL);
    if (ret != 0) {
        fprintf(stderr, "pthread_create error: %s\n", strerror(ret));
        exit(1);
    }

    // 设置线程分离
    ret = pthread_detach(tid);
    if (ret != 0) {
        fprintf(stderr, "pthread_detach error: %s\n", strerror(ret));
        exit(1);
    }

    sleep(1);

    ret = pthread_join(tid, NULL);
    if (ret != 0) {
        fprintf(stderr, "pthread_join error: %s\n", strerror(ret));
        exit(1);
    }

    printf("main: pid = %d, tid = %lu\n", getpid(), pthread_self());

    pthread_exit((void *) 0);

    return 0;
}