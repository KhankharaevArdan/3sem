#include <stdio.h>
#include <sys/types.h>
#include <unistd.h>
#include <grp.h>
#include <pwd.h>



int main (int argc, char **argv)
{
    struct group *grp;
    struct passwd *pwd;
    gid_t groups[100] = {0};
    if (argc == 2)
    {
        int ngroups = 100;
        if ((pwd = getpwnam (argv[1])) == NULL)
        {
            printf ("Wrong user name\n");
            return 0;
        }
        if ((grp = getgrgid(*groups)) == NULL)
            printf ("getgrgid() error");
        getgrouplist(argv[1], pwd->pw_gid, groups, &ngroups);
            
        int uid = pwd -> pw_uid;
        int gid = pwd -> pw_gid;
        
        printf ("uid=%d(%s) ", uid, pwd -> pw_name);
        printf ("gid=%d(%s) ", gid, pwd -> pw_name);
        printf ("groups=%d(%s)", (int) groups[ngroups - 1], grp -> gr_name);
                                                                                                                                                                                                                                                                
        for (int i = 0; i < ngroups - 1; i++) 
        {
            grp = getgrgid(groups[i]);
            printf (",%d(%s)", groups[i], grp->gr_name);
        }
        printf ("\n");
        return 0;
    }
    else if (argc == 1)
    {
        uid_t uid = getuid();
        uid_t gid = getgid();
        if ((pwd = getpwuid(uid)) == NULL)
            printf ("getpwuid() error");
        int ngroups = getgroups(100, groups);
        grp = getgrgid(groups[ngroups - 1]);
        printf ("uid=%d(%s) ", uid, pwd -> pw_name);
        printf ("gid=%d(%s) ", gid, pwd -> pw_name);
        printf ("groups=%d(%s)", groups[ngroups - 1], grp -> gr_name);                                                                                                                                                                                                                                              
        for (int i = 0; i < ngroups - 1; i++) 
        {
            grp = getgrgid(groups[i]);
            printf (",%d(%s)", groups[i], grp->gr_name);
        }
        printf ("\n");
        return 0;
    }
    else 
        printf ("Too many arguments\n");
    return 0;
}