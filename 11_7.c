#include <stdio.h>
#include <err.h>
#include <signal.h>
#include <unistd.h>
#include <pthread.h>
#include <stdlib.h>
struct job
{
        struct job * j_next;
        struct job * j_prev;
        pthread_t j_id;
        int num;
};
struct queue {
        struct job * q_head;
        struct job * q_tail;
        pthread_rwlock_t q_lock;//两个rwlock
        pthread_rwlock_t q_lock2;
        pthread_cond_t qp_cond;//condition variable
        pthread_mutex_t qp_mutex;
};

#define NUMNUM  3 //3个子线程
struct queue queue_list;

int queue_init(struct queue *qp)
{
        qp->q_head = NULL;
        qp->q_tail = NULL;
        if(pthread_rwlock_init(&qp->q_lock, NULL) != 0)
                return -1;
        if(pthread_rwlock_init(&qp->q_lock2, NULL) != 0)
                return -1;
        if(pthread_mutex_init(&qp->qp_mutex,NULL) != 0)
                return -1;
        if(pthread_cond_init(&qp->qp_cond,NULL) != 0)
                return -1;
        return(0);
}

//在队列头插入一个节点
void job_insert(struct queue *qp, struct job *jp)
{
        pthread_rwlock_wrlock(&qp->q_lock);
        jp->j_next = qp->q_head;
        jp->j_prev = NULL;
        if (qp->q_head != NULL)
                qp->q_head->j_prev = jp;
        else
                qp->q_tail = jp;
                qp->q_head = jp;
        pthread_rwlock_unlock(&qp->q_lock);
}
//移除指定的节点
void job_remove(struct queue *qp, struct job *jp)
{
        pthread_rwlock_wrlock(&qp->q_lock);
        if (jp == qp->q_head)
        {
                qp->q_head = jp->j_next;
                if (qp->q_tail == jp)
                        qp->q_tail = NULL;
                else
                        jp->j_next->j_prev = jp->j_prev;
        } else if (jp == qp->q_tail)
        {
                qp->q_tail = jp->j_prev;
                jp->j_prev->j_next = jp->j_next;
        } else
        {
                jp->j_prev->j_next = jp->j_next;
                jp->j_next->j_prev = jp->j_prev;
        }
        pthread_rwlock_unlock(&qp->q_lock);
}

//根据给定的线程ID寻找一个对应的节点（队列最前端）
struct job * job_find(struct queue *qp, pthread_t id)
{
        struct job * jp;
        pthread_rwlock_rdlock(&qp->q_lock);
        for (jp = qp->q_head; jp != NULL; jp = jp->j_next)
                if (pthread_equal(jp->j_id, id))
                        break;
        pthread_rwlock_unlock(&qp->q_lock);
        return(jp);
}
//每个子线程处理queue的函数
int job_settle(struct queue *qp, pthread_t id)
{
        struct job * jp;
        pthread_rwlock_rdlock(&qp->q_lock2);//避免了如上所述的竞态
        if((jp = job_find(qp,id)) == NULL)
        {
                return -1;
        }
        job_remove(qp,jp);
        pthread_rwlock_unlock(&qp->q_lock2);
        printf("crrent thread id:%lu, and crrent job num:%d. we have done it.\n",pthread_self(),jp->num);
        free(jp);
        return 0;

}

void job_modify(struct queue *qp)
{
        struct job * jp;
        for (jp = qp->q_head; jp->j_next != NULL; jp = jp->j_next);
        pthread_rwlock_wrlock(&qp->q_lock2);
        jp->j_id=5;//此时将最后一个job替换成不会被检索的job id,如果程序运行正常，将少打印一个job
        pthread_rwlock_unlock(&qp->q_lock2);
}

struct job * job_create(pthread_t id,int num)
{
        struct job * jp;
        if ((jp = malloc(sizeof(struct job))) != NULL)
        {
                jp->j_id = id;
                jp->num = num;
        }
        return(jp);
}

void * thr_fn(void *arg)
{
        char i;
        struct job * jp;
        for(;;)
        {
                pthread_mutex_lock(&(&queue_list)->qp_mutex);
                pthread_cond_wait(&(&queue_list)->qp_cond,&(&queue_list)->qp_mutex);
                if(job_settle(&queue_list,pthread_self()) != 0)
                {
                        printf("there is no  job for thread id:%lu \n",pthread_self());

                }
                pthread_mutex_unlock(&(&queue_list)->qp_mutex);
        }
}
//每5秒钟由主线程通知子线程处理queue
void sig_usr(int signo)
{
        pthread_cond_broadcast(&(&queue_list)->qp_cond);
        alarm(5);
}


int main(int argc,char **argv )
{
        char i,j;
        pthread_t tid[NUMNUM];
        sigset_t newmask,oldmask;
        struct job * jp;
	if(queue_init(&queue_list) == -1 )
		errx(1,"queue_init error");
        //信号的屏蔽和处理
        if (signal(SIGALRM, sig_usr) == SIG_ERR)
                errx(1,"signal(SIGALRM) error");

        //线程的创建,3个线程(不含主线程)
        for (i = 0; i < NUMNUM; i++)
        {
                if(pthread_create(tid+i, NULL, thr_fn, NULL) != 0)
                        errx(1, "can’t create thread");
        }
        //工作的创建,9个工作，每个线程3个工作
        printf("initial job arrangement:\n");
        for (i=0,j = 0; i < NUMNUM*3; i++)
        {
                if(j>=3)
                        j=0;
                if((jp=job_create(tid[j],i)) != NULL)
                {
                        printf("job num %d for thread id %lu\n",i,tid[j++]);
                        job_insert(&queue_list,jp);
                }
                else
                        printf("error in job_create\n");
        }


        //每5秒通知子线程状态的改变
        alarm(5);
        sleep(6);
        //在6秒修改最后一个成员的信息
        job_modify(&queue_list);
        for(;;)
        {
        }
}
