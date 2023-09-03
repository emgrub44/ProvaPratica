#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
#include <string.h>
#include <sys/stat.h>

int main(int argc, char *argv[]) {
    if (argc != 2) {
        fprintf(stderr, "Utilizzo: %s <pathname della named pipe>\n", argv[0]);
        exit(1);
    }

    const char *fifo_path = argv[1];

    // Creazione della named pipe (FIFO)
    if (mkfifo(fifo_path, 0666) == -1) {
        perror("Errore nella creazione della named pipe");
        exit(1);
    }

    int fifo_fd = open(fifo_path, O_RDONLY);
    if (fifo_fd == -1) {
        perror("Errore nell'apertura della named pipe");
        exit(1);
    }

    char buffer[1024];
    ssize_t bytes_read;

    while (1) {
        bytes_read = read(fifo_fd, buffer, sizeof(buffer));
        if (bytes_read == -1) {
            perror("Errore nella lettura dalla named pipe");
            exit(1);
        } else if (bytes_read == 0) {
            // La named pipe è stata chiusa, riaprila
            close(fifo_fd);
            fifo_fd = open(fifo_path, O_RDONLY);
            if (fifo_fd == -1) {
                perror("Errore nell'apertura della named pipe");
                exit(1);
            }
        } else {
            // Stampa il testo ricevuto
            write(STDOUT_FILENO, buffer, bytes_read);
            for (int i = 0; i < bytes_read; i++) {
                if (buffer[i] == '\n') {
                    buffer[i] = '\0'; // Termina la stringa
                    if (strcmp(buffer, "FINE") == 0) {
                        // Termina il ciclo se riceviamo "FINE"
                        close(fifo_fd);
                        unlink(fifo_path);
                        exit(0);
                    }
                    break;
                }
            }
        }
    }

    // Rimozione della named pipe
    if (unlink(fifo_path) == -1) {
        perror("Errore nella rimozione della named pipe");
        exit(1);
    }

    close(fifo_fd);
    return 0;
}
