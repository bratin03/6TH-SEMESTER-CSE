#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>

int main()
{
   int n, mypid, parpid;

   printf("Parent: n = ");
   scanf("%d", &n);

   /* Child creation */
   if (fork())
   { /* Parent process */
      mypid = getpid();
      parpid = getppid();
      printf("Parent: PID = %u, PPID = %u...\n", mypid, parpid);
   }
   else
   { /* Child process */
      mypid = getpid();
      parpid = getppid();
      printf("Child : PID = %u, PPID = %u...\n", mypid, parpid);
      n = n * n;
   }

   printf("Process PID = %u: n = %d\n", mypid, n);

   exit(0);
}

/*
Parent: n = 12
Parent: PID = 31147, PPID = 29841...
Process PID = 31147: n = 12
Child : PID = 31168, PPID = 31147...
Process PID = 31168: n = 144
*/