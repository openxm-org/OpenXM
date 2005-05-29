/* $OpenXM$ */
/*
 * Camellia
 * http://info.isl.ntt.co.jp/crypt/camellia/index.html
 */

#include "camellia.h"


#if HAVE_CONFIG_H
#include "config.h"
#endif /* HAVE_CONFIG_H */


#define leftrot32(x, n)      ((((uint32_t)x) << n) | ((uint32_t)x) >> (32 - n))


#define DPRINTF(str, x, n)	/* DPRINTF__(str, (x), (n)) */
#define DPRINTF__(str, x, n)	{                        \
	int _i;                                  \
	printf("%s: ", str);                      \
	for (_i = 0; _i < n; _i++) {             \
		printf("%02x", ARY(x, _i) & 0xff);    \
		if (_i % 8 == 7)                 \
			printf(" ");             \
	}                                        \
	printf("\n");                            \
	fflush(stdout);                          \
}

#ifdef WORDS_BIGENDIAN
#define ARY(x, i)  (((uint8_t *)(x))[i])
#else
#define ARY(x, i)  (((uint8_t *)(x))[(int)("\x03\x02\x01\x00\x07\x06\x05\x04\x0b\x0a\x09\x08\x0f\x0e\x0d\x0c\x13\x12\x11\x10\x17\x16\x15\x14\x1b\x1a\x19\x18\x1f\x1e\x1d\x1c"[i])])
#endif


static const uint32_t G_camellia_sigma[] = {
	0xa09e667f, 0x3bcc908b,
	0xb67ae858, 0x4caa73b2,
	0xc6ef372f, 0xe94f82be,
	0x54ff53a5, 0xf1d36f1c,
	0x10e527fa, 0xde682d1d,
	0xb05688c2, 0xb3e6c1fd,
};

#define SIGMA(i)   (G_camellia_sigma + (2 * ((i) - 1)))


static const uint8_t G_camellia_sbox[256] = {
	112,130, 44,236,179, 39,192,229,228,133, 87, 53,234, 12,174, 65,
	 35,239,107,147, 69, 25,165, 33,237, 14, 79, 78, 29,101,146,189,
	134,184,175,143,124,235, 31,206, 62, 48,220, 95, 94,197, 11, 26,
	166,225, 57,202,213, 71, 93, 61,217,  1, 90,214, 81, 86,108, 77,
	139, 13,154,102,251,204,176, 45,116, 18, 43, 32,240,177,132,153,
	223, 76,203,194, 52,126,118,  5,109,183,169, 49,209, 23,  4,215,
	 20, 88, 58, 97,222, 27, 17, 28, 50, 15,156, 22, 83, 24,242, 34,
	254, 68,207,178,195,181,122,145, 36,  8,232,168, 96,252,105, 80,
	170,208,160,125,161,137, 98,151, 84, 91, 30,149,224,255,100,210,
	 16,196,  0, 72,163,247,117,219,138,  3,230,218,  9, 63,221,148,
	135, 92,131,  2,205, 74,144, 51,115,103,246,243,157,127,191,226,
	 82,155,216, 38,200, 55,198, 59,129,150,111, 75, 19,190, 99, 46,
	233,121,167,140,159,110,188,142, 41,245,249,182, 47,253,180, 89,
	120,152,  6,106,231, 70,113,186,212, 37,171, 66,136,162,141,250,
	114,  7,185, 85,248,238,172, 10, 54, 73, 42,104, 60, 56,241,164,
	 64, 40,211,123,187,201, 67,193, 21,227,173,244,119,199,128,158,
};

#define SBOX1(n)	G_camellia_sbox[n]
#define SBOX2(n)	(uint8_t)((SBOX1(n) >> 7 ^ SBOX1(n) << 1) & 0xff)
#define SBOX3(n)	(uint8_t)((SBOX1(n) >> 1 ^ SBOX1(n) << 7) & 0xff)
#define SBOX4(n)	SBOX1(((n) << 1 ^ (n) >> 7) & 0xff)


