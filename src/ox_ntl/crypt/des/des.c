/* $OpenXM: OpenXM/src/ox_ntl/crypt/des/des.c,v 1.1 2004/07/11 00:32:17 iwane Exp $ */
/*
 * FIPS PUB 46-3
 *   DATA ENCRYPTION STANDARD
 */
#include <stdio.h>

#include "des.h"
#include "block.h"

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#define BLOCK 8


/*===========================================================*
 * KEY STRUCT
 *===========================================================*/
static int inline
keychk0(uint32_t n)
{
	int i, j, c;

	for (i = 0; i < 4; i++) {
		c = 0;
		for (j = 0; j < 8; j++) {
			if ((n >> (8 * i + j)) & 1)
				c++;
		}
		if (c % 2 == 0)
			return (0);
	}
	return (1);
}

int
des_keychk(des_key *key)
{
	return (keychk0(key->key.ui[0]) & keychk0(key->key.ui[1]));
}

void
des_keyset(const unsigned char *key, des_key *dkey)
{
	uint32_t k;
	int i, j;

	for (j = 0; j < 2; j++) {
		k = 0;
		for (i = 0; i < 4; i++) {
			k |= (key[i + 4 * j] & 0xff) << (8 * (3 - i));
		}
		dkey->key.ui[j] = k;
	}

}


#ifdef DES_DEBUG
/*===========================================================*
 * DEBUG
 *===========================================================*/
static void
pr(uint32_t n)
{
	int i;

	for (i = 31; i >= 0; i--) {
		printf("%d", (n >> i) & 1);
		if (i % 8 == 0)
			printf(" ");
	}
}
#endif


/*===========================================================*
 * SHIFT
 *===========================================================*/
static const int SHIFT_IDX[] = {
	1, 1, 2, 2, 2, 2, 2, 2, 1, 2, 2, 2, 2, 2, 2, 1,
};

static inline uint32_t
lshift28(uint32_t x, int c)
{
	int n = SHIFT_IDX[c];
	return (((x << n) | (x >> (28 - n))) & 0x0fffffff);
}

static inline uint32_t
rshift28(uint32_t x, int c)
{
	int n = SHIFT_IDX[15 - c];
	return (((x >> n) | (x << (28 - n))) & 0x0fffffff);
}



/*===========================================================*
 * TRANS
 *===========================================================*/
static void
trans(uint32_t li, uint32_t ri, uint32_t *lo, uint32_t *ro, const int *index, int m)
{
	int idx, n;
	uint32_t a[2];
	uint32_t r, l;
	const static int N = 32;

	a[0] = li;
	a[1] = ri;

	l = 0;
	r = 0;
	for (n = 0; n < N; n++) {
		idx = index[n];
		l |= ((a[idx / N] >> (N - (idx % N) - 1)) & 1) << (N - n - 1);
	}

	for (; n < m; n++) {
		idx = index[n];
		r |= ((a[idx / N] >> (N - (idx % N) - 1)) & 1) << (2 * N - n - 1);
	}

	*lo = l;
	*ro = r;
}




/*
 * input  64 bit  32+32
 * output 64 bit  32+32
 */
static void
ip(uint32_t li, uint32_t ri, uint32_t *lo, uint32_t *ro)
{
	static const int INDEX[] = {
		57, 49, 41, 33, 25, 17,  9,  1, 
		59, 51, 43, 35, 27, 19, 11,  3, 
		61, 53, 45, 37, 29, 21, 13,  5, 
		63, 55, 47, 39, 31, 23, 15,  7, 
		56, 48, 40, 32, 24, 16,  8,  0, 
		58, 50, 42, 34, 26, 18, 10,  2, 
		60, 52, 44, 36, 28, 20, 12,  4, 
		62, 54, 46, 38, 30, 22, 14,  6, 
	};

	trans(li, ri, lo, ro, INDEX, 64);
}


/*
 * input  64 bit 32+32
 * output 64 bit 32+32
 */
static void
ip_inv(uint32_t li, uint32_t ri, uint32_t *lo, uint32_t *ro)
{
	static const int INDEX[] = {
		39,  7, 47, 15, 55, 23, 63, 31, 
		38,  6, 46, 14, 54, 22, 62, 30, 
		37,  5, 45, 13, 53, 21, 61, 29, 
		36,  4, 44, 12, 52, 20, 60, 28, 
		35,  3, 43, 11, 51, 19, 59, 27, 
		34,  2, 42, 10, 50, 18, 58, 26, 
		33,  1, 41,  9, 49, 17, 57, 25, 
		32,  0, 40,  8, 48, 16, 56, 24, 
	};

	trans(li, ri, lo, ro, INDEX, 64);
}



/* PC1
 * input  64 bit 32+32
 * output 56 bit 28+28
 */
