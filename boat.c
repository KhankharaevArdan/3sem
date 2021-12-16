#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/ipc.h>
#include <sys/sem.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>

#define RAMP 0
#define TRIP 1
#define BOAT 2
#define SEAT 3

int passenger (int passenger_num, int id) 
{
    struct sembuf ramp_up[1] = {{RAMP, 1, 0}};
    struct sembuf seat[3] = {{TRIP, 0, 0}, {RAMP, -1, 0}, {BOAT, -1, 0}};
    struct sembuf leave[3] = {{SEAT, 0, 0}, {RAMP, -1, 0}, {BOAT, 1, 0}};
    int sem;

    printf ("The passenger %d arrived at the coast\n", passenger_num);
    while (1) 
    {
        sem = semop (id, seat, 3);
        if ((sem == -1) && (errno == EIDRM)) 
        {
            printf ("The passenger %d left the coast\n", passenger_num);
            return 0;
        }
        printf ("The passenger %d boarded the boat\n", passenger_num);
        semop (id, ramp_up, 1);
        semop (id, leave, 3);
        printf ("The passenger %d left the boat\n", passenger_num);
        semop (id, ramp_up, 1);
    }
}

int captain (int num_seats, int num_trips, int id) 
{
    struct sembuf start[2] = {{BOAT, num_seats, 0}, {SEAT, 1, 0}};
    struct sembuf ramp_up[1] = {{RAMP, 1, 0}};
    struct sembuf ramp_down[1] = {{RAMP, -1, 0}};
    struct sembuf wait_full[3] = {{BOAT, 0, 0}, {RAMP, -1, 0}, {SEAT, -1, 0}};
    struct sembuf wait_empty[1] = {{BOAT, -num_seats, 0}};
    struct sembuf departure[1] = {{TRIP, 1, 0}};
    struct sembuf arrive[1] = {{TRIP, -1, 0}};

    printf ("Start of trips.\n");
    semop (id, ramp_up, 1);
    for (int i = 0; i < num_trips; i++) 
    {
        semop (id, start, 2);
        semop (id, wait_full, 3);
        printf ("The boat is full\n");
        printf ("The ramp up.\n");
        semop (id, departure, 1);
        printf ("The boat departed\n");
        printf ("The boat arrived.\n");
        printf ("The ramp down.\n");
        semop (id, ramp_up, 1);
        semop (id, wait_empty, 1);
        semop (id, ramp_down, 1);
        printf ("The boat is empty\n");
        semop (id, ramp_up, 1);
        semop (id, arrive, 1);
    }
    semop (id, ramp_down, 1);
    printf ("End of trips.\n");
    return 0;
}

int main (int argc, char* argv[]) 
{
    int num_pass, num_seats, num_trips, status;
    pid_t pid;

    if (argc == 4) 
    {
        num_pass = atoi (argv[1]);
        num_seats = atoi (argv[2]);
        num_trips = atoi (argv[3]);

        if (num_pass < num_seats)
        num_seats = num_pass;

        int id = semget (IPC_PRIVATE, 4, 0777);
        if (id < 0) 
        {
            perror ("semget error");
            errno;
            exit(1);
        }

        for (int i = 1; i <= num_pass; i++) 
        {
            pid = fork();
            if (pid < 0) 
            {
                perror ("fork error");
                errno;
                exit(1);
            }
            if (pid == 0) 
            {
                passenger (i, id);
                return 0;
            }
        }
        captain (num_seats, num_trips, id);
        int ctl = semctl (id, 0, IPC_RMID);
        if (ctl < 0) 
        {
            perror ("semctl error");
            errno;
            exit(1);
        }

        for (int i = 0; i < num_pass; i++) 
        {
            wait (&status);
        }
        return 0;
    } 
    else 
    {
        printf("Write only number of passengers, seats on the boat and trips.\n");
        return -1;
    }
}