#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <fcntl.h>
#include <unistd.h>
#include <pthread.h>

#define N 1

/**
 * 对于系统调用 read/write 来说，只有操作系统缓冲区，即 write 函数写入的内容会先缓冲在内核的缓冲区中，等到缓冲区满之后，再写入到操作系统中
 * 因此，每调用一次 read/write 都会进行一次用户态和内核态的切换，会耗费大量的时间，相比于调用 fgetc/fputc 这种库函数（有用户级缓冲区），
 * 速度也会慢很多
 */

int read_cmp_getc(int argc, char *argv[]) {
    char buf[N];
    
    int n = 0;
    int fd1 = open("dict.txt", O_RDONLY);

    if (fd1 == -1) {
        perror("open dict.txt error");
        exit(1);
    }

    // 将 fd1 代表的文件内容写入到 fd2 中，如果 fd2 代表的文件不存在，那么创建 (O_CREAT)
    // 如果 fd2 代表的文件存在，那么就截断或者说清零 fd2 文件 (O_TRUNC)
    int fd2 = open("dict.cp", O_WRONLY | O_CREAT | O_TRUNC, 0644);
    if (fd2 == -1) {
        perror("open dict.cp error");
        exit(1);
    }

    while ((n = read(fd1, buf, N)) != 0) {
	    if (n < 0) {
	        perror("read error");
            break;
        }	    
	    write(fd2, buf, n);
    }

    close(fd1);
    close(fd2);

    return 0;
}    

   
       
