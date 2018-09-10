
#include <stdio.h>
#include <pwd.h>
#include <unistd.h>
#include <sys/types.h>


int main (void)
{
int i ;
struct passwd *passwd;
i = getuid();
passwd = getpwuid (i);
printf ("UID: %d \nUser:\t%s\n",i, passwd->pw_name);
}
