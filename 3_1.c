#include <fcntl.h>
#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>


#define handle_error(msg) \
    do { perror(msg); exit(EXIT_FAILURE); } while (0)

char buffer[20];

int main(int argc, int ** argv)
{
        int fd[2];
        pid_t pid;
        int charnum;
        if((fd[1]=open("data.txt",O_RDWR|O_CREAT|O_TRUNC,S_IRUSR|S_IWUSR|S_IRGRP|S_IROTH)) < 0)
                handle_error("open");
                
        if((pid = fork()) < 0 )
                handle_error("fork");
        if(pid > 0 )
        {
                close(1);
                dup(fd[1]);
                if((fd[2] = open("input.txt", O_RDONLY)) < 0)
                        handle_error("open");
                dup2(fd[2],STDIN_FILENO);
                for(;;)
                {
                        if((charnum = read(STDIN_FILENO, buffer,20)) == -1 )
                        {
                                handle_error("parent_read");
                        }
                        else if(charnum == 0)
                        {
                               	handle_error("parent_read_end_of_file");
                        }
                        else
                        {
                                if(write(STDOUT_FILENO,buffer,charnum) == -1)
                                        handle_error("parent_write");
                        }
                }
                exit(0);
        }
        else
        {
                close(0);
                dup(fd[1]);
                for(;;)
                {
                        if((charnum = read(STDIN_FILENO,buffer,20)) == -1)
                        {
                                handle_error("child_read");
                        }
                        else if(charnum == 0)
                        {
                                handle_error("child_read_end_of_file");
                        }
                        else
                                printf("%s",buffer);

                }

                exit(0);                                                                                                                                                                                               
        }                                                                                                                                                                                                              
        return 0;                                                                                                                                                                                                      
}    
