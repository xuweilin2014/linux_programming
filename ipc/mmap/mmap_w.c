//
// Created by xuweilin on 22-11-4.
//
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <errno.h>
#include <fcntl.h>
#include <sys/mman.h>
#include <string.h>
#include <sys/wait.h>

struct student {
    int id;
    char name[256];
    int age;
};

int main() {
    struct student stu = {1, "xiaoming", 18};
    struct student *p;
    int fd;

    fd = open("../dir/test_map", O_RDWR|O_CREAT|O_TRUNC, 0644);
    if (fd == -1) {
        perror("open error");
        exit(1);
    }
    ftruncate(fd, sizeof(stu));

    p = mmap(NULL, sizeof(stu), PROT_READ|PROT_WRITE, MAP_SHARED, fd, 0);
    if (p == MAP_FAILED) {
        perror("mmap error");
        exit(1);
    }

    close(fd);

    while (1) {
        memcpy(p, &stu, sizeof(stu));
        stu.id++;
        sleep(1);
    }
}