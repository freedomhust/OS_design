#include <gtk/gtk.h>
#include <sys/types.h>
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

int main(void){
    pid_t p1,p2,p3;
    if((p1 = fork()) == 0){
        execv("./time",NULL);
    }
    else{
        if((p2 = fork()) == 0){
            execv("./cycle",NULL);
        }
        else{
            execv("./sum",NULL);
        }
    }
}