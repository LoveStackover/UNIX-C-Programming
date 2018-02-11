#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <err.h>
#include <fcntl.h>
#include <unistd.h>
#include <string.h>
void prepare(void)
{
        printf("preparing locks...\n");
}
void * thr_fn(void *arg)
{
        printf("thread started...\n");
        fflush(stdout);//这里专门针对DeBUG
        pause();
        return(0);
}
int main(void)
{
        int err;
        pid_t pid;
        pthread_t tid;
        int fd;
        char * buffer="hi, it's a flag to control flow\n";
        //可以使用多种手段重定向标准输入到文件
        close(1);
        if((fd=open("data.txt",O_RDWR|O_CREAT|O_TRUNC,S_IRUSR|S_IWUSR|S_IRGRP|S_IROTH)) < 0)
                errx(1,"error in creating a file\n");
        //这里已经将pthread_atfork本来的目的忽略了！
        if ((err = pthread_atfork(prepare, NULL, NULL)) != 0)
                errx(1, "can’t install fork handlers");
        if ((err = pthread_create(&tid, NULL, thr_fn, 0)) != 0)
                errx(1, "can’t create thread");
        sleep(2);
        //这里将会更直观的看到一些区别
        write(STDOUT_FILENO,buffer,strlen(buffer));
        printf("parent about to fork....\n");
        if ((pid = fork()) < 0)
                errx(1,"fork failed");
        else if (pid == 0)
                printf("child returned from fork\n");
        else
                printf("parent returned from fork\n");
        exit(0);
}
