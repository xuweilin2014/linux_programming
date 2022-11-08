//
// Created by xuweilin on 22-11-8.
//

#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <pthread.h>
#include <semaphore.h>

/**
 * 使用信号量实现的单生产者单消费者模型
 * @return
 */

#define NUM 5

int queue[NUM];
sem_t blank_number, product_number;

void *producer1(void *argc) {
    int i = 0;

    while (1) {
        sem_wait(&blank_number);
        queue[i] = rand() % 1000 + 1;
        printf("----Produce----%d\n", queue[i]);
        sem_post(&product_number);

        i = (i + 1) % NUM;
        sleep(rand() % 2);
    }
}

void *consumer1(void *argc) {
    int i = 0;

    while (1) {
        // 消费者将产品数--，为 0 则阻塞等待
        sem_wait(&product_number);
        printf("----Consume----%d\n", queue[i]);
        // 消费一个产品
        queue[i] = 0;
        // 消费掉以后，将空格子数++
        sem_post(&blank_number);

        i = (i + 1) % NUM;
        sleep(rand() % 2);
    }
}

int semaphore_pc() {

    pthread_t pid, cid;

    // 初始化空格数量为 NUM
    sem_init(&blank_number, 0, NUM);
    // 初始化产品数量为 0
    sem_init(&product_number, 0, 0);

    pthread_create(&pid, NULL, producer1, NULL);
    pthread_create(&cid, NULL, consumer1, NULL);

    pthread_join(pid, NULL);
    pthread_join(cid, NULL);

    sem_destroy(&blank_number);
    sem_destroy(&product_number);

    return 0;
}