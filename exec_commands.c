#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <time.h>
#include <sys/wait.h>
#include <fcntl.h>

int main(int argc, char* argv[]){

    int pid = fork();
    if(pid == -1){return 1;}
    if(pid == 0){
       int file = open("pingResults.txt", O_WRONLY | O_CREAT, 0777);
       if(file == -1){return 2;}
       int file1 = dup2(file, STDOUT_FILENO);  //dup -> duplica il file e ritorna un altro file descriptor che punta allo stesso file
       int err =  execlp("ping", "ping", "-c", "1", "google.com", NULL);
       if(err == -1){
        printf("Could not find program to execute!\n");
        return 2;
       }
    } else {
        int wstatus;
        wait(&wstatus);
        if(WIFEXITED(wstatus)){ //se è uscito 
            int status_code = WEXITSTATUS(wstatus); //prendo lo stato di uscita
            if( status_code == 0){
                printf("Success!\n"); //se è uscito correttamente
            } else {
                printf("Failure with status code: %d\n", status_code);  //se è uscito a causa di un errore
            }
        }
    }
    return 0;
}