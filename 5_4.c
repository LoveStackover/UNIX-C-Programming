#include<stdio.h>
#include<err.h>
#include<stdlib.h>
#include<unistd.h>
char buffer[3][4]={"abc"};
int main(void)
{
        FILE * fp;
        pid_t pid;
        if((pid=fork())<0)
        {
                errx(1,"error in forking\n");
        }
        if(pid >0) //parent
        {
                if((fp=fopen("state1.txt","r+"))==NULL)
                {
                           errx(1,"error in open a file\n");
                }
                if(fputs(buffer[0],fp) == EOF)
                {
                           errx(1,"output error");
                }
                //fflush(fp);//结果1是这条语句
                //rewind(fp);//结果2是这条语句
                if(fgets(buffer[1],4,fp) != NULL )
                {
                           printf("parent:%s\n",buffer[1]);
                }
                exit(0);
        }
        //child
        if((fp=fopen("state2.txt","r+"))==NULL)
        {
                errx(1,"error in open a file\n");
        }
        while(fgets(buffer[2],4,fp) != NULL);
        printf("child: %s \n",buffer[2]);
        if(fputs(buffer[0],fp) == EOF )
        {
                errx(1,"error in fputs a file\n");
        }
        exit(0);
}
