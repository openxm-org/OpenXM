/* $OpenXM$ */

#ifndef __BLOWFISH_H__
#define __BLOWFISH_H__

#include <unistd.h>

typedef struct __blowfish_key_t {
	uint32_t p[18];
	uint32_t s[4 * 256];
} blowfish_key;


void	blowfish_enc_i(const blowfish_key *, uint32_t, uint32_t, uint32_t *, uint32_t *);
void	blowfish_dec_i(const blowfish_key *, uint32_t, uint32_t, uint32_t *, uint32_t *);

void	blowfish_setkey(const unsigned char *, int, blowfish_key *);

#endif /* __BLOWFISH_H__ */



