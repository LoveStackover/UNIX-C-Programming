#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
#define handle_error(msg) \
	do { perror(msg); exit(EXIT_FAILURE);} while (0)

int main( int agrc, char ** argv)
{
        pid_t pid[2];

        if((pid[0] = fork()) == -1)
                handle_error("fork");
        if(pid[0] >0 )
	{
                printf("parent process id :%d\n",getpid());
                if(setpgid(pid[0],0) != 0)
                        handle_error("setpgid");
        }
        else
        {
                if(setpgid(0,0) != 0)
                        handle_error("setpgid");
                printf("child process id :%d\n",getpid());
        }
        pid[0]=getpid();
        pid[1]=getpgid(getpid());
        printf("current process id :%d, current process group id:%d\n",pid[0],pid[1]);
        exit(0);
}
