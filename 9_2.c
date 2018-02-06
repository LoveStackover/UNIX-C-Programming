#include <unistd.h>
#include <stdio.h>
#include <err.h>
#include <stdlib.h>
#include <fcntl.h>
#include <termios.h>
#define handle_error(msg) \
	        do { perror(msg); exit(EXIT_FAILURE);} while (0)


int main( int agrc, char ** argv)
{
        pid_t pid[3];

        if((pid[0] = fork())<0)
                handle_error("fork");
        if(pid[0] >0)
        {
                exit(0);
        }
        else
        {
                int fd;

                if((fd=open("/dev/tty",O_RDWR,S_IRUSR|S_IWUSR|S_IRGRP|S_IROTH))<0)
                        handle_error("open");
                pid[0]=setsid();
                pid[1]=getpgid(getpid());
                pid[2]=getsid(getpid());
                if( pid[0] == pid[1]&& pid[1] == pid[2])
                {
                        printf("current process id :%d, current process group id:%d,current session id:%d\n",getpid(),pid[1],pid[2]);
                        pid[0]=tcgetpgrp(fd);
                        printf("current terminal's session leader's group id:%d\n",pid[0]);
                        while(1);
                }

        }
}

