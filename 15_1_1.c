#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <err.h>
#include <string.h>
int main(void)
{
        int n, i;
        char line[20];
        while ((n = read(STDIN_FILENO, line,20)) > 0)
        {
                n=strlen(line);
                for(i=0;i<n-1;i++)
                        line[i]+=1;
                i=0;
                if (write(STDOUT_FILENO, line, n) != n)
                        errx(1,"write error");
        }
        exit(0);
}
