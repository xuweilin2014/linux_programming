#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <fcntl.h>
#include <unistd.h>
#include <pthread.h>

int main(int argc, char *argv[]) {
    char buf[1024];
    
    int n = 0;
    int fd1 = open(argv[1], O_RDONLY);
    if (fd1 == -1) {
	 perror("open argv1 error");
         exit(1);
    }	 
    // 将 fd1 代表的文件内容写入到 fd2 中，如果 fd2 代表的文件不存在，那么创建 (O_CREAT)
    // 如果 fd2 代表的文件存在，那么就截断或者说清零 fd2 文件 (O_TRUNC)
    int fd2 = open(argv[2], O_RDWR | O_CREAT | O_TRUNC, 0664);
    if (fd2 == -1) {
	 perror("open argv2 error");
         exit(1);
    }

    while ((n = read(fd1, buf, 1024)) != 0) {
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

   
       
