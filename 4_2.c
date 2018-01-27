#include <sys/types.h>
#include <sys/stat.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
#include <err.h>
#define handle_error(msg) \
	    do { perror(msg); exit(EXIT_FAILURE); } while (0)
struct stat buf;

int main(int argc,char ** argv)
{
	int fd;
	if(argc !=2)
		errx(1,"usage filename1");
	if(stat(argv[1],&buf) == -1)
		handle_error("stat");
	printf("link count before open: %d\n",buf.st_nlink);

	if(( fd = open(argv[1],O_RDWR)) < 0)
	        handle_error("open");
	if(stat(argv[1],&buf) == -1)
		handle_error("stat");
	printf("link count after open: %d\n",buf.st_nlink);

}
