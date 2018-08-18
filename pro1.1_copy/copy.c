#include <pthread.h>
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
#include <sys/stat.h>
#include <fcntl.h>


int sigid;
#define shared_segment_size 100


union semun {

        int val; /* value for SETVAL */

        struct semid_ds *buf; /* buffer for IPC_STAT, IPC_SET */

        unsigned short *array; /* array for GETALL, SETALL */

        struct seminfo *__buf; /* buffer for IPC_INFO */

};

//P 操作
int P(int sigid,int semnum){

    struct  sembuf buf;  
    buf.sem_num = semnum;  
    buf.sem_op = -1;  
    buf.sem_flg = 0;  
    if(semop(sigid,&buf,1) < 0)  
    {  
        perror("FAIL TO semop");  
        exit(EXIT_FAILURE);  
    }  
    return 0;  
}

//V 操作
int V(int sigid,int semnum){

    struct sembuf buf;  
    buf.sem_num = semnum;  
    buf.sem_op = 1;  
    buf.sem_flg = 0;  
    if(semop(sigid,&buf,1) < 0)  
    {  
        perror("FAIL to sempo");  
        exit(EXIT_FAILURE);  
    }  
    return 0;  

}

int read_data(char *shared_memory,int fp){
//open file
        
    // int count = 1;
    int i = 0,n;
        while(1){

            
            P(sigid,0);
            P(sigid,2);
            if((n = read(fp,&shared_memory[i],1)) <= 0) {
                shared_memory[shared_segment_size-2] = '0';
                shared_memory[shared_segment_size-1] = i;
            }
            //printf("write:%d %c\n",i,shared_memory[i]);
            i = (i + 1) % (shared_segment_size-2);
            V(sigid,2);
            V(sigid,1);

            if(n <= 0) break;
        }

        close(fp);
        exit(0);
}

int write_data(char *shared_memory,int gp){

    
    int j = 0;
    char s = 0;
    while(1){
            
            P(sigid,1);
            P(sigid,2);
            if(shared_memory[shared_segment_size-2] != '0' || (s != shared_memory[shared_segment_size-1])){
                write(gp,&shared_memory[j],1);
                //printf("read:%d %c\n",j,shared_memory[j]);
                j = (j + 1) % (shared_segment_size-2);
                s = j;
            }
            V(sigid,2);            
            V(sigid,0);
           
            
            if(shared_memory[shared_segment_size-2] == '0' && (s == shared_memory[shared_segment_size-1]))
                break;
            
        }
        close(gp);
        //shmdt(shared_memory);        
        exit(0);
}

int main(int argc,char **argv){
    key_t key;
    union semun sem1,sem2,sem3;
    pid_t readbuf,writebuf;
    int segment_id;
    char* shared_memory;
    struct shmid_ds shmbuffer;
    //int shared_segment_size = 21;
    int i = 0,j = 0;
    int fp,gp;

    fp = open(argv[1],O_RDONLY);
    gp = open(argv[2],O_RDONLY | O_WRONLY| O_CREAT, S_IRUSR | S_IWUSR | S_IRGRP | S_IEXEC);



    //create a shared memory
    segment_id = shmget(IPC_PRIVATE,shared_segment_size,IPC_CREAT|IPC_EXCL|S_IRUSR|S_IWUSR);

    shared_memory = (char *)shmat(segment_id,0,0);

    shmctl(segment_id,IPC_STAT,&shmbuffer);
   

    //create key
    key = ftok("/home",1);
    if(key == -1){
        printf("Create key_t error!\n");
    }

    //create sem
    sigid = semget(key,3,IPC_CREAT|0666);
    if(sigid == -1) printf("Create sigid error!\n");

    //give sigid 1 an initial value
    sem1.val = shared_segment_size-2;
    if((semctl(sigid,0,SETVAL,sem1)) < 0){
        perror("ctl sem error\n");

        semctl(sigid,0,IPC_RMID,sem1);

        return -1 ;
    }


    //give sigid 2 an initial value
    sem2.val = 0;
    if((semctl(sigid,1,SETVAL,sem2)) < 0){
        perror("ctl sem error\n");

        semctl(sigid,1,IPC_RMID,sem2);

        return -1 ;
    }

    //give sigid 3 an initial value
    sem3.val = 1;
    if((semctl(sigid,2,SETVAL,sem3)) < 0){
        perror("ctl sem error\n");

        semctl(sigid,2,IPC_RMID,sem3);

        return -1 ;
    }

    shared_memory[shared_segment_size-2] = '1';

    
    //create child process1
    if((readbuf = fork()) < 0){
        printf("Create child process1 failed!\n");
        return -1;
    }
    
    else if(readbuf == 0){
       read_data(shared_memory,fp);
    }

    else {
        //create child process2
        if((writebuf = fork()) < 0){
        printf("Create child process2 failed!\n");
        return -1;
        }
        else if(writebuf == 0 )
        {
        write_data(shared_memory,gp);
        }
        else {
            waitpid(readbuf,NULL,0);
            waitpid(writebuf,NULL,0);
        
            semctl(sigid,0,IPC_RMID,sem1);
            semctl(sigid,1,IPC_RMID,sem2);
            semctl(sigid,2,IPC_RMID,sem3);
            shmctl(segment_id,IPC_RMID,0);
            exit(0);
        }
    }
}

