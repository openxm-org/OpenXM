/* $OpenXM: OpenXM/src/ox_ntl/crypt/sha1/sha1.c,v 1.2 2004/03/25 13:34:19 iwane Exp $ */
/* RFC 3174 - SHA-1 (US Secure Hash Algorithm 1 (SHA1))*/

#include <stdio.h>

#include "sha1.h"

/* Global Constant */
static const unsigned int K[4] = {0x5a827999, 0x6ed9eba1, 0x8f1bbcdc, 0xca62c1d6};
static const unsigned int H[5] = {0x67452301, 0xefcdab89, 0x98badcfe, 0x10325476, 0xc3d2e1f0};

#define BLOCK (512 / 8)


static inline unsigned int lshift32(unsigned int x, int n)
{
	return ((x << n) | (x >> (32 - n)));
};



/**
 * sizeof(buf) >= 512 * ((len * 8 + 1) / 64)
 * len * 8 < 2^64 ==> len < 2^61
 */
static unsigned int
padding(unsigned char *buf, const unsigned char *msg, int length)
{
	int n;
	int i;
	int len = length % BLOCK;

	if (len == 0)
		len = BLOCK;

	memcpy(buf, msg, len);

	buf[len++] = 0x80;

	/* 56 < len % 64 */
	if (BLOCK - 8 < len % BLOCK) {
		/* too long */
		n = BLOCK - (len % BLOCK) + BLOCK - 8;
	} else {
		n = BLOCK - 8 - (len % BLOCK);
	}

	memset(buf + len, 0x00, n);

	n += len;

	for (i = 0; i < 4; i++) {
		buf[n] = 0x00;
		buf[n + 4] = ((length * 8) >> (8 * (3 - i))) & 0xff;
		n++;
	}

	return ((n + 4) / BLOCK);
}




static unsigned int
f(unsigned int t, unsigned int b, unsigned int c, unsigned int d)
{
	if (t < 20) {
		return ((b & c) | ((~b) & d));
	}
	if (t < 40 || t >= 60) {
		return (b ^ c ^ d);
	}

	if (t < 60) {
		return ((b & c) | (b & d) | (c & d));
	}
	
	/* Invalid Parameter. */
	return (0);
}


/* sizeof(msg) == 512 / 8.
 * padding.
 */
void
sha1_md(unsigned int *h, const unsigned char *msg)
{
	int t;
	unsigned int a, b, c, d, e, temp;
	int i;
	unsigned int w[80];

	/* ... */
	for (t = 0; t < 16; t++) {
		w[t] = 0;
		for (i = 0; i < 4; i++) {
			w[t] |= (msg[i + 4 * t] & 0xff) << ((3 - i) * 8);
		}
	}

	for (t = 16; t < 80; t++) {
		w[t] = lshift32(w[t - 3] ^ w[t - 8] ^ w[t - 14] ^ w[t - 16], 1);
	}

	a = h[0];
	b = h[1];
	c = h[2];
	d = h[3];
	e = h[4];
	
	for (t = 0; t < 80; t++) {
		temp = lshift32(a, 5) + f(t, b, c, d) + e + w[t] + K[t / 20];
		e = d;
		d = c;
		c = lshift32(b, 30);
		b = a;
		a = temp;
	}

	h[0] += a;
	h[1] += b;
	h[2] += c;
	h[3] += d;
	h[4] += e;
}

int
sha1_h(unsigned char *Ph, const unsigned char *msg, int len, const unsigned int *hp)
{
	int i, j, cnt, l = len;
	unsigned char buf[1024];
	unsigned int h[sizeof(H) / sizeof(H[0])];

	if (hp == NULL)
		memcpy(h, H, sizeof(H));
	else
		memcpy(h, hp, sizeof(h));

	while (l > BLOCK) {
		sha1_md(h, msg);
		msg += BLOCK;
		l -= BLOCK;
	}

	cnt = padding(buf, msg, len);
	for (i = 0; i < cnt; i++) {
		sha1_md(h, buf + BLOCK * i);
	}

	memset(Ph, 0x00, sizeof(H));
	for (i = 0; i < sizeof(H) / sizeof(H[0]); i++) {
		for (j = 0; j < 32; j++) {
			Ph[4 * i + j / 8] |= ((h[i] >> (31 - j)) & 1) << (7 - j % 8);
		}
	}

	return (0);
}


int
sha1(unsigned char *Ph, const unsigned char *msg, int len)
{
	return (sha1_h(Ph, msg, len, NULL));
}


#ifdef SHA_DEBUG
/* debug */
#include <stdio.h>

int
main()
{
	char *a;
	int m, i;
	unsigned char h[32 * 5];
	char b[10000000];

	for (i = 0; i < 1000000; i++)
		b[i] = 'a';
	b[i] = '\0';

	a = "abcdbcdecdefdefgefghfghighijhijkijkljklmklmnlmnomnopnopq";
	a = b;
	a = "abc";

	m = sha1(h, a, strlen(a));

	for (i = 0; i < 160 / 8; i++) {
		printf("%02x", h[i] & 0xff);
		if (i % 4 == 3)
			printf(" ");
	}
	printf("\n");


	return (0);
}

#endif

