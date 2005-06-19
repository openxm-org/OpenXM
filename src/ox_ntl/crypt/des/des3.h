/* $OpenXM$ */
/*
 * Triple-DES
 */


#ifndef __DES3_H__
#define __DES3_H__

#include "des.h"


typedef struct {
	des_key key[3];
} des3_key;


#ifdef __cplusplus
extern "C" {
#endif

int	des3_keychk(des3_key *);
void	des3_keyset(const unsigned char *key, des3_key *);

int	des3_enc_c(const des3_key *, const unsigned char *, unsigned char *);
int	des3_dec_c(const des3_key *, const unsigned char *, unsigned char *);


int	des3_enc_ecb(const des3_key *, int len, const unsigned char *, unsigned char *);
int	des3_dec_ecb(const des3_key *, int len, const unsigned char *, unsigned char *);

int	des3_enc_cbc(const des3_key *, const unsigned char *, int len, const unsigned char *, unsigned char *);
int	des3_dec_cbc(const des3_key *, unsigned char *, int len, const unsigned char *, unsigned char *);

int	des3_enc_cfb(const des3_key *, int bit, const unsigned char *iv, int len, const unsigned char *, unsigned char *);
int	des3_dec_cfb(const des3_key *, int bit, const unsigned char *iv, int len, const unsigned char *, unsigned char *);

int	des3_ofb(const des3_key *, int bit, const unsigned char *iv, int len, const unsigned char *, unsigned char *);

#ifdef __cplusplus
}
#endif

#endif /* __DES_H__ */

/* EOF */

