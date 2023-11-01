#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <pthread.h>
#include <sys/types.h>
#include <unistd.h>
#include <gmp.h>
#include <assert.h>

#define NUM_THREADS 2
#define RAND_LIMIT 50

gmp_randstate_t state;

pthread_mutex_t WRITING_MESSAGE = PTHREAD_MUTEX_INITIALIZER;
pthread_mutex_t READING_MESSAGE = PTHREAD_MUTEX_INITIALIZER;
pthread_cond_t MESSAGE_SENT = PTHREAD_COND_INITIALIZER;
pthread_cond_t MESSAGE_READ = PTHREAD_COND_INITIALIZER;

unsigned int messages_pending = 0;
unsigned int message_sig = -1;

int pipefd[2];

FILE *fp;

typedef struct ClientFuncArgs
{
    mpz_t* prime_number;
    mpz_t* generator;
    mpz_t* private_number;
    char *name;
    unsigned int parent;
} fdata_t;


/*
 * This function calculates the private key
 * 
 * @param generator: The generator used for the key exchange
 * @param private number: The private number used for the key exchange
 * @param prime number: The prime number used for the key exchange 
 * @return: The private key
 */
void calculate_public_key(mpz_t public_key, mpz_t generator, mpz_t private_number, mpz_t prime_number){
    mpz_init(public_key);

    mpz_t gen; mpz_init_set(gen, generator);

    mpz_powm(public_key, gen, private_number, prime_number);
}

/*
 * This function calculates the secret key
 *
 * @param public key: The public key of the other user
 * @param private key: The private key of the current user
 * @param prime number: The prime number used for the key exchange
 * @return: The secret key
 */
void calculate_secret_key(mpz_t secret_key, mpz_t public_key, mpz_t private_key, mpz_t prime_number){
    mpz_init(secret_key);

    gmp_printf  ("Public key: %Zd\n", public_key);
    gmp_printf  ("Private key: %Zd\n", private_key);
    gmp_printf  ("Prime number: %Zd\n", prime_number);
    
    mpz_powm(secret_key, public_key, private_key, prime_number);

    gmp_printf("Secret key: %Zd\n", secret_key);
}

/*
 * This function generates a prime number between 0 and 2^PRIME_LIMIT
 *
 * @param tid: an integer is used to throw off the random number generator seed.
 * @return: The prime number
 */
void generate_prime_number(mpz_t prime_number){
    mpz_init(prime_number);

    gmp_randinit_mt(state);
    gmp_randseed_ui(state, time(NULL));
    mpz_urandomb(prime_number, state, RAND_LIMIT);
    do{    
        mpz_nextprime (prime_number, prime_number);
    }while(mpz_probab_prime_p(prime_number, 25) == 0);
}

void *WriterFunc(void* args){
    fdata_t* data = (fdata_t*)args;
    mpz_t* prime_number = data->prime_number;
    mpz_t* generator = data->generator;
    unsigned int parent = data->parent;

    pthread_mutex_lock(&WRITING_MESSAGE);

    while(messages_pending != 0){
        pthread_mutex_unlock(&WRITING_MESSAGE);
        pthread_cond_wait(&MESSAGE_READ, &WRITING_MESSAGE);
    }
    

    mpz_t public_key; calculate_public_key(public_key, *generator, *data->private_number, *prime_number);
    char public_key_str[100]; mpz_get_str(public_key_str, 2, public_key);

    printf("\t\t%u sent public key: %s\n", parent, public_key_str);

    write(pipefd[1], public_key_str, 100);
    message_sig = parent;

    messages_pending++;

    pthread_cond_broadcast(&MESSAGE_SENT);
    pthread_mutex_unlock(&WRITING_MESSAGE);
    

    return NULL;
}

void *ReaderFunc(void* args){
    int key_size = 100;

    fdata_t *data = (fdata_t *)args;
    unsigned int parent = data->parent;

    pthread_mutex_lock(&READING_MESSAGE);

    while(messages_pending == 0 || message_sig == parent){
        pthread_mutex_unlock(&READING_MESSAGE);
        pthread_cond_wait(&MESSAGE_SENT, &READING_MESSAGE);
    }

    char *client_key = (char *)malloc(key_size*sizeof(char));

    read(pipefd[0], client_key, key_size);
    printf("\t\t%u Received public key: %s\n",parent, client_key);
    
    --messages_pending;
    message_sig = parent;

    pthread_cond_broadcast(&MESSAGE_READ);
    pthread_mutex_unlock(&READING_MESSAGE);

    pthread_exit(client_key);
}

