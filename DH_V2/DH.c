#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <sys/types.h>
#include <unistd.h>
#include <gmp.h>
#include <assert.h>
#include <sys/time.h>

#define NUM_THREADS 2
#define RAND_LIMIT 20

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

/* File where output is written to  */
FILE *fp;

/*
 * This function calculates the public key
 * 
 * @param public_key : The public key to be generated. Declare the 
 *                      variable first, then pass it as an arg here.
 * @param generator: The generator used for the key exchange
 * @param private number: The private number used for the key exchange
 * @param prime number: The prime number used for the key exchange 
 */
void calculate_public_key(mpz_t public_key, mpz_t generator, mpz_t private_number, mpz_t prime_number){
    mpz_init(public_key);
    mpz_powm(public_key, generator, private_number, prime_number);
}

/*
 * This function calculates the secret key
 *
 * @param secret key : The secret key to be calculated. Declare the 
 *                      variable first, than pass it as an arg here.
 * @param public key: The public key of the other user
 * @param private key: The private key of the current user
 * @param prime number: The prime number used for the key exchange
 * @return: The secret key
 */
void calculate_secret_key(mpz_t secret_key, mpz_t public_key, mpz_t private_key, mpz_t prime_number){
    mpz_init(secret_key);
   
    mpz_powm(secret_key, public_key, private_key, prime_number);
}

/*
 * This function generates a prime number between 0 and 2^PRIME_LIMIT
 *
 * @param prime_number : The prime number to be generated. Declare the
 *                          variable first, then pass it as an arg here.
 */
void generate_prime_number(mpz_t prime_number){
    mpz_init(prime_number);

    gmp_randinit_mt(state);
    gmp_randseed_ui(state, time(NULL)+1);
    mpz_urandomb(prime_number, state, RAND_LIMIT);
    do{    
        mpz_nextprime (prime_number, prime_number);
    }while(mpz_probab_prime_p(prime_number, 25) == 0);
}

int main( int argc, char *argv[]) {
    /* Random Number Generator Init*/
    gmp_randinit_mt(state);

    int p = 0, g = 0, a = 0, b = 0, h = 0;

    for (int i = 1; i < argc; i++ ){
        char* arg = argv[i];

        if (strcmp(arg, "-o") == 0){
            fp = fopen(argv[++i], "a");
            if (fp == NULL){
                printf("Error opening file\n");
                exit(1);
            }
        } else if (strcmp(arg, "-p") == 0){
            assert (i+1 <= argc && "Missing argument for -p");
            p = ++i;
        } else if (strcmp(arg, "-g") == 0){
            assert (i+1 <= argc && "Missing argument for -g");
            g = ++i;
        } else if (strcmp(arg, "-a") == 0){
            assert (i+1 <= argc && "Missing argument for -a");
            a = ++i;
        } else if (strcmp(arg, "-b") == 0){
            assert (i+1 <= argc && "Missing argument for -b");
            b = ++i;
        } else if (strcmp(arg, "-h") == 0){
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
        } else {
            printf("Invalid argument: %s\n", arg);
            exit(1);
        }
    }

    /* Init prime number */
    mpz_t prime_number; mpz_init(prime_number);
    if (p == 0) {
        generate_prime_number(prime_number);
        logmpzv("\tGenerated prime number %Zd\n", prime_number);
    } else {
        mpz_set_str(prime_number, argv[p], 10);
        logmpzv("\tSelected prime number %Zd\n", prime_number);
    }

    mpz_t rand_limit; mpz_init_set_ui(rand_limit, RAND_LIMIT);

    /* Init base number */
    mpz_t generator; mpz_init(generator); 
    if (g == 0){
        gmp_randseed_ui(state,time(NULL)+2);
        mpz_urandomm(generator, state, rand_limit-1);
        mpz_add_ui(generator, generator, 1); 
        logmpzv("\tGenerated base number %Zd\n", generator);
    } else {
        mpz_set_str(generator, argv[g], 10);
        logmpzv("\tSelected base number %Zd\n", generator);
    }

    mpz_t host_private_number; mpz_init(host_private_number);
    mpz_t client_private_number; mpz_init(client_private_number);

    /*  Set _bobs_ private key  */
    if(b > 0 ){
        mpz_set_str(client_private_number, argv[a], 10);
        logmpzv("\tSelected client private number %Zd\n", client_private_number);
    } else {
        gmp_randseed_ui(state, time(NULL)+b+2);
        mpz_urandomm(client_private_number, state, prime_number);
        // mpz_add_ui(host_private_number,host_private_number, 1);
        logmpzv("\tGenerated client private number %Zd\n", client_private_number)
    }

    /*  Set _Alices_ private key    */
    if(a > 0 ){
        mpz_set_str(host_private_number, argv[a], 10);
        logmpzv("\tSelected host private number %Zd\n", host_private_number)
    }else{
        gmp_randseed_ui(state, time(NULL)+a+1);
        mpz_urandomm(host_private_number, state, prime_number);
        // mpz_add_ui(host_private_number,host_private_number, );
        logmpzv("\tGenerated host private number %Zd\n", host_private_number)
    }
    
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

    /* Cleaning Up              */
    mpz_clears(prime_number,generator, host_private_number, client_private_number, rand_limit, NULL);

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
