//
// Created by xuweilin on 22-10-30.
//
#include <dirent.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <sys/stat.h>

/*
 * 递归遍历目录：ls -R
 *      1.判断命令行参数，获取用户要查询的目录名。argv[1]
 *      2.判断用户指定的是否是目录，stat S_ISDIR(); --> 封装函数 isFile
 *      3.读目录：
 *          opendir(dir)
 *          while (readdir()) {
 *              普通文件，直接打印
 *              目录：
 *                  拼接目录访问绝对路径
 *                  递归调用自己
 *          }
 *          closedir()
 */

#define PATH_LEN 256

// 打开目录，处理目录项
void fetch_dir(const char *dir, void (*fcn)(char *)) {
    char name[PATH_LEN];
    struct dirent *sdp;
    DIR *dp;

    if ((dp = opendir(dir)) == NULL) {
        fprintf(stderr,"fetch dir:can't open %s\n", dir);
        return;
    }

    // 读取目录项
    while ((sdp = readdir(dp)) != NULL) {
        // 防止出现无限递归
        if (strcmp(sdp->d_name, ".") == 0 || strcmp(sdp->d_name, "..") == 0) {
            continue;
        }

        // +2 是考虑到 / 和 \0 这两个字符
        if (strlen(dir) + strlen(sdp->d_name) + 2 > sizeof(name)) {
            fprintf(stderr, "fetch dir: name %s %s too long\n", dir, sdp->d_name);
        } else {
            // 目录项本身不可访问，拼接 目录/目录项
            sprintf(name, "%s/%s", dir, sdp->d_name);
            // 通过函数指针，回调 isfile
            (*fcn)(name);
        }
    }

    closedir(dp);
}

// 处理目录/文件
void isfile(char *name) {
    struct stat sbuf;

    // -1 表示文件/目录名无效
    if(stat(name, &sbuf) == -1) {
        fprintf(stderr, "isfile: can't access %s\n", name);
        exit(1);
    }

    // 判定为目录的话
    if((sbuf.st_mode & S_IFMT) == S_IFDIR) {
        fetch_dir(name, isfile);
    }

    // 判定为普通文件，直接打印信息
    printf("%8ld %s\n", sbuf.st_size, name);
}

int ls_recursive(int argc, char *argv[]) {
    if (argc == 1)
        isfile(".");
    else
        while (--argc > 0) {
            isfile(*++argv);
            printf("\n");
        }

    return 0;
}