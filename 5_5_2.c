#include<stdio.h>
#include<err.h>
#include<string.h>
#define BSZ 48
int main(int agrc,char ** agrv)
{
        FILE * fp;
        char buf[BSZ];
        if ((fp = fmemopen(buf, BSZ, "w+")) == NULL)
                errx(1,"fmemopen failed");
        memset(buf,'c', BSZ-2);
        buf[BSZ-2] = '\0';
        buf[BSZ-1] = 'X';
        fprintf(fp, "hello, world");
        fclose(fp);
        printf("after fclose: %s\n", buf);
        printf("len of string in buf = %ld\n", (long)strlen(buf));
        return(0);
}
