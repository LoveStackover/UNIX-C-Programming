#include<stdio.h>
#include<err.h>
#include<stdlib.h>
#include<signal.h>
#include <errno.h>
void sig_int(int);
unsigned char temp=0;
int main(void)
{
        char buf[4];
        signal(SIGINT,sig_int);
        while (fgets(buf, 4, stdin) != NULL)
        {       if (fputs(buf, stdout) == EOF)
                        errx(1,"output error");
                temp++;
        }
      	if(feof(stdin))
		printf("fgets end of file\n");
      	exit(-1);
}
void sig_int(int signo)
{
        fprintf(stderr,"Loop times:%d",temp);
        signal(SIGINT,SIG_DFL);
}
