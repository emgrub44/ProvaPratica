#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
#include <getopt.h> // Aggiunto l'header per getopt

int main(int argc, char* argv[]) {
    int num_processes = 1; // Impostato il valore predefinito a 1

    // Utilizziamo getopt per ottenere il grado di parallelismo desiderato
    int opt;
    while ((opt = getopt(argc, argv, "j:")) != -1) {
        switch (opt) {
            case 'j':
                num_processes = atoi(optarg); // Converti l'argomento di -j in un numero intero
                break;
            default:
                fprintf(stderr, "Usage: %s -j <num_processes> file1 file2\n", argv[0]);
                return 1;
        }
    }

    if (optind + 2 != argc) {
        fprintf(stderr, "Usage: %s -j <num_processes> file1 file2\n", argv[0]);
        return 1;
    }

    char* first_file = argv[optind];
    char* second_file = argv[optind + 1];

    int fd_source = open(first_file, O_RDONLY);
    if (fd_source < 0) {
        perror("Error opening source file");
        return 2;
    }

    int fd_dest = open(second_file, O_RDWR);
    if (fd_dest < 0) {
        perror("Error opening destination file");
        close(fd_source);
        return 3;
    }

    off_t total_size = lseek(fd_source, 0, SEEK_END);
    off_t chunk_size = total_size / num_processes;

    for (int i = 0; i < num_processes; i++) {
        int child_pid = fork();

        if (child_pid == 0) {
            // Codice del processo figlio
            off_t start_offset = i * chunk_size;
            off_t end_offset = (i == num_processes - 1) ? total_size : (i + 1) * chunk_size;

            lseek(fd_source, start_offset, SEEK_SET);
            lseek(fd_dest, start_offset, SEEK_SET);

            char buffer[1024];
            ssize_t bytesRead;

            while ((bytesRead = read(fd_source, buffer, sizeof(buffer))) > 0) {
                if (lseek(fd_dest, 0, SEEK_CUR) >= end_offset) {
                    break; // Il processo figlio ha completato il suo lavoro
                }

                ssize_t bytesToWrite = (lseek(fd_dest, 0, SEEK_CUR) + bytesRead <= end_offset)
                                          ? bytesRead
                                          : (end_offset - lseek(fd_dest, 0, SEEK_CUR));

                write(fd_dest, buffer, bytesToWrite);
            }

            close(fd_source);
            close(fd_dest);
            exit(0);
        }
    }

    // Processo genitore: attende il completamento dei processi figli
    for (int i = 0; i < num_processes; i++) {
        wait(NULL);
    }

    close(fd_source);
    close(fd_dest);

    return 0;
}
