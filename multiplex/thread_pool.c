//
// Created by xuweilin on 22-11-17.
//

#include <stdlib.h>
#include <pthread.h>
#include <unistd.h>
#include <assert.h>
#include <stdio.h>
#include <string.h>
#include <signal.h>
#include <errno.h>
#include "thread_pool.h"

/* 管理线程每隔 10s 检测一次，对线程池中的线程进行扩容或者销毁 */
#define DEFAULT_TIME 10
/* 如果 queue_size > MIN_WAIT_TASK_NUM 添加新的线程到线程池 */
#define MIN_WAIT_TASK_NUM 10
/* 每次创建和销毁线程的个数，类似于步长 */
#define DEFAULT_THREAD_VARY 10
#define true 1
#define false 0

typedef struct {
    /* 函数指针，回调函数 */
    void *(*function)(void *);
    /* 上面函数的参数 */
    void *arg;
} threadpool_task_t;                    /* 各子线程任务结构体 */

/* 描述线程池相关信息 */

struct threadpool_t {
    pthread_mutex_t lock;               /* 用于锁住本结构体，也就是对本结构体信息的互斥访问 */
    pthread_mutex_t thread_counter;     /* 记录忙状态线程个数的琐 -- 用户互斥访问 busy_thr_num 变量*/

    pthread_cond_t queue_not_full;      /* 当任务队列满时，添加任务的线程阻塞，等待此条件变量 */
    pthread_cond_t queue_not_empty;     /* 当任务队列为空时，执行任务的线程阻塞，等待此条件变量 */

    pthread_t *threads;                 /* 存放线程池中每个线程的tid，数组 */
    pthread_t adjust_tid;               /* 存管理线程tid */
    threadpool_task_t *task_queue;      /* 任务队列(数组首地址) */

    int min_thr_num;                    /* 线程池最小线程数 */
    int max_thr_num;                    /* 线程池最大线程数 */
    int live_thr_num;                   /* 当前存活线程个数 */
    int busy_thr_num;                   /* 忙状态线程个数 */
    int wait_exit_thr_num;              /* 要销毁的线程个数 */

    int queue_front;                    /* task_queue 队头下标 */
    int queue_rear;                     /* task_queue 队尾下标 */
    int queue_size;                     /* task_queue 队中实际任务数 */
    int queue_max_size;                 /* task_queue 队列可容纳任务数上限 */

    int shutdown;                       /* 标志位，线程池使用状态，true 或 false */
};

void *threadpool_thread(void *threadpool);
void *adjust_thread(void *threadpool);
int is_thread_alive(pthread_t tid);
int threadpool_free(threadpool_t *pool);

// threadpool_create(3,100,100);
/**
 * 初始化线程池中的各个参数，并且创建好 min_thr_num = live_thr_num 个线程，同时再创建好一个管理线程
 */
threadpool_t *threadpool_create(int min_thr_num, int max_thr_num, int queue_max_size) {

    int i;
    /* 线程池 结构体 */
    threadpool_t *pool = NULL;

    /* do while(0) 用来模拟 goto 语句，也就是当条件满足时，可以使用 break 直接退出，不执行后面的语句 */
    do {
        if((pool = (threadpool_t *)malloc(sizeof(threadpool_t))) == NULL) {
            printf("malloc threadpool fail");
            /* 跳出do while */
            break;
        }

        pool->min_thr_num = min_thr_num;
        pool->max_thr_num = max_thr_num;
        pool->busy_thr_num = 0;
        /* 活着的线程数 初值=最小线程数 */
        pool->live_thr_num = min_thr_num;
        pool->wait_exit_thr_num = 0;
        /* 任务队列中有 0 个产品 */
        pool->queue_size = 0;
        /* 最大任务队列数，任务队列的容量 */
        pool->queue_max_size = queue_max_size;
        pool->queue_front = 0;
        pool->queue_rear = 0;
        /* 不关闭线程池 */
        pool->shutdown = false;

        /* 根据最大线程上限数， 给工作线程数组开辟空间, 并清零 */
        pool->threads = (pthread_t *)malloc(sizeof(pthread_t) * max_thr_num);
        if (pool->threads == NULL) {
            printf("malloc threads fail");
            break;
        }
        memset(pool->threads, 0, sizeof(pthread_t) * max_thr_num);

        /* 给 任务队列 开辟空间 */
        pool->task_queue = (threadpool_task_t *) malloc(sizeof(threadpool_task_t) * queue_max_size);
        if (pool->task_queue == NULL) {
            printf("malloc task_queue fail");
            break;
        }

        /* 初始化互斥琐、条件变量 */
        if (pthread_mutex_init(&(pool->lock), NULL) != 0
            || pthread_mutex_init(&(pool->thread_counter), NULL) != 0
            || pthread_cond_init(&(pool->queue_not_empty), NULL) != 0
            || pthread_cond_init(&(pool->queue_not_full), NULL) != 0) {
            printf("init the lock or cond fail");
            break;
        }

        /* 启动 min_thr_num 个 work thread */
        for (i = 0; i < min_thr_num; i++) {
            /* pool 指向当前线程池结构体，threadpool_thread 是一个函数指针，用来获取任务队列中的任务进行执行 */
            pthread_create(&(pool->threads[i]), NULL, threadpool_thread, (void *)pool);
            printf("start thread 0x%x...\n", (unsigned int)pool->threads[i]);
        }
        /* 创建管理者线程，adjust_thread 是一个函数指针，用来对线程池中的线程进行扩容或者销毁 */
        pthread_create(&(pool->adjust_tid), NULL, adjust_thread, (void *)pool);

        return pool;

    } while (0);

    /* 前面代码调用失败时，释放 poll 存储空间 */
    threadpool_free(pool);

    return NULL;
}

