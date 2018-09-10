#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <signal.h>
#include <unistd.h>
#include <string.h>
#include <dirent.h>
#include <sys/stat.h>
#include <sys/statfs.h>
#include <pwd.h>
#include <sys/types.h>
    
    
int main(void){
    char buffer[512];
    char cpu_info[100];
    int fd;
    char *Scpu_time[9];
    int i;

    fd = open ("/proc/stat", O_RDONLY);//ReadFile
    read (fd, buffer, sizeof (buffer));
    close (fd);
    char *p1, *p2;
    p1 = strstr(buffer, "cpu0 ");
    p2 = strstr(buffer, "cpu1 ");
    if (p1 == NULL || p2 == NULL || p1 > p2) {
    printf("Not found\n");
    } 
    else {
    p1 += strlen("cpu0 ");
    memcpy(cpu_info, p1, p2 - p1 - 1);
    printf("%s\n", cpu_info);
    }
    Scpu_time[0] = strtok (buffer, " ");//split
    //strtok()函数用来将字符串分割成一个个片段
    for (i = 1; i < 8 ; i++)  Scpu_time[i] = strtok (NULL, " ");

    for(i = 0; i < 9 ; i++) printf("%s ",Scpu_time[i]);
    printf("\n");
}
    
    
