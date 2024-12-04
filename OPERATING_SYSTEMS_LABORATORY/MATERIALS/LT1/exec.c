#include <stdio.h>
#include <unistd.h>
int main()
{
    execl("/bin/ls", "ls", "-l", (char *)0);
}
/* Output of the C code:
total 64
-rwxrwxr-x 1 bratin bratin 16696 Mar 16 19:35 a.out
-rw-rw-r-- 1 bratin bratin    98 Mar 16 19:35 bin.c
-rwxrwxr-x 1 bratin bratin 16696 Mar 16 19:36 bin.out
-rw-rw-r-- 1 bratin bratin 11087 Mar 16 19:29 fork.c
-rw-rw-r-- 1 bratin bratin  1026 Mar 16 19:34 fork.md
-rw-rw-r-- 1 bratin bratin  2436 Mar 16 19:01 fork_q.c
*/

#include <stdio.h>
#include <unistd.h>
int main()
{
    execlp("ls", "ls", "-l", (char *)0);
}

/* Output of the C code:
total 68
-rwxrwxr-x 1 bratin bratin 16696 Mar 16 19:35 a.out
-rw-rw-r-- 1 bratin bratin    95 Mar 16 19:36 bin.c
-rwxrwxr-x 1 bratin bratin 16696 Mar 16 19:36 bin.out
-rw-rw-r-- 1 bratin bratin   456 Mar 16 19:36 exec.c
-rw-rw-r-- 1 bratin bratin 11087 Mar 16 19:29 fork.c
-rw-rw-r-- 1 bratin bratin  1026 Mar 16 19:34 fork.md
-rw-rw-r-- 1 bratin bratin  2436 Mar 16 19:01 fork_q.c
*/

#include <stdio.h>
#include <unistd.h>
int main()
{
    char *args[] = {"ls", "-l", (char *)0};
    execv("/bin/ls", args);
}

/* Output of the C code:
total 68
-rwxrwxr-x 1 bratin bratin 16696 Mar 16 19:35 a.out
-rw-rw-r-- 1 bratin bratin   128 Mar 16 19:37 bin.c
-rwxrwxr-x 1 bratin bratin 16744 Mar 16 19:37 bin.out
-rw-rw-r-- 1 bratin bratin   963 Mar 16 19:36 exec.c
-rw-rw-r-- 1 bratin bratin 11087 Mar 16 19:29 fork.c
-rw-rw-r-- 1 bratin bratin  1026 Mar 16 19:34 fork.md
-rw-rw-r-- 1 bratin bratin  2436 Mar 16 19:01 fork_q.c
*/

#include <stdio.h>
#include <unistd.h>
int main()
{
    char *args[] = {"ls", "-l", (char *)0};
    execvp(args[0], args);
}

/* Output of the C code:
total 68
-rwxrwxr-x 1 bratin bratin 16696 Mar 16 19:35 a.out
-rw-rw-r-- 1 bratin bratin   126 Mar 16 19:37 bin.c
-rwxrwxr-x 1 bratin bratin 16752 Mar 16 19:37 bin.out
-rw-rw-r-- 1 bratin bratin  1501 Mar 16 19:37 exec.c
-rw-rw-r-- 1 bratin bratin 11087 Mar 16 19:29 fork.c
-rw-rw-r-- 1 bratin bratin  1026 Mar 16 19:34 fork.md
-rw-rw-r-- 1 bratin bratin  2436 Mar 16 19:01 fork_q.c
*/

#include <stdio.h>
#include <unistd.h>
int main()
{
    char *args[] = {"this is a error", "error", (char *)0};
    execvp(args[0], args);
    printf("If you see this, execvp failed\n");
}
/* Output of the C code:
If you see this, execvp failed
*/

#include <stdio.h>
#include <unistd.h>
#include <errno.h>
#include <stdlib.h>
int main()
{
    char *args[] = {"this is a error", "error", (char *)0};
    int ret = execvp(args[0], args);
    perror("execvp");
    exit(1);
}

/*
execvp: No such file or directory
C code executed successfully. Output added as a comment in bin.c.
*/