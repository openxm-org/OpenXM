/* $OpenXM$ */
/* see sha1.c */

#include <NTL/ZZ.h>

#include <stdlib.h>

#include "sha1.h"

int
ntl_sha1_h(ZZ &sha, const ZZ &m, unsigned int *t)
{
	unsigned char h[20]; // 160 bit
	unsigned char *msg, msg_buf[1024];
	unsigned int msglen = sizeof(msg);
	unsigned int i;
	unsigned char uch;
	int ret;

	msglen = NumBytes(m);
	if (msglen <= sizeof(msg)) {
		msg = msg_buf;
	} else {
		msg = new unsigned char[msglen];
	}

	BytesFromZZ(msg, m, msglen);

	for (i = 0; i < msglen / 2; i++) {
		uch = msg[i];
		msg[i] = msg[msglen - i - 1];
		msg[msglen - i - 1] = uch;
	}

	if (t == NULL) {
		ret = sha1(h, msg, msglen);
	} else {
		ret = sha1_h(h, msg, msglen, t);

	}

	if (msg != msg_buf)
		delete [] msg;

	for (i = 0; i < sizeof(h) / 2; i++) {
		uch = h[i];
		h[i] = h[sizeof(h) - i - 1];
		h[sizeof(h) - i - 1] = uch;
	}

	ZZFromBytes(sha, h, sizeof(h));

	return (ret);
}

int
ntl_sha1(ZZ &sha, const ZZ &m)
{
	return (ntl_sha1_h(sha, m, NULL));
}


