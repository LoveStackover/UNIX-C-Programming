#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <err.h>
#include <fcntl.h>
#include <sys/mman.h>
#include <sys/sem.h>
#include <stdlib.h>
#define NLOOPS 20
#define SIZE sizeof(long) /* size of shared memory area */
union semun {
               int              val;    /* Value for SETVAL */
               struct semid_ds *buf;    /* Buffer for IPC_STAT, IPC_SET */
               unsigned short  *array;  /* Array for GETALL, SETALL */
               struct seminfo  *__buf;  /* Buffer for IPC_INFO
                                           (Linux-specific) */
            };
int ident;
void TELL_WAIT(void)
{
    union semun sem;
    sem.val=0;
    //进行锁的初始化,两个Sem 0,1
    if((ident=semget(IPC_PRIVATE,2,0770)) == -1 )
            errx(1,"error in msgget\n");
    if(semctl(ident,0,SETVAL,sem) == -1 )
            errx(1,"error in semctl\n");
    if(semctl(ident,1,SETVAL,sem) == -1 )
            errx(1,"error in semctl\n");
}

void TELL_PARENT(void)
{
    //sem 0 对应父 1 对应子
    struct sembuf buf;
    buf.sem_num=1;
    buf.sem_op=1;
    buf.sem_flg=SEM_UNDO;
    //子进程释放资源
    if(semop(ident,&buf,1) == -1 )
            errx(1,"error in semop\n");

}

void WAIT_PARENT(void)
{
    //如果TELL_CHILD
    struct sembuf buf;
    buf.sem_num=0;
    buf.sem_op=-1;
    buf.sem_flg=SEM_UNDO;
    //等待TELL_CHILD执行
    if(semop(ident,&buf,1) == -1 )
           errx(1,"error in semop\n");
}
void TELL_CHILD(void)
{
    struct sembuf buf;
    buf.sem_num=0;
    buf.sem_op=1;
    buf.sem_flg=SEM_UNDO;
    //父进程释放资源
    if(semop(ident,&buf,1) == -1 )
            errx(1,"error in semop\n");

}
void WAIT_CHILD(void)
{
    struct sembuf buf;
    buf.sem_num=1;
    buf.sem_op=-1;
    buf.sem_flg=SEM_UNDO;
    //等待TELL_PARENT执行
    if(semop(ident,&buf,1) == -1 )
           errx(1,"error in semop\n");
}

static int update(long *ptr)
{
    return((*ptr)++); /* return value before increment */
}
int main(void)
{
    int fd, i, counter,ident;
    pid_t pid;
    void *area;
    if ((fd = open("/dev/zero", O_RDWR)) < 0)
          errx(1,"open error");
    if ((area = mmap(0, SIZE, PROT_READ | PROT_WRITE, MAP_SHARED,fd, 0)) == MAP_FAILED)
          errx(1,"mmap error");
    close(fd); /* can close /dev/zero now that it’s mapped */
    TELL_WAIT();
    if ((pid = fork()) < 0)
    {
          errx(1,"fork error");
    }
    else if (pid > 0)
    { /* parent */
          for (i = 0; i < NLOOPS; i += 2)
          {
                if ((counter = update((long *)area)) != i)
                    errx(1,"parent: expected %d, got %d", i, counter);
                printf("parent:%d\n",counter);
                TELL_CHILD();
                WAIT_CHILD();
          }
    }
    else
    { /* child */
          for (i = 1; i < NLOOPS + 1; i += 2)
          {
                WAIT_PARENT();
                if ((counter = update((long *)area)) != i)
                    errx(1,"child: expected %d, got %d", i, counter);
                printf("child :%d\n",counter);
                TELL_PARENT();
          }
    }
    exit(0);
}
