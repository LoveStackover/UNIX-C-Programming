#include <unistd.h>
#include <stdio.h>
#include <err.h>
#include <stdlib.h>
#define handle_error(msg) \
                do { perror(msg); exit(EXIT_FAILURE);} while (0)

int main( int agrc, char ** argv)
{
        pid_t pid;

        if((pid = fork())<0)
                handle_error("fork");
        if(pid == 0)
        {
                exit(0);
        }
        else
        {
                if(system("ps axo pid,ppid,pgid,sid,comm ") == -1)
                        handle_error("system");
        }
}
