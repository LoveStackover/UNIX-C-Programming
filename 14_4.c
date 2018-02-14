#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <err.h>
#include <unistd.h>
#include <sys/epoll.h>
#define MAX 20


void set_fl(int fd, int flags)
{
        int val;
        if ((val = fcntl(fd, F_GETFL, 0)) < 0)
                errx(1,"fcntl F_GETFL error");
        val |= flags;
        if (fcntl(fd, F_SETFL, val) < 0)
                errx(1,"fcntl F_SETFL error");
}

int main(void)
{
        int n;
        char buf[MAX];
        int efd,m;
        struct epoll_event even,events[2];
        even.events=EPOLLIN|EPOLLOUT;
        even.data.fd=STDIN_FILENO;
        set_fl(STDOUT_FILENO, O_NONBLOCK);
        set_fl(STDIN_FILENO, O_NONBLOCK);
        if((efd=epoll_create(2)) == -1)
                errx(1,"error in epoll_create\n");

        if(epoll_ctl(efd,EPOLL_CTL_ADD,STDIN_FILENO,&even) == -1)
                errx(1,"error in epoll_ctl\n");
        while((m=epoll_wait(efd,events,2,-1)) != -1)
                for(int i=0;i<m;i++)
                        if(events[i].events & EPOLLIN == EPOLLIN)
                                if(events[i].data.fd == STDIN_FILENO)
                                        if(fgets(buf,MAX,stdin) != NULL)
                                        {
                                                if(fputs(buf,stdout) == EOF)
                                                        errx(1,"error in write\n");
                                        }
                                        else
                                                errx(1,"end of file\n");

        exit(0);
}
