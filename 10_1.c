#include<stdlib.h>
#include<stdio.h>
#include<fcntl.h>
#include<signal.h>
#include<unistd.h>
#include<err.h>
#include <sys/types.h>

void sig_usr(int signo)
{
}


int main(int argc,char **argv)
{
        pid_t pid;
        int fd;
        sigset_t newmask,oldmask;
        unsigned char number=0;
        if (signal(SIGUSR1, sig_usr) == SIG_ERR)
                errx(1,"signal(SIGUSR1) error");
        if (signal(SIGUSR2, sig_usr) == SIG_ERR)
                errx(1,"signal(SIGUSR2) error");
        sigemptyset(&newmask);
        sigaddset(&newmask, SIGUSR1);
        sigaddset(&newmask, SIGUSR2);
        if (sigprocmask(SIG_BLOCK, &newmask, &oldmask) < 0)
                errx(1,"SIG_BLOCK error");
        if((fd=open("state.txt",O_RDWR|O_CREAT|O_TRUNC,S_IRUSR|S_IWUSR|S_IRGRP|S_IROTH)) < 0)
                errx(1,"error in creating a file\n");
        else
        {
                if(write(fd,&number,1)< 0)
                        errx(1,"error in writing this file\n");
        }
        lseek(fd,0,SEEK_SET);
        if((pid=fork())<0)
                errx(1,"fork error\n");
        if(pid>0)
        {
                unsigned char temp;
                for(;;)
                {
                        sigsuspend(&oldmask);
                        lseek(fd,0,SEEK_SET);
                        if(read(fd,&temp,1) == -1 )
                        {
                                errx(1,"parent:error in reading a file\n");
                        }
                        else
                        {
                                lseek(fd,0,SEEK_SET);
                                if(++temp>200)
                                {
                                    if (sigprocmask(SIG_SETMASK, &oldmask, NULL) < 0)
                                        errx(1,"SIG_SETMASK error");
                                    errx(1,"parent: the number is too lager now\n");
                                }
                                if(write(fd,&temp,1) == -1)
                                        errx(1,"parent:error in writing a number to this file\n");
                                else
                                        printf("This number is incremented by the parent.It's value is:%d\n",temp);
                        }
                        kill(pid,SIGUSR2);
                }
         }
         else
         {
                unsigned char temp;
                for(;;)
                {
                        kill(getppid(),SIGUSR1);
                        sigsuspend(&oldmask);
                        lseek(fd,0,SEEK_SET);
                        if(read(fd,&temp,1) == -1)
                        {
                                errx(1,"child:error in reading a file\n");
                        }
                        else
                        {
                                lseek(fd,0,SEEK_SET);
                                if(++temp>200)
                                {
                                        if (sigprocmask(SIG_SETMASK, &oldmask, NULL) < 0)
                                                errx(1,"SIG_SETMASK error");
                                        errx(1,"child: the number is too lager now\n");
                                }
                                if(write(fd,&temp,1) == -1)
                                        errx(1,"child:error in writing a number to this file\n");
                                else
                                        printf("This number is incremented by the child.It's value is:%d\n",temp);
                        }
                }
         }
}
