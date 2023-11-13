#include <stdio.h>
#include <stdlib.h>
#include <string.h>

int main(int argc, char *argv[]){

    char message[] = "Hello World !!!\n";
    char *buffer = (char *)malloc(sizeof(char) * 1024);

    FILE *fp = fopen("test.txt", "a+");
    if (fp == NULL){
        perror("fopen");
        exit(EXIT_FAILURE);
    }

    printf("Got here\n");

    // write to file "Hello World !!!"
    fwrite(message,sizeof(char) , strlen(message), fp);

    fseek(fp, 0, SEEK_SET);

    // read from file
    printf("We read : \n");
    while(fread(buffer, sizeof(char), strlen(message), fp) != 0){
        if (feof(fp)){
            break;
        }
        printf("%s", buffer);
    }

    free(buffer);

    fclose(fp);

    return 0;
}