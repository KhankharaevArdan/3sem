#include <stdio.h>
#include <string.h>

int main (int argc, char** argv)
{
    if (argc == 1)
        return 0;
    char command[5] = "echo";
    char key[3] = "-n";
    if (strcmp (argv[1], command) == 0) 
    {
        if (strcmp (argv[2], key) == 0)
        {
            for (int i = 3; i < argc; i++)
                printf ("%s ", argv[i]);
        }
        else
        {
            for (int i = 2; i < argc; i++)
                printf ("%s ", argv[i]);
            printf ("\n");
        }
    }  
    return 0;
}