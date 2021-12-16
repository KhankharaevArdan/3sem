#include <stdio.h>
#include <sys/types.h>
#include <unistd.h>
#include <grp.h>
#include <pwd.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/wait.h>

int main (int argc, char** argv)
{
    int n = atoi (argv[1]);
    int st;
    pid_t pid;  
    for (int i = 0; i < n; i++)
    {
        pid = fork ();
        if (pid == 0)
        {
            printf ("parent pid %d    current pid %d\n", getppid (), getpid ());
        }
        else if (pid > 0)
        {
            wait (&st);
            if (st == -1) 
                perror ("wait ");
            if (WIFEXITED(st)) 
                exit (WEXITSTATUS(st));
            return 0;
        }
        else 
            printf ("fork error");
    }
    return 0;
}