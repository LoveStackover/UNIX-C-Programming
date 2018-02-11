#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <err.h>
void cleanup(void *arg)
{
        printf("cleanup: %s\n", (char *)arg);
}
void * thr_fn1(void *arg)
{
        pthread_cleanup_push(cleanup, "thread 1 first handler");
        pthread_cleanup_push(cleanup, "thread 1 second handler");
        printf("thread 1 push complete\n");
        pthread_cleanup_pop(0);
        pthread_cleanup_pop(0);
        pthread_exit((void *)0);
}
int main(void)
{
        int err;
        pthread_t tid1;
        err = pthread_create(&tid1, NULL, thr_fn1, NULL);
        if (err != 0)
                errx(1, "can’t create thread 1");
        err = pthread_join(tid1, NULL);
        if (err != 0)
                errx(1, "can’t join with thread 2");
        exit(0);
}
