#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>


int main(int argc, char* argv[]){
    int pid = fork(); //da quando chiamo la fork ho due istanze del mio programma, per questo viene stampato due volte Hello World
    if(pid != 0){
        fork();
    }
    printf("Hello World\n");
    return 0;
}