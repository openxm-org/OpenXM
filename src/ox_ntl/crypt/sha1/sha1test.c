/* $OpenXM$ */

/* openssl sha1 ${FILE} */

#include <stdio.h>

#include "sha1.h"

void
Usage(char *cmd)
{
	printf("Usage: %s file\n", cmd);
	exit (-1);
}


int
main(int argc, char *argv[])
{
	FILE *fp;
	char buf[1000000];
	int ret;
	int i;
	unsigned char sha[100];

	if (argc != 2)
		Usage(argv[0]);

	fp = fopen(argv[1], "r");
	if (fp == NULL) {
		fprintf(stderr, "file not found: %s\n", argv[1]);
		exit (-1);
	}

	ret = fread(buf, 1, sizeof(buf), fp);

	sha1(sha, (unsigned char *)buf, ret);

	fclose(fp);

	printf("SHA1(%s)= ", argv[1]);
	for (i = 0; i < 20; i++) {
		printf("%02x", sha[i] & 0xff);
	}
	printf("\n");

	return (0);
}
