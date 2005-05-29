/* $OpenXM: OpenXM/src/ox_ntl/crypt/rsa/gmprsatest.c,v 1.1 2004/08/16 03:59:58 iwane Exp $ */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <gmp.h>

#include "gmprsa.h"

/*+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*
 * ALLOC
 *+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/
#define RSA_KEY_DEBUG 0
#define RSA_KEY_PRINT 1


static int
rsa_alloc_cnt(int n)
{
	static int cnt = 0;

	cnt += n;

	return (cnt);
}


static void *
rsa_malloc(size_t size)
{
	void *ptr;

	ptr = malloc(size);
	if (ptr) {
		rsa_alloc_cnt(1);
#if RSA_KEY_DEBUG
		printf("%2d: %p: alloc  [%d]\n", rsa_alloc_cnt(0), ptr, size);
#endif
	}

	return (ptr);
}

static void
rsa_free(void *ptr, size_t size)
{
	free(ptr);
	rsa_alloc_cnt(-1);
#if RSA_KEY_DEBUG
	printf("%2d: %p: free\n", rsa_alloc_cnt(0) + 1, ptr);
#endif
}


void *
rsa_realloc(void *org, size_t old, size_t size)
{
	void *ptr;

	ptr = malloc(size);
	memcpy(ptr, org, old);

#if RSA_KEY_DEBUG
	printf("%2d: -%p: realloc [%d]\n", rsa_alloc_cnt(0), org, old);
	printf("%2d: +%p: realloc [%d]\n", rsa_alloc_cnt(0), ptr, size);
#endif

	free(org);
	return (ptr);
}

void
rsa_set_memory_functions(void)
{
	mp_set_memory_functions(rsa_malloc, rsa_realloc, rsa_free);
}



int
main(int argc, char *argv[])
{
	mpz_t a, b, c;
	int add = 1024;
	rsa_key rsa;

	char *msg = "OpenXM Project.";

	char *msgp = msg;
	int total = strlen(msg) + 1;
	unsigned char eb[9024], *ebp = eb;
	unsigned char db[9024], *dbp = db;
	int x;
	int a1, a2 = 1;

	if (argc >= 2)
		add = atoi(argv[1]);

	rsa_set_memory_functions();

	mpz_init(a);
	mpz_init(b);
	mpz_init(c);

	mpz_fac_ui(a, 120);

	rsa_init(&rsa);

printf("keygen\n"); fflush(stdout);
	for (a1 = 0; a1 < a2; a1++) {
		x = rsa_keygen(&rsa, a, b, 1024, 80);
		printf("\nkeygen = %d, a1=%d\n", x, a1); fflush(stdout);
		mpz_add_ui(a, a, a1);
		mpz_sub_ui(b, b, a1);
	}

#if 1
	gmp_printf("n=%Zx\n", rsa.mod);
	gmp_printf("p=%Zx\n", rsa.p);
	gmp_printf("q=%Zx\n", rsa.q);
#endif


#define ENC 0


	for (x = 0; total > 0; x++) {
#if ENC
		a1 = rsa_encrypt_by_public_key(&rsa, ebp, msgp, strlen(msgp) + 1, RSA_PKCS_1_PADDING);
#else
		a1 = rsa_encrypt_by_private_key(&rsa, ebp, msgp, strlen(msgp) + 1, RSA_PKCS_1_PADDING);
#endif

		printf("a1 = %d, total = %d\n", a1, total);
		if (a1 < 0)
			exit(0);

		msgp += a1;
		ebp += rsa.k;

		total -= a1;
	}

	ebp = eb;
	for (; x > 0; x--) {
#if ENC
		a2 = rsa_decrypt_by_private_key(&rsa, dbp, ebp, RSA_PKCS_1_PADDING);
#else
		a2 = rsa_decrypt_by_public_key(&rsa, dbp, ebp, RSA_PKCS_1_PADDING);
#endif

printf("a2 = %d: x = %d\n", a2, x);
		if (a2 < 0) {
			printf("decode failed\n");
			exit(0);
		}

		dbp += a2;
		ebp += rsa.k;
	}

	printf("db = %s\n", db);

	mpz_clear(a);
	mpz_clear(b);
	mpz_clear(c);

	rsa_clear(&rsa);

	printf("gmp_info: %d, %d -- leak = %d\n", __GNU_MP_VERSION, __GNU_MP_VERSION_MINOR, rsa_alloc_cnt(0));

	return (0);
}


