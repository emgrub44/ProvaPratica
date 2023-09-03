#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <errno.h>

int main(int argc, char* argv[]) {
    //execl("/sbin/ping", "ping", "google.com", NULL);
    execlp("ping", "ping", "google.com", NULL); // execlp -> prende da solo il path, non c'è bisogno di specificarlo

   /* char* arr[] = {
        "ping",
        "google.com",
        NULL
    };*/
    //execvp("ping", arr);
    // Se execl() ritorna, si è verificato un errore
    //Exec -> esegue solo il programma specificato, dopo non riprende il controllo dal programma corrente
    perror("execl");

    int err = errno;

    printf("Ping finished executing\n");

    return 1;
}
