#include <stdio.h>
#include <sys/types.h>
#include <unistd.h>
#include <grp.h>
#include <pwd.h>
#include <stdlib.h>

const int size = 100;

void print_data (struct group *grp, struct passwd *pwd, gid_t* groups, int ngroups);

int main (int argc, char **argv)
{
    struct group *grp;
    struct passwd *pwd;
    if (argc == 2)
    {
        gid_t *groups = (gid_t *) calloc (sizeof(uid_t), size);
        int ngroups = size;
        if ((pwd = getpwnam (argv[1])) == NULL)
        {
            printf ("Wrong user name\n");
            return 0;
        }
        grp = getgrgid (pwd->pw_gid);
        getgrouplist (argv[1], pwd->pw_gid, groups, &ngroups);
        printf ("uid=%d(%s) ", pwd -> pw_uid, pwd -> pw_name);
        printf ("gid=%d(%s) ", pwd -> pw_gid, pwd -> pw_name);         
        grp = getgrgid (groups[0]);
        printf ("groups=%d(%s)", groups[0], grp -> gr_name);                                                                                                                                                                                                                                  
        for (int i = 1; i < ngroups; i++) 
        {
            grp = getgrgid (groups[i]);
            printf (",%d(%s)", groups[i], grp -> gr_name);
        }
        printf ("\n");
        free (groups);
        return 0;
    }
    else if (argc == 1)
    {
        gid_t *groups = (gid_t *) calloc (sizeof(uid_t), size);
        pwd = getpwuid (getuid());
        int ngroups = getgroups (size, groups);
        grp = getgrgid (getgid());
        printf ("uid=%d(%s) ", pwd -> pw_uid, pwd -> pw_name);
        printf ("gid=%d(%s) ", pwd -> pw_gid, pwd -> pw_name);    
        grp = getgrgid (groups[ngroups - 1]);
        printf ("groups=%d(%s)", groups[ngroups - 1], grp -> gr_name);                                                                                                                                                                                                                                       
        for (int i = 0; i < ngroups - 1; i++) 
        {
            grp = getgrgid (groups[i]);
            printf (",%d(%s)", groups[i], grp -> gr_name);
        }
        printf ("\n");
        free (groups);
        return 0;
    }
    else 
        printf ("Too many arguments\n");
    return 0;
}

