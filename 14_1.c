#include <sys/mman.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <err.h>

int main(int argc, char *argv[])
{
        char *addr[2];
        int fd[2];
        struct stat sb;
        off_t offset, pa_offset;
        size_t length;
        mode_t mode = S_IRUSR | S_IWUSR | S_IRGRP | S_IROTH;
        if (argc < 4 || argc > 5)
        {
              errx(1, "usage:%s file1 file2 offset [length] \n", argv[0]);
        }

        fd[0] = open(argv[1], O_RDONLY);
        if (fd[0] == -1)
              errx(1,"fil1 open failed\n");
        fd[1] = open(argv[2], O_RDWR | O_CREAT | O_TRUNC,mode);
        if (fd[1] == -1)
              errx(1,"fil2 open failed\n");
        if (fstat(fd[0], &sb) == -1)
              errx(1,"fstat failed\n");
        if (ftruncate(fd[1], sb.st_size) < 0)
              errx(1,"ftruncate file2 failed\n");
        offset = atoi(argv[3]);
        pa_offset = offset & ~(sysconf(_SC_PAGE_SIZE) - 1);

        if (offset >= sb.st_size)
        {
              errx(1, "offset is past end of file\n");
        }
        if (argc == 5)
        {
              length = atoi(argv[4]);
              if (offset + length > sb.st_size)
                      length = sb.st_size - offset;
                      /* Can't display bytes past end of file */
        }
        else
        {    /* No length arg ==> display to end of file */
              length = sb.st_size - offset;
        }

        addr[0] = mmap(NULL, length + offset - pa_offset, PROT_READ, MAP_PRIVATE, fd[0], pa_offset);
        if (addr[0] == MAP_FAILED)
              errx(1,"mmap file1 error\n");
        addr[1]= mmap(NULL, length, PROT_WRITE, MAP_SHARED, fd[1], 0);
        if (addr[1] == MAP_FAILED)
              errx(1,"mmap file2 error\n");

        memcpy(addr[1], addr[0]+ offset - pa_offset,length);
        msync(addr[1],length,MS_SYNC);
        munmap(addr[0], length + offset - pa_offset);
        munmap(addr[1], length);
        close(fd[0]);
        close(fd[1]);
        exit(EXIT_SUCCESS);
}
