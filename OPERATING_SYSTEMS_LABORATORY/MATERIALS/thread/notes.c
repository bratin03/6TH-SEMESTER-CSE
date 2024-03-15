#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>

int sum;
void *runner(void *param);


int main(int argc, char *argv[])
{
    pthread_t tid;
    pthread_attr_t attr;

    if (argc != 2)
    {
        fprintf(stderr, "usage: a.out <integer value>\n");
        return -1;
    }
    if (atoi(argv[1]) < 0)
    {
        fprintf(stderr, "%d must be >= 0\n", atoi(argv[1]));
        return -1;
    }

    pthread_attr_init(&attr);
    pthread_create(&tid, &attr, runner, argv[1]);
    pthread_join(tid, NULL);

    printf("sum = %d\n", sum);
}

void *runner(void *param)
{
    int i, upper = atoi(param);
    sum = 0;

    for (i = 1; i <= upper; i++)
        sum += i;

    pthread_exit(0);
}
/*
 bratin  bratin03  ../MATERIALS/thread  gcc bin.c -pthread 

 bratin  bratin03  ../MATERIALS/thread  ./a.out 5
sum = 15
*/

#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>

int main()
{
    pthread_t thread_id;
    int ret;

    printf("Main thread starts.\n");

    // Create a new thread
    printf("Creating a new thread.\n");
    ret = pthread_create(&thread_id, NULL, (void *)main, NULL);
    if (ret != 0)
    {
        printf("Error: pthread_create() failed\n");
        exit(EXIT_FAILURE);
    }
    printf("New thread created.\n");

    // Wait for the thread to finish
    ret = pthread_join(thread_id, NULL);
    if (ret != 0)
    {
        printf("Error: pthread_join() failed\n");
        exit(EXIT_FAILURE);
    }

    printf("Main thread exits.\n");

    return 0;
}

// The output of the program is:
// It recursively creates new threads, and the program never exits.
