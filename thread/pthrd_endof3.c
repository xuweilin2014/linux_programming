//
// Created by xuweilin on 22-11-6.
//

#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <pthread.h>

/**
 * 下面的程序演示了线程的取消方式，主要有以下几种：
 *
 * 1. 线程中的回调函数执行 return 语句并返回指定值
 * 2. 线程调用 pthread_exit()
 * 3. 调用 pthread_cancel()取消线程。
 * 4. 任意线程调用了 exit()，或者主线程执行了 return 语句（在 main()函数中），都会导致进程中的所有线程立即终止
 *
 * 对于 1 和 2，如果在使用 return 或者 pthread_exit 函数时指定了退出值，那么另外线程使用 pthread_join 回收时，
 * 会获取到对应的值。对于 3，如果线程【被】使用了 pthread_cancel 函数取消，调用 pthread_exit 获取到 -1
 *
 * 下面的程序测试了这一点
 */

void *call1(void *argc) {
    printf("thread 1 returning\n");
    return (void *) 111;
}

void *call2(void *argc) {
    printf("thread 2 exiting\n");
    pthread_exit((void *) 222);
}

void *call3(void *argc) {
    while (1) {
        printf("thread 3: I'm going to die in 3 seconds\n");
        sleep(1);
    }

    return (void *) 666;
}

int pthrd_endof() {

    pthread_t tid;
    void *tret = NULL;

    pthread_create(&tid, NULL, call1, NULL);
    // ret = 111
    pthread_join(tid, &tret);
    printf("thread 1 exit code = %d\n\n", (int) tret);

    pthread_create(&tid, NULL, call2, NULL);
    pthread_join(tid, &tret);
    // ret = 222
    printf("thread 2 exit code = %d\n\n", (int) tret);

    pthread_create(&tid, NULL, call3, NULL);
    sleep(3);
    pthread_cancel(tid);
    // ret = -1，因为前面使用了 pthread_cancel 取消了函数
    pthread_join(tid, &tret);
    printf("thread 3 exit code = %d\n\n", (int) tret);

    return 0;
}