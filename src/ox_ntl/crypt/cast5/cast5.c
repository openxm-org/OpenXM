/* $OpenXM: OpenXM/src/ox_ntl/crypt/cast5/cast5.c,v 1.1 2004/07/15 14:51:42 iwane Exp $ */
/*
 * rfc 2144
 *  The CAST-128 Encryption Algorithm
 */

#include "cast5.h"
#include "cast5tbl.h"


#if HAVE_CONFIG_H
#include "config.h"
#endif /* HAVE_CONFIG_H */

#define leftrot32(x, n)      ((((uint32_t)x) << n) | ((uint32_t)x) >> (32 - n))

#ifdef WORDS_BIGENDIAN
#define ARY(x, i)  (((unsigned char *)(x))[i])
#else
#define ARY(x, i)  (((unsigned char *)(x))[(int)("\x03\x02\x01\x00\x07\x06\x05\x04\x0b\x0a\x09\x08\x0f\x0e\x0d\x0c"[i])])
#endif


/*
#define ARY(x, i)  (((unsigned char *)(x))[((((i) / 4) * 4) + (3 - (i) % 4))])
 * compute: 7.473
 *         15.100
 *         11.805
 *
#define ARY(x, i)  (((unsigned char *)(x))[(int)("\x03\x02\x01\x00\x07\x06\x05\x04\x0b\x0a\x09\x08\x0f\x0e\x0d\x0c"[i])])
 * char array: 7.044
 *            14.124
 *            11.366
 *
static const int _ar[] = {3, 2, 1, 0, 7, 6, 5, 4, 11, 10, 9, 8, 15, 14, 13, 12};
#define ARY(x, i)  (((unsigned char *)(x))[_ar[i]])
 * int array: 8.679
 *           17.233
 *           12.705
 */



/*
 * length of key is 16 byte = 128 bit.
 */
