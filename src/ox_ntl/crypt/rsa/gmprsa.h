/* $OpenXM$ */

#ifndef __GMP_RSA_H__
#define __GMP_RSA_H__


#include <gmp.h>


typedef struct rsa_key_tag {
	mpz_t p; /* prime */
	mpz_t q; /* prime */
	mpz_t mod; /* = p * q */
	mpz_t private_key;
	mpz_t public_key;
	int k;
} rsa_key;


#define RSA_PKCS_1_PADDING           (0x00000001)

#define RSA_PKCS_1_PADDING_PRIVATE0  (0x00)
#define RSA_PKCS_1_PADDING_PRIVATE   (0x01)
#define RSA_PKCS_1_PADDING_PUBLIC    (0x02)

#ifdef __cplusplus
extern "C" {
#endif


void rsa_init(rsa_key *r);
void rsa_clear(rsa_key *r);

/*
 * the probability of a false positive is (1/4) ** reps.
 * see mpz_probab_prime_p(GMP)
 */
void rsa_genprime(mpz_ptr prime, int len, const mpz_ptr seed, int rep);

/*
 * bit >= 96 (MUST)
 * rep >= 80(SHOULD) : see rsa_genprime().
 */
int rsa_keygen(rsa_key *, const mpz_ptr seed1, const mpz_ptr seed2, unsigned int bit, int rep);

void rsa_set_publickey(rsa_key *rsa, mpz_ptr public_key, mpz_ptr mod);
void rsa_set_key(rsa_key *rsa, mpz_ptr public_key, mpz_ptr private_key, mpz_ptr mod);

int rsa_encrypt_by_public_key(rsa_key *, unsigned char *eb, const unsigned char *msg, int len, int);
int rsa_encrypt_by_private_key(rsa_key *, unsigned char *eb, const unsigned char *msg, int len, int);

int rsa_decrypt_by_public_key(rsa_key *, unsigned char *buf, const unsigned char *eb, int);
int rsa_decrypt_by_private_key(rsa_key *, unsigned char *buf, const unsigned char *eb, int);


#ifdef __cplusplus
}
#endif


#endif
