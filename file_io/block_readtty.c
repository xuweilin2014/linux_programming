#include <unistd.h>
#include <stdlib.h>
#include <stdio.h>

/**
 * 从 STDIN_FILENO 读取字符，但是从文件中读取字符时，一般默认是阻塞读取，下面的方法中，
 * 如果用户不从终端中输入字符，那么程序就会一直阻塞在 read 方法上
 */

int block_readtty() {
   char buf[10];
   int n;
   
   n = read(STDIN_FILENO, buf, 10);
   if (n < 0) {
	    perror("read STDIN_FILENO");
        exit(1);
   }

   write(STDOUT_FILENO, buf, n);

   return 0;
}   
