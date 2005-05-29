/* $OpenXM$ */
/*
 * Camellia
 * http://info.isl.ntt.co.jp/crypt/camellia/index.html
 */

#include "camellia.h"

#include <stdio.h>


#if HAVE_CONFIG_H
#include "config.h"
#endif /* HAVE_CONFIG_H */


#define DPRINTF(str, x, n)   do DPRINTF__(str, (x), (n)) while(0)

/*
*/
#define DPRINTF__(str, x, n)    { \
        int _i;                    \
        printf("%s: ", str);       \
        for (_i = 0; _i < n; _i++) {             \
                printf("%02x", x[_i] & 0xff);    \
                if (_i % 8 == 7)                 \
                        printf(" ");             \
        }                                        \
	printf("\n"); \
}



int
main(int argc, char *argv[])
{
	char k[1028] = "\x01\x23\x45\x67\x89\xab\xcd\xef\xfe\xdc\xba\x98\x76\x54\x32\x10\x00\x11\x22\x33\x44\x55\x66\x77\x88\x99\xaa\xbb\xcc\xdd\xee\xff";
	char v[1028] = "\x01\x23\x45\x67\x89\xab\xcd\xef\xfe\xdc\xba\x98\x76\x54\x32\x10";
	char e1[1000];
	char m1[1000];
	camellia_key ckey;
	int len = 256;
	int bits[] = {128, 192, 256};
	const char *ans[] = {
		"\x67\x67\x31\x38\x54\x96\x69\x73\x08\x57\x06\x56\x48\xea\xbe\x43",
		"\xb4\x99\x34\x01\xb3\xe9\x96\xf8\x4e\xe5\xce\xe7\xd7\x9b\x09\xb9",
		"\x9a\xcc\x23\x7d\xff\x16\xd7\x6c\x20\xef\x7c\x91\x9e\x3a\x75\x09",
	};
	int i;

	if (argc > 1) {
		memcpy(k, argv[1], strlen(argv[1]));
		ans[0] = ans[1] = ans[2] = NULL;
	}
	if (argc > 2) {
		memcpy(v, argv[2], strlen(argv[2]));
	}


	for (i = 0; i < sizeof(bits) / sizeof(bits[0]); i++) {
		len = bits[i];
		printf("----\n");

		DPRINTF("key", k, len / 8);

		camellia_keyset(k, len / 8, &ckey);
		camellia_enc(&ckey, v, e1);

		DPRINTF("enc", e1, 128 / 8);

		if (ans[i] && memcmp(e1, ans[i], 128 / 8)) {
			printf("ENC ERROR\n");
			exit(1);
		}


		camellia_dec(&ckey, e1, m1);
		DPRINTF("dec", m1, 128 / 8);

		if (memcmp(m1, v, 128 / 8)) {
			printf("DEC ERROR\n");
			exit(1);
		}
	}


	return (0);
}

/* EOF */

