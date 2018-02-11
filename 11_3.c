#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <err.h>
typedef struct dhuang {
        int a;
        int b;
} Dhuang;
Dhuang var;
void * thr_fn1(void *arg1)
{
        Dhuang * arg=arg1;
        arg->a=1;
        arg->b=2;
        printf("thread 1 dhuang struct:%d,%d\n",arg->a,arg->b);
        return((void *)0);
}
void * thr_fn2(void *arg1)
{
        Dhuang * arg=arg1;
        printf("thread 2 dhuang struct:%d,%d\n",arg->a,arg->b);
        return((void *)0);
}
int main(void)
{
        int err;
        pthread_t tid1,tid2;
        err = pthread_create(&tid1, NULL, thr_fn1, (void *)&var);
        if (err != 0)
                errx(1, "can’t create thread 1");
        err = pthread_join(tid1, NULL);
        if (err != 0)
                errx(1, "can’t join with thread 1");
        err = pthread_create(&tid2, NULL, thr_fn2, (void *)&var);
        if (err != 0)
                errx(1, "can’t create thread 1");
        err = pthread_join(tid2, NULL);
        if (err != 0)
                errx(1, "can’t join with thread 2");
        exit(0);
}
