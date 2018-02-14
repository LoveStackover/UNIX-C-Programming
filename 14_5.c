#include <stdlib.h>
#include <unistd.h>
#include <stdio.h>
#include <err.h>
#include <aio.h>
#include <signal.h>
#include <string.h>
#define BUF_SIZE 20
#define IO_SIGNAL SIGUSR1

void  thread_aio_write(union sigval arg)
{
        int s;
        struct aiocb * aiocb_write;
        aiocb_write = arg.sival_ptr;
        //为了辨识是否真的完成了aio_read
        printf("receive data and write data:\n");
        s = aio_write(aiocb_write);
        if (s == -1)
            errx(1,"aio_read\n");
}


void aioinit(struct aiocb * aiocbList)
{

        aiocbList[0].aio_fildes = STDIN_FILENO;
        aiocbList[0].aio_buf = malloc(BUF_SIZE);
        if (aiocbList[0].aio_buf == NULL)
                errx(1,"malloc ERROR\n");
        aiocbList[0].aio_nbytes = BUF_SIZE;
        aiocbList[0].aio_reqprio = 0;
        aiocbList[0].aio_offset = 0;
        //读操作完成的时候创建新线程
        aiocbList[0].aio_sigevent.sigev_notify = SIGEV_THREAD;
        aiocbList[0].aio_sigevent.sigev_notify_attributes = NULL;
        aiocbList[0].aio_sigevent.sigev_notify_function =thread_aio_write;
        //将写标准输入相关的aiocb地址传递给新线程所调用的函数
        aiocbList[0].aio_sigevent.sigev_value.sival_ptr =&aiocbList[1];

        aiocbList[1].aio_fildes = STDOUT_FILENO;
        //写和读共享一个buffer
        aiocbList[1].aio_buf = aiocbList[0].aio_buf;
        aiocbList[1].aio_nbytes = BUF_SIZE;
        aiocbList[1].aio_reqprio = 0;
        aiocbList[1].aio_offset = 0;
        //使用signal作为写aiocb的通知方式
        aiocbList[1].aio_sigevent.sigev_notify = SIGEV_SIGNAL;
        aiocbList[1].aio_sigevent.sigev_signo = IO_SIGNAL;
        aiocbList[1].aio_sigevent.sigev_value.sival_ptr =&aiocbList[1];
}

static void aioSigHandler(int sig, siginfo_t *si, void *ucontext)
{
        //每次写完都清空buffer！很重要
        memset(((struct aiocb * )(si->si_value.sival_ptr))->aio_buf,0,BUF_SIZE);
}


int main(int argc, char *argv[])
{
        struct aiocb *aiocbList;
        int s;
        struct sigaction sa;
        sa.sa_flags = SA_RESTART | SA_SIGINFO;
        sa.sa_sigaction = aioSigHandler;
        if (sigaction(IO_SIGNAL, &sa, NULL) == -1)
                errx(1,"sigaction\n");
        aiocbList = calloc(2, sizeof(struct aiocb));
        if (aiocbList == NULL)
                errx(1,"calloc ERROR\n");
        aioinit(aiocbList);
        //笔者只是简单的循环调用aio_read
        while(aio_read(&aiocbList[0]) != -1 );

        exit(-1);
} 
