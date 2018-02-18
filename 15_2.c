#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <err.h>
#include <unistd.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <signal.h>


void sig_usr(int signo)
{
    printf("catch a signal:SIGPIPE\n");
}

int main(void)
{
    int n, fd[2];
    pid_t pid;
    char line[20];
    if (signal(SIGPIPE, sig_usr) == SIG_ERR)
        errx(1,"signal(SIGPIPE) error");
    if(mkfifoat(AT_FDCWD,"fifo1",S_IRUSR|S_IWUSR|S_IRGRP|S_IROTH) < 0 )
        errx(1,"error in mkfifo fifo1\n");
    if(mkfifoat(AT_FDCWD,"fifo2",S_IRUSR|S_IWUSR|S_IRGRP|S_IROTH) < 0 )
        errx(1,"error in mkfifo fifo2\n");
    if ((pid = fork()) < 0)
    {
        errx(1,"fork error\n");
    }
    else if (pid > 0)
    {
        if((fd[0]=openat(AT_FDCWD,"fifo1",O_WRONLY,S_IRUSR|S_IWUSR|S_IRGRP|S_IROTH)) < 0)
            errx(1,"open error\n");
        if((fd[1]=openat(AT_FDCWD,"fifo2",O_RDONLY,S_IRUSR|S_IWUSR|S_IRGRP|S_IROTH)) < 0)
            errx(1,"open error\n");
        while (fgets(line, 20, stdin) != NULL)
        {
            n = strlen(line);
            if (write(fd[0], line, n) != n)
                errx(1,"write error to pipe");
            if ((n = read(fd[1], line, 20)) < 0)
                errx(1,"read error from pipe");
            if (n == 0)
            {
                printf("child closed fifo\n");
                break;
            }
            line[n] = 0;
            if (fputs(line, stdout) == EOF)
                errx(1,"fputs error");
        }
        if (ferror(stdin))
            errx(1,"fgets error on stdin");
        exit(0);
    }
    else
    {
        if((fd[0]=open("fifo1",O_RDONLY,S_IRUSR|S_IWUSR|S_IRGRP|S_IROTH)) < 0)
            errx(1,"open error\n");
        if((fd[1]=open("fifo2",O_WRONLY,S_IRUSR|S_IWUSR|S_IRGRP|S_IROTH)) < 0)
            errx(1,"open error\n");
        if (fd[0] != STDIN_FILENO)
        {
            if (dup2(fd[0], STDIN_FILENO) != STDIN_FILENO)
                errx(1,"dup2 error to stdin");
            close(fd[0]);
        }
        if (fd[1] != STDOUT_FILENO)
        {
            if (dup2(fd[1], STDOUT_FILENO) != STDOUT_FILENO)
                errx(1,"dup2 error to stdout");
            close(fd[1]);
        }
        if (execl("./15_1_1", "15_1_1", (char *)0) < 0)
            errx(1,"execl error");
    }
    exit(0);
}