#if 0
static void
camellia_binprt(const uint32_t *t, int n)
{
	int i;
	int q, r;
	for (i = 0; i < n; i++) {
		q = i / 32;
		r = i % 32;

		printf("%d", (t[q] >> r) & 1);
		if (i % 32 == 31)
			printf(" ");
	}
	printf("\n");
}
#endif

/**
 * return (a ^ b)
 *  a - 128 bit
 *  b - 128 bit
 */
static void
camellia_xor128(uint32_t *t, const uint32_t *a, const uint32_t *b)
{
	int i;
	for (i = 0; i < 4; i++) {
		t[i] = a[i] ^ b[i];
	}
}
static void
camellia_xor64(uint32_t *t, const uint32_t *a, const uint32_t *b)
{
	int i;
	for (i = 0; i < 2; i++) {
		t[i] = a[i] ^ b[i];
	}
}

static void
camellia_rot128(uint32_t *t, const uint32_t *a, int n)
{
	const int N = 128 / 32;
	int i;
	int r = n % 32;
	int r2 = 32 - r;
	int q = (n / 32) % N;

	if (r) {
		for (i = 0; i < N; i++) {
			t[i] = (a[(i + q) % N] << r) | (a[(i + q + N + 1) % N] >> r2);
		}
	} else {
		for (i = 0; i < N; i++) {
			t[i] = a[(i + q) % N];
		}
	}
}




/**
 * return s(l)
 *  t - 64 bit
 *  l - 64 bit
 */
static inline void
camellia_s(uint32_t *t, const uint32_t *l)
{
	ARY(t, 0) = SBOX1(ARY(l, 0));
	ARY(t, 1) = SBOX2(ARY(l, 1));
	ARY(t, 2) = SBOX3(ARY(l, 2));
	ARY(t, 3) = SBOX4(ARY(l, 3));
	ARY(t, 4) = SBOX2(ARY(l, 4));
	ARY(t, 5) = SBOX3(ARY(l, 5));
	ARY(t, 6) = SBOX4(ARY(l, 6));
	ARY(t, 7) = SBOX1(ARY(l, 7));
}


/**
 * return p(l)
 *  t - 64 bit
 *  l - 64 bit
 */
static inline void
camellia_p(uint32_t *t, const uint32_t *l)
{
#define L(i) (ARY(l, (i) - 1))
	ARY(t, 0) = L(1) ^ L(3) ^ L(4) ^ L(6) ^ L(7) ^ L(8);
	ARY(t, 1) = L(1) ^ L(2) ^ L(4) ^ L(5) ^ L(7) ^ L(8); 
	ARY(t, 2) = L(1) ^ L(2) ^ L(3) ^ L(5) ^ L(6) ^ L(8);
	ARY(t, 3) = L(2) ^ L(3) ^ L(4) ^ L(5) ^ L(6) ^ L(7);
	ARY(t, 4) = L(1) ^ L(2) ^ L(6) ^ L(7) ^ L(8);
	ARY(t, 5) = L(2) ^ L(3) ^ L(5) ^ L(7) ^ L(8);
	ARY(t, 6) = L(3) ^ L(4) ^ L(5) ^ L(6) ^ L(8);
	ARY(t, 7) = L(1) ^ L(4) ^ L(5) ^ L(6) ^ L(7);
#undef L
}


/**
 * return p(s(x^k))
 *  t - 64 bit
 *  l - 64 bit
 */
static inline void
camellia_f(uint32_t *y, const uint32_t *x, const uint32_t *k)
{
	uint32_t xor[4];
	uint32_t s[4];

	camellia_xor64(xor, x, k);
	camellia_s(s, xor);
	camellia_p(y, s);
}


/**
 * kl :  64 bit
 * kr :  64 bit
 * smg:  64 bit
 */
