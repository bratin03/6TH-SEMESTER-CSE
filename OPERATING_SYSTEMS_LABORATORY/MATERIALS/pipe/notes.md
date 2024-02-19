1.  
```c
#include <stdio.h>
#include <unistd.h>

int main()
{
    int p[2];
    pipe(p);
    printf("% d % d", p[0], p[1]);
}
```

```txt
 3  4
```

2.  
```c
#include <stdio.h>
#include <sys/types.h>
#include <unistd.h>
#include <sys/wait.h>

#define MAX 7
int main()
{
    char *msg = "hello\n";
    char buff[MAX];
    int p[2];
    pipe(p);
    pid_t pid = fork();
    if (pid > 0)
    {
        printf("Parent starts:\n");
        write(p[1], msg, MAX);
        waitpid(pid, NULL, 0);
        printf("Parent ends:\n");
    }
    else
    {
        sleep(1);
        printf("Child starts:\n");
        read(p[0], buff, MAX);
        printf("%s", buff);
        printf("Child ends:\n");
    }
}
```

```txt
Parent starts:
Child starts:
hello
Child ends:
Parent ends:
```

3.  
```c
#include <stdio.h>
#include <sys/types.h>
#include <unistd.h>
#include <sys/wait.h>

#define MAX 100
int main()
{
    char *msg_p = "hello parent\n";
    char *msg_c = "hello child\n";
    char buff[MAX];
    int p[2];
    pipe(p);
    pid_t pid = fork();
    if (pid > 0)
    {
        printf("Parent starts:\n");
        write(p[1], msg_p, MAX);
        waitpid(pid, NULL, 0);
        printf("Parent ends:\n");
    }
    else
    {
        sleep(1);
        printf("Child starts:\n");
        write(p[1], msg_c, MAX);
        for (int i = 0; i < 2; i++)
        {
            read(p[0], buff, MAX);
            printf("%s", buff);
        }
        printf("Child ends:\n");
    }
}
```

```txt
Parent starts:
Child starts:
hello parent
hello child
Child ends:
Parent ends:
```

4.  
```c
#include <stdio.h>
#include <sys/types.h>
#include <unistd.h>
#include <sys/wait.h>

#define MAX 100
int main()
{
    char *msg_p = "hello parent\n";
    char *msg_c = "hello child\n";
    char buff[MAX];
    int p[2];
    pipe(p);
    pid_t pid = fork();
    if (pid > 0)
    {
        printf("Parent starts:\n");
        write(p[1], msg_p, MAX);
        for (int i = 0; i < 2; i++)
        {
            read(p[0], buff, MAX);
            printf("%s", buff);
        }
        waitpid(pid, NULL, 0);
        printf("Parent ends:\n");
    }
    else
    {
        sleep(1);
        printf("Child starts:\n");
        write(p[1], msg_c, MAX);
        for (int i = 0; i < 2; i++)
        {
            read(p[0], buff, MAX);
            printf("%s", buff);
        }
        printf("Child ends:\n");
    }
}
```

```txt
Parent starts:
hello parent
Child starts:
hello child
^C
```

5.  
```c
#include <stdio.h>
#include <sys/types.h>
#include <unistd.h>
#include <sys/wait.h>

#define MAX 100
int main()
{
    char *msg = "hello\n";
    char buff[MAX];
    int p[2];
    pipe(p);
    pid_t pid = fork();
    if (pid == 0)
    {
        printf("Child exiting\n");
    }
    else
    {
        read(p[0], buff, MAX);
    }
}
```

```txt
Child exiting
^C
```

6.   
```c
#include <stdio.h>
#include <sys/types.h>
#include <unistd.h>
#include <sys/wait.h>

#define MAX 100
int main()
{
    char *msg = "hello\n";
    char buff[MAX];
    int p[2];
    pipe(p);
    pid_t pid = fork();
    if (pid == 0)
    {
        printf("Child exiting\n");
    }
    else
    {
        close(p[1]);
        read(p[0], buff, MAX);
    }
}
```

```txt
Child exiting
```

7.  
```c
#include <stdio.h>
#include <sys/types.h>
#include <unistd.h>
#include <sys/wait.h>

#define MAX 100
int main()
{
    char *msg = "hello\n";
    char buff[MAX];
    int p[2];
    pipe(p);
    pid_t pid = fork();
    if (pid == 0)
    {
        printf("Child exiting\n");
    }
    else
    {
        write(p[1], buff, MAX);
    }
}
```

```txt
Child exiting
```

8.  
```c
#include <stdio.h>
#include <sys/types.h>
#include <unistd.h>
#include <sys/wait.h>

#define MAX 100
int main()
{
    char *msg = "hello\n";
    char buff[MAX];
    int p[2];
    pipe(p);
    pid_t pid = fork();
    if (pid == 0)
    {
        printf("Child exiting\n");
    }
    else
    {
        sleep(1);
        close(p[0]);
        write(p[1], buff, MAX);
    }
}
```

```txt
Child exiting

-------------

Along with Exit Code 141
```