/* 向线程池的任务队列 task_queue 数组中 添加一个任务 */
// threadpool_add(thp, process, (void*)&num[i]);   /* 向线程池中添加任务 process: 小写---->大写*/

int threadpool_add(threadpool_t *pool, void*(*function)(void *arg), void *arg) {

    pthread_mutex_lock(&(pool->lock));

    /* == 为真，说明任务队列已经满， 调 wait 阻塞在 queue_not_full 条件变量上 */
    while ((pool->queue_size == pool->queue_max_size) && (!pool->shutdown)) {
        pthread_cond_wait(&(pool->queue_not_full), &(pool->lock));
    }

    if (pool->shutdown) {
        pthread_cond_broadcast(&(pool->queue_not_empty));
        pthread_mutex_unlock(&(pool->lock));
        return 0;
    }

    /* 清空 工作线程 调用的回调函数 的参数arg */
    if (pool->task_queue[pool->queue_rear].arg != NULL) {
        pool->task_queue[pool->queue_rear].arg = NULL;
    }

    /* 添加任务到任务队列里，也就是设置到 task_queue 中 */
    pool->task_queue[pool->queue_rear].function = function;
    pool->task_queue[pool->queue_rear].arg = arg;

    /* 任务入队，队尾指针移动；如果是出队的话，队头指针移动, 模拟环形队列 */
    pool->queue_rear = (pool->queue_rear + 1) % pool->queue_max_size;
    pool->queue_size++;

    /* 添加完任务后，队列不为空，唤醒线程池中 等待处理任务的线程 */
    pthread_cond_signal(&(pool->queue_not_empty));
    pthread_mutex_unlock(&(pool->lock));

    return 0;
}

/* 线程池中各个工作线程 */
/**
 * 线程池中各个工作线程的执行逻辑，具体逻辑就是一个循环，不断从任务队列中取出任务，唤醒阻塞在 queue_not_full 变量上的线程，通知它们可以
 * 往任务队列中添加任务。在真正执行任务之前，忙状态线程数增加一，然后回调函数执行任务，在执行任务之后，忙状态线程数减一。但是有两种情况需要注意：
 *
 * 第一种就是如果工作线程被唤醒之后，如果发现 wait_exit_thr_num 变量大于 0，则说明管理线程在对线程池进行缩容，随机唤醒 10 个线程，调用
 * pthread_exit 退出。
 *
 * 第二种就是发现线程池已经被关闭，那么需要销毁线程池中的所有线程，也就是进行线程分离，这样线程调用 pthread_exit 退出后，内核会自动回收其
 * 资源。
 *
 * @param threadpool
 * @return
 */
