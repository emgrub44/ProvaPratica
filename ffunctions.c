#include <stdio.h>

int main(int argc, char* argv[]){

    char* first_file = argv[1];
    FILE *f = fopen(first_file, "r" );
    if(f == NULL){
        printf("%s does not exist\n",argv[1]);
    } else {
        printf("Success!\n");
    }

    int c;
    while((c = fgetc(f)) != EOF){}

    char* second_file = argv[2];
    FILE *g = fopen(second_file, "wr");
    char* s = "stringa prova";
    int s1 = fputs(s, g);
    if( s1 == -1){
        printf("Error while trying to put the string\n");
    } else {
        printf("Success\n");
    }
    char s2[8];
    char* ret;

    ret = fgets(s2, 8, g);
    printf("%s\n", s2);
    fclose(f);
    fclose(g);
    return 0;
}