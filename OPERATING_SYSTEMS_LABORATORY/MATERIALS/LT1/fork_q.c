#include <stdio.h>
#include <unistd.h>
int main()
{
    if (fork() || fork())
        fork();
    printf("1 ");
    return 0;
}
/* Output of the C code:
1 1 1 1 1
*/

#include <stdio.h>
#include <unistd.h>
int main()
{
    if (fork() && fork())
        fork();
    printf("1 ");
    return 0;
}

/* Output of the C code:
1 1 1 1
*/

#include <stdio.h>
#include <unistd.h>

int main()
{
    if (fork())
    {
        if (!fork())
        {
            fork();
            printf("1 ");
        }
        else
        {
            printf("2 ");
        }
    }
    else
    {
        printf("3 ");
    }
    printf("4 ");
    return 0;
}
/* Output of the C code:
2 4 3 4 1 4 1 4
*/

#include <stdio.h>
#include <unistd.h>

int main()
{
    if (fork() && (!fork()))
    {
        if (fork() || fork())
        {
            fork();
        }
    }
    printf("2 ");
    return 0;
}
/* Output of the C code:
2 2 2 2 2 2 2
*/

#include <stdio.h>
#include <unistd.h>
int main()
{
    int a = 0;
    int rc = fork();
    if (rc == 0)
    {
        rc = fork();
    }
    else
    {
        printf("Hello");
    }
    printf("Hello");
    return 0;
}
/* Output of the C code:
HelloHelloHelloHello
*/

#include <stdio.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>
int main(void)
{
    for (int i = 2; i <= 5; i++)
    {
        fork();
    }
    printf("CSE IIT KGP\n");
}

/* Output of the C code:
CSE IIT KGP
CSE IIT KGP
CSE IIT KGP
CSE IIT KGP
CSE IIT KGP
CSE IIT KGP
CSE IIT KGP
CSE IIT KGP
CSE IIT KGP
CSE IIT KGP
CSE IIT KGP
CSE IIT KGP
CSE IIT KGP
CSE IIT KGP
CSE IIT KGP
CSE IIT KGP
*/

#include <stdio.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>

int main(void)
{
    int n = 3;
    for (int i = 1; i <= n; i++)
    {
        fork();
        printf("CSE IIT KGP\n");
    }
}

/*
2(2^n-1) times "CSE IIT KGP" will be printed
*/

#include <stdio.h>
#include <unistd.h>
#include <sys/types.h>
int main()
{
    int i = 3;
    while (i > 0)
    {
        i--;
        if (fork() || fork())
        {
            printf("Hi\n");
        }
    }
}

/*
T(n)=3T(n-1)+2
T(1)=2
So T(n)=3^n-1
*/

#include <stdio.h>
#include <unistd.h>
#include <sys/types.h>
#include <stdlib.h>
void myFunc()

{
    if (fork() == 0)

    {
        fork();
        printf("HI\n");
        exit(0);
    }
    return;
}
int main()
{
    printf("HI\n");
    myFunc();
    printf("HI\n");
    return 1;
}
/*
HI
HI
HI
HI
*/