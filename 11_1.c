#include <stdio.h>
#include <err.h>
#include <stdlib.h>
#include <pthread.h>
#include <unistd.h>
#define NUM 6
pthread_t ntid[NUM];
void * thr_fn(void *arg)
{
        printf("new thread:%d\n",(int)arg);
        return((void *)0);
}
int main(void)
{
        int err;
        int num;
        for( num=0;num<NUM;num++)
        {       err = pthread_create(ntid+num, NULL, thr_fn,(void *)num);
                if (err != 0)
                        errx(1, "can’t create thread:%d\n",num);
        }
        sleep(10);//睡眠足够长来保证所有子线程都能运行结束
        exit(0);
}
