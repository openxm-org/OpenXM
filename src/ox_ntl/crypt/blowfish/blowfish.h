/* $OpenXM: OpenXM/src/ox_ntl/crypt/blowfish/blowfish.h,v 1.1 2004/07/22 12:12:05 iwane Exp $ */

#ifndef __BLOWFISH_H__
#define __BLOWFISH_H__

#include <inttypes.h>

typedef struct __blowfish_key_t {
	uint32_t p[18];
	uint32_t s[4 * 256];
} blowfish_key;


#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

void	blowfish_enc_i(const blowfish_key *, uint32_t, uint32_t, uint32_t *, uint32_t *);
void	blowfish_dec_i(const blowfish_key *, uint32_t, uint32_t, uint32_t *, uint32_t *);

void	blowfish_setkey(const unsigned char *, int, blowfish_key *);

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif /* __BLOWFISH_H__ */



