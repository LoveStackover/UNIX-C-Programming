#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <err.h>
#include <unistd.h>
#include <poll.h>
#include <signal.h>
void sig_usr(int signo)
{
        printf("catch a signal:SIGPIPE\n");
}

int main(void)
{
        int n, fd1[2], fd2[2];
        pid_t pid;
        char line[20];
        struct pollfd fdarray;
        if (signal(SIGPIPE, sig_usr) == SIG_ERR)
                errx(1,"signal(SIGPIPE) error");
        if (pipe(fd1) < 0 || pipe(fd2) < 0)
                errx(1,"pipe error");
        if ((pid = fork()) < 0)
        {
                errx(1,"fork error");
        }
        else if (pid > 0)
        {
                //关闭不需要的端口
                close(fd1[0]);
                close(fd2[1]);
                fdarray.fd=fd1[1];
                fdarray.events=POLLOUT;
                fdarray.revents=0;
                while(fgets(line, 20, stdin) != NULL)
                {
                        n = strlen(line);
                        if(poll(&fdarray,1,-1) != -1)
                                if (write(fd1[1], line, n) != n)
                                        errx(1,"write error to pipe");
                        if ((n = read(fd2[0], line, 20)) < 0)
                                errx(1,"read error from pipe");
                        if (n == 0)
                        {
                                errx(1,"child closed pipe");
                        }
                        line[n] = 0;
                        if (fputs(line, stdout) == EOF)
                                errx(1,"fputs error");
                }

                exit(0);
        }
        else
	{           /* child */
                close(fd1[1]);
                close(fd2[0]);
                if (fd1[0] != STDIN_FILENO)
                {
                        if (dup2(fd1[0], STDIN_FILENO) != STDIN_FILENO)
                                errx(1,"dup2 error to stdin");
                        close(fd1[0]);
                }
                if (fd2[1] != STDOUT_FILENO)
                {
                        if (dup2(fd2[1], STDOUT_FILENO) != STDOUT_FILENO)
                                errx(1,"dup2 error to stdout");
                        close(fd2[1]);
                }
                if (execl("./15_1_1", "15_1_1", (char *)0) < 0)
                        errx(1,"execl error");
        }
        exit(0);
}
