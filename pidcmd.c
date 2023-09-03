#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <dirent.h>

int main(int argc, char* argv[]) {
    if (argc != 2) {
        fprintf(stderr, "Usage: %s <command>\n", argv[0]);
        return 1;
    }

    char* targetCmd = argv[1];
    size_t targetCmdLen = strlen(targetCmd);
    int processCount = 0;

    DIR* dir = opendir("/proc"); //apre la directory proc
    if (dir == NULL) {
        perror("Error opening /proc directory");
        return 1;
    }

    struct dirent* entry;
    while ((entry = readdir(dir)) != NULL) {
        // Verifica se l'entry è una directory numerica (rappresenta un processo)
        if (entry->d_type == DT_DIR && atoi(entry->d_name) > 0) {
            char cmdPath[256];
            snprintf(cmdPath, sizeof(cmdPath), "/proc/%s/cmdline", entry->d_name);

            FILE* cmdFile = fopen(cmdPath, "r");
            if (cmdFile != NULL) {
                char cmdLine[256];
                fgets(cmdLine, sizeof(cmdLine), cmdFile);
                fclose(cmdFile);

                // Rimuovi il terminatore nullo aggiunto da fgets
                cmdLine[strlen(cmdLine) - 1] = '\0';

                // Confronta la riga di comando con il comando target
                if (strcmp(cmdLine, targetCmd) == 0) {
                    printf("PID: %s\n", entry->d_name);
                    processCount++;
                }
            }
        }
    }

    closedir(dir);

    if (processCount == 0) {
        printf("Nessun processo trovato con il comando: %s\n", targetCmd);
    } else {
        printf("Totale processi trovati: %d\n", processCount);
    }

    return 0;
}
