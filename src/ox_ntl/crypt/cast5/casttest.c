/* $OpenXM$ */
/*
 * rfc 2144
 *  The CAST-128 Encryption Algorithm
 *  Appendix B.
 */

#include <stdio.h>

#include "cast5.h"


int
main()
{
	cast128_key ckey;
	unsigned char key0[] = "\x01\x23\x45\x67\x12\x34\x56\x78\x23\x45\x67\x89\x34\x56\x78\x9a";
	unsigned char a[] = "\x01\x23\x45\x67\x12\x34\x56\x78\x23\x45\x67\x89\x34\x56\x78\x9a";
	unsigned char b[] = "\x01\x23\x45\x67\x12\x34\x56\x78\x23\x45\x67\x89\x34\x56\x78\x9a";
	int bits[] = {16, 10, 5};
	unsigned char ans[][9] = {
		"\x23\x8b\x4f\xe5\x84\x7e\x44\xb2",
		"\xeb\x6a\x71\x1a\x2c\x02\x27\x1b",
		"\x7a\xc8\x16\xd1\x6e\x9b\x30\x2e" };
	unsigned char plain[] = "\x01\x23\x45\x67\x89\xab\xcd\xef";
	unsigned char enc[1024], dec[1024];
	unsigned char ax[] = "\xee\xa9\xd0\xa2\x49\xfd\x3b\xa6\xb3\x43\x6f\xb8\x9d\x6d\xca\x92";
	unsigned char bx[] = "\xb2\xc9\x5e\xb0\x0c\x31\xad\x71\x80\xac\x05\xb8\xe8\x3d\x69\x6e";

	uint32_t an[4], bn[4], cn[4], dn[4];

	int i, j;
	int bit;

	for (i = 0; i < 3; i++) {

		bit = bits[i];
		cast128_keyset(key0, bit, &ckey);

		cast128_enc_c(&ckey, plain, enc);

		if (memcmp(ans[i], enc, 8) != 0) {
			printf("error. %d - enc\n", i);
			exit(1);
		}

		cast128_dec_c(&ckey, enc, dec);
		if (memcmp(plain, dec, 8) != 0) {
			printf("error. %d - dec\n", i);
			exit(1);
		}

		printf("%3d-bit is ok.\n", bit * 8);
	}


	bit = 128;
	an[0] = bn[0] = 0x01234567;
	an[1] = bn[1] = 0x12345678;
	an[2] = bn[2] = 0x23456789;
	an[3] = bn[3] = 0x3456789a;
	for (i = 0; i < 1000000; i++) {
		cast128_keyset(b, bit, &ckey);

		memcpy(dn, an, sizeof(an));

		cast128_enc_i(&ckey, an[0], an[1], &an[0], &an[1]);
		cast128_enc_i(&ckey, an[2], an[3], &an[2], &an[3]);

		cast128_dec_i(&ckey, an[0], an[1], &cn[0], &cn[1]);
		cast128_dec_i(&ckey, an[2], an[3], &cn[2], &cn[3]);

		if (dn[0] != cn[0] || dn[1] != cn[1] || dn[2] != cn[2] || dn[3] != cn[3]) {
			fprintf(stderr, "dec error. a. %d\n", i);
		}

		for (j = 0; j < 16; j++) {
			a[j] = (an[j / 4] >> (8 * (3 - j % 4))) & 0xff;
		}

		memcpy(dn, bn, sizeof(bn));

		cast128_keyset(a, bit, &ckey);
		cast128_enc_i(&ckey, bn[0], bn[1], &bn[0], &bn[1]);
		cast128_enc_i(&ckey, bn[2], bn[3], &bn[2], &bn[3]);

		cast128_dec_i(&ckey, bn[0], bn[1], &cn[0], &cn[1]);
		cast128_dec_i(&ckey, bn[2], bn[3], &cn[2], &cn[3]);

		if (dn[0] != cn[0] || dn[1] != cn[1] || dn[2] != cn[2] || dn[3] != cn[3]) {
			fprintf(stderr, "dec error. a. %d\n", i);
		}

		for (j = 0; j < 16; j++) {
			b[j] = (bn[j / 4] >> (8 * (3 - j % 4))) & 0xff;
		}
	}

	if (memcmp(a, ax, 16) != 0) {
		fprintf(stderr, "error: a\n");
		exit(1);
	}
	if (memcmp(b, bx, 16) != 0) {
		fprintf(stderr, "error: b\n");
		exit(1);
	}


	fprintf(stderr, "all test is ok\n");

	return (0);
}


