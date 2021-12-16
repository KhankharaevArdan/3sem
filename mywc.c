#include <stdio.h>
#include <sys/types.h>
#include <unistd.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <sys/time.h>
#include <ctype.h>

int words_counter (char* buf, int bytesread);

int main (int argc, char** argv)
{
    int st = 0;
    int bytesread = 0;
    char buf [1000] = {0};
    struct timeval tv1,tv2,dtv;  
    gettimeofday (&tv1, NULL);
    int opt = getopt(argc, argv, "+q");
    int fd[2] = {0};
    pipe (fd);
    pid_t pid = fork();
    if (pid == 0)
    {
        dup2 (fd[1], 1);
        close (fd[0]);
        close (fd[1]);
        if (opt != -1)
            execvp (argv[2], argv + 2);
        else 
            execvp (argv[1], argv + 1);
    }
    else
    { 
        close (fd[1]);
        bytesread = read (fd[0], buf, 1000);
        if (opt == 'q')
            printf ("%s\n", buf);
        printf (" %d bytes ", bytesread);
        int lines = 0;
        for (int i = 0; i < bytesread; i++)
        {
            if (buf[i] == '\n') 
                lines++;
        }
        int words = words_counter (buf, bytesread);
        printf (" %d lines ", lines);
        printf (" %d words ", words);
        close (fd[0]);   
        wait (&st);
    }
    gettimeofday (&tv2, NULL);
    dtv.tv_sec = tv2.tv_sec - tv1.tv_sec;
    dtv.tv_usec = tv2.tv_usec - tv1.tv_usec;
    long double result = (long double) dtv.tv_sec + (long double) dtv.tv_usec / 1000000;
    printf ("\n %Lf seconds \n", result);
    return 0;
}

int words_counter (char* buf, int bytesread)
{
    int is_space = 1;
    int counter = 0;
    int i = 0;
    while (i < bytesread)
    {
        if (((buf[i] == ' ') && (is_space == 0)) || ((buf[i] == '\n') && (is_space == 0)))
        {
            is_space = 1;
        }
        if ((buf[i] != ' ') && (is_space == 1))
        {
            counter++;
            is_space = 0;
        }
        i++;
    }
    return counter - 1;
}