static inline void
camellia_feistel(uint32_t *w, const uint32_t *kl, const uint32_t *kr, const uint32_t *sgm)
{
	camellia_f(w, kl, sgm);
	camellia_xor64(w, w, kr);
}

static inline void
camellia_fl(uint32_t *y, const uint32_t *x, const uint32_t *k)
{
	uint32_t t;

	t = x[0] & k[0];
	y[1] = leftrot32(t, 1) ^ x[1];
	y[0] = (y[1] | k[1]) ^ x[0];
}

static inline void
camellia_fl_(uint32_t *x, const uint32_t *y, const uint32_t *k)
{
	uint32_t t;

	x[0] = (y[1] | k[1]) ^ y[0];
	t = x[0] & k[0];
	x[1] = leftrot32(t, 1) ^ y[1];
}



/*
 * len =  16 or  24 or  32
 * bit = 128 or 192 or 256
 */
int
camellia_keyset(const unsigned char *key, int len, camellia_key *ckey)
{
	int i;
	const uint32_t *kr, *kl, *kb;
	uint32_t ka[4];
	uint32_t t[4];  /* 128 bit */
	uint32_t w[4];  /* 128 bit */
	uint32_t v[4];  /* 128 bit */

#define Kl(i) (ARY(ckey->key, i))
#define Kr(i) (ARY(ckey->key, (i + 16)))
#define T(i)  (ARY(t,  i))

	if (len == 16) {
		for (i = 0; i < 16; i++) {
			Kl(i) = key[i];
			Kr(i) = 0x00;
		}
	} else if (len == 24) {
		for (i = 0; i < 16; i++) {
			Kl(i) = key[i];
		}
		for (; i < 24; i++) {
			Kr(i - 16) = key[i];
			Kr(i - 8) = ~key[i];
		}
	} else if (len == 32) {
		for (i = 0; i < 16; i++) {
			Kl(i) = key[i];
			Kr(i) = key[i + 16];
		}
	} else {
		return (-1);
	}

	kl = ckey->key;
	kr = ckey->key + 4;

	ckey->len = len;

	camellia_xor128(t, kl, kr);


	camellia_feistel(w + 2, t, t + 2, SIGMA(1));
	camellia_feistel(w, w + 2, t, SIGMA(2));

	camellia_xor128(v, w, kl);

	camellia_feistel(w + 2, v, v + 2, SIGMA(3));
	camellia_feistel(w, w + 2, v, SIGMA(4));

	if (len == 16) {
		/*
		 * KA = w
		 * KL = key
		 */
		camellia_rot128(ckey->kw, ckey->key, 0);
		camellia_rot128(ckey->kw + 4, w, 111);

		camellia_rot128(ckey->kl, w, 30);
		camellia_rot128(ckey->kl + 4, ckey->key, 77);

		camellia_rot128(ckey->k +  0, w, 0);
		camellia_rot128(ckey->k +  4, ckey->key, 15);
		camellia_rot128(ckey->k +  8, w, 15);
		camellia_rot128(ckey->k + 12, ckey->key, 45);
		camellia_rot128(ckey->k + 16, w, 45);
		camellia_rot128(ckey->k + 20, w, 60);
		camellia_rot128(ckey->k + 24, ckey->key, 94);
		camellia_rot128(ckey->k + 28, w, 94);
		camellia_rot128(ckey->k + 32, ckey->key, 111);

		camellia_rot128(v, ckey->key, 60);
		ckey->k[18] = v[2];
		ckey->k[19] = v[3];

		return (0);
	}

	memcpy(ka, w, sizeof(w));

	camellia_xor128(v, w, kr);

	camellia_feistel(w + 2, v, v + 2, SIGMA(5));
	camellia_feistel(w + 0, w + 2, v, SIGMA(6));

	kb = w;

	camellia_rot128(ckey->kw + 0, kl, 0);
	camellia_rot128(ckey->kw + 4, kb, 111);

	camellia_rot128(ckey->kl + 0, kr, 30);
	camellia_rot128(ckey->kl + 4, kl, 60);
	camellia_rot128(ckey->kl + 8, ka, 77);

	camellia_rot128(ckey->k +  0, kb, 0);
	camellia_rot128(ckey->k +  4, kr, 15);
	camellia_rot128(ckey->k +  8, ka, 15);
	camellia_rot128(ckey->k + 12, kb, 30);
	camellia_rot128(ckey->k + 16, kl, 45);
	camellia_rot128(ckey->k + 20, ka, 45);

	camellia_rot128(ckey->k + 24, kr, 60);
	camellia_rot128(ckey->k + 28, kb, 60);
	camellia_rot128(ckey->k + 32, kl, 77);
	camellia_rot128(ckey->k + 36, kr, 94);
	camellia_rot128(ckey->k + 40, ka, 94);
	camellia_rot128(ckey->k + 44, kl, 111);


	return (0);
#undef X
}


