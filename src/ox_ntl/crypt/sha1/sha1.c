/* $OpenXM: OpenXM/src/ox_ntl/crypt/sha1/sha1.c,v 1.3 2004/05/16 15:02:39 iwane Exp $ */
/* RFC 3174 - SHA-1 (US Secure Hash Algorithm 1 (SHA1))*/

#include <stdio.h>

#include "sha1.h"

#include <unistd.h>
#include <errno.h>
#include <sys/stat.h>

/* Global Constant */
static const uint32_t K[4] = {0x5a827999, 0x6ed9eba1, 0x8f1bbcdc, 0xca62c1d6};
static const uint32_t H[5] = {0x67452301, 0xefcdab89, 0x98badcfe, 0x10325476, 0xc3d2e1f0};

#define BLOCK (512 / 8)


static inline uint32_t lshift32(uint32_t x, int n)
{
	return ((x << n) | (x >> (32 - n)));
};



/**
 * sizeof(buf) >= 512 * ((len * 8 + 1) / 64)
 * len * 8 < 2^64 ==> len < 2^61
 */
static uint32_t
padding(unsigned char *buf, const unsigned char *msg, size_t length)
{
	int n;
	int i;
	size_t len = length % BLOCK;

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




static uint32_t
f(uint32_t t, uint32_t b, uint32_t c, uint32_t d)
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
sha1_md(uint32_t *h, const unsigned char *msg)
{
	int t;
	int i;

	uint32_t a, b, c, d, e, temp;
	uint32_t w[80];

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
sha1_h(unsigned char *Ph, const unsigned char *msg, size_t len, const uint32_t *hp)
{
	int i, j, cnt;
	size_t l = len;
	unsigned char buf[1024];
	uint32_t h[sizeof(H) / sizeof(H[0])];

	if (hp == NULL)
		memcpy(h, H, sizeof(H));
	else
		memcpy(h, hp, sizeof(h));

	while (l >= BLOCK) {
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
sha1(unsigned char *Ph, const unsigned char *msg, size_t len)
{
	return (sha1_h(Ph, msg, len, NULL));
}


int
fsha1_h(unsigned char *Ph, int fd, const uint32_t *hp)
{
	int i, j, cnt, ret;
	size_t l;
	off_t len;
	unsigned char buf[1024], *msg, msgbuf[1024];
	uint32_t h[sizeof(H) / sizeof(H[0])];
	int pad = 0;
	struct stat stbuf;

	if (hp == NULL)
		memcpy(h, H, sizeof(H));
	else
		memcpy(h, hp, sizeof(h));

	ret = fstat(fd, &stbuf);
	len = stbuf.st_size;
	if (len == 0)
		goto _PADDING;

	for (;;) {
		l = read(fd, msgbuf, sizeof(msgbuf));
		if (l < 0) {
			return (errno);
		}
		if (l == 0)
			break;

		len -= l;
		msg = msgbuf;

		while (l >= BLOCK) {
			sha1_md(h, msg);
			msg += BLOCK;
			l -= BLOCK;
		}

		if (len == 0) {
_PADDING:
			cnt = padding(buf, msg, stbuf.st_size);
			for (i = 0; i < cnt; i++) {
				sha1_md(h, buf + BLOCK * i);
			}
		}
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
fsha1(unsigned char *Ph, int fd)
{
	return (fsha1_h(Ph, fd, NULL));
}

