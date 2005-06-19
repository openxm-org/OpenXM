/* $OpenXM: OpenXM/src/ox_ntl/crypt/des/des3.c,v 1.1 2005/06/19 15:30:00 iwane Exp $ */
/*
 * Triple-DES
 *   see des.c
 */

#include <stdio.h>

#include "des3.h"
#include "block.h"

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#define BLOCK 8


/*===========================================================*
 * KEY STRUCT
 *===========================================================*/

int
des3_keychk(des3_key *key)
{
	int ret = 1;
	int i;

	for (i = 0; i < 3; i++) {
		ret = (ret && des_keychk(key->key + i));
	}

	return (ret);
}

void
des3_keyset(const unsigned char *key, des3_key *dkey)
{
	int i;

	for (i = 0; i < 3; i++) {
		des_keyset(key + 8 * i, dkey->key + i);
	}
}




/*===========================================================*
 * BASE CRYPTO
 *===========================================================*/
/**
 * input: data: 8 byte
 */
int
des3_enc_c(const des3_key *key, const unsigned char *data, unsigned char *enc)
{
	unsigned char b1[8];
	unsigned char b2[8];

	des_enc_c(key->key + 0, data, b1);
	des_dec_c(key->key + 1, b1, b2);
	des_enc_c(key->key + 2, b2, enc);

	return (0);
}


/**
 * input: data: 8 byte
 */
int
des3_dec_c(const des3_key *key, const unsigned char *enc, unsigned char *data)
{
	unsigned char b1[8];
	unsigned char b2[8];

	des_dec_c(key->key + 0, enc, b1);
	des_enc_c(key->key + 1, b1, b2);
	des_dec_c(key->key + 2, b2, data);

	return (0);
}

/*===========================================================*
 * CRYPTO: length of input data is "64 * n" byte
 *===========================================================*/
int
des3_enc_ecb(const des3_key *key, int len, const unsigned char *data, unsigned char *buf)
{
	int ret;

	ret = crypt_ecb((void *)key, BLOCK, len, data, buf, (void *)des3_enc_c);

	return (ret);
}


int
des3_dec_ecb(const des3_key *key, int len, const unsigned char *enc, unsigned char *data)
{
	int ret;

	ret = crypt_ecb((void *)key, BLOCK, len, enc, data, (void *)des3_dec_c);

	return (ret);
}



int
des3_enc_cbc(const des3_key *key, unsigned char *iv, int len,
    const unsigned char *data, unsigned char *buf)
{
	int ret;

	ret = crypt_enc_cbc((void *)key, BLOCK, iv, len, data, buf, (void *)des3_enc_c);

	return (ret);
}


int
des3_dec_cbc(const des3_key *key, unsigned char *iv, int len,
    const unsigned char *data, unsigned char *buf)
{
	int ret;

	ret = crypt_dec_cbc((void *)key, BLOCK, iv, len, data, buf, (void *)des3_dec_c);

	return (ret);
}


int
des3_enc_cfb(const des3_key *key, int bit,
    const unsigned char *iv, int len,
    const unsigned char *data, unsigned char *buf)
{
	int ret;

	ret = crypt_cfb((void *)key, 0, bit, BLOCK, iv, len, data, buf, (void *)des3_enc_c);

	return (ret);
}

int
des3_dec_cfb(const des3_key *key, int bit,
    const unsigned char *iv, int len,
    const unsigned char *data, unsigned char *buf)
{
	int ret;

	ret = crypt_cfb((void *)key, 1, bit, BLOCK, iv, len, data, buf, (void *)des3_enc_c);

	return (ret);
}


int
des3_ofb(const des3_key *key, int bit,
    const unsigned char *iv, int len,
    const unsigned char *data,
    unsigned char *buf)
{
	int ret;
	ret = crypt_ofb((void *)key, bit, BLOCK, iv, len, data, buf, (void *)des3_enc_c);

	return (ret);
}


