#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <time.h>
#include <stdio.h>
#include <sys/wait.h>
#include <errno.h>

int main(int argc, char* argv[]){
    int arr[6] = {1, 2, 3, 4, 1, 2};
    int fd[2];
    if(pipe(fd) == -1){
        printf("An error occured with the pipe\n");
        return 1;
    }
    int id = fork();
    if(id == 0){
        //child
        close(fd[0]);
        int sum = 0;
        int i;
        for(i = 0; i < 3; i++){
            sum += arr[i];
        }
        if(write(fd[1], &sum, sizeof(int)) == -1){
            printf("An error occured with the write\n");
            return 2;
        }
        close(fd[1]);
    } else {
        //parent 
        close(fd[1]);
        int starting_sum;
        if(read(fd[0], &starting_sum, sizeof(int)) == -1){
            printf("An error occured while reading \n");
            return 3;
        }
        int i;
        for(i = 3; i < 6; i++){
            starting_sum += arr[i];
        }
        close(fd[0]);
        printf("The final sum is: %d\n", starting_sum);
    }
    return 0;
}