static void
ktrans1(uint32_t li, uint32_t ri, uint32_t *lo, uint32_t *ro)
{
	static const int INDEX[] = {
		0, 0, 0, 0,
		56, 48, 40, 32, 24, 16,  8, 
		 0, 57, 49, 41, 33, 25, 17, 
		 9,  1, 58, 50, 42, 34, 26, 
		18, 10,  2, 59, 51, 43, 35, 
		0, 0, 0, 0,
		62, 54, 46, 38, 30, 22, 14, 
		 6, 61, 53, 45, 37, 29, 21,
		13,  5, 60, 52, 44, 36, 28,
		20, 12,  4, 27, 19, 11,  3,
	};

	trans(li, ri, lo, ro, INDEX, 64);

	*lo &= 0x0fffffff;
	*ro &= 0x0fffffff;
}


/* PC2
 * Input  56 bit = 28+28
 * Output 48 bit = 32+16
 */
static void
ktrans2(uint32_t li, uint32_t ri, uint32_t *lo, uint32_t *ro)
{
	uint32_t l, r;
	static const int CTRANS2_INDEX[64] = {
		13, 16, 10, 23,  0,  4,
		 2, 27, 14,  5, 20,  9,
		22, 18, 11,  3, 25,  7,
		15,  6, 26, 19, 12,  1,
		40, 51, 30, 36, 46, 54,
		29, 39, 50, 44, 32, 47,
		43, 48, 38, 55, 33, 52,
		45, 41, 49, 35, 28, 31,
	};

	l = (li << 4) | ((ri >> 24) & 0xf);
	r = ri << 8;

	trans(l, r, lo, ro, CTRANS2_INDEX, 48);

	*ro &= 0xffff0000;
}

/*
 * input   32 bit = 32
 * output  48 bit = 32 + 16
 */
static void
etrans1(uint32_t li, uint32_t *lo, uint32_t *ro)
{
	uint32_t dummy = 0;
	static const int INDEX[64] = {
		31,  0,  1,  2,  3,  4, 
		 3,  4,  5,  6,  7,  8, 
		 7,  8,  9, 10, 11, 12, 
		11, 12, 13, 14, 15, 16, 
		15, 16, 17, 18, 19, 20, 
		19, 20, 21, 22, 23, 24, 
		23, 24, 25, 26, 27, 28, 
		27, 28, 29, 30, 31,  0, 
	};

	trans(li, dummy, lo, ro, INDEX, 48);
}


/*
 * input   32 bit = 32
 * output  32 bit = 32
 */
static uint32_t
etrans2(uint32_t n)
{
	static const int INDEX[64] = {
		15,  6, 19, 20,
		28, 11, 27, 16,
		 0, 14, 22, 25,
		 4, 17, 30,  9,
		 1,  7, 23, 13,
		31, 26,  2,  8,
		18, 12, 29,  5,
		21, 10,  3, 24,
	};

	uint32_t p, b;

	trans(n, 0, &p, &b, INDEX, 48);

	return (p);
}

/*
 * input  : 6 bit
 * oiutput: 6 bit
 */
