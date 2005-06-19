/* $OpenXM: OpenXM/src/ox_ntl/crypt/des/des.h,v 1.2 2004/09/20 00:10:24 iwane Exp $ */
/*
 * FIPS PUB 46-3
 *   DATA ENCRYPTION STANDARD
 */


#ifndef __DES_H__
#define __DES_H__

#include <inttypes.h>

typedef struct {
	union {
		uint32_t ui[2];
		unsigned char ks[8];
	} key;
} des_key;

#ifdef __cplusplus
extern "C" {
#endif

int	des_keychk(des_key *);
void	des_keyset(const unsigned char *key, des_key *);

int	des_enc_c(const des_key *, const unsigned char *, unsigned char *);
int	des_dec_c(const des_key *, const unsigned char *, unsigned char *);


int	des_enc_ecb(const des_key *, int len, const unsigned char *, unsigned char *);
int	des_dec_ecb(const des_key *, int len, const unsigned char *, unsigned char *);

int	des_enc_cbc(const des_key *, unsigned char *, int len, const unsigned char *, unsigned char *);
int	des_dec_cbc(const des_key *, unsigned char *, int len, const unsigned char *, unsigned char *);

int	des_enc_cfb(const des_key *, int bit, const unsigned char *iv, int len, const unsigned char *, unsigned char *);
int	des_dec_cfb(const des_key *, int bit, const unsigned char *iv, int len, const unsigned char *, unsigned char *);

int	des_ofb(const des_key *, int bit, const unsigned char *iv, int len, const unsigned char *, unsigned char *);

#ifdef __cplusplus
}
#endif

#endif /* __DES_H__ */
