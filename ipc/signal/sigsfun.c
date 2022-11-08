//
// Created by xuweilin on 22-11-5.
//
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/wait.h>
#include <sys/time.h>

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
    sigaddset(&set, SIGINT);

    ret = sigprocmask(SIG_BLOCK, &set, &oldset);
    if (ret == -1) {
        perror("sigprocmask error");
        exit(1);
    }

    while (1) {
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