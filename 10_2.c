#include<stdlib.h>
#include<stdio.h>
#include<fcntl.h>
#include<signal.h>
#include<string.h>
#include<unistd.h>
#include<err.h>
#define  MAX  1073741824

void sig_alrm(int signo)
{
          printf("the signal we got is:%d\n",signo);
}


int main(int argc,char **argv)
{
          pid_t pid;
          FILE * fp;
          char * buffer;
          if((buffer=malloc(MAX)) == NULL)
          {
                  errx(1,"error calling the malloc\n");
          }
          memset(buffer,'1',MAX);
          if (signal(SIGALRM, sig_alrm) == SIG_ERR)
                  errx(1,"signal(SIGUSR1) error");
          if((fp=fopen("state.txt","w+")) == NULL)
                  errx(1,"error in creating a file\n");
          else
          {
                  alarm(1);
                  if(fwrite(buffer,sizeof(char),MAX,fp)<MAX)
                        errx(1,"error in writing this file\n");
                  else
                        printf("fwirte complete!\n");
                  alarm(0);
          }
}
