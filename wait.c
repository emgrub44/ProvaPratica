
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <time.h>
#include <stdio.h>


int main(int argc, char* argv[]){
    int id = fork();
    int n;
    if(id == 0){
        n = 1;
    } else {
        wait(NULL); //se non mettiamo la wait non abbiamo alcun ordine di precedenza tra i processi, quindi non stampano le cose in ordine
        //se mettiamo la wait al padre allora aspetterà che il figlio finisca e poi parte
        n = 6;
    }

    for(int i = n; i < n + 5; i++){
        printf("%d", i);
        fflush(stdout); //ogni volta che deve printare qualcosa viene immediatamente stampata sullo schermo
    }
    return 0;
}