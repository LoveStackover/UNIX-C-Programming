#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <err.h>
#include <string.h>
#include <sys/msg.h>
typedef struct {
long mtype;
char mtext[20];
} Mymsg;

int main(int argc,char **argv)
{
        int n, i;
        char line[20];
        int ident=atoi(argv[1]);
        Mymsg msg;
        while((n= msgrcv(ident,&msg,20,getpid(),0)) !=  -1)
        {
                n=strlen(msg.mtext);
                for(i=0;i<n-1;i++)
                        msg.mtext[i]+=1;
                i=0;
                msg.mtype=getppid();
                if (msgsnd(ident,&msg,20,0) == -1 )
                        errx(1,"error in msgsnd");

        }
        printf("error in child\n");
        exit(0);
}
