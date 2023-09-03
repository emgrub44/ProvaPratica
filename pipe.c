#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <time.h>
#include <stdio.h>
#include <sys/wait.h>



int main(int argc, char* argv[]){
    int fd[2]; //file descriptor read and write
    //fd[0] -> read fd[1] -> write
    if(pipe(fd) == -1){
        printf("And error occured with opening the pipe\n");
        return 1;
    }
    int id = fork();
    if(id == 0){
        //child
        close(fd[0]);
        int x;
        printf("Insert a number\n");
        scanf("%d", &x);
        if(write(fd[1], &x, sizeof(int)) == -1){
            printf("An error occured with the write \n");
            return 2;
        }
        close(fd[1]);
    } else {
        //parent 
        wait(NULL);
        close(fd[1]);
        int y;
        if(read(fd[0], &y, sizeof(int)) == -1){
             printf("An error occured while reading \n");
            return 3;
        }
        printf("The number you inserted is: %d\n", y);
        close(fd[0]);
    }
    return 0;
}
