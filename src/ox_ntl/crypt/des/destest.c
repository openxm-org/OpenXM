/* $OpenXM: OpenXM/src/ox_ntl/crypt/des/destest.c,v 1.2 2005/06/19 15:30:01 iwane Exp $ */
/*
 * FIPS 81 - Des Modes of Operation
 */

#include <stdio.h>
#include <string.h>

#include "des3.h"
#define N 4096

int
main()
{
	des_key dkey;
	des3_key dkey3;
	unsigned char key[] = "\x01\x23\x45\x67\x89\xab\xcd\xef"; /* secret key */
	unsigned char key3[3 * 8];
	unsigned char *ivp = (unsigned char *)"\x12\x34\x56\x78\x90\xab\xcd\xef";   /* initial vector */
	unsigned char iv[8];
	unsigned char enc[N], dec[N];
	int i, k;
	int ret;
	int cnt = 0;

	unsigned char ecb[] =
		"\x3f\xa4\x0e\x8a\x98\x4d\x48\x15"
		"\x6a\x27\x17\x87\xab\x88\x83\xf9"
		"\x89\x3d\x51\xec\x4b\x56\x3b\x53";
	unsigned char cbc[] =
		"\xe5\xc7\xcd\xde\x87\x2b\xf2\x7c"
		"\x43\xe9\x34\x00\x8c\x38\x9c\x0f"
		"\x68\x37\x88\x49\x9a\x7c\x05\xf6";
	unsigned char cfb8[] =
		"\xf3\x1f\xda\x07\x01\x14\x62\xee"
		"\x18\x7f\x43\xd8\x0a\x7c\xd9\xb5"
		"\xb0\xd2\x90\xda\x6e\x5b\x9a\x87";
	unsigned char cfb16[] =
		"\xf3\x09\x87\x87\x7f\x57\xf7\x3c"
		"\x36\xb6\xdb\x70\xd8\xd5\x34\x19"
		"\xd3\x86\xb2\x23\xb7\xb2\xad\x1b";
	unsigned char cfb32[] =
		"\xf3\x09\x62\x49\xa4\xdf\xa4\x9f"
		"\x33\xdc\x7b\xad\x4c\xc8\x9f\x64"
		"\xe4\x53\xe5\xec\x67\x20\xda\xb6";
	unsigned char cfb48[] =
		"\xf3\x09\x62\x49\xc7\xf4\x30\xb5"
		"\x15\xec\xbb\x85\x97\x5a\x13\x8c"
		"\x68\x60\xe2\x38\x34\x3c\xdc\x1f";
	unsigned char cfb64[] =
		"\xf3\x09\x62\x49\xc7\xf4\x6e\x51"
		"\xa6\x9e\x83\x9b\x1a\x92\xf7\x84"
		"\x03\x46\x71\x33\x89\x8e\xa6\x22";
/*
	unsigned char cfb56[] =
		"\xf3\x09\x62\x49\xc7\xf4\x6e\x2f"
		"\xfb\xe9\x55\x13\xa4\xc9\x5a\xa7"
		"\xac\x61\x21\x62\x52";
*/
	unsigned char ofb8[] =
		"\xf3\x4a\x28\x50\xc9\xc6\x49\x85"
		"\xd6\x84";
	unsigned char ofb64[] =
		"\xf3\x09\x62\x49\xc7\xf4\x6e\x51"
		"\x35\xf2\x4a\x24\x2e\xeb\x3d\x3f"
		"\x3d\x6d\x5b\xe3\x25\x5a\xf8\xc3";
	unsigned char cbc_2[] =
		"\xb9\x91\x6b\x8e\xe4\xc3\xda\x64"
		"\xb4\xf4\x4e\x3c\xbe\xfb\x99\x48"
		"\x45\x21\x38\x8f\xa5\x9a\xe6\x7d"
		"\x58\xd2\xe7\x7e\x86\x06\x27\x33";
	unsigned char cfb8_2[] =
		"\x8a\x87\x01\xd9\x21\x30\x8f\x35"
		"\x74\xbd\x1e\x74\xf4\x32\x14\x77"
		"\x0c\x7a\x34\x8f\x84\x3d\x0d\x08"
		"\x34\xb2\xc4\xfa";
	const unsigned char *plain = (unsigned char *)"Now is the time for all ";
	const unsigned char *plain2 = (unsigned char *)"7654321 Now is the time for \0\0\0\0\0\0\0\0";
	const unsigned char *p;

	for (i = 0; i < 3; i++) {
		memcpy(key3 + 8 * i, key, 8);
	}

	des_keyset(key, &dkey);
	des3_keyset(key3, &dkey3);

	k = 24;
	ret = des_enc_ecb(&dkey, k, plain, enc);

	if (memcmp(ecb, enc, k)) {
		printf("ECB enc failed.\n");
		printf("ret = %d\n", ret);
		goto _CBC;
	}

	ret = des_dec_ecb(&dkey, k, enc, dec);
	if (memcmp(plain, dec, k)) {
		printf("ECB dec failed.\n");
		printf("ret = %d\n", ret);
		goto _CBC;
	}

	printf("ECB ok.\n");
	cnt++;

_CBC:
	for (i = 0; i < 2; i++) {
		const unsigned char *ci[] = {cbc, cbc_2};
		int ks[] = {24, 32};
		const unsigned char *d[] = {plain, plain2};

		memcpy(iv, ivp, 8);

		k = ks[i];
		p = d[i];

		ret = des_enc_cbc(&dkey, iv, k, p, enc);
		if (memcmp(ci[i], enc, k)) {
			printf("CBC enc failed.\n");
			printf("ret = %d, i = %d\n", ret, i);
			goto _CFB;
		}
	
		memcpy(iv, ivp, 8);

		ret = des_dec_cbc(&dkey, iv, k, enc, dec);
		if (memcmp(p, dec, k)) {
			printf("CBC dec failed.\n");
			printf("ret = %d, i = %d\n", ret, i);
			goto _CFB;
		}

		memcpy(iv, ivp, 8);

		ret = des3_enc_cbc(&dkey3, iv, k, p, enc);
		if (memcmp(ci[i], enc, k)) {
			printf("CBC enc failed. [des3]\n");
			printf("ret = %d, i = %d\n", ret, i);
			goto _CFB;
		}

		memcpy(iv, ivp, 8);

		ret = des3_dec_cbc(&dkey3, iv, k, enc, dec);
		if (memcmp(p, dec, k)) {
			printf("CBC dec failed. [des3]\n");
			printf("ret = %d, i = %d\n", ret, i);
			goto _CFB;
		}


	}

	printf("CBC ok.\n");
	cnt++;

_CFB:
	for (i = 0; i < 6; i++) {
		int bit[] = {8, 16, 32, 48, 64, 8};
		const unsigned char *ci[] = {cfb8, cfb16, cfb32, cfb48, cfb64, cfb8_2};
		int ks[] = {24, 24, 24, 24, 24, 28};
		const unsigned char *d[] = {plain, plain, plain, plain, plain, plain2};

		memcpy(iv, ivp, 8);

		k = ks[i];
		p = d[i];
		ret = des_enc_cfb(&dkey, bit[i], iv, k, p, enc);
		if (memcmp(ci[i], enc, k)) {
			printf("CFB 8-bit enc failed.\n");
			printf("ret = %d, i = %d\n", ret, i);
			goto _OFB;
		}
	
		ret = des_dec_cfb(&dkey, bit[i], iv, k, enc, dec);
		if (memcmp(p, dec, k)) {
			printf("CFB 8-bit dec failed.\n");
			printf("ret = %d, i = %d\n", ret, i);
			goto _OFB;
		}
	}

	printf("CFB ok.\n");
	cnt++;

_OFB:
	for (i = 0; i < 1; i++) {
		int bit[] = {8, 64};
		const unsigned char *ci[] = {ofb8, ofb64};
		int ks[] = {10, 24};
		const unsigned char *d[] = {plain, plain};

		memcpy(iv, ivp, 8);

		k = ks[i];
		p = d[i];
		ret = des_ofb(&dkey, bit[i], iv, k, p, enc);
		if (memcmp(ci[i], enc, k)) {
			printf("CFB 8-bit enc failed.\n");
			printf("ret = %d, i = %d\n", ret, i);
			goto _END;
		}
	
		ret = des_ofb(&dkey, bit[i], iv, k, enc, dec);
		if (memcmp(p, dec, k)) {
			printf("CFB 8-bit dec failed.\n");
			printf("ret = %d, i = %d\n", ret, i);
			goto _END;
		}
	}

	printf("OFB ok.\n");
	cnt++;

_END:
	if (cnt == 4) {
		printf("all test is ok.\n");
	} else {
		printf("error has found.\n");
	}

	return (0);
}