void *threadpool_thread(void *threadpool) {

    threadpool_t *pool = (threadpool_t *)threadpool;
    threadpool_task_t task;

    while (true) {
        /* Lock must be taken to wait on conditional variable */
        /* 刚创建出线程，等待任务队列里有任务，否则阻塞等待任务队列里有任务后再唤醒接收任务 */
        pthread_mutex_lock(&(pool->lock));

        /* queue_size == 0 说明没有任务，调 wait 阻塞在条件变量上, 若有任务，跳过该while */
        while ((pool->queue_size == 0) && (!pool->shutdown)) {
            printf("thread 0x%x is waiting\n", (unsigned int)pthread_self());
            pthread_cond_wait(&(pool->queue_not_empty), &(pool->lock));

            /* 清除指定数目的空闲线程，如果要结束的线程个数大于 0，结束线程 */
            /* 如果 wait_exit_thr_num 参数大于 0，那么说明线程池要进行缩容，在线程池的所有线程中随机选取 10 个
             * 线程，唤醒，然后终止这些线程
             */
            if (pool->wait_exit_thr_num > 0) {
                pool->wait_exit_thr_num--;

                /* 如果线程池里线程个数大于最小值时可以结束当前线程 */
                if (pool->live_thr_num > pool->min_thr_num) {
                    printf("thread 0x%x is exiting\n", (unsigned int)pthread_self());
                    pool->live_thr_num--;
                    pthread_mutex_unlock(&(pool->lock));

                    pthread_exit(NULL);
                }
            }
        }

        /* 如果指定了 true，要关闭线程池里的每个线程，自行退出处理---销毁线程池 */
        if (pool->shutdown) {
            pthread_mutex_unlock(&(pool->lock));
            printf("thread 0x%x is exiting\n", (unsigned int)pthread_self());
            /* 使用 pthread_detach()，线程可以自行分离，望系统在线程终止时能够自动清理并移除之 */
            pthread_detach(pthread_self());
            /* 线程自行终止 */
            /* pthread_exit() 函数将终止调用线程，且其返回值可由另一线程通过调用 pthread_join() 来获取。 */
            pthread_exit(NULL);
        }

        /* 从任务队列里获取任务, 是一个出队操作 */
        task.function = pool->task_queue[pool->queue_front].function;
        task.arg = pool->task_queue[pool->queue_front].arg;

        /* 出队，模拟环形队列 */
        pool->queue_front = (pool->queue_front + 1) % pool->queue_max_size;
        pool->queue_size--;

        /* 通知可以有新的任务添加进来 */
        pthread_cond_broadcast(&(pool->queue_not_full));

        /* 任务取出后，立即将 线程池琐 释放 */
        pthread_mutex_unlock(&(pool->lock));

        /* 执行任务 */
        printf("thread 0x%x start working\n", (unsigned int) pthread_self());
        /* 忙状态线程数变量琐 */
        pthread_mutex_lock(&(pool->thread_counter));
        /* 忙状态线程数 + 1 */
        pool->busy_thr_num++;
        pthread_mutex_unlock(&(pool->thread_counter));

        /* 执行回调函数任务 */
        (*(task.function))(task.arg);

        /* 任务结束处理 */
        printf("thread 0x%x end working\n", (unsigned int) pthread_self());
        pthread_mutex_lock(&(pool->thread_counter));
        /* 处理掉一个任务，忙状态数线程数 - 1 */
        pool->busy_thr_num--;
        pthread_mutex_unlock(&(pool->thread_counter));
    }

    pthread_exit(NULL);
}

/* 管理线程 */
/* 管理线程会对线程池进行扩容和缩容，条件分别如下：
 *
 * 1.任务数 大于 线程池中最小任务个数, 且此时存活的线程数 少于 最大线程个数，会一次创建 10 个线程
 * 2.忙线程 X2 小于 存活的线程数 且此时存活的线程数 大于 最小线程数时，会随机唤醒阻塞在 queue_not_empty 条件变量上的 10 个线程，
 *   这些线程会自己调用 pthread_exit 方法退出
 */
void *adjust_thread(void *threadpool) {

    int i;
    threadpool_t *pool = (threadpool_t *)threadpool;
    while (!pool->shutdown) {

        /* 定时（每隔 5s）对线程池管理 */
        sleep(DEFAULT_TIME);

        pthread_mutex_lock(&(pool->lock));
        /* 任务队列中的任务数 */
        int queue_size = pool->queue_size;
        /* 现在线程池中存活的线程数 */
        int live_thr_num = pool->live_thr_num;
        pthread_mutex_unlock(&(pool->lock));

        pthread_mutex_lock(&(pool->thread_counter));
        /* 忙着的线程数 */
        int busy_thr_num = pool->busy_thr_num;
        pthread_mutex_unlock(&(pool->thread_counter));

        /* 创建新线程 算法：任务数大于线程池中最小任务个数, 且存活的线程数少于最大线程个数时 如：30>=10 && 40<100 */
        /* 任务数大于线程池中最小任务个数，说明任务存在积压，所以需要对线程池扩容，但是此时线程数量不能超过用户指定的最大线程池数量 */
        if (queue_size >= MIN_WAIT_TASK_NUM && live_thr_num < pool->max_thr_num) {
            pthread_mutex_lock(&(pool->lock));
            int add = 0;

            /* 一次增加 DEFAULT_THREAD 个线程 */
            for (i = 0; i < pool->max_thr_num && add < DEFAULT_THREAD_VARY
                        && pool->live_thr_num < pool->max_thr_num; i++) {
                if (pool->threads[i] == 0 || !is_thread_alive(pool->threads[i])) {
                    pthread_create(&(pool->threads[i]), NULL, threadpool_thread, (void *)pool);
                    add++;
                    pool->live_thr_num++;
                }
            }

            pthread_mutex_unlock(&(pool->lock));
        }

        /* 销毁多余的空闲线程 算法：忙线程X2 小于 存活的线程数 且 存活的线程数 大于 最小线程数时*/
        /* 只有不到一半的线程数量有任务执行，其它线程都处于空闲状态，那么说明需要缩容，但此时线程数量不能小于用户指定的最小线程池数量 */
        if ((busy_thr_num * 2) < live_thr_num  &&  live_thr_num > pool->min_thr_num) {

            /* 一次销毁 DEFAULT_THREAD 个线程, 隨機唤醒阻塞在 queue_not_empty 条件变量上的 10 個线程即可 */
            pthread_mutex_lock(&(pool->lock));
            /* 要销毁的线程数 设置为10 */
            pool->wait_exit_thr_num = DEFAULT_THREAD_VARY;
            pthread_mutex_unlock(&(pool->lock));

            for (i = 0; i < DEFAULT_THREAD_VARY; i++) {
                /* 通知处在空闲状态的线程, 他们会自行终止 */
                pthread_cond_signal(&(pool->queue_not_empty));
            }
        }
    }

    return NULL;
}

