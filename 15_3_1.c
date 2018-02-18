#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <err.h>
#include <string.h>
int main(void)
{
    int n, i;
    char line[20]={"end of file\n"};
    if (write(STDOUT_FILENO, line, n) != n)
        errx(1,"write error\n");
    exit(0);
}
