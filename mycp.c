#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <errno.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <getopt.h>
#include <string.h>

#define SIZE_OF_BUF 2048

int read_file (int src, char* buf, int size)
{
    int read_bytes;
    char* read_err_msg = "Read error";
    read_bytes = read (src, buf, size);

    switch (read_bytes)
    {
        case 0: return 0;
        case -1: perror (read_err_msg); return -1;
    }
    return read_bytes;
}

int write_file (int dest, char* buf, int size)
{
    int write_bytes = 0;
    int offset = 0;
    char* write_err_msg = "Write error";

    while (offset < size)
    {
        write_bytes = write (dest, buf + offset, size);
        if (write_bytes == -1)
        {
            perror (write_err_msg);
            return -1;
        }
        offset += write_bytes;
    }
    return 0;
}

int copy_file (int src, int dest)
{
    int read_error = 0;
    int write_error = 0;
    char buf[SIZE_OF_BUF];
    read_error = read_file (src, buf, SIZE_OF_BUF);

    while (read_error != 0)
    {
        if (read_error == -1) 
            return -1;
        write_error = write_file (dest, buf, read_error);
        if (write_error == -1) 
            return -1;
        read_error = read_file (src, buf, SIZE_OF_BUF);
    }
    return 0;
}

void verbose_func (char* fulldest, char* src)
{
    printf ("%s -> %s\n", src, fulldest);
    return;
}

int force_func (char* fulldest)
{
    int fd = open (fulldest, O_RDWR | O_APPEND);
    if (fd == -1)
    {
        if (unlink(fulldest) == -1)
        {
            perror("mycp: unlink error");
            return -1;
        }
    }
    close(fd);
    return 0;
}

int interactive_func (char* fulldest)
{
    char key;
    printf ("mycp: overwrite '%s'? ", fulldest);
    scanf ("%c", &key);
    printf ("\n\n");
    if ((key == 'n') || (key == 'N'))
        return 1;
    return 0;
}

int mycp (char* src, char* dest, int verbose, int force, int interactive)
{
    struct stat st;
    char* fulldest = NULL;

    if ((stat(dest, &st) != -1) && (S_ISDIR(st.st_mode) == 1))
    {
        fulldest = (char*) malloc(strlen(dest) + strlen(src) + 2);
        if (fulldest == NULL)
        {
            perror ("Malloc error");
            return -1;
        }
        strcat (fulldest, dest);
        strcat (fulldest, "/");
        strcat (fulldest, src);
    }
    else
    {
        fulldest = (char*) malloc(strlen(dest));
        if (fulldest == NULL)
        {
            perror ("Malloc error");
            return -1;
        }
        strcat (fulldest, dest);
    }

    if (verbose)
        verbose_func (fulldest, src);

    if (stat(fulldest, &st) == 0)
    {
        if (interactive)
        { 
            if (interactive_func (fulldest) == 1)
                return 0;
        }
        if (force)
        { 
            force_func (fulldest);
        }
    }

    int fd_dest = open (fulldest, O_WRONLY | O_CREAT, 0664);
    if (fd_dest == -1)
    {
        perror ("mycp: dest file open error");
        return -1;
    }

    int fd_src = open (src, O_RDONLY);
    if (fd_src == -1)
    {
        perror ("mycp: src file open error");
        return -1;
    }

    copy_file (fd_src, fd_dest);
    close (fd_src);
    close (fd_dest);

    return 1;
}

extern int optind;

int main (int argc, char* argv[])
{
    int verbose = 0;
    int force = 0;
    int interactive = 0;
    struct option longopts[4] = {
        { "verbose", no_argument, NULL, 'v' },
        { "force", no_argument, NULL, 'f' },
        { "interactive", no_argument, NULL, 'i' },
        {0}
    };
    int op; 
    while ((op = getopt_long (argc, argv, "vfi", longopts, NULL)) != -1)
    {
        switch (op)
        {
            case 'v': verbose = 1;
                      break;
            case 'f': force = 1;
                      break;
            case 'i': interactive = 1;
                      break;
        }
    }

    if ((argc - optind) == 2)
    {
        return mycp(argv[argc - 2], argv[argc - 1], verbose, force, interactive);
    }
    else if ((argc - optind) > 2)
    {
        errno = 0;
        struct stat st;
        if ((stat(argv[argc - 1], &st) > -1) && (S_ISDIR(st.st_mode) == 1))
        {
            for (int i = optind; i < argc - 1; i++)
            {
                mycp (argv[i], argv[argc - 1], verbose, force, interactive);
            }
        }
        else
        {
            if (errno)
            {
                perror ("stat error");
                return -1;
            }
            printf ("Last argument is not a directory\n");
            return -1;
        }
    }
    else
    {
        printf ("mycp: required some file or dest names\n");
        return -1;
    }
    return 0;
}
 