void
cast128_keyset(const unsigned char *key, int len, cast128_key *ckey)
{
#define X(i) (ARY(&x, i))
#define Z(i) (ARY(&z, i))
	uint32_t x[4], z[4];
	int i;
	uint32_t K[32], *k;

	if (len > 16)
		len = 16;
	for (i = 0; i < len; i++) {
		X(i) = key[i];
	}

	for (; i < (int)sizeof(x); i++) {
		X(i) = 0;
	}

	k = K;

	for (i = 0; i < 2; i++) {
#define s5 __cast128_tbl_s5
#define s6 __cast128_tbl_s6
#define s7 __cast128_tbl_s7
#define s8 __cast128_tbl_s8
		z[0] = x[0] ^ s5[X(0xD)] ^ s6[X(0xF)] ^ s7[X(0xC)] ^ s8[X(0xE)] ^ s7[X(0x8)];
		z[1] = x[2] ^ s5[Z(0x0)] ^ s6[Z(0x2)] ^ s7[Z(0x1)] ^ s8[Z(0x3)] ^ s8[X(0xA)];
		z[2] = x[3] ^ s5[Z(0x7)] ^ s6[Z(0x6)] ^ s7[Z(0x5)] ^ s8[Z(0x4)] ^ s5[X(0x9)];
		z[3] = x[1] ^ s5[Z(0xA)] ^ s6[Z(0x9)] ^ s7[Z(0xB)] ^ s8[Z(0x8)] ^ s6[X(0xB)];

		k[ 0] = s5[Z(0x8)] ^ s6[Z(0x9)] ^ s7[Z(0x7)] ^ s8[Z(0x6)] ^ s5[Z(0x2)];
		k[ 1] = s5[Z(0xA)] ^ s6[Z(0xB)] ^ s7[Z(0x5)] ^ s8[Z(0x4)] ^ s6[Z(0x6)];
		k[ 2] = s5[Z(0xC)] ^ s6[Z(0xD)] ^ s7[Z(0x3)] ^ s8[Z(0x2)] ^ s7[Z(0x9)];
		k[ 3] = s5[Z(0xE)] ^ s6[Z(0xF)] ^ s7[Z(0x1)] ^ s8[Z(0x0)] ^ s8[Z(0xC)];
	
		x[0] = z[2] ^ s5[Z(0x5)] ^ s6[Z(0x7)] ^ s7[Z(0x4)] ^ s8[Z(0x6)] ^ s7[Z(0x0)];
		x[1] = z[0] ^ s5[X(0x0)] ^ s6[X(0x2)] ^ s7[X(0x1)] ^ s8[X(0x3)] ^ s8[Z(0x2)];
		x[2] = z[1] ^ s5[X(0x7)] ^ s6[X(0x6)] ^ s7[X(0x5)] ^ s8[X(0x4)] ^ s5[Z(0x1)];
		x[3] = z[3] ^ s5[X(0xA)] ^ s6[X(0x9)] ^ s7[X(0xB)] ^ s8[X(0x8)] ^ s6[Z(0x3)];
	
		k[ 4] = s5[X(0x3)] ^ s6[X(0x2)] ^ s7[X(0xC)] ^ s8[X(0xD)] ^ s5[X(0x8)];
		k[ 5] = s5[X(0x1)] ^ s6[X(0x0)] ^ s7[X(0xE)] ^ s8[X(0xF)] ^ s6[X(0xD)];
		k[ 6] = s5[X(0x7)] ^ s6[X(0x6)] ^ s7[X(0x8)] ^ s8[X(0x9)] ^ s7[X(0x3)];
		k[ 7] = s5[X(0x5)] ^ s6[X(0x4)] ^ s7[X(0xA)] ^ s8[X(0xB)] ^ s8[X(0x7)];
	
		z[0] = x[0] ^ s5[X(0xD)] ^ s6[X(0xF)] ^ s7[X(0xC)] ^ s8[X(0xE)] ^ s7[X(0x8)];
		z[1] = x[2] ^ s5[Z(0x0)] ^ s6[Z(0x2)] ^ s7[Z(0x1)] ^ s8[Z(0x3)] ^ s8[X(0xA)];
		z[2] = x[3] ^ s5[Z(0x7)] ^ s6[Z(0x6)] ^ s7[Z(0x5)] ^ s8[Z(0x4)] ^ s5[X(0x9)];
		z[3] = x[1] ^ s5[Z(0xA)] ^ s6[Z(0x9)] ^ s7[Z(0xB)] ^ s8[Z(0x8)] ^ s6[X(0xB)];
	
		k[ 8] = s5[Z(0x3)] ^ s6[Z(0x2)] ^ s7[Z(0xC)] ^ s8[Z(0xD)] ^ s5[Z(0x9)];
		k[ 9] = s5[Z(0x1)] ^ s6[Z(0x0)] ^ s7[Z(0xE)] ^ s8[Z(0xF)] ^ s6[Z(0xC)];
		k[10] = s5[Z(0x7)] ^ s6[Z(0x6)] ^ s7[Z(0x8)] ^ s8[Z(0x9)] ^ s7[Z(0x2)];
		k[11] = s5[Z(0x5)] ^ s6[Z(0x4)] ^ s7[Z(0xA)] ^ s8[Z(0xB)] ^ s8[Z(0x6)];
	
		x[0] = z[2] ^ s5[Z(0x5)] ^ s6[Z(0x7)] ^ s7[Z(0x4)] ^ s8[Z(0x6)] ^ s7[Z(0x0)];
		x[1] = z[0] ^ s5[X(0x0)] ^ s6[X(0x2)] ^ s7[X(0x1)] ^ s8[X(0x3)] ^ s8[Z(0x2)];
		x[2] = z[1] ^ s5[X(0x7)] ^ s6[X(0x6)] ^ s7[X(0x5)] ^ s8[X(0x4)] ^ s5[Z(0x1)];
		x[3] = z[3] ^ s5[X(0xA)] ^ s6[X(0x9)] ^ s7[X(0xB)] ^ s8[X(0x8)] ^ s6[Z(0x3)];
	
		k[12] = s5[X(0x8)] ^ s6[X(0x9)] ^ s7[X(0x7)] ^ s8[X(0x6)] ^ s5[X(0x3)];
		k[13] = s5[X(0xA)] ^ s6[X(0xB)] ^ s7[X(0x5)] ^ s8[X(0x4)] ^ s6[X(0x7)];
		k[14] = s5[X(0xC)] ^ s6[X(0xD)] ^ s7[X(0x3)] ^ s8[X(0x2)] ^ s7[X(0x8)];
		k[15] = s5[X(0xE)] ^ s6[X(0xF)] ^ s7[X(0x1)] ^ s8[X(0x0)] ^ s8[X(0xD)];
	
#undef s5
#undef s6
#undef s7
#undef s8
		k += 16;
	}

	for (i = 0; i < 16; i++) {
		ckey->km[i] = K[i];
		ckey->kr[i] = K[i + 16] & 0x1f;
	}
	ckey->len = len;
	if (len <= 10) /* < 80 bit */
		ckey->loop = 12;
	else
		ckey->loop = 16;

#undef X
#undef Z

}


