#include <stdio.h>
#include <sys/types.h>
#include <unistd.h>
#include <grp.h>
#include <pwd.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <sys/time.h>

int main (int argc, char** argv)
{
    int st;
    struct timeval tv1,tv2,dtv;  
    gettimeofday (&tv1, NULL);
    //printf("seconds : %ld\nmicro seconds : %ld", tv1.tv_sec, tv1.tv_usec);
    if (fork() == 0)
    {
        int error = execvp (argv[1], argv + 1);
        if (error)
        {
            perror ("execvp error");
            return -1;
        }
    }
    else 
        wait(&st);
    gettimeofday (&tv2, NULL);
    //printf("seconds : %ld\nmicro seconds : %ld", tv2.tv_sec, tv2.tv_usec);
    dtv.tv_sec = tv2.tv_sec -tv1.tv_sec;
    dtv.tv_usec = tv2.tv_usec-tv1.tv_usec;
    long double result = (long double) dtv.tv_sec + (long double) dtv.tv_usec / 1000000;
    printf ("\n %Lf seconds \n", result);
    return 0;
}