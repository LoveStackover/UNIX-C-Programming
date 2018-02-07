#include <stdio.h>
#include <stdlib.h>
#include <signal.h>
#include <sys/resource.h>
#include <unistd.h>
#include <err.h>
#define BUFFSIZE 100
#define MAX 1024
typedef void Sigfunc(int);
Sigfunc * signal_intr(int signo, Sigfunc *func)
{
        struct sigaction act, oact;
        act.sa_handler = func;
        sigemptyset(&act.sa_mask);
        act.sa_flags = 0;
        act.sa_flags |= SA_INTERRUPT;
        if (sigaction(signo, &act, &oact)<0)
                return(SIG_ERR);
        return(oact.sa_handler);
}
void sig_fsz(int signo)
{
        fprintf(stderr,"the signal we got is:%d\n",signo);
}
int main(void)
{
        int n,m;
        char buf[BUFFSIZE];
        struct rlimit new={MAX,MAX};
        if(setrlimit(RLIMIT_FSIZE, &new)== -1)
                errx(1,"error in setting limit\n");
        if(getrlimit(RLIMIT_FSIZE,&new)== -1)
                errx(1,"error in getting limit\n");
        else
                fprintf(stderr,"limit is :%d\n",new.rlim_cur);
        if (signal_intr(SIGXFSZ, sig_fsz) == SIG_ERR)
                errx(1,"signal(SIGUSR1) error");
        while ((n = read(STDIN_FILENO, buf, BUFFSIZE)) > 0)
        {
                fprintf(stderr,"read number:%d\n",n);
                if ((m= write(STDOUT_FILENO, buf, n)) != n)
                {
                        if(m == -1)
                                errx(1,"error in writing");
                        else
                                fprintf(stderr,"write number:%d\n",m);

                }
        }
        if (n < 0)
                errx(1,"read error");
        exit(0);
}  
