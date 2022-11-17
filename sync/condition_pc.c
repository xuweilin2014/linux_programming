//
// Created by xuweilin on 22-11-7.
//

#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <pthread.h>

/**
 * 借助条件变量模拟 生产者-消费者 问题
 */

/* 链表作为共享数据，需要被互斥量保护 */
struct msg {
    struct msg *next;
    int num;
};

struct msg *head;

/* 静态初始化 一个条件变量 和 一个互斥量 */
pthread_cond_t has_product = PTHREAD_COND_INITIALIZER;
pthread_mutex_t lock = PTHREAD_MUTEX_INITIALIZER;

void *consumer(void *p) {
    struct msg *mp;

    for (;;) {
        pthread_mutex_lock(&lock);
        // 头指针为空，说明没有节点
        // pthread_cond_wait 方法和 pthread_cond_signal 方法必须要和 lock 同步使用，因为需要一种同步机制保证，condition
        // 的检查与 wait() 操作，以及 condition 的更新与 notify() 是互斥的
        // 使用 while 是因为线程可能因为其他原因被唤醒过来，而不是通过 signal，此时 condition 条件还没有满足
        while (head == NULL) {
            pthread_cond_wait(&has_product, &lock);
        }
        mp = head;
        head = mp->next;
        pthread_mutex_unlock(&lock);

        printf("-Consume %lu---%d\n", pthread_self(), mp->num);
        free(mp);
        sleep(rand() % 5);
    }
}

void *producer(void *p) {
    struct msg *mp;

    for (;;) {
        mp = malloc(sizeof(struct msg));
        mp->num = rand() % 1000 + 1;
        printf("-Produce -------------- %d\n", mp->num);

        pthread_mutex_lock(&lock);
        mp->next = head;
        head = mp;
        pthread_mutex_unlock(&lock);

        pthread_cond_signal(&has_product);
        sleep(rand() % 5);
    }
}

int condition_pc() {

    pthread_t  pid, cid;
    srand(time(NULL));

    pthread_create(&pid, NULL, producer, NULL);
    pthread_create(&cid, NULL, consumer, NULL);

    pthread_join(&pid, NULL);
    pthread_join(&cid, NULL);

    return 0;
}