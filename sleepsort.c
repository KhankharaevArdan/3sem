#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>
 
int main (int argc, char **argv)
{
        while (--argc > 1 && !fork ());
        usleep (10000 * (argc = atoi (argv[argc])));
        printf ("%d\n", argc);
        wait (0);
        return 0;
}

