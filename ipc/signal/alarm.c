//
// Created by xuweilin on 22-11-5.
//
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/wait.h>

int alarm_test() {

    int i = 0;
    alarm(1);

    while (1) {
        printf("%d\n", i++);
    }

}