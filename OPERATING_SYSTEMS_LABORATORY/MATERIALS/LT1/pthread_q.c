#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>
void *thread(void *vargp)
{
    pthread_exit((void *)42);
}
int main()
{
    int i;
    pthread_t tid;
    pthread_create(&tid, NULL, thread, NULL);
    pthread_join(tid, (void **)&i);
    printf("%d\n", i);
}

/*
42
*/

#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>
void *thread(void *vargp)
{
    exit(42);
}
int main()
{
    int i;
    pthread_t tid;
    pthread_create(&tid, NULL, thread, NULL);
    pthread_join(tid, (void **)&i);
    printf("%d\n", i);
}

/*
*/

#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>
void *thread(void *vargp)
{
    int *ptr = (int *)vargp;
    pthread_exit((void *)*ptr);
}
void *thread2(void *vargp)
{
    int *ptr = (int *)vargp;
    *ptr = 0;
    pthread_exit((void *)31);
}
int main()
{
    int i = 42;
    pthread_t tid, tid2;
    pthread_create(&tid, NULL, thread, (void *)&i);
    pthread_create(&tid2, NULL, thread2, (void *)&i);
    pthread_join(tid, (void **)&i);
    pthread_join(tid2, NULL);
    printf("%d\n", i);
}

/*
Can print both 0 and 42
*/

#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>
void *thread(void *vargp)
{
    pthread_detach(pthread_self());
    pthread_exit((void *)42);
}
int main()
{
    int i = 0;
    pthread_t tid;
    pthread_create(&tid, NULL, thread, (void *)&i);
    int ret;
    sleep(1);
    ret = pthread_join(tid, (void **)&i);
    if (ret != 0)
    {
        printf("Ret: %d\n", ret);
    }
    printf("%d\n", i);
}
/*
Ret: 22
0
*/

#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>
int i = 42;
void *thread(void *vargp)
{
    printf("%d\n", i);
}
void *thread2(void *vargp)
{
    i = 31;
}
int main()
{
    pthread_t tid, tid2;
    pthread_create(&tid2, NULL, thread2, (void *)&i);
    pthread_create(&tid, NULL, thread, (void *)&i);
    pthread_join(tid, (void **)&i);
    pthread_join(tid2, NULL);
}

/*
31
*/