//
// Created by xuweilin on 22-11-5.
//
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/wait.h>
#include <sys/time.h>

void sig_catch(int signo) {
    if (signo == SIGINT) {
        printf("catch you!! %d\n", signo);
    } else if (signo == SIGQUIT) {
        printf("---------catch you!!%d\n", signo);
    }
}

int sigaction_test() {
    struct sigaction act, old_act;

    act.sa_handler = sig_catch;
    sigemptyset(&act.sa_mask);
    act.sa_flags = 0;

    int ret = sigaction(SIGINT, &act, &old_act);
    if (ret == -1) {
        perror("sigaction error");
        exit(1);
    }

    ret = sigaction(SIGQUIT, &act, &old_act);
    if (ret == -1) {
        perror("sigaction error");
        exit(1);
    }

    while (1);

    return 0;
}