9.  
```c
#include <stdio.h>
#include <sys/types.h>
#include <unistd.h>
#include <sys/wait.h>
#include <fcntl.h>
#include <stdlib.h>
int main()
{
    int file_desc = open("dup.txt", O_WRONLY | O_APPEND);
    if (file_desc < 0)
    {
        printf("Error opening the file\n");
        exit(EXIT_FAILURE);
    }

    int copy_desc = dup(file_desc);

    write(copy_desc, "This will be output to the file named dup.txt\n", 46);
    write(file_desc, "This will also be output to the file named dup.txt\n", 51);
    close(file_desc);
    close(copy_desc);
    return 0;
}
```

```txt
 bratin  bratin03  ../MATERIALS/pipe  cat dup.txt
This will be output to the file named dup.txt
This will also be output to the file named dup.txt
```

10.  
```c
#include <stdio.h>
#include <sys/types.h>
#include <unistd.h>
#include <sys/wait.h>
#include <fcntl.h>
#include <stdlib.h>
int main()
{
    int file_desc = open("dup.txt", O_WRONLY | O_APPEND);
    if (file_desc < 0)
    {
        printf("Error opening the file\n");
        exit(EXIT_FAILURE);
    }

    int copy_desc = dup(file_desc);

    printf("file_desc = %d\n", file_desc);
    printf("copy_desc = %d\n", copy_desc);

    close(file_desc);
    close(copy_desc);
    return 0;
}
```

```txt
file_desc = 3
copy_desc = 4
```

11.  
```c
#include <stdio.h>
#include <sys/types.h>
#include <unistd.h>
#include <sys/wait.h>
#include <fcntl.h>
#include <stdlib.h>
int main()
{
    int file_desc = open("dup.txt", O_WRONLY | O_APPEND);
    if (file_desc < 0)
    {
        printf("Error opening the file\n");
        exit(EXIT_FAILURE);
    }

    int copy_desc = open("dup.txt", O_WRONLY | O_APPEND);

    printf("file_desc = %d\n", file_desc);
    printf("copy_desc = %d\n", copy_desc);

    write(copy_desc, "This will be output to the file named dup.txt\n", 46);
    write(file_desc, "This will also be output to the file named dup.txt\n", 51);
    close(file_desc);
    close(copy_desc);
    return 0;
}
```

```txt
file_desc = 3
copy_desc = 4

 bratin  bratin03  ../MATERIALS/pipe  cat dup.txt 
This will be output to the file named dup.txt
This will also be output to the file named dup.txt
```

12.  
```c
#include <stdio.h>
#include <sys/types.h>
#include <unistd.h>
#include <sys/wait.h>
#include <fcntl.h>
#include <stdlib.h>
int main()
{
    int old, new;
    old = open("dup.txt", O_WRONLY | O_CREAT);
    //    new = dup2(old,1);

    close(1);
    new = dup(old);

    printf("hello\n");
    return 0;
}
```

```txt
 bratin  bratin03  ../MATERIALS/pipe  cat dup.txt 
hello
```

13.  
```c
#include <stdio.h>
#include <sys/types.h>
#include <unistd.h>
#include <sys/wait.h>
#include <fcntl.h>
#include <stdlib.h>
int main()
{
    int old, new;
    old = open("dup.txt", O_WRONLY | O_CREAT);

    new = dup2(old, 1);

    // close(1);
    // new = dup(old);

    printf("hello\n");
    return 0;
}
```

```txt
 bratin  bratin03  ../MATERIALS/pipe  cat dup.txt 
hello
```

13.  
```c
#include <stdio.h>
#include <stdlib.h>

int main()
{
    // Command to be executed
    const char *command = "ls -l";

    // Open a pipe to execute the command and read its output
    FILE *pipe = popen(command, "r");

    if (pipe == NULL)
    {
        perror("popen failed");
        return EXIT_FAILURE;
    }

    // Read and print the output of the command
    char buffer[128];
    while (fgets(buffer, sizeof(buffer), pipe) != NULL)
    {
        printf("%s", buffer);
    }

    // Close the pipe
    if (pclose(pipe) == -1)
    {
        perror("pclose failed");
        return EXIT_FAILURE;
    }

    return EXIT_SUCCESS;
}
```

```txt
total 384
-rwxrwxr-x 1 bratin bratin  16920 Jan 21 10:58 a.out
-rw-rw-r-- 1 bratin bratin    637 Jan 21 10:58 bin.c
-rw-rw-r-- 1 bratin bratin   1581 Jan 20 21:57 dup.c
-rw-r--r-- 1 bratin bratin   1580 Jan 20 21:55 DUP.c
-rw-rw-r-- 1 bratin bratin      6 Jan 21 10:21 dup.txt
-rw-rw-r-- 1 bratin bratin  84397 Jan 21 10:57 man_page.md
-rw-rw-r-- 1 bratin bratin   7680 Jan 21 10:59 notes.md
-rw-rw-r-- 1 bratin bratin   4867 Jan 20 21:51 pipe.c
-rw-rw-r-- 1 bratin bratin 251845 Jan 20 21:00 pipe.pdf
```