void *Comm_Init(void* args){
    unsigned int pid = pthread_self();
    fdata_t *data = (fdata_t *)args;

    data->parent = pid;

    mpz_t private_number; mpz_init(private_number);
    mpz_urandomm(private_number, state, *data->prime_number);

    data->private_number = &private_number;

    pthread_t Writer, Reader;
    if(pthread_create(&Writer, NULL, WriterFunc, (void*)data) != 0){ 
        printf("Error creating thread\n");
        exit(1);
    }
    if(pthread_create(&Reader, NULL, ReaderFunc, (void*)data) != 0){ 
        printf("Error creating thread\n");
        exit(1);
    }

    void *vptr_return;
    printf("all is well here\n");
    mpz_t public_key; mpz_init(public_key); mpz_set_str(public_key, (char *)vptr_return, 10);
    printf("all is well here as well\n");
    mpz_t secret_key; mpz_init(secret_key);
    printf("we are out\n");

    pthread_join(Reader, &vptr_return);
    pthread_join(Writer, NULL);



    gmp_printf("Public key: %Zd\n", public_key);

    calculate_secret_key(secret_key, public_key, *data->private_number, *data->prime_number);
}


int main( int argc, char *argv[]) {
    /* Random Number Generator Init*/
    gmp_randinit_mt(state);
    gmp_randseed_ui(state,time(NULL));

    int p = 0, g = 0, a = 0, b = 0, h = 0, w = 0;

    for (int i = 1; i < argc; i++ ){
        char* arg = argv[i];
        
        printf("arg: [%s]\n", arg);

        if (strcmp(arg, "-o") == 0){

            printf("it thinks its -o");

            fp = fopen(argv[++i], "w");
            if (fp == NULL){
                printf("Error opening file\n");
                exit(1);
            }
            w = 1;

        } else if (strcmp(arg, "-p") == 0){

            printf("it thinks its -p\n");

            assert (i+1 <= argc && "Missing argument for -p");
            p = ++i;
            printf("p: %s\n", argv[p]);
        } else if (strcmp(arg, "-g\n") == 0){

            printf("it thinks its -g");

            assert (i+1 <= argc && "Missing argument for -g");
            g = ++i;
            printf("g: %s\n", argv[g]);
        } else if (strcmp(arg, "-a\n") == 0){

            printf("it thinks its -a");

            assert (i+1 <= argc && "Missing argument for -a");
            a = ++i;
        } else if (strcmp(arg, "-b") == 0){

            printf("it thinks its -b\n");

            assert (i+1 <= argc && "Missing argument for -b");
            b = ++i;
        } else if (strcmp(arg, "-h") == 0){
            /* TODO : PRINT HELP */
        } else {
            printf("Invalid argument: %s\n", arg);
            exit(1);
        }
       
    }

    mpz_t prime_number; mpz_init(prime_number);
    if (p == 0) {
        generate_prime_number(prime_number);
    } else {
        mpz_set_str(prime_number, argv[p], 10);
    }

    mpz_t rand_limit; mpz_init_set_ui(rand_limit, RAND_LIMIT);

    /* Generate Generator */
    mpz_t generator; mpz_init(generator); 
    if (g == 0){
        mpz_urandomm(generator, state, rand_limit-1);
        mpz_add_ui(generator, generator, 1); 
    } else {
        mpz_set_str(generator, argv[g], 10);
    }
    


    fdata_t *ClientArgs = (fdata_t *)malloc(sizeof(fdata_t));
    ClientArgs->prime_number = &prime_number;
    ClientArgs->generator = &generator;
    ClientArgs->name = "Client";
    ClientArgs->parent = pthread_self();

    fdata_t *HostArgs = (fdata_t *)malloc(sizeof(fdata_t));
    HostArgs->prime_number = &prime_number;
    HostArgs->generator = &generator;
    HostArgs->name = "Host";
    HostArgs->parent = pthread_self();

    gmp_printf("Prime number: %Zd\n", prime_number);
    gmp_printf("Generator: %Zd\n\n", generator);

    if(pipe(pipefd) < 0) exit(1);

    pthread_t client_thread, host_thread;

    pthread_create(&client_thread, NULL, Comm_Init, (void*)ClientArgs);
    pthread_create(&host_thread, NULL, Comm_Init, (void *)HostArgs);
    
    pthread_join(host_thread, NULL);
    pthread_join(client_thread, NULL);

    close(pipefd[0]);
    close(pipefd[1]);
}