static uint32_t
strans1(uint32_t s, int n)
{
	static const int S[][4 * 16] = {
		{
			14,  4, 13,  1,  2, 15, 11,  8,  3, 10,  6, 12,  5,  9,  0,  7,
			 0, 15,  7,  4, 14,  2, 13,  1, 10,  6, 12, 11,  9,  5,  3,  8,
			 4,  1, 14,  8, 13,  6,  2, 11, 15, 12,  9,  7,  3, 10,  5,  0,
			15, 12,  8,  2,  4,  9,  1,  7,  5, 11,  3, 14, 10,  0,  6, 13,
		}, {
			15,  1,  8, 14,  6, 11,  3,  4,  9,  7,  2, 13, 12,  0,  5, 10,
			 3, 13,  4,  7, 15,  2,  8, 14, 12,  0,  1, 10,  6,  9, 11,  5,
			 0, 14,  7, 11, 10,  4, 13,  1,  5,  8, 12,  6,  9,  3,  2, 15,
			13,  8, 10,  1,  3, 15,  4,  2, 11,  6,  7, 12,  0,  5, 14,  9,
		}, {
			10,  0,  9, 14,  6,  3, 15,  5,  1, 13, 12,  7, 11,  4,  2,  8,
			13,  7,  0,  9,  3,  4,  6, 10,  2,  8,  5, 14, 12, 11, 15,  1,
			13,  6,  4,  9,  8, 15,  3,  0, 11,  1,  2, 12,  5, 10, 14,  7,
			 1, 10, 13,  0,  6,  9,  8,  7,  4, 15, 14,  3, 11,  5,  2, 12,
		}, {
			 7, 13, 14,  3,  0,  6,  9, 10,  1,  2,  8,  5, 11, 12,  4, 15,
			13,  8, 11,  5,  6, 15,  0,  3,  4,  7,  2, 12,  1, 10, 14,  9,
			10,  6,  9,  0, 12, 11,  7, 13, 15,  1,  3, 14,  5,  2,  8,  4,
			 3, 15,  0,  6, 10,  1, 13,  8,  9,  4,  5, 11, 12,  7,  2, 14,
		}, {
			 2, 12,  4,  1,  7, 10, 11,  6,  8,  5,  3, 15, 13,  0, 14,  9,
			14, 11,  2, 12,  4,  7, 13,  1,  5,  0, 15, 10,  3,  9,  8,  6,
			 4,  2,  1, 11, 10, 13,  7,  8, 15,  9, 12,  5,  6,  3,  0, 14,
			11,  8, 12,  7,  1, 14,  2, 13,  6, 15,  0,  9, 10,  4,  5,  3,
		}, {
			12,  1, 10, 15,  9,  2,  6,  8,  0, 13,  3,  4, 14,  7,  5, 11,
			10, 15,  4,  2,  7, 12,  9,  5,  6,  1, 13, 14,  0, 11,  3,  8,
			 9, 14, 15,  5,  2,  8, 12,  3,  7,  0,  4, 10,  1, 13, 11,  6,
			 4,  3,  2, 12,  9,  5, 15, 10, 11, 14,  1,  7,  6,  0,  8, 13,
		}, {
			 4, 11,  2, 14, 15,  0,  8, 13,  3, 12,  9,  7,  5, 10,  6,  1,
			13,  0, 11,  7,  4,  9,  1, 10, 14,  3,  5, 12,  2, 15,  8,  6,
			 1,  4, 11, 13, 12,  3,  7, 14, 10, 15,  6,  8,  0,  5,  9,  2,
			 6, 11, 13,  8,  1,  4, 10,  7,  9,  5,  0, 15, 14,  2,  3, 12,
		}, {
			13,  2,  8,  4,  6, 15, 11,  1, 10,  9,  3, 14,  5,  0, 12,  7,
			 1, 15, 13,  8, 10,  3,  7,  4, 12,  5,  6, 11,  0, 14,  9,  2,
			 7, 11,  4,  1,  9, 12, 14,  2,  0,  6, 10, 13, 15,  3,  5,  8,
			 2,  1, 14,  7,  4, 10,  8, 13, 15, 12,  9,  0,  3,  5,  6, 11,
		},
	};

	int x, y;

	x = (((s >> 5) & 1) << 1) | (s & 1);
	y = (s >> 1) & 0xf;
 
	return (S[n][16 * x + y]);

}

/*
 * input : 48 bit
 * output: 32 bit
 */
static void
strans(uint32_t li, uint32_t ri, uint32_t *out)
{
	int i;
	int m;

	uint32_t s[8];
	uint32_t ui;

	/* ... X-( */
	s[0] = li >> 26;
	s[1] = (li >> 20) & 0x3f; 
	s[2] = (li >> 14) & 0x3f; 
	s[3] = (li >>  8) & 0x3f; 
	s[4] = (li >>  2) & 0x3f; 
	s[5] = ((li & 0x3) << 4) | (ri >> 28);
	s[6] = (ri >> 22) & 0x3f;
	s[7] = (ri >> 16) & 0x3f;

	ui = 0;
	for (i = 0; i < 8; i++) {
		m =  strans1(s[i], i);
		ui |= m << 4 * (7 - i);
	}

	*out = ui;
}


/*
 * input : r : 32 bit
 *       : k : 48 bit = 32+16
 * output:     32 bit
 */
static uint32_t
f_(uint32_t r, uint32_t kl, uint32_t kr)
{
	uint32_t rl, rr;
	uint32_t s;
	uint32_t p;

	etrans1(r, &rl, &rr);

	strans(kl ^ rl, kr ^ rr, &s);

	p = etrans2(s);

	return (p);
}

/*===========================================================*
 * BASE CRIPTO
 *===========================================================*/
void
des_dec_i(
	uint32_t keyl, uint32_t keyr,
	uint32_t msgl, uint32_t msgr,
	uint32_t *el, uint32_t *er)
{
	uint32_t c, d;
	uint32_t kl, kr;
	uint32_t l, r;
	uint32_t f;
	uint32_t xor;
	int i;

	ktrans1(keyl, keyr, &c, &d);

	ip(msgl, msgr, &r, &l);

	for (i = 0; i < 16; i++) {
		ktrans2(c, d, &kl, &kr);

		f = f_(l, kl, kr);

		xor = r ^ f;
		r = l;
		l = xor;

		c = rshift28(c, i);
		d = rshift28(d, i);
	}

	ip_inv(l, r, el, er);
}


