/*

    1° -> fork per creare i due figli
    2° -> prima deve partire il primo figlio e solo dopo il secondo (gestire la sincronizzazione)
    3° -> prendono dal file1 e copiano nel file 2, quindi ho due file descriptor diversi


*/

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>

int main(int argc, char* argv[]){

    if(argc != 3){
        printf("Not enough arguments passed\n");
        return 1;
    }

    char* first_file  = argv[1]; //puntatore al primo file
    char* second_file = argv[2]; //puntatore al secondo file

    int fd_source = open(first_file, O_RDONLY); //apro il primo file
    if(fd_source < 0){
        return 2;
    }

    int fd_dest = open(second_file, O_RDWR); //apro il secondo file
    if(fd_dest < 0){
        return 3;
    }

    off_t total_size = lseek(fd_source, 0, SEEK_END); //mi salvo la grandezza del file

    off_t half_size = total_size/2;

    int first_child = fork();

    if(first_child == 0){
        //do something
        lseek(fd_source, 0, SEEK_SET); //setto il puntatore all'inizio del file
        lseek(fd_dest, 0, SEEK_SET);
        ssize_t bytesRead = 0;

        char* buffer[1024];
        off_t totalBytesRead;
        while (totalBytesRead < half_size) //fin quando posso leggere
        {
            bytesRead = read(fd_source , buffer, sizeof(*buffer));
            write(fd_dest, buffer, bytesRead);
            totalBytesRead += bytesRead;
        }

        close(fd_source); //chiudo tutto ed esco
        close(fd_dest);
        exit(0);
    } else {
        int second_child = fork();
        if(second_child == 0) {
            lseek(fd_source, half_size, SEEK_SET);
            lseek(fd_dest, half_size, SEEK_SET);
            ssize_t bytesRead = 0;

            char* buffer[1024];
            off_t totalBytesRead;
            while ( (bytesRead = read(fd_source , buffer, sizeof(*buffer))) > 0)
            {
                write(fd_dest, buffer, bytesRead);
            }
            close(fd_source);
            close(fd_dest);
            exit(0);
        } 
    }
    return 0;
}