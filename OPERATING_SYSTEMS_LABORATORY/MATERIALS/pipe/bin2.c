#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <fcntl.h>
#include <unistd.h>

int main()
{
    int fd1;
    char buf[100];
    fd1 = open("mypipe", O_RDONLY);
    read(fd1, buf, 100);
    printf("%s\n", buf);
    close(fd1);
}