#define I(i) (ARY(&j, i))

#define _CAST_TYPE(FUNC, OP1, OP2, OP3) \
static inline uint32_t                                                       \
FUNC(uint32_t d, uint32_t km, uint32_t kr)                                   \
{                                                                            \
	uint32_t j = leftrot32(km OP3 d, kr);                                   \
	return (((__cast128_tbl_s1[I(0)] OP1 __cast128_tbl_s2[I(1)]) OP2 __cast128_tbl_s3[I(2)]) OP3 __cast128_tbl_s4[I(3)]);        \
}

_CAST_TYPE(_type1, ^, -, +)
_CAST_TYPE(_type2, -, +, ^)
_CAST_TYPE(_type3, +, ^, -)

#undef I


void
cast128_enc_i(cast128_key *key,
    uint32_t ml, uint32_t mr, 
    uint32_t *el, uint32_t *er)
{
	int i;

	uint32_t l;
	uint32_t (*f[3])(uint32_t, uint32_t, uint32_t) = {_type1, _type2, _type3};

	for (i = 0; i < key->loop; i++) {
		l = mr;
		mr = ml ^ f[i % 3](mr, key->km[i], key->kr[i]);
		ml = l;
	}

	*er = ml;
	*el = mr;
}



void
cast128_enc_c(cast128_key *key,
    const unsigned char *msg, unsigned char *enc)
{
	uint32_t m[2], e[2];
	int i;

	for (i = 0; i < 8; i++) {
		ARY(m, i) = msg[i];
	}

	cast128_enc_i(key, m[0], m[1], &e[0], &e[1]);

	for (i = 0; i < 8; i++) {
		enc[i] = ARY(e, i);
	}
}



void
cast128_dec_i(cast128_key *key,
    uint32_t ml, uint32_t mr, 
    uint32_t *el, uint32_t *er)
{
	int i;

	uint32_t l;
	uint32_t (*f[3])(uint32_t, uint32_t, uint32_t) = {_type1, _type2, _type3};

	for (i = key->loop - 1; i >= 0; i--) {
		l = mr;
		mr = ml ^ f[i % 3](mr, key->km[i], key->kr[i]);
		ml = l;
	}

	*er = ml;
	*el = mr;
}


void
cast128_dec_c(cast128_key *key,
    const unsigned char *msg, unsigned char *enc)
{
	uint32_t m[2], e[2];
	int i;

	for (i = 0; i < 8; i++) {
		ARY(m, i) = msg[i];
	}

	cast128_dec_i(key, m[0], m[1], &e[0], &e[1]);

	for (i = 0; i < 8; i++) {
		enc[i] = ARY(e, i);
	}
}