int threadpool_destroy(threadpool_t *pool) {

    int i;
    if (pool == NULL) {
        return -1;
    }
    pool->shutdown = true;

    /* 先销毁管理线程 */
    pthread_join(pool->adjust_tid, NULL);

    for (i = 0; i < pool->live_thr_num; i++) {
        /* 通知所有的空闲线程，这些线程调用 pthread_exit 自行终止 */
        pthread_cond_broadcast(&(pool->queue_not_empty));
    }

    // 调用 pthread_join 获取到线程的退出值
    for (i = 0; i < pool->live_thr_num; i++) {
        pthread_join(pool->threads[i], NULL);
    }

    threadpool_free(pool);

    return 0;
}

int threadpool_free(threadpool_t *pool) {

    if (pool == NULL) {
        return -1;
    }

    if (pool->task_queue) {
        free(pool->task_queue);
    }
    if (pool->threads) {
        free(pool->threads);
        pthread_mutex_lock(&(pool->lock));
        pthread_mutex_destroy(&(pool->lock));
        pthread_mutex_lock(&(pool->thread_counter));
        pthread_mutex_destroy(&(pool->thread_counter));
        pthread_cond_destroy(&(pool->queue_not_empty));
        pthread_cond_destroy(&(pool->queue_not_full));
    }
    free(pool);
    pool = NULL;

    return 0;
}

int threadpool_all_threadnum(threadpool_t *pool) {

    int all_threadnum = -1;                 // 总线程数

    pthread_mutex_lock(&(pool->lock));
    all_threadnum = pool->live_thr_num;     // 存活线程数
    pthread_mutex_unlock(&(pool->lock));

    return all_threadnum;

}

int threadpool_busy_threadnum(threadpool_t *pool) {
    // 忙线程数
    int busy_threadnum = -1;

    pthread_mutex_lock(&(pool->thread_counter));
    busy_threadnum = pool->busy_thr_num;
    pthread_mutex_unlock(&(pool->thread_counter));

    return busy_threadnum;

}

int is_thread_alive(pthread_t tid) {

    // 发 0 号信号，测试线程是否存活
    // sending the signal 0 to a given PID just checks if any process with the given PID is running,
    // and you have the permission to send a signal to it. It returns -1 (and sets errno to ESRCH)
    // when something goes wrong.
    int kill_rc = pthread_kill(tid, 0);
    if (kill_rc == ESRCH) {
        return false;
    }
    return true;
}

/*测试*/

#if 1

/* 线程池中的线程，模拟处理业务 */
void *process(void *arg) {

    printf("thread 0x%x working on task %d\n ",(unsigned int)pthread_self(),(int)arg);
    sleep(1);                           //模拟 小---大写
    printf("task %d is end\n",(int)arg);

    return NULL;
}

int main(void) {

    /* threadpool_t *threadpool_create(int min_thr_num, int max_thr_num, int queue_max_size);*/
    /* 创建线程池，池里最小 3 个线程，最大 100，队列最大 100 */
    threadpool_t *thp = threadpool_create(3,100,100);
    printf("pool inited");

    // int *num = (int *)malloc(sizeof(int)*20);
    int num[20], i;
    for (i = 0; i < 20; i++) {
        num[i] = i;
        printf("add task %d\n",i);

        /* int threadpool_add(threadpool_t *pool, void*(*function)(void *arg), void *arg) */
        /* 向线程池中添加任务 */
        threadpool_add(thp, process, (void*)&num[i]);
    }

    /* 等子线程完成任务 */
    sleep(10);
    threadpool_destroy(thp);

    return 0;
}

#endif
