/* $OpenXM: OpenXM/src/ox_ntl/crypt/sha1/sha1test.c,v 1.1 2004/05/16 15:02:39 iwane Exp $ */

/* openssl sha1 ${FILE} */

#include <stdio.h>
#include <unistd.h>
#include <fcntl.h>

#include "sha1.h"

void
Usage(char *cmd)
{
	printf("Usage: %s [-f][-s] file\n", cmd);
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
	int flag = 0;

	enum {
		SHA = 1,
		F_SHA = 2
	};

	for (i = 1; i < argc; i++) {
		if (strcmp(argv[i], "-f") == 0) {
			flag |= F_SHA;
		} else if (strcmp(argv[i], "-s") == 0) {
			flag |= SHA;
		} else {
			break;
		}
	}

	if (flag == 0)
		flag |= SHA;
	argc -= i;

	if (argc != 1)
		Usage(argv[0]);

	argv += i - 1;

	if (flag & SHA) {
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
	}

	if (flag & F_SHA) {
		int fd;

		fd = open(argv[1], O_RDONLY, NULL);
		if (fd == -1) {
			fprintf(stderr, "file not found: %s\n", argv[1]);
			exit (-1);
		}

		fsha1(sha, fd);
		close(fd);

		printf("SHA1(%s)= ", argv[1]);
		for (i = 0; i < 20; i++) {
			printf("%02x", sha[i] & 0xff);
		}
		printf("\n");
	}


	return (0);
}
