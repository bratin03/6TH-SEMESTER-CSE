#include<stdio.h>
#include<unistd.h>

int main()
{
    int p[2];
    pipe(p);
    printf("p[0] = %d\n", p[0]);
    printf("p[1] = %d\n", p[1]);
}
/* Output of the C code:
p[0] = 3
p[1] = 4
*/

#include<stdio.h>
#include<unistd.h>

int main()
{
    int p[1];
    if(pipe(p) == -1)
    {
        perror("Pipe failed\n");
    }
    printf("p[0] = %d\n", p[0]);
    printf("p[1] = %d\n", p[1]);
    return 0;
}
 
/*
p[0] = 3
p[1] = 4
*** stack smashing detected ***: terminated
Aborted (core dumped)
*/

#include<stdio.h>
#include<unistd.h>

int main()
{
    int p[3];
    if(pipe(p) == -1)
    {
        perror("Pipe failed\n");
    }
    printf("p[0] = %d\n", p[0]);
    printf("p[1] = %d\n", p[1]);
    printf("p[2] = %d\n", p[2]);
    return 0;
}
 
/*
p[0] = 3
p[1] = 4
p[2] = 32764
*/