/* -*- mode: C -*- */
/* $OpenXM$ */

/* A sample implementation of an OpenXM client with OpenXM C library */

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <errno.h>
#include <fcntl.h>
#include <sys/param.h>

#include "mysocket.h"
#include "ox_toolkit.h"

OXFILE *sv;

int dumpx(OXFILE *oxfp, int n)
{
    unsigned char buff[2048]; 
    int i;
    int len = oxf_read(buff, 1, n, oxfp);

    fprintf(stderr, "I have read %d byte from socket.\n", len);
    for(i = 0; i < len; i++) {
        fprintf(stderr, "%02x ", buff[i]);
        if (i%20 == 19) {
            fprintf(stderr, "\n");
        }
    }
    fprintf(stderr, "\n");
    return len;
}

#define SIZE_CMDLINE  8192

static int  size = SIZE_CMDLINE;
static char cmdline[SIZE_CMDLINE];

static int prompt()
{
    fprintf(stdout, "> ");
    fgets(cmdline, size, stdin);
    init_parser(cmdline);
}

#define VERSION    0
#define ID_STRING  "v0.0"

int test_0()
{
    cmo* c = NULL;
#ifdef DEBUG
    fprintf(stderr, "zclient:: calling ox_mathcap().\n");
    c = (cmo *)ox_mathcap(sv);
    fprintf(stderr, "zclient:: cmo received.(%p)\n", c);
#else
    c = (cmo *)ox_mathcap(sv);
#endif
    print_cmo(c);
    fflush(stderr);

    mathcap_sysinfo_set(VERSION, ID_STRING, "zclient");
    send_ox_cmo(sv, mathcap_get());

    ox_reset(sv);
    send_ox_cmo(sv, (cmo *)new_cmo_string("N[ArcTan[1]]"));
    send_ox_command(sv, SM_executeStringByLocalParser);
    send_ox_command(sv, SM_popCMO);
    receive_ox_tag(sv);
    c = receive_cmo(sv);
    fprintf(stderr, "zclient:: cmo received.\n");
    print_cmo(c);
}

int test_1()
{
    cmo *c, *m;

	mathcap_sysinfo_set(1000, "test!", "zclient");
	m = mathcap_get();
    fprintf(stderr, "zclient:: test cmo_mathcap.\n");
    send_ox_cmo(sv, m);
    send_ox_command(sv, SM_popCMO);
    receive_ox_tag(sv);
    c = receive_cmo(sv);
    fprintf(stderr, "zclient:: cmo received.(%p)\n", c);
    print_cmo(c);
    fputc('\n', stderr);
}

static OXFILE *mysocketAccept2(int listened, char *passwd)
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
    char localhost[MAXHOSTNAMELEN];
    char *passwd = generate_otp();

    if (gethostname(localhost, MAXHOSTNAMELEN)==0) {
        fprintf(stderr, "zclient:: localhost = %s.\n", localhost);
        listen = mysocketListen(localhost, &port);
        if (oxc_start(remote_host, port, passwd) != 0) {
            fprintf(stderr, "zclient:: remotehost = %s.\n", remote_host);
            return mysocketAccept2(listen, passwd);
        }
    }
    return NULL;
}

/*  Example:
  zclient
  >(OX_DATA,(CMO_INT32,123))
  >(OX_COMMAND,(SM_popCMO))
 */

int main(int argc, char* argv[])
{
    OXFILE *ctlp;
    char *server = "ox_sm1";

    setbuf(stderr, NULL);

    if (argc>1) {
        server = argv[1];
    }

    fprintf(stderr, "zclient:: I try to open a server.\n");
    if ((ctlp = open_server("izumi")) == NULL) {
        fprintf(stderr, "zclient:: I cannot open a server.\n");
        exit(1);
    }
    fprintf(stderr, "zclient:: I succeed to open a server.\n");
    return 0;
}
