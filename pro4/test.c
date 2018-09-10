#include <sys/types.h>
#include <sys/sem.h>
#include <sys/ipc.h>
#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <sys/shm.h>
#include <sys/stat.h>
#include <sys/wait.h>
#include <string.h>
#include <sys/statfs.h>

void get_memory_info();

int main(){
    get_memory_info();
    return 0;
}

void get_memory_info(){
    FILE *fp;
    int i;
    char s[1024];
    char *mem[9];
    struct statfs diskInfo; 
    unsigned long long blocksize;
    unsigned long long totalsize;
    unsigned long long freeDisk;
    unsigned long long availableDisk;
      
    if((fp = fopen("/etc/mtab","r")) == NULL){
        printf("error!\n");
    }
    else{
        printf("设备  目录  类型\n");
        while(fgets(s,1024,fp) != NULL){
            i = 0;
            if(s[0] == '/'){
                mem[0] = strtok (s," ");
                for(i = 1; i < 9; i++) mem[i] = strtok(NULL," ");
                statfs(mem[1],&diskInfo);
                printf("%s\n",mem[0]);
                blocksize = diskInfo.f_bsize; //每个block里包含的字节数 
                totalsize = blocksize * diskInfo.f_blocks; //总的字节数，f_blocks为block的数目 
                printf("Total_size = %llu B = %llu KB = %llu MB = %llu GB\n", 
                    totalsize, totalsize>>10, totalsize>>20, totalsize>>30); 

                if((totalsize>>20) < 1024)   printf("%llu MB\n",totalsize>>20);
                else printf("%llu GB\n",totalsize>>30);
      
                freeDisk = diskInfo.f_bfree * blocksize; //剩余空间的大小 
                availableDisk = diskInfo.f_bavail * blocksize; //可用空间大小 
                printf("Disk_free = %llu MB = %llu GB\nDisk_available = %llu MB = %llu GB\n", 
                    freeDisk>>20, freeDisk>>30, availableDisk>>20, availableDisk>>30);
                if((availableDisk>>20) < 1024)   printf("%llu MB\n",availableDisk>>20);
                else printf("%llu GB\n",availableDisk>>30);

                unsigned long long usedDisk;
                double rate;
                usedDisk = totalsize - availableDisk;

                if((usedDisk>>20) < 1024)   printf("%llu MB\n",usedDisk>>20);
                else printf("%llu GB\n",usedDisk>>30);
                
                rate = (double)usedDisk / totalsize;
                printf("Disk_used = %llu MB = %llu GB\nDisk_rate = %.1lf \n", 
                    usedDisk>>20, usedDisk>>30, rate * 100);

                
                
            }
        }
    }
}