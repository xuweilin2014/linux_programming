//
// Created by xuweilin on 22-10-30.
//
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <dirent.h>

/*
 * 使用 dir 目录的相关函数实现 ls 命令
 */

int my_ls(const char *path) {

    DIR *dp;
    struct dirent *sdp;

    dp = opendir(path);
    if (dp == NULL) {
        perror("opendir error");
        exit(1);
    }

    while ((sdp = readdir(dp)) != NULL) {
        if (strcmp(&(sdp->d_name[0]), ".") == 0) {
            continue;
        }

        printf("%s\t", sdp->d_name);
    }

    printf("\n");
    closedir(dp);

    return 0;
}