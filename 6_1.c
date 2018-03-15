#include <stdio.h>
#include <time.h>
#include <stdlib.h>
#include <err.h>
#include <unistd.h>
#include <string.h>
int main(int argc,char ** argv)
{
    time_t t;
    int c;
    struct tm * tmp[2];
    char buf2[64];
    char str[64]="TZ=";
    char * cp=NULL;
    time(&t);
    while ((c = getopt(argc, argv, "t:")) != -1)
    {
	    switch(c)
	    {
		    case 't':
			cp=optarg;
			break;
		    case '?':
			exit(-1);
			
	    }


    }
    if(cp != NULL)
    {
    	strcat(str,cp);
    	if(putenv(str) != 0)
    	{
		perror("putenv:");
		exit(-1);
	}
    }
    printf("TZ:%s\n",cp);
    tmp[1]= gmtime(&t);
    tmp[0]= localtime(&t);
    if (strftime(buf2, 64, "%a %b %d %X %Z %Y", tmp[0]) == 0)
        printf("buffer length 64 is too small\n");
    else
        printf("%s\n", buf2);
    if (strftime(buf2, 64, "%a %b %d %X %Z %Y", tmp[1]) == 0)
        printf("buffer length 64 is too small\n");
    else
        printf("%s\n", buf2);
    exit(0);
}
