/*
    Fare un programma di copia parallelo di file.
            pcp file1 file2
    pcp deve creare due processi figli; mentre un processo copia la prima meta’ del file, l'altro deve
    copiare l’altra meta’.
*/

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <fcntl.h>

int main(int argc, char *argv[]) {

    if (argc != 3) { //se non diamo in input i due file dà errore
        fprintf(stderr, "Usage: %s source_file dest_file\n", argv[0]);
        return 1;
    }

    const char *source_file = argv[1]; //salviamo il file da cui leggere
    const char *dest_file = argv[2]; //salviamo il file di destinazione

    int fd_source = open(source_file, O_RDONLY); //apro il file descriptor sorgente
    if (fd_source == -1) { //se non lo riesce ad aprire
        perror("Error opening source file");
        return 1;
    }

    int fd_dest = open(dest_file, O_WRONLY | O_CREAT | O_TRUNC, 0644); //chiedere a chatgpt
    if (fd_dest == -1) {  //se dà errore
        perror("Error opening destination file");
        close(fd_source);
        return 1;
    }

    off_t source_size = lseek(fd_source, 0, SEEK_END); //off_t -> tipo di dato per rappresentare offset o posizioni all'interno di un file
    /*
        In questo caso:
            fd_source -> file da prendere in considerazione
            0 -> vuol dire che deve iniziare dall'inizio (in pratica mette l'offset a 0)
            SEEK_END -> mette il puntatore al file alla fine
    */
    off_t half_size = source_size / 2; 

    pid_t child1 = fork(); //crea il primo processo figlio

    if (child1 == -1) { //se dà errore
        perror("Error forking process");
        close(fd_source);
        close(fd_dest);
        return 1;
    } else if (child1 == 0) {  // First child process
        lseek(fd_source, 0, SEEK_SET); //lseek -> funzione che consente di spostare il puntatore di lettura o scrittura all'interno di un file
        lseek(fd_dest, 0, SEEK_SET); //quindi qua viene settato all'inizio in entrambi i file
        

        char buffer[1024]; //buffer in cui salva quello che legge
        ssize_t bytesRead; //numero di byte letti
        off_t totalBytesRead = 0; 

        while ((bytesRead = read(fd_source, buffer, sizeof(buffer))) > 0 && totalBytesRead < half_size) {
            write(fd_dest, buffer, bytesRead); //scrive nel file di destinazione quello che ha salvato nel buffer
            totalBytesRead += bytesRead; //incrementa il numero di caratteri letti
        }

        close(fd_source); 
        close(fd_dest);
        exit(0); //semplicemente dice al processo di uscire
    } else {  // Parent process
        pid_t child2 = fork();

        if (child2 == -1) {
            perror("Error forking process");
            close(fd_source);
            close(fd_dest);
            return 1;
        } else if (child2 == 0) {  // Second child process
            lseek(fd_source, half_size, SEEK_SET);
            lseek(fd_dest, half_size, SEEK_SET);

            char buffer[1024];
            ssize_t bytesRead;

            while ((bytesRead = read(fd_source, buffer, sizeof(buffer))) > 0) {
                write(fd_dest, buffer, bytesRead);
            }

            close(fd_source);
            close(fd_dest);
            exit(0);
        } else {  // Parent process
            // Wait for both child processes to finish
            waitpid(child1, NULL, 0);
            waitpid(child2, NULL, 0);
            
            close(fd_source);
            close(fd_dest);

            printf("File copy complete.\n");
        }
    }

    return 0;
}
