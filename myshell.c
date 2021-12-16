#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>
#include <string.h>

int main (int argc, char* argv[]) 
{
    while (feof(stdin) == 0) 
    {
        printf ("myshell: ");
        char buf[1000];
        if (fgets(buf, 1000, stdin) == NULL) break;
        buf[strlen(buf) - 1] = 0;
        if ((buf == NULL) || !strcmp (buf, "exit")) break;

        char* tokens[1000];
        char* tok;
        int num_tokens = 1;
        tok = strtok (buf, "|");
        tokens[0] = tok;
        while (tok != NULL) 
        {
            tok = strtok (NULL, "|");
            tokens[num_tokens] = tok;
            num_tokens++;
        }
        num_tokens--;

        int ppipe = -1;
        int pid;
        for (int i = 0; i < num_tokens; i++)
        {
            int pipefd[2];
            if (pipe(pipefd) == -1)
            {
                printf ("Pipe error\n");
                break;
            }

            pid = fork ();
            if (pid == -1)
            {
                printf ("Fork error\n");
                break;
            }

            if (pid != 0)
            {
                close (pipefd[1]);
                close (ppipe);
                ppipe = pipefd[0];
            }
            else
            {
                if (ppipe != -1)
                    if (dup2(ppipe, 0) == -1)
                    {
                        printf ("Fd duplication error\n");
                        return -1;
                    }

                if (i < (num_tokens - 1))
                    if (dup2(pipefd[1], 1) == -1)
                    {
                        printf ("Fd duplication error\n");
                        return -1;
                    }

                char** cmds = tokens + i;
                int cmd_num = 1;
                char* command = strtok (tokens[i], " ");
                cmds[0] = command;
                while (command != NULL) 
                {
                    command = strtok (NULL, " ");
                    cmds[cmd_num] = command;
                    cmd_num++;
                }

                execvp (cmds[0], cmds);
                perror (cmds[0]);

                close (pipefd[0]);
                close (pipefd[1]);
                close (ppipe);
                return -1;
            }
        }
        waitpid (pid, NULL, WUNTRACED);
        close (ppipe);
    }
    return 0;
}