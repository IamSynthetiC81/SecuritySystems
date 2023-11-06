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

/* Mutex decleration */
pthread_mutex_t WRITING_MESSAGE = PTHREAD_MUTEX_INITIALIZER;
pthread_mutex_t READING_MESSAGE = PTHREAD_MUTEX_INITIALIZER;
/* Condvar decleration */
pthread_cond_t MESSAGE_SENT = PTHREAD_COND_INITIALIZER;
pthread_cond_t MESSAGE_READ = PTHREAD_COND_INITIALIZER;

unsigned int messages_pending = 0;
unsigned int message_sig = -1;

/*  Pipe where communication happens */
int pipefd[2];

/* File where output is written to  */
FILE *fp;

/**
 * This struct is used to pass arguments to the threads.
*/
typedef struct comm_op_args
{
    mpz_t* prime_number;
    mpz_t* generator;
    mpz_t* private_number;
    char *name;
    unsigned int parent;
} fdata_t;

mpz_t prime_number;
mpz_t generator;
mpz_t bit_count;
mpz_t client_private_number; 
mpz_t host_private_number;

/**
 * This function runs on a thread and is responsible for sending data.
 *
 * @param args : Arguments are pass in the form of a (void *) pointer to 
 *               a struct of type (fdata_t).
*/
void *WriterFunc(void* args){
    fdata_t* data = (fdata_t*)args;
    mpz_t* prime_number = data->prime_number;
    mpz_t* generator = data->generator;
    unsigned int parent = data->parent;

    pthread_mutex_lock(&WRITING_MESSAGE);

    while(messages_pending != 0){
        pthread_mutex_unlock(&WRITING_MESSAGE);
        logf("     -->[{%u} | %s writer is going to sleep]\n", parent, data->name);

        struct timespec ts;
        struct timeval tv;

        gettimeofday(&tv, NULL);

        ts.tv_sec = tv.tv_sec;
        ts.tv_nsec = tv.tv_usec * 1000 + 500;
        pthread_cond_timedwait(&MESSAGE_READ, &WRITING_MESSAGE, &ts);
    }

    logf("     -->[{%u} | %s writer is waking up]\n", parent, data->name);
    
    mpz_t public_key; calculate_public_key(public_key, *generator, *data->private_number, *prime_number);
    char public_key_str[100]; mpz_get_str(public_key_str, 10, public_key);

    logv("\t[{%u} | %s sent public key: %s]\n", parent, data->name, public_key_str);
    write(pipefd[1], public_key_str, 100);
    
    message_sig = parent;
    messages_pending++;

    if(fp != NULL){
        fprintf(fp, "%s, ", public_key_str);
    }

    pthread_cond_broadcast(&MESSAGE_SENT);
    pthread_mutex_unlock(&WRITING_MESSAGE);
    

    return NULL;
}

/**
 * This function runs on a thread and is responsible for reading data.
 *
 * @param args : Arguments are pass in the form of a (void *) pointer to 
 *               a struct of type (fdata_t).
*/
void *ReaderFunc(void* args){
    int key_size = 100;

    fdata_t *data = (fdata_t *)args;
    unsigned int parent = data->parent;

    pthread_mutex_lock(&READING_MESSAGE);

    while(messages_pending == 0 || message_sig == parent){
        pthread_mutex_unlock(&READING_MESSAGE);
        logf("     -->[{%u} | %s reader is going to sleep]\n", parent, data->name);

        struct timespec ts;
        struct timeval tv;

        gettimeofday(&tv, NULL);

        ts.tv_sec = tv.tv_sec;
        ts.tv_nsec = tv.tv_usec * 1000 + 500;

        pthread_cond_timedwait(&MESSAGE_SENT, &READING_MESSAGE,&ts);
    }
    logf("     -->[{%u} | %s reader is waking up]\n", parent, data->name);

    char *client_key = (char *)malloc(key_size*sizeof(char));

    read(pipefd[0], client_key, key_size);
    logv("\t[{%u} | %s Received public key: %s]\n",parent, data->name, client_key);
    
    --messages_pending;
    message_sig = parent;

    pthread_cond_broadcast(&MESSAGE_READ);
    pthread_mutex_unlock(&READING_MESSAGE);

    pthread_exit(client_key);
}