/*
 * m: message 128 bit
 * e: encrypt 128 bit
 */
void
camellia_enc(camellia_key *ckey, const uint8_t *msg, uint8_t *e)
{
	uint32_t m[4];
	uint32_t w[8] = {0, 0, 0, 0, 0, 0, 0, 0};
	int i, j, k;
	int loop;


	if (ckey->len == 16) { /* 128 bit */
		loop = 2;
	} else {               /* 192 or 256 bit */
		loop = 3;
	}

	for (i = 0; i < 16; i++) {
		ARY(m, i) = msg[i];
	}

	for (i = 0; i < 4; i++) {
		w[(i + 2) % 4] = m[i] ^ ckey->kw[i];
	}

	i = 0;
	for (j = 0;; j++) {

		for (k = 0; k < 6; k++, i++) {
			camellia_feistel(w + (4 + 2 * k) % 8, w + (2 + 2 * k) % 8, w + (0 + 2 * k) % 8, ckey->k + 2 * i);
		}

		if (j == loop)
			break;

		camellia_fl(w + 2, w + 6, ckey->kl + 4 * j);
		camellia_fl_(w + 0, w + 4, ckey->kl + 4 * j + 2);

	}


	camellia_xor128(w, w + 4, ckey->kw + 4);

	for (i = 0; i < 32; i++) {
		e[i] = ARY(w, i);
	}

}

/*
 * e: encrypt 128 bit
 * m: message 128 bit
 */
void
camellia_dec(camellia_key *ckey, const uint8_t *enc, uint8_t *m)
{
	uint32_t e[4];
	uint32_t w[8] = {0, 0, 0, 0, 0, 0, 0, 0};
	int i, j, k;
	int loop;

	if (ckey->len == 16) { /* 128 bit */
		loop = 2;
/*18*/
	} else {
		loop = 3;
/*24*/
	}

	for (i = 0; i < 16; i++) {
		ARY(e, i) = enc[i];
	}


	for (i = 0; i < 4; i++) {
		w[(i + 2) % 4] = e[i] ^ ckey->kw[(i + 0) % 4 + 4];
	}


	i = (loop + 1) * 6 - 1;
	for (j = loop - 1;; j--) {

		for (k = 0; k < 6; k++, i--) {
			camellia_feistel(w + (4 + 2 * k) % 8, w + (2 + 2 * k) % 8, w + (0 + 2 * k) % 8, ckey->k + 2 * i);
		}

		if (j < 0)
			break;


		camellia_fl(w + 2, w + 6, ckey->kl + 4 * j + 2);
		camellia_fl_(w + 0, w + 4, ckey->kl + 4 * j + 0);

	}


	camellia_xor128(w, w + 4, ckey->kw + 0);

	for (i = 0; i < 32; i++) {
		m[i] = ARY(w, i);
	}

}



/* End of File */

