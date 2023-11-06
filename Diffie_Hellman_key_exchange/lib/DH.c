#include "DH.h"

#define BIT_CNT 64
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
    mpz_urandomb(prime_number, state, BIT_CNT);
    do{    
        mpz_nextprime (prime_number, prime_number);
    }while(mpz_probab_prime_p(prime_number, 25) == 0);
}

/*
 * This function generates a base number between 0 and prime_number
 *
 * @param base_number : The base number to be generated. Declare the
 *                         variable first, then pass it as an arg here.
 * @param prime_number: The prime number used for the key exchange
 */
void generate_base_number(mpz_t base_number, mpz_t prime_number){
    mpz_init(base_number);
    gmp_randinit_mt(state);
    gmp_randseed_ui(state, time(NULL)+2);
    mpz_urandomm(base_number, state, prime_number);
}
