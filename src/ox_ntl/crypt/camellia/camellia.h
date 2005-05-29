/* $OpenXM$ */
/*
 * Camellia
 * http://info.isl.ntt.co.jp/crypt/camellia/index.html
 */

#ifndef __CAMELLIA_H__
#define __CAMELLIA_H__

#include <inttypes.h>


typedef struct __camellia_key_t {
	int len;
	uint32_t key[8];
	uint32_t k[24 * 2];
	uint32_t kw[4 * 2];
	uint32_t kl[6 * 2];

} camellia_key;


#ifdef __cplusplus
extern "C" {
#endif

int	camellia_keyset(const unsigned char *, int, camellia_key *);

void camellia_enc(camellia_key *ckey, const uint8_t *msg, uint8_t *e);
void camellia_dec(camellia_key *ckey, const uint8_t *msg, uint8_t *e);


#ifdef __cplusplus
}
#endif

#endif /* __CAMELLIA_H__ */


