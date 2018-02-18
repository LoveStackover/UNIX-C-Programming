#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <err.h>
#include <fcntl.h>
#include <sys/mman.h>
#include <semaphore.h>
#include <stdlib.h>
#define NLOOPS 20
#define SIZE  sizeof(long)
#define SIZE2 sizeof(sem_t) /* size of shared memory area */
sem_t * parent_ident;
sem_t * child_ident;
void TELL_WAIT(void)
{
     int fd;
     if ((fd = open("/dev/zero", O_RDWR)) < 0)
             errx(1,"open error");
     if ((parent_ident = mmap(0, SIZE2, PROT_READ | PROT_WRITE, MAP_SHARED,fd, 0)) == MAP_FAILED)
   	     errx(1,"mmap error");
     /* can close /dev/zero now that itmapped */
     if ((child_ident = mmap(0, SIZE2, PROT_READ | PROT_WRITE, MAP_SHARED,fd, 0)) == MAP_FAILED)
             errx(1,"mmap error");
     close(fd); /* can close /dev/zero now that itmapped */
     if(sem_init(parent_ident,1,0) == -1 )
             errx(1,"error in sem_open\n");
     if(sem_init(child_ident,1,0) == -1 )
             errx(1,"error in sem_open\n");
}
void TELL_PARENT(void)
{
    if(sem_post(child_ident) == -1 )
	   errx(1,"error in sem_post\n");
}

void WAIT_PARENT(void)
{
    if(sem_wait(parent_ident) == -1 )
           errx(1,"error in sem_wait\n");
}
void TELL_CHILD(void)
{
    if(sem_post(parent_ident) == -1 )
	   errx(1,"error in sem_post\n");
}
void WAIT_CHILD(void)
{
    if(sem_wait(child_ident) == -1 )
	   errx(1,"error in sem_wait\n");
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
