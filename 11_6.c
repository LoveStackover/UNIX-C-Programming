#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <err.h>
#include <unistd.h>
#include <stdbool.h>
#define NHASH 3
#define HASH(id) (((unsigned long)id)%NHASH)
struct foo
{
        int f_id;
        struct foo * f_next;
};

struct foo *fh[NHASH];
pthread_spinlock_t spin;
pthread_mutex_t hashr_mutex = PTHREAD_MUTEX_INITIALIZER;
pthread_cond_t hashr_cond = PTHREAD_COND_INITIALIZER;

void  foo_printf(struct foo * a)
{
        printf("thread %lu, foo_id:%d\n",pthread_self(),a->f_id);
}
struct foo * foo_alloc(int id)
{
        struct foo * fp;
        int idx;
        if ((fp = malloc(sizeof(struct foo))) != NULL)
        {
                fp->f_id = id;
                idx = HASH(id);
                pthread_spin_lock(&spin);
                fp->f_next = fh[idx];
                fh[idx] = fp;
                foo_printf(fp);
                pthread_spin_unlock(&spin);
        }
        return(fp);
}

struct foo * foo_find(int id)
{
        struct foo * fp;
        pthread_spin_lock(&spin);
        for (fp = fh[HASH(id)]; fp != NULL; fp = fp->f_next)
        {
                if (fp->f_id == id)
                {
                        break;
                }
        }
        pthread_spin_unlock(&spin);
        return(fp);
}

bool foo_rele(int id)
{
        struct foo * fp;
	struct foo * fp1;
        pthread_spin_lock(&spin);
        for (fp = fh[HASH(id)]; fp != NULL; fp = fp->f_next)
        {
                if (fp->f_id == id)
                {
                        if(fp == fh[HASH(id)])
                        {
                                fh[HASH(id)] = fp->f_next;
                        }
                        else
                        {
                            fp1=fh[HASH(id)];
                                while (fp1->f_next != fp)
                                        fp1 = fp1->f_next;
                                fp1->f_next = fp->f_next;
                        }
                        break;
                }
        }
        pthread_spin_unlock(&spin);
        if(fp == NULL)
                return 1;
        free(fp);
        return 0;
}



void * thr_fn(void *arg)
{
        if(foo_alloc((int)arg) == NULL)
        {
                errx(1,"foo_alloc error in thread %d\n",pthread_self());
        }
        else
        {
                pthread_mutex_lock(&hashr_mutex);
                pthread_cond_wait(&hashr_cond,&hashr_mutex);
                if(foo_rele((int)arg) != 0)
                {
                        errx(1,"error in release the foo of the id : %d",(int)arg);
                }
                pthread_mutex_unlock(&hashr_mutex);
                return((void *)0);
        }
}
bool not_in_done(int* n,int * m)
{
        int i;
        for(i=0;i<NHASH;i++)
        {
                if(*m != n[i])
                        continue;
                else
                        return 1;
        }
        return 0;
}

int main(void)
{
        int err;
        int num;
        struct foo * fp;
        int done[3]={4,4,4};
        pthread_t tid;
        char i=0;
        for( num=0;num<NHASH;num++)
        {
                err = pthread_create(&tid, NULL, thr_fn,(void *)(num*NHASH));
                if (err != 0)
                        errx(1, "can’t create thread:%d\n",num);
        }
        for(;;)
        {
                for(num=0;num<NHASH;num++)
                        if(not_in_done(done,&num) == 0)
                               if((fp = foo_find(num*NHASH)) != NULL)
                                {
                                        foo_printf(fp);
                                        done[i++]=num;
                                        if(i>=NHASH)
                                        {
                                                pthread_mutex_lock(&hashr_mutex);
                                                pthread_cond_broadcast(&hashr_cond);
                                                pthread_mutex_unlock(&hashr_mutex);
                                                exit(0);
                                        }
                                }
        }
}
