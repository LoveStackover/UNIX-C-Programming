#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <err.h>
#include <unistd.h>
#include <sys/msg.h>
typedef struct {
long mtype;
char mtext[20];
} Mymsg;



int main(void)
{
        int ident;
        pid_t pid;
        Mymsg msg;
        char id[32];
        if((ident=msgget(IPC_PRIVATE,0770)) == -1 )
                errx(1,"error in msgget\n");
        sprintf(id,"%d",ident);
        if ((pid = fork()) < 0)
        {
                errx(1,"fork error");
        }
        else if (pid > 0)
        {

                while (fgets(msg.mtext, 20, stdin) != NULL)
                {
                        msg.mtype=pid;
                        if (msgsnd(ident,&msg,20,0) == -1 )
                                errx(1,"error in msgsnd");
                        if (msgrcv(ident,&msg,20,getpid(),0) ==  -1)
                                errx(1,"error in msgrcv");
                        if (fputs(msg.mtext, stdout) == EOF)
                                errx(1,"fputs error");
                }
                if(msgctl(ident,IPC_RMID,NULL) == -1)
                        errx(1,"error in msgctl");
                exit(0);
        }
        else
        { /* child */
                if (execl("./15_5_1", "15_5_1",id,(char *)0) < 0)
                        errx(1,"execl error");
        }
        exit(0);
}
