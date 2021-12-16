#include <stdio.h>
#include <errno.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>

void write_file (int fd, char* name, size_t size, char* buf, int fd_out);

int main (int argc, char **argv)
{   
    size_t size = 0;
    char command [4] = "cat";
    //printf ("command is %s\n ", command);
    //printf ("argc is %d\n ", argc);
    //printf ("argv[argc - 1] is %s\n ", argv[argc - 1]);
    char name [20];
    char buf [1000];
    int fd = 0;
    int fd_out = 1;
    if (strcmp (argv[1], command) == 0)
    {
        int nfiles = 2;
        while (nfiles < argc)
        {   
            strcpy(name, argv [nfiles]); 
            //printf ("name is %s\n ", name);
            write_file (fd, name, size, buf, fd_out);
            nfiles++;
        } 
    }
    else 
        printf ("command not found\n");
    return 0;
}

void write_file (int fd, char* name, size_t size, char* buf, int fd_out)
{
    fd = open (name, O_RDWR);
    //printf ("fd is %d\n ", fd);
    if (fd == -1)
        perror ("open failed");
    int count;
    while ( (count = read (fd, buf, 1000)) > 0 )
        size += count;
    //printf ("size is %ld\n ", size);
    int byteswritten = 0;
    while (size != 0)
    {
        byteswritten = write (fd_out, buf, size);
        size = size - byteswritten;
    }  
    //printf ("byteswritten %d\n\n ", byteswritten);  
    int close_result = close (fd);
    if (close_result < 0)
        perror ("close failed");    
    return;
}