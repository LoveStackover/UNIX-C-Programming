#include<unistd.h>
#include<stdio.h>
#include<fcntl.h>
#include<err.h>
#include<stdlib.h>

#define handle_error(msg) \
    do { perror(msg); exit(EXIT_FAILURE); } while (0)

int main(int argc,char ** argv)
{
        int fd;
        off_t offset;
        char buffer[20]="lovestackover\n";
        if(argc != 2)
        {
                errx(1,"error in argc,it should be two.\n");
        }
        if((fd = open(argv[1],O_RDWR)) < 0)
                handle_error("open");
        else
        {
                if((offset = lseek(fd,0,SEEK_END)) == -1)
                        handle_error("lseek");
                else
                {
                        pwrite(fd,buffer,20,offset);
                        printf("%d\n",offset);
                }
                if((offset = lseek(fd,0,SEEK_CUR)) == -1)
                {
                        handle_error("lseek");
                }
                else
                        printf("%d\n",offset);
        }
        return 0;

}
