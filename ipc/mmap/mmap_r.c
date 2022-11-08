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
    struct student stu;
    struct student *p;
    int fd;

    fd = open("../dir/test_map", O_RDONLY);
    if (fd == -1) {
        perror("open error");
        exit(1);
    }

    p = mmap(NULL, sizeof(stu), PROT_READ, MAP_SHARED, fd, 0);
    if (p == MAP_FAILED) {
        perror("mmap error");
        exit(1);
    }

    close(fd);

    while (1) {
        printf("id=%d, name=%s, age=%d\n", p->id, p->name, p->age);
        sleep(3);
    }
}