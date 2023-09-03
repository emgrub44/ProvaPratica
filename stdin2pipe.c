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

    char linea1[256];
    char linea2[256];

    // Leggi le due linee di comando dal file
    if (fgets(linea1, sizeof(linea1), file) == NULL) {
        perror("Errore nella lettura della prima linea di comando");
        fclose(file);
        return 3;
    }
    if (fgets(linea2, sizeof(linea2), file) == NULL) {
        perror("Errore nella lettura della seconda linea di comando");
        fclose(file);
        return 4;
    }

    // Rimuovi il carattere newline alla fine delle linee di comando
    linea1[strcspn(linea1, "\n")] = '\0';
    linea2[strcspn(linea2, "\n")] = '\0';

    // Crea una pipe
    int pipe_fd[2];
    if (pipe(pipe_fd) == -1) {
        perror("Errore nella creazione della pipe");
        fclose(file);
        return 5;
    }

    // Fork per eseguire il primo comando e inviarne l'output alla pipe
    pid_t pid = fork();
    if (pid == -1) {
        perror("Errore nella creazione del processo figlio");
        fclose(file);
        close(pipe_fd[0]);
        close(pipe_fd[1]);
        return 6;
    }

    if (pid == 0) {
        // Processo figlio
        close(pipe_fd[0]);  // Chiudi il lato di lettura della pipe nel processo figlio
        dup2(pipe_fd[1], STDOUT_FILENO);  // Collega stdout alla pipe
        close(pipe_fd[1]);  // Chiudi il lato di scrittura della pipe nel processo figlio

        // Esegui il primo comando
        execlp("/bin/sh", "/bin/sh", "-c", linea1, NULL);
        perror("Errore nell'esecuzione del primo comando");
        fclose(file);
        return 7;
    }

    // Processo padre

    // Aspetta che il processo figlio termini
    int status;
    waitpid(pid, &status, 0);

    // Verifica se il primo comando è stato eseguito correttamente
    if (WIFEXITED(status) && WEXITSTATUS(status) == 0) {
        // Esegui il secondo comando leggendo dall'altro lato della pipe
        close(pipe_fd[1]);  // Chiudi il lato di scrittura della pipe nel processo padre
        dup2(pipe_fd[0], STDIN_FILENO);  // Collega stdin alla pipe
        close(pipe_fd[0]);  // Chiudi il lato di lettura della pipe nel processo padre
        execlp("/bin/sh", "/bin/sh", "-c", linea2, NULL);
        perror("Errore nell'esecuzione del secondo comando");
        fclose(file);
        return 8;
    } else {
        fprintf(stderr, "Il primo comando non è stato eseguito correttamente\n");
        fclose(file);
        return 9;
    }

    fclose(file);
    return 0;
}