/**
 * This function runs on a thread and is responsible for the communication between the two users.
 * 
 * @param args : Arguments are passed in the form of a (void *) pointer to
 *              a struct of type (fdata_t).
 * @return : The secret key
*/
void *Comm_Init(void* args){
    unsigned int pid = pthread_self();
    fdata_t *data = (fdata_t *)args;

    void *vptr_return;

    data->parent = pid;

    pthread_t Writer, Reader;
    if(pthread_create(&Writer, NULL, WriterFunc, (void*)data) != 0){ 
        printf("Error creating thread\n");
        exit(1);
    }
    if(pthread_create(&Reader, NULL, ReaderFunc, (void*)data) != 0){ 
        printf("Error creating thread\n");
        exit(1);
    }

    pthread_join(Reader, &vptr_return);
    pthread_join(Writer, NULL);

    mpz_t public_key; mpz_init(public_key); mpz_set_str(public_key, (char *)vptr_return, 10);
    mpz_t *secret_key; secret_key = (mpz_t*)malloc(sizeof(mpz_t));

    calculate_secret_key(*secret_key, public_key, *data->private_number, *data->prime_number);
    

    logmpzv("\tSecret key for thread[%u]: %Zd\n",pid, *secret_key);

    pthread_exit(secret_key);
}

void *WriteToFile(void *arg){
    fp = fopen(arg, "a");
    if (fp == NULL){
        printf("Error opening file\n");
        exit(1);
    }
}

void *GeneratePrimeNumber(void *arg){
    mpz_init(prime_number);

    mpz_set_str(prime_number, (char*)arg , 10);

    logmpzv("\tSelected prime number %Zd\n", prime_number);
}

void *GenerateBaseNumber(void *arg){
    mpz_init(generator); 

    mpz_set_str(generator, (char*)arg, 10);

    logmpzv("\tSelected base number %Zd\n", generator);
}

void *HostPrivateKey(void *arg){
    mpz_init(host_private_number);

    mpz_set_str(host_private_number, arg, 10);

    logmpzv("\tSelected client private number %Zd\n", host_private_number);
}

void *ClientPrivateKey(void *arg){
    mpz_init(client_private_number);

    mpz_set_str(client_private_number, arg, 10);

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

    /*  Init arguments for Host */
    fdata_t *HostArgs = (fdata_t *)malloc(sizeof(fdata_t));
    HostArgs->prime_number = &prime_number;
    HostArgs->generator = &generator;
    HostArgs->name = "Host";
    HostArgs->parent = pthread_self();
    HostArgs->private_number = &host_private_number;
    
    /*  Init arguments for Client */
    fdata_t *ClientArgs = (fdata_t *)malloc(sizeof(fdata_t));
    ClientArgs->prime_number = &prime_number;
    ClientArgs->generator = &generator;
    ClientArgs->name = "Client";
    ClientArgs->parent = pthread_self();
    ClientArgs->private_number = &client_private_number;

    /* Pipe init */
    if(pipe(pipefd) < 0) exit(1);                                                               // Create pipe

    pthread_t client_thread, host_thread;                                                       // Threads                               
    void *vptr_host_return, *vptr_client_return;                                                // Return values from threads

    /* Create Threads           */
    pthread_create(&client_thread, NULL, Comm_Init, (void*)ClientArgs);
    pthread_create(&host_thread, NULL, Comm_Init, (void *)HostArgs);
    
    /* Wait for communication   */
    pthread_join(host_thread, &vptr_host_return);
    pthread_join(client_thread, &vptr_client_return);

    /* Cleaning Up              */
    close(pipefd[0]);
    close(pipefd[1]);

    free(ClientArgs);
    free(HostArgs);

    mpz_clears(prime_number,generator, host_private_number, client_private_number, bit_count, NULL);

    /*  Compare keys            */
    mpz_t host_key; mpz_init(host_key); mpz_set(host_key, vptr_host_return);
    mpz_t client_key; mpz_init(client_key); mpz_set(client_key, vptr_client_return);

    int result = mpz_cmp(host_key, client_key);
    if (fp != NULL && result == 0){
        gmp_fprintf(fp, "%Zd\n", host_key);
        fclose(fp);
    }
    mpz_clears(host_key, client_key, NULL);

    if (result == 0){
            printf("Keys match\n");
            
        return 0;
    } else {
            printf("Keys do not match\n");
        return -1;
    }
}