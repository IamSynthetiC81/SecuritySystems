#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <pthread.h>
#include <sys/types.h>
#include <unistd.h>
#include <gmp.h>
#include <assert.h>
#include <sys/time.h>

#include "lib/DH.h"
#include "lib/parser.h"

#define NUM_THREADS 2
#define BIT_CNT 64

#ifdef DEBUG
    #define logf(...) printf( __VA_ARGS__)
    #define logmpz(...) gmp_printf( __VA_ARGS__);
#else
    #define logf(...) 
    #define logmpz(...) 
#endif

#ifdef VERBOSE
    #define logv(...) printf( __VA_ARGS__)
    #define logmpzv(...) gmp_printf( __VA_ARGS__);
#else
    #define logv(...)
    #define logmpzv(...)
#endif

gmp_randstate_t state;

unsigned int messages_pending = 0;
unsigned int message_sig = -1;

/*  Pipe where communication happens */
int pipefd[2];

/* File where output is written to  */
FILE *fp;

mpz_t prime_number;
mpz_t generator;
mpz_t bit_count;
mpz_t client_private_number; 
mpz_t host_private_number;

void *WriteToFile(void *arg){
    printf("Writing to file %s\n", (char*)arg);

    fp = fopen((char*)arg, "a");
    if (fp == NULL){
        printf("Error opening file\n");
        exit(1);
    }
}

void *GeneratePrimeNumber(void *arg){
    mpz_init(prime_number);

    mpz_set_ui(prime_number, (long)arg);

    logmpzv("\tSelected prime number %Zd\n", prime_number);
}

void *GenerateBaseNumber(void *arg){
    mpz_init(generator);

    mpz_set_ui(generator, (long)arg);

    logmpzv("\tSelected base number %Zd\n", generator);
}

void *HostPrivateKey(void *arg){
    mpz_init(host_private_number);

    mpz_set_ui(host_private_number, (long)arg);

    logmpzv("\tSelected client private number %Zd\n", host_private_number);
}

void *ClientPrivateKey(void *arg){
    mpz_init(client_private_number);

    mpz_set_ui(client_private_number, (long)arg);

    logmpzv("\tSelected client private number %Zd\n", client_private_number);
}

void *PrintHelp(){
    FILE *help = fopen("etc/help.txt", "r");
        
    if (help == NULL){
        printf("Error opening help file\n");
        exit(1);
    }
    char c;
    while((c = fgetc(help)) != EOF){
        printf("%c", c);
    }
    fclose(help);
    exit(1);
}

int main( int argc, char *argv[]) {
    /* Random Number Generator Init*/
    gmp_randinit_mt(state);
    gmp_randseed_ui(state, time(NULL) + pthread_self());


    command *commands[] = {
        &(command){.command = "-o", .includedParam = 1, .NumericParam = 0 , .func = WriteToFile},
        &(command){.command = "-p", .includedParam = 1, .NumericParam = 1 , .func = GeneratePrimeNumber},
        &(command){.command = "-g", .includedParam = 1, .NumericParam = 1 , .func = GenerateBaseNumber},
        &(command){.command = "-a", .includedParam = 1, .NumericParam = 1 , .func = HostPrivateKey},
        &(command){.command = "-b", .includedParam = 1, .NumericParam = 1 , .func = ClientPrivateKey},
        &(command){.command = "-h", .includedParam = 0, .NumericParam = 0 , .func = PrintHelp}
    };

    parse(argc, argv, commands);

    mpz_cmp_ui(prime_number, 0) == 0 ? generate_prime_number(prime_number) : 0;
    mpz_cmp_ui(generator, 0) == 0 ? generate_base_number(generator, prime_number) : 0;
    mpz_cmp_ui(host_private_number, 0) == 0 ? mpz_urandomb(host_private_number, state, BIT_CNT) : 0;
    mpz_cmp_ui(client_private_number, 0) == 0 ? mpz_urandomb(client_private_number, state, BIT_CNT) : 0;

    /* print all values */    
    mpz_t client_public_key; mpz_init(client_public_key);
    mpz_t host_public_key; mpz_init(host_public_key);

    calculate_public_key(host_public_key, generator, host_private_number, prime_number);
    calculate_public_key(client_public_key, generator, client_private_number, prime_number);

    mpz_t host_secret_key; mpz_init(host_secret_key); 
    mpz_t client_secret_key; mpz_init(client_secret_key);

    calculate_secret_key(client_secret_key, host_public_key, client_private_number, prime_number);
    calculate_secret_key(host_secret_key, client_public_key, host_private_number, prime_number);   

    logmpzv("\tSecret key for host: %Zd\n", host_secret_key);
    logmpzv("\tSecret key for client: %Zd\n", client_secret_key);   


    /* Log to file */
    if (fp != NULL){
        gmp_fprintf(fp, "%Zd , %Zd , ", host_public_key, client_public_key);
    }

    /* Cleaning Up              */
    mpz_clears(prime_number,generator, host_private_number, client_private_number, bit_count, NULL);

    /*  Compare keys */
    int result = mpz_cmp(host_secret_key, client_secret_key);
    if (fp != NULL && result == 0){
        gmp_fprintf(fp, "%Zd\n", host_secret_key);
        fclose(fp);
    }
    mpz_clears(host_secret_key, client_secret_key, NULL);

    if (result == 0){
            printf("Keys match\n"); 
        return 0;
    } else {
            printf("Keys do not match\n");
        return -1;
    }
}
