//
// Created by xuweilin on 22-11-6.
//

#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <pthread.h>

/**
 *
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
    pthread_join(tid, &tret);
    printf("thread 1 exit code = %d\n\n", (int) tret);

    pthread_create(&tid, NULL, call2, NULL);
    pthread_join(tid, &tret);
    printf("thread 2 exit code = %d\n\n", (int) tret);

    pthread_create(&tid, NULL, call3, NULL);
    sleep(3);
    pthread_cancel(tid);
    pthread_join(tid, &tret);
    printf("thread 3 exit code = %d\n\n", (int) tret);

    return 0;
}