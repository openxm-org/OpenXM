/* $OpenXM: OpenXM/src/ox_ntl/crypt/radix/radix64.c,v 1.1 2004/01/12 14:52:18 iwane Exp $ */
/* RFC 2440 */

#include "radix64.h"

#define CRC24_INIT 0xb704ceL
#define CRC24_POLY 0x1864cfbL

crc24
crc_octets(const unsigned char *octets, size_t len)
{
	crc24 crc = CRC24_INIT;
	int i;

	while (len--) {
		crc ^= (*octets++) << 16;
		for (i = 0; i < 8; i++) {
			crc <<= 1;
			if (crc & 0x1000000) {
				crc ^= CRC24_POLY;
			}
		}
	}

	return (crc & 0xffffffL);
}




static const char *RADIX_STR =
	"ABCDEFGHIJKLMNOPQRSTUVWXYZ"
	"abcdefghijklmnopqrstuvwxyz"
	"0123456789+/";



#define RADIX64_CONV(data, x) \
do { \
	s[0] = (data[0] >> 2) & 0x3f; \
	s[1] = ((data[0] & 0x3) << 4) | ((data[1] & 0xff) >> 4); \
	s[2] = ((data[1] & 0xf) << 2) | ((data[2] & 0xff) >> 6); \
	s[3] = data[2] & 0x3f; \
} while (0)

#define radix64_conv(data, x) RADIX64_CONV((data), (x))


int
radix64_enc(const unsigned char *data, int datalen, unsigned char *buf, int buflen)
{
	int i, j, n = 0;
	int s[4];
	char x[3] = {'\0'};

	/* parameter check */
	for (i = 0; i + 2 < datalen; i += 3) {
		radix64_conv(data + i, s);

		for (j = 0; j < 4; j++)
			buf[n++] = RADIX_STR[s[j]];
		if (n % 76 == 0)
			buf[n++] = '\n';
	}

	switch (datalen % 3) {
	case 0:
		goto _END;
	case 2:
		x[1] = data[i + 1];
	case 1:
		x[0] = data[i];
		break;
	}

	radix64_conv(x, s);
	for (j = 0; j < datalen % 3 + 1; j++)
		buf[n++] = RADIX_STR[s[j]];
	if (n % 76 == 0)
		buf[n++] = '\n';
	for (j = 0; j < 3 - datalen % 3; j++)
		buf[n++] = '=';

_END:
	buf[n] = '\0';
	return (n);
}

int
radix64_dec(const unsigned char *data, int datalen, unsigned char *buf, int buflen)
{
	int i, j, n = 0; 
	unsigned char m[4];
	unsigned char alf[128] = {'\0'};

	for (i = 0; i < 26; i++) {
		alf['A' + i] = i + 1;
		alf['a' + i] = i + 26 + 1;
	}
	for (i = 0; i < 10; i++)
		alf['0' + i] = 52 + i + 1;
	alf['+'] = 62 + 1;
	alf['/'] = 63 + 1;

	j = 0;
	for (i = 0; i < datalen; i++) {
		if (data[i] > sizeof(alf))
			continue;

		if (data[i] == '=')
			break;

		if (alf[data[i]] == 0)
			continue;

		m[j++] = alf[data[i]] - 1;

		if (j == 4) {
			buf[n++] = (0x3f & m[0]) << 2 | ((m[1] >> 4) & 0x3);
			buf[n++] = ((m[1] << 4) & 0xf0) | ((m[2] & 0xfc) >> 2);
			buf[n++] = ((m[2] & 0x3) << 6) | (m[3] & 0x3f);

			j = 0;
		}
	}

	if (j != 0) {
		for (i = j; i < 4; i++)
			m[i] = '\0';

		buf[n++] = (0x3f & m[0]) << 2 | ((m[1] >> 4) & 0x3);
		buf[n++] = ((m[1] << 4) & 0xf0) | ((m[2] & 0xfc) >> 2);
		buf[n++] = ((m[2] & 0x3) << 6) | (m[3] & 0x3f);

		n = n - 4 + j;
	}

	return (n);
}



#ifdef RADIX64_DEBUG

int
main()
{
	int n;
	int m = 0, i, len;
	int x = 1000;
	char str[1024], buf[1024];

        char *a = " yDgBO22WxBHv7O8X7O/jygAEzol56iUKiXmV+XmpCtmpqQUKiQrFqclFqUDBovzS\n vBSFjNSiVHsuAA==\n";

	m=0;
	str[m++] = '\x14';
	str[m++] = '\xfb';
	str[m++] = '\x9c';
	str[m++] = '\x03';
	str[m++] = '\xd9';
	str[m++] = '\x7e';
	radix64_enc(str, m, buf, x);
	printf("%s\n", buf);

	radix64_enc(str, m - 1, buf, x);
	printf("%s\n", buf);

	radix64_enc(str, m - 2, buf, x);
	printf("%s\n", buf);

	len = radix64_dec(a, strlen(a), buf, x);

	printf("dec=%d\n0x", m);
	for (i = 0; i < m; i++)
		printf("%02x", buf[i] & 0xff);
	printf("\n\n");

	m = radix64_enc(buf, m, str, x);

	for (i = 0; i < 80; i++)
		printf("%d", i % 10);
	printf("\n");

	printf(" %s\n", str);
	printf("%s\n", a);

	m = crc_octets(buf, len);

	str[0] = (m >> 16) & 0xff;
	str[1] = (m >>  8) & 0xff;
	str[2] =  m        & 0xff;

	radix64_enc(str, 3, buf, x);

	printf(" %s : %06x\n", buf, m);

	return (0);
}

#endif /* RADIX64_DEBUG */

