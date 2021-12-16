#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/ipc.h>
#include <sys/sem.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>
#include <sys/msg.h>

#define NEXT 1
#define START 2
#define READY 3
#define AMOUNT_OF_GAYKA 2

struct msgbuf
{
	long mtype;
	char mtext[0];
};

int gayka_job (int id1, int id2) 
{
	struct msgbuf buf;
	while(1) 
    {
		ssize_t rcv = msgrcv (id2, &buf, 0, START, MSG_NOERROR);
		if ((rcv == -1) && (errno == EIDRM))
			return 0;
        if (rcv < 0) 
        {
		    perror ("msgrcv error in gayka_job");
		    return -1;
	    }
		printf ("The worker tightened the gayka.\n");
		buf.mtype = READY;
		int snd = msgsnd (id1, &buf, 0, MSG_NOERROR);
        if (snd < 0) 
        {
		    perror ("msgsnd error in gayka_job");
		    return -1;
	    }
	}
}

int vint_job (int id1, int id2, int details_num) 
{
	struct msgbuf buf;
	printf ("Start work\n");
	buf.mtype = NEXT;
	int snd = msgsnd (id1, &buf, 0, MSG_NOERROR);
    if (snd < 0) 
    {
		perror ("msgsnd error in vint_job");
	    return -1;
	}

	for (int i = 0; i < details_num; i++) 
    {
		ssize_t rcv = msgrcv (id1, &buf, 0, NEXT, MSG_NOERROR);
		if (rcv < 0) 
        {
		    perror ("msgrcv error in vint_job");
		    break;
	    }
		printf ("The worker is making the detail '%d'.\n", i + 1);
		buf.mtype = START;
		for (int j = 0; j < AMOUNT_OF_GAYKA; j++) 
        {
			snd = msgsnd (id2, &buf, 0, MSG_NOERROR);
			if (snd < 0) 
            {
		        perror ("msgsnd error in vint_job");
		        break;
	        }
		}
		printf ("The worker tightened the vint.\n");
		for (int j = 0; j < AMOUNT_OF_GAYKA; j++) 
        {
			rcv = msgrcv (id1, &buf, 0, READY, MSG_NOERROR);
			if (rcv < 0) 
            {
		        perror ("msgrcv error in vint_job");
		        break;
	        }
		}
		printf ("The worker has finished detail '%d'.\n", i + 1);
		buf.mtype = NEXT;
		snd = msgsnd (id1, &buf, 0, MSG_NOERROR);
		if (snd < 0) 
        {
		    perror ("msgsnd error in vint_job");
		    break;
	    }
	}
	printf ("End of work\n");
	return 0;
}

int main (int argc, char* argv[])
{	
	if (argc == 2) 
    {
		int details_num = atoi (argv[1]);
		int id1 = msgget (IPC_PRIVATE, IPC_CREAT | 0777);
        if (id1 < 0) 
        {
		    perror ("msgget error");
		    return -1;
	    }
		int id2 = msgget (IPC_PRIVATE, IPC_CREAT | 0777);
        if (id2 < 0) 
        {
		    perror ("msgget error");
		    return -1;
	    }
		
		for (int i = 0; i < AMOUNT_OF_GAYKA; i++) 
        {
			pid_t pid = fork();
			if (pid == 0) 
            {
				gayka_job (id1, id2);
				return 0;
			}
		}
		
		vint_job (id1, id2, details_num);	
		int ctl = msgctl (id1, IPC_RMID, NULL);
        if (ctl < 0) 
        {
		    perror ("msgctl error");
		    return -1;
	    }
		ctl = msgctl (id2, IPC_RMID, NULL);
        if (ctl < 0) 
        {
		    perror ("msgctl error");
		    return -1;
	    }
		return 0;
	}
    else
    {
		printf ("Enter only the number of details.\n");
		return -1;
	}
}