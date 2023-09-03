#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
#include <string.h>
#include <sys/wait.h>

int main(int argc, char* argv[]) {
    if (argc != 2) {
        fprintf(stderr, "Utilizzo: %s <file_comandi>\n", argv[0]);
        return 1;
    }

    const char* comandi_file = argv[1];

    // Apri il file dei comandi
    FILE* file = fopen(comandi_file, "r");
    if (file == NULL) {
        perror("Errore nell'apertura del file dei comandi");
        return 2;
    }

    char linea[256];
    char* comandi[256];  // Array per memorizzare i comandi
    int num_comandi = 0; // Numero di comandi letti

    // Leggi i comandi dal file e memorizzali in un array
    while (fgets(linea, sizeof(linea), file) != NULL) {
        // Rimuovi il carattere newline alla fine della riga
        linea[strcspn(linea, "\n")] = '\0';

        // Alloca memoria per la stringa del comando e copiala
        comandi[num_comandi] = strdup(linea);

        if (comandi[num_comandi] == NULL) {
            perror("Errore nell'allocazione di memoria per il comando");
            fclose(file);
            return 3;
        }

        num_comandi++;
    }

    // Chiudi il file
    fclose(file);

    // Crea una serie di pipe
    int pipe_fd[num_comandi - 1][2];
    for (int i = 0; i < num_comandi - 1; i++) {
        if (pipe(pipe_fd[i]) == -1) {
            perror("Errore nella creazione della pipe");
            return 4;
        }
    }

    // Esegui i comandi in sequenza collegando l'output di ciascun comando all'input del successivo
    for (int i = 0; i < num_comandi; i++) {
        pid_t pid = fork();

        if (pid == -1) {
            perror("Errore nella creazione del processo figlio");
            return 5;
        }

        if (pid == 0) {
            // Processo figlio
            if (i > 0) {
                // Collega l'input del processo figlio all'output della pipe precedente
                dup2(pipe_fd[i - 1][0], STDIN_FILENO);
                close(pipe_fd[i - 1][0]);
            }

            if (i < num_comandi - 1) {
                // Collega l'output del processo figlio all'input della pipe successiva
                dup2(pipe_fd[i][1], STDOUT_FILENO);
                close(pipe_fd[i][0]);
            }

            // Esegui il comando
            execlp("/bin/sh", "/bin/sh", "-c", comandi[i], NULL);
            perror("Errore nell'esecuzione del comando");
            return 6;
        }

        // Processo padre
        if (i > 0) {
            close(pipe_fd[i - 1][0]);
        }
        if (i < num_comandi - 1) {
            close(pipe_fd[i][1]);
        }

        // Aspetta che il processo figlio termini
        int status;
        waitpid(pid, &status, 0);

        if (WIFEXITED(status) && WEXITSTATUS(status) != 0) {
            fprintf(stderr, "Errore nell'esecuzione del comando: %s\n", comandi[i]);
            return 7;
        }
    }

    // Libera la memoria allocata per i comandi
    for (int i = 0; i < num_comandi; i++) {
        free(comandi[i]);
    }

    return 0;
}
