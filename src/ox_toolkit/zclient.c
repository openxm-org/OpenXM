/* -*- mode: C -*- */
/* $OpenXM: OpenXM/src/ox_toolkit/zclient.c,v 1.1 2000/10/10 06:51:41 ohara Exp $ */

/* A sample implementation of an OpenXM client with OpenXM C library */

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include "ox_toolkit.h"

static OXFILE *connection(int listened, char *passwd)
{
    OXFILE *oxfp = oxf_connect_passive(listened);
    if(oxf_confirm_client(oxfp, passwd)) {
        oxf_determine_byteorder_client(oxfp);
        return oxfp;
    }
    oxf_close(oxfp);
    return NULL;
}

OXFILE *open_server(char *remote_host)
{
    short port;
    int listen;
    char *passwd = generate_otp();

    if ((listen = oxf_listen(&port)) != -1) {
        if (oxc_start(remote_host, port, passwd) != 0) {
            fprintf(stderr, "zclient:: remotehost = %s.\n", remote_host);
            return connection(listen, passwd);
        }
    }
    return NULL;
}

int main(int argc, char* argv[])
{
    OXFILE *oxfp;
	char *remote, *cmd;

    setbuf(stderr, NULL);
	if (argc < 3) {
		fprintf(stderr, "we have a few argument.\n");
		fprintf(stderr, "Usage:\n  %s [remotehost] [command]\n", argv[0]);
		return 0;
	}

	remote = argv[1];
	cmd    = argv[2];

    if ((oxfp = open_server(remote)) == NULL) {
        fprintf(stderr, "zclient:: I cannot open a server.\n");
        exit(1);
    }
    fprintf(stderr, "zclient:: I succeed to open an OX server.\n");

	if(oxf_execute_cmd(oxfp, cmd) != NULL) {
		fprintf(stderr, "zclient:: I succeed to connect a calc server!!\n");
	}

	oxf_close(oxfp);
    fprintf(stderr, "zclient:: closed.\n");
    return 0;
}
