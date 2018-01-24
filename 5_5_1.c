#include<stdio.h>
#include<unistd.h>
#include<err.h>
#include<stdlib.h>
int main(int agrc,char ** agrv)
{
        FILE * fp;
        if((fp=fopen("state3.txt","r+"))==NULL)
        {
             errx(1,"error in open a file\n");
        }
        fprintf(fp,"hello");
        fclose(fp);
        if(execlp("od","od","-Ax","-tcx1","state3.txt",(char * )0)<0)
        {
             errx(1,"error in exec\n");
        }
        exit(0);
}
