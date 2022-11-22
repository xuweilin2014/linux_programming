//
// Created by xuweilin on 22-11-5.
//
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/wait.h>
#include <sys/time.h>

/**
 * 下面的程序演示了信号集操作函数的使用
 *
 * sigset_t set;  自定义信号集
 * sigemptyset(sigset_t *set);	清空信号集
 * sigfillset(sigset_t *set);	全部置 1
 * sigaddset(sigset_t *set, int signum);  将一个信号添加到集合中
 * sigdelset(sigset_t *set, int signum);  将一个信号从集合中移除
 * sigismember（const sigset_t *set，int signum);  判断一个信号是否在集合中
 *
 * sigprocmask(int how, const sigset_t *set, sigset_t *oldset); 设置屏蔽字和解除屏蔽字
 *         how: SIG_BLOCK、SIG_UNBLOCK、SIG_SETMASK
 *
 * sigpending(sigset_t *set); 查看未觉信号集
 *
 * 下面的程序阻塞 SIGINT 信号，然后不断打印出进程的未决信号集：
 *
 * 0000000000000000000000000000000
 * 0100000000000000000000000000000（按了 CTRL+C 之后）
 */

void print_set(sigset_t *set) {
    int i = 0;

    for (i = 1; i < 32; i++) {
        if (sigismember(set, i)) {
            putchar('1');
        } else {
            putchar('0');
        }
    }
    printf("\n");
}

int sigsfunc() {
    sigset_t set, oldset, pedset;
    int ret = 0;

    sigemptyset(&set);
    // 添加 SIGINT 信号到信号集 set 中
    sigaddset(&set, SIGINT);

    // 阻塞 sigset_t 指定的信号集，即 set 指定的信号不会被进程处理
    ret = sigprocmask(SIG_BLOCK, &set, &oldset);
    if (ret == -1) {
        perror("sigprocmask error");
        exit(1);
    }

    while (1) {
        // 获取未决信号集
        ret = sigpending(&pedset);
        if (ret == -1) {
            perror("sigpending error");
            exit(1);
        }
        print_set(&pedset);
        sleep(1);
    }

    return 0;
}