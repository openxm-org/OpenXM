/* $OpenXM$ */

/*
 * BLOWFISH: Bruce Schneier
 *  http://www.schneier.com/blowfish.html
 */

#include <stdio.h>
#include <string.h>

#include "blowfish.h"



int
main()
{
	blowfish_key key;
	uint32_t er, el;
	uint32_t dr, dl;

	int i;

	unsigned char *keystr[] = {
		(unsigned char *)"abcdefghijklmnopqrstuvwxyz",
		(unsigned char *)"Who is John Galt?"};
	uint32_t plain[][2] = {
		{0x424c4f57U, 0x46495348U},
		{0xfedcba98U, 0x76543210U},
	};
	uint32_t chiper[][2] = {
		{0x324ed0feU, 0xf413a203U},
		{0xcc91732bU, 0x8022f684U},
	};

	for (i = 0; i < 2; i++) {
		blowfish_setkey(keystr[i], strlen((char *)keystr[i]), &key);

		blowfish_enc_i(&key, plain[i][0], plain[i][1], &el, &er);

		if (el == chiper[i][0] && er == chiper[i][1]) {
			printf("test[1-%3d]. ok\n", i);
		} else {
			printf("test[1-%3d]. NG\n", i);
		}

		blowfish_dec_i(&key, el, er, &dl, &dr);
		if (dl == plain[i][0] && dr == plain[i][1]) {
			printf("test[2-%3d]. ok\n", i);
		} else {
			printf("test[2-%3d]. NG\n", i);
			printf("dec  = %08x %08x\n", dl, dr);
			printf("plain= %08x %08x\n", plain[i][0], plain[i][1]);
		}

	}

	return (0);
}

