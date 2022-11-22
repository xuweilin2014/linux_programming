#include <stdio.h>
#include <stdlib.h>

/*
 * 对于系统调用 write 和 read 而言，没有用户级缓冲区，只有操作系统缓冲区，即 write 函数写入的内容会先缓冲在内核的缓冲区中，
 * 等到缓冲区满之后，再写入到操作系统中
 *
 * 而库函数，比如 fgetc 和 fputc 都有用户级的缓冲区，即用户写入的一个字符，会先缓冲在用户缓冲区中，等待缓冲区中的字符数满足一定
 * 数目时，然后再调用内核
 */

int getc_cmp_read() {
    FILE *fp, *fp_out;
    int n;

    fp = fopen("dict.txt", "r");
    if (fp == NULL) {
	    perror("fopen error");
        exit(1);
    }

    fp_out = fopen("dict.cp", "w");
    if (fp == NULL) {
	    perror("fopen error");
        exit(1);
    }

    while ((n = fgetc(fp)) != EOF) {
	    fputc(n, fp_out);
    }

    fclose(fp);
    fclose(fp_out);

    return 0;
}    
