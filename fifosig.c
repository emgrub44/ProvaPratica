#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
#include <string.h>
#include <signal.h>

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
            // Parsa i due numeri (PID e segnale) dalla riga ricevuta
            int pid, signal_num;
            if (sscanf(buffer, "%d %d", &pid, &signal_num) != 2) {
                fprintf(stderr, "Riga malformata: %s", buffer);
                continue;
            }

            // Invia il segnale al processo
            if (kill(pid, signal_num) == -1) {
                perror("Errore nell'invio del segnale");
            }
        }
    }

    return 0;
}
