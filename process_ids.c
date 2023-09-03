#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <time.h>
#include <stdio.h>
#include <sys/wait.h>

int main(int argc, char* argv[]){
    int id = fork();
    if(id == 0){
        sleep(1);
    }
    printf("Current ID: %d, parent ID: %d\n", 
            getpid(), getppid());
    
    int res = wait(NULL);
    if(res== -1){
        printf("No children to wait for");
    } else {
        printf("%d finished execution \n", res);
    }

    return 0;
}


/*
    getpid() -> prende il process id di un processo qualsiasi
    getppid() -> prende il process id del padre

    sleep(1) -> è una wait di un secondo

    Se il padre finisce prima del figlio abbiamo dei processi zombie

*/