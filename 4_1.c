#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
#include <err.h>
#define NUMBER 1
#define handle_error(msg) \
	    do { perror(msg); exit(EXIT_FAILURE); } while (0)

int main(int argc,char ** argv)
{
	int num,fd[2];
	char buf;
	if(argc !=3)
		errx(1,"usage filename1 filename2");
	if(( fd[0] = open(argv[1],O_RDWR)) < 0)
	        handle_error("open");
        if(( fd[1] = open(argv[2],O_RDWR|O_CREAT|O_TRUNC,S_IRUSR|S_IWUSR|S_IRGRP|S_IROTH)) < 0)
		handle_error("open");
	while((num = read(fd[0],&buf,NUMBER)) > 0)                                           
	{                                                          
		if(buf == 0)
		{	
			if (lseek(fd[1],1, SEEK_CUR) == -1)
				handle_error("lseek");
		}
		else 
			if(write(fd[1],&buf,num) == -1)
				handle_error("write");
	}		     
	if(num == 0)
	{
		handle_error("read end of file");                     
	}				        
        else
		handle_error("read");                                               

}