void
des_enc_i(
	uint32_t keyl, uint32_t keyr,
	uint32_t msgl, uint32_t msgr,
	uint32_t *el, uint32_t *er)
{
	uint32_t c, d;
	uint32_t kl, kr;
	uint32_t l, r;
	uint32_t f;
	uint32_t xor;
	int i;

	ktrans1(keyl, keyr, &c, &d);

	ip(msgl, msgr, &l, &r);

	for (i = 0; i < 16; i++) {
		c = lshift28(c, i);
		d = lshift28(d, i);

		ktrans2(c, d, &kl, &kr);

		f = f_(r, kl, kr);

		xor = l ^ f;

		l = r;
		r = xor;
	}
 
	ip_inv(r, l, el, er);
}

/**
 * input: data: 8 byte
 */
int
des_enc_c(const des_key *key, const unsigned char *data, unsigned char *enc)
{
	uint32_t dl, dr;
	uint32_t el, er;
	int i;

	dl = dr = 0;
	for (i = 0; i < 4; i++) {
		dl |= (data[i] & 0xff) << (8 * (3 - i));
		dr |= (data[i + 4] & 0xff) << (8 * (3 - i));
	}

	des_enc_i(key->key.ui[0], key->key.ui[1], dl, dr, &el, &er);


	for (i = 0; i < 4; i++) {
		enc[i] = (el >> (8 * (3 - i))) & 0xff;
		enc[i + 4] = (er >> (8 * (3 - i))) & 0xff;
	}

	return (0);
}


/**
 * input: data: 8 byte
 */
int
des_dec_c(const des_key *key, const unsigned char *enc, unsigned char *data)
{
	uint32_t dl, dr;
	uint32_t el, er;
	int i;

	dl = dr = 0;
	for (i = 0; i < 4; i++) {
		dl |= (enc[i] & 0xff) << (8 * (3 - i));
		dr |= (enc[i + 4] & 0xff) << (8 * (3 - i));
	}

	des_dec_i(key->key.ui[0], key->key.ui[1], dl, dr, &el, &er);


	for (i = 0; i < 4; i++) {
		data[i] = (el >> (8 * (3 - i))) & 0xff;
		data[i + 4] = (er >> (8 * (3 - i))) & 0xff;
	}

	return (0);
}

/*===========================================================*
 * CRIPTO: length of input data is "64 * n" byte
 *===========================================================*/
static int
des_prm_chk(int datalen, int buflen)
{
	if (datalen % BLOCK != 0)
		return (-1);
	if (buflen < datalen)
		return (-2);
	return (0);
}


int
des_enc_ecb(const des_key *key, int len, const unsigned char *data, unsigned char *buf)
{
	int ret;

	ret = crypt_ecb((void *)key, BLOCK, len, data, buf, (void *)des_enc_c);

	return (ret);
}


int
des_dec_ecb(const des_key *key, int len, const unsigned char *enc, unsigned char *data)
{
	int ret;

	ret = crypt_ecb((void *)key, BLOCK, len, enc, data, (void *)des_dec_c);

	return (ret);
}



int
des_enc_cbc(const des_key *key, const unsigned char *iv, int len,
    const unsigned char *data, unsigned char *buf)
{
	int ret;

	ret = crypt_enc_cbc((void *)key, BLOCK, iv, len, data, buf, (void *)des_enc_c);

	return (ret);
}


int
des_dec_cbc(const des_key *key, const unsigned char *iv, int len,
    const unsigned char *data, unsigned char *buf)
{
	int ret;

	ret = crypt_dec_cbc((void *)key, BLOCK, iv, len, data, buf, (void *)des_dec_c);

	return (ret);
}


int
des_enc_cfb(const des_key *key, int bit,
    const unsigned char *iv, int len,
    const unsigned char *data, unsigned char *buf)
{
	int ret;

	ret = crypt_cfb((void *)key, 0, bit, BLOCK, iv, len, data, buf, (void *)des_enc_c);

	return (ret);
}

int
des_dec_cfb(const des_key *key, int bit,
    const unsigned char *iv, int len,
    const unsigned char *data, unsigned char *buf)
{
	int ret;

	ret = crypt_cfb((void *)key, 1, bit, BLOCK, iv, len, data, buf, (void *)des_enc_c);

	return (ret);
}


int
des_ofb(const des_key *key, int bit,
    const unsigned char *iv, int len,
    const unsigned char *data,
    unsigned char *buf)
{
	int ret;
	ret = crypt_ofb((void *)key, bit, BLOCK, iv, len, data, buf, (void *)des_enc_c);

	return (ret);
}


