/* $OpenXM$ */
/*
 * rfc 2144
 *  The CAST-128 Encryption Algorithm
 */

#ifndef __CAST_128_H__
#define __CAST_128_H__

#include <unistd.h>


typedef struct __cast128_key_t {
	int len;
	int loop;

	uint32_t key[4];

	uint32_t km[16];
	uint32_t kr[16];
} cast128_key;


void	cast128_keyset(const unsigned char *, int, cast128_key *);

void	cast128_enc_i(cast128_key *, uint32_t ml, uint32_t mr, uint32_t *el, uint32_t *er);
void	cast128_enc_c(cast128_key *, const unsigned char *, unsigned char *);

void	cast128_dec_i(cast128_key *, uint32_t ml, uint32_t mr, uint32_t *el, uint32_t *er);
void	cast128_dec_c(cast128_key *, const unsigned char *, unsigned char *);


#endif /* __CAST_128_H__ */


