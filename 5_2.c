#include<stdio.h>
#include<err.h>
#include<stdlib.h>
#include<time.h>
#include<unistd.h>
int main(void)
{
        char buf[4];
        FILE * fp;
        int time1=clock();
        if((fp=fopen("data1.txt","r")) == NULL)
                errx(1,"error in open a file\n");
        while (fgets(buf, 4, fp) != NULL)
        {
                if (fputs(buf, stdout) == EOF)
                        errx(1,"output error");
        //      fsync(STDOUT_FILENO);
        }
        int time2=clock();
        printf("run time:%d\n",time2-time1);
}
