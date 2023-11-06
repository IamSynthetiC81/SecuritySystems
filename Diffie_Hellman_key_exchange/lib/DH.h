#include <gmp.h>
#include <time.h>

#define RAND_LIMIT 20

gmp_randstate_t state;

/*
 * This function calculates the public key
 * 
 * @param public_key : The public key to be generated. Declare the 
 *                      variable first, then pass it as an arg here.
 * @param generator: The generator used for the key exchange
 * @param private number: The private number used for the key exchange
 * @param prime number: The prime number used for the key exchange 
 */
void calculate_public_key(mpz_t public_key, mpz_t generator, mpz_t private_number, mpz_t prime_number);
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
void calculate_secret_key(mpz_t secret_key, mpz_t public_key, mpz_t private_key, mpz_t prime_number);

/*
 * This function generates a prime number between 0 and 2^PRIME_LIMIT
 *
 * @param prime_number : The prime number to be generated. Declare the
 *                          variable first, then pass it as an arg here.
 */
void generate_prime_number(mpz_t prime_number);

/*
 * This function generates a base number between 0 and prime_number
 *
 * @param base_number : The base number to be generated. Declare the
 *                         variable first, then pass it as an arg here.
 * @param prime_number: The prime number used for the key exchange
 */
void generate_base_number(mpz_t base_number, mpz_t prime_number);