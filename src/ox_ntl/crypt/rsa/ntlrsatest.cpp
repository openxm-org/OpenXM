/* $OpenXM: OpenXM/src/ox_ntl/crypt/rsa/ntlrsatest.cpp,v 1.1 2004/05/16 15:04:54 iwane Exp $ */

#include "ntlrsa.h"
#include <stdio.h>
#include <string.h>

static void
enctest(NtlRsa *rsa, const char *C)
{
	unsigned char code[1024];
	unsigned char buf[1024];
	int a1 = rsa->encryptByPublicKey(code, C);
	int a2 = rsa->decryptByPrivateKey(buf, code);

	printf("<%2d>: [%s] ==> [%s][%02x %02x %02x %02x %02x]", strncmp(C, (char *)buf, a1), C, buf, buf[0], buf[1], buf[2], buf[3], buf[4]);
	printf("%d %d %d\n", a1, a2, strlen(C));
	if (a1 < 0 || a2 < 0) {
		printf("error\n");
		exit(-1);
	}

	a1 = rsa->encryptByPrivateKey(code, C);
	a2 = rsa->decryptByPublicKey(buf, code);
	printf("<%2d>: [%s] ==> [%s]", strncmp(C, (char *)buf, a2), C, buf);
	printf("\t\t%d %d\n", a1, a2);
	if (a1 < 0 || a2 < 0) {
		printf("error\n");
		exit(-1);
	}
}

/*
 * example: NtlRsa
 */
int
main(int argc, char *argv[])
{
	NtlRsa rsa;
	char msg[] = "1234567890abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ%$#FRTJ$JGJSDJJEWRAQJRJEWJ\"J$J#"
			"fhdsjkahflsjdkhaflkjhdslfkajhflkjh32ruihf\n4hkjhdsajklfhlskajhflksjhdflkjshalfjkhsdfkjhjsh"
			"fdsajfdksjafksdajf\tfsajkhlfkjhsalkjfhdfdsahj hlkjahfld sj flksjdhflkjhasdlfkjhasldjfhalksjh";
	unsigned char code[1024];
	unsigned char buf[1024];
	int a1, a2;

	// keygen(seed, bit) 
	rsa.keygen(0, 512);
	cout << rsa << endl;



	for (int i = 0; i < argc; i++) {
		enctest(&rsa, argv[i]);
	}

	char *ptr = msg;
	unsigned char *p = code;
	int total = strlen(msg) + 1;
	int xxx;

	for (xxx = 0; total > 0; xxx++) {
		a1 = rsa.encryptByPublicKey(p, ptr);
		if (a1 < 0) {
			printf("encrypt error\n");
			break;
		}

		ptr += a1;
		p += rsa.getModLen();

		total -= a1;
	}

	cout << "total = " << total << endl;
	cout << "xxx = " << xxx << endl;
	p = buf;
	
	for (int lll = 0; lll < xxx; lll++) {
		
		a2 = rsa.decryptByPrivateKey(p, code + lll * rsa.getModLen()); 
		if (a2 < 0) {
			printf("decrypt error: %d, %d\n", a2, lll);
			break;
		}

		p += a2;
	}

	cout << "decrypt: " << strcmp((char *)buf, msg) << endl;
}


