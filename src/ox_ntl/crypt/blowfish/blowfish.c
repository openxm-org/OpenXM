/* $OpenXM: OpenXM/src/ox_ntl/crypt/blowfish/blowfish.c,v 1.1 2004/07/22 12:12:05 iwane Exp $ */

/*
 * BLOWFISH: Bruce Schneier
 *  http://www.schneier.com/blowfish.html
 */

#include "blowfish.h"
#include "bf_tbl.h"

#if HAVE_CONFIG_H
#include "config.h"
#endif

#include <unistd.h>


static uint32_t
f(const blowfish_key *key, uint32_t x)
{
#define S(i) key->s[256 * i + a[i]]
	uint8_t a[4];
	int i;

	for (i = 0; i < 4; i++) {
		a[i] = (x >> ((3 - i) * 8)) & 0xff;
	}

	return (((S(0) + S(1)) ^ S(2)) + S(3));
#undef S
}


void
blowfish_enc_i(const blowfish_key *key,
	uint32_t ml, uint32_t mr,
	uint32_t *el, uint32_t *er)
{
	int i;
	uint32_t tmp;

	for (i = 0; i < 16; i++) {
		ml = ml ^ key->p[i];
		tmp = f(key, ml) ^ mr;

		mr = ml;
		ml = tmp;
	}

	*er = ml ^ key->p[i++];
	*el = mr ^ key->p[i++];
}

void
blowfish_dec_i(const blowfish_key *key,
	uint32_t ml, uint32_t mr,
	uint32_t *el, uint32_t *er)
{
	int i;
	uint32_t tmp;

	for (i = 17; i >= 2; i--) {
		ml = ml ^ key->p[i];
		tmp = f(key, ml) ^ mr;

		mr = ml;
		ml = tmp;
	}

	*er = ml ^ key->p[i--];
	*el = mr ^ key->p[i--];
}


#define BLOWFISH_MAXKEYLEN 448

void
blowfish_setkey(const unsigned char *str, int len, blowfish_key *key)
{
	int i, j, k;
	uint32_t t;
	uint32_t ml, mr;

	if (len > BLOWFISH_MAXKEYLEN )
		len = BLOWFISH_MAXKEYLEN;

	memcpy(key->p, __blowfish_tbl_p, sizeof(uint32_t) * 18);
	memcpy(key->s, __blowfish_tbl_s, sizeof(uint32_t) * 256 * 4);

	j = 0;
	for (i = 0; i < 18; i++) {
		t = 0;
		for (k = 0; k < 4; k++)
			t |= str[j++ % len] << (8 * (3 - k));

		key->p[i] ^= t;
	}

	ml = mr = 0;
	for (i = 0; i < 18; i += 2) {
		blowfish_enc_i(key, ml, mr, &ml, &mr);

		key->p[i] = ml;
		key->p[i + 1] = mr;
	}
	for (i = 0; i < 4 * 256; i += 2) {
		blowfish_enc_i(key, ml, mr, &ml, &mr);

		key->s[i] = ml;
		key->s[i + 1] = mr;
	}
}



