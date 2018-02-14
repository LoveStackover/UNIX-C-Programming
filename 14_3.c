#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <err.h>
#include <unistd.h>
#include <poll.h>
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
        struct pollfd fdarray;
        fdarray.fd=STDIN_FILENO;
        fdarray.events=POLLIN;
        fdarray.revents=0;
        set_fl(STDIN_FILENO, O_NONBLOCK);
        set_fl(STDOUT_FILENO, O_NONBLOCK);
        while(poll(&fdarray,1,-1) != -1)
                if(fgets(buf,MAX,stdin) != NULL)
                {
                        if(fputs(buf,stdout) == EOF)
                                errx(1,"error in write\n");
                }
                else
                        errx(1,"end of file\n");
        exit(0);
}
