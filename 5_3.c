#include<stdio.h>
#include<unistd.h>
#include<err.h>
#include<stdlib.h>
int main(void)
{       
	pid_t pid;
	if((pid=fork())<0)
		errx(1,"erro in forking\n");
	if(pid>0)
	{    
		printf("parent");
		pause();
	}       
	else   
	{      
		printf("child");
		fgets(NULL,4,stdin);
	}      
}
