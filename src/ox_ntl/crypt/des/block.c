/* $OpenXM$ */

#include <stdlib.h>
#include <stdio.h>
#include <string.h>


#define CRYPTO_INPUT_CHECK 1

#include "block.h"

int
crypt_ecb(void *key, int block, int len,
    const unsigned char *data, unsigned char *buf,
   void *func)
{
	int i, ret;
	int (*crypto)(void *key, const unsigned char *, unsigned char *) =
	    (int (*)(void *, const unsigned char *, unsigned char *))func;

#if CRYPTO_INPUT_CHECK 
	if (len % block != 0)
		return (-1);
#endif

	for (i = 0; i < len / block; i++) {
		ret = crypto(key, data + i * block, buf + i * block);
	}

	return (0);
}



int
crypt_enc_cbc(void *key, int block, const unsigned char *iv, int len,
    const unsigned char *data, unsigned char *buf, void *func)
{
	int i, j, ret;
	unsigned char xor[1024];
	const unsigned char *ivp;
	int (*crypto)(void *key, const unsigned char *, unsigned char *) =
	    (int (*)(void *, const unsigned char *, unsigned char *))func;

#if CRYPTO_INPUT_CHECK 
	if (len % block != 0)
		return (-1);
#endif

	ivp = iv;
	for (i = 0; i < len; i += block) {
		for (j = 0; j < block; j++) {
			xor[j] = (unsigned char)((data[i + j] ^ ivp[j]) & 0xff);
		}
		ret = crypto(key, xor, buf + i);
		ivp = buf + i;
	}

	return (0);
}


int
crypt_dec_cbc(void *key, int block, const unsigned char *iv, int len,
    const unsigned char *enc,
    unsigned char *buf,
    void *func)
{
	int i, j, ret;
	unsigned char xor[1024];
	const unsigned char *ivp;
	int (*crypto)(void *key, const unsigned char *, unsigned char *) =
	    (int (*)(void *, const unsigned char *, unsigned char *))func;

#if CRYPTO_INPUT_CHECK 
	if (len % block != 0)
		return (-1);
#endif

	ivp = iv;
	for (i = 0; i < len; i += block) {
		ret = crypto(key, enc + i, xor);

		for (j = 0; j < block; j++) {
			buf[i + j] = (unsigned char)((xor[j] ^ ivp[j]) & 0xff);
		}
		ivp = enc + i;
	}

	return (0);
}




/*
 * if (mode == 0) encode. else decode.
 */
int
crypt_cfb(void *key, int mode, int bit, int block,
    const unsigned char *iv, int len,
    const unsigned char *data, unsigned char *buf,
    void *func)
{
	int i, j, ret;
	int b = bit / 8, c;
	unsigned char enc[1024];   /* sizeof(enc) >= block */
	unsigned char ivb[1024];
	unsigned char *ivp;
	unsigned char *in;
	const unsigned char *p;
	int (*crypto)(void *key, const unsigned char *, unsigned char *) =
	    (int (*)(void *, const unsigned char *, unsigned char *))func;

#if CRYPTO_INPUT_CHECK 
	if (len % b != 0)
		return (-1);
	if (bit % 8 != 0)
		return (-3);
#endif

	if (mode)
		p = data;
	else
		p = buf;


	ivp = ivb;
	memcpy(ivp, iv, block);
	c = 0;
	for (i = 0; i < len; i += b) {
		in = ivp + c;
		ret = crypto(key, ivp + c, enc);
		for (j = 0; j < b; j++) {
			buf[i + j] = (char)((data[i + j] ^ enc[j]) & 0xff);
			ivp[c + j + block] = p[i + j];
		}

		if (c + b > (int)sizeof(ivb) - block - 2) {
			memcpy(ivp, ivp + c + b, block);
			c = 0;
		} else {
			c += b;
		}
	}

	return (0);
}



int
crypt_ofb(void *key, int bit, int block,
    const unsigned char *iv, int len,
    const unsigned char *data, unsigned char *buf,
    void *func)
{
	int i, j, ret;
	int b = bit / 8, c;
	unsigned char ivb[1024];
	unsigned char *ivp;
	unsigned char enc[1024];
	int (*crypto)(void *key, const unsigned char *, unsigned char *) =
	    (int (*)(void *, const unsigned char *, unsigned char *))func;

#if CRYPTO_INPUT_CHECK 
	if (len % b != 0)
		return (-1);
	if (bit % 8 != 0)
		return (-3);
#endif

	memcpy(ivb, iv, block);
	ivp = ivb;
	c = 0;
	for (i = 0; i < len; i += b) {
		ret = crypto(key, ivp + c, enc);
		for (j = 0; j < b; j++) {
			buf[i + j] = (char)((data[i + j] ^ enc[j]) & 0xff);
			ivp[c + j + block] = enc[j];
		}
		if (c + b > (int)sizeof(ivb) - block - 2) {
			memcpy(ivp, ivp + c + b, block);
			c = 0;
		} else {
			c += b;
		}
	}

	return (0);
}






int
crypt_enc_cfb_pgp(void *key, int block,
    const unsigned char *data, int len,
    unsigned char *buf, int buflen, 
    void *func)
{
	int i, j, c, ret;
	unsigned char enc[1024];
	const unsigned char *ivp;
	unsigned char fr[128], fre[128], rndm[128];
	int (*crypto)(void *key, const unsigned char *, unsigned char *) =
	    (int (*)(void *, const unsigned char *, unsigned char *))func;

#if CRYPTO_INPUT_CHECK 
	if (buflen < len + block + 2)
		return (-1);
#endif

	for (j = 0; j < block; j++) {
		rndm[j] = (char)((random() >> 5) & 0xff);
	}

	memset(fr, 0x00, block);
	ret = crypto(key, fr, fre);
	for (j = 0; j < block; j++) {
		buf[j] = (char)((fre[j] ^ rndm[j]) & 0xff);
	}
	ivp = buf;
	ret = crypto(key, ivp, enc);

	buf[block] = (char)((enc[0] ^ rndm[block - 2]) & 0xff);
	buf[block + 1] = (char)((enc[1] ^ rndm[block - 1]) & 0xff);

	ivp = buf + 2;
	for (i = 0; i < len; i += block) {
		ret = crypto(key, ivp, enc);

		c = (len - i) / block ? block : len % block;

		for (j = 0; j < c; j++) {
			buf[block + 2 + i + j] = (char)((data[i + j] ^ enc[j]) & 0xff);
		}
		ivp = buf + i;
	}

	return (block + len + 2);
}



