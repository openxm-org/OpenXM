/* -*- mode: C -*- */
/* $OpenXM: OpenXM/src/oxc/oxc.c,v 1.2 2000/10/13 07:39:10 ohara Exp $ */

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include "mysocket.h"
#include "ox_toolkit.h"
#include "sm.h"

/* oxc -c password -p port -h hostname */
static char *remote_host = "";
static short port = 0;
static char *password = "";

int oxf_connect_dup(char *remote, short port)
{
    int fd = mysocketOpen(remote, port);
    /* Here do we need to confirm? */
    dup2(fd, 3);
    dup2(fd, 4); /* is it necessary? maybe fd == 4. */
    return fd;
}

int lf_oxc_open_main(char *cmd, short port)
{
    int pid = 0;
    if (cmd != NULL && (pid = fork()) == 0) {
        oxf_connect_dup(remote_host, port);
        fprintf(stderr, "oxc: oxc_open(%s, %d)\n", cmd, port);
        execlp(cmd, cmd, NULL);
    }
    fprintf(stderr, "oxc: cannnot oxc_open(%s, %d).\n", cmd, port);
    return pid; /* if error, pid == 0 */
}

OXFILE *connection()
{
    OXFILE *oxfp = oxf_connect_active(remote_host, port);
    if (oxfp != NULL) {
        oxf_confirm_server(oxfp, password);
        oxf_determine_byteorder_server(oxfp);
    }
    return oxfp;
}

/* xterm, kterm, rxvt, gnome-terminal, ... */
static char *xterminal()
{
	char *e = getenv("OpenXM_XTERM");
	return (e != NULL)? e: "xterm";
}

int main(int argc, char *argv[])
{
    OXFILE *oxfp;
	char *port_s = "";
	char *xterm =  xterminal();
	char *myname = argv[0];
	int oxlog = 0;
    int c;

    while ((c = getopt(argc, argv, "c:p:h:x")) != -1) {
        switch(c) {
        case 'h':
            remote_host = optarg;
            break;
        case 'c':
            password = optarg;
            break;
        case 'p':
            port = atoi(optarg);
			port_s = optarg;
            break;
		case 'x':
			if (getenv("DISPLAY") != NULL) {
				oxlog = 1;
			}
            break;
        default:
        }
    }
    argc -= optind;
    argv += optind;

	if (oxlog) {
		execlp(xterm, xterm, "-e", myname, 
			   "-h", remote_host, "-p", port_s, "-c", password);
	}
    fprintf(stderr, "start connection!\n");
    if (strlen(remote_host) == 0 || strlen(password) == 0 || port == 0) {
        fprintf(stderr, "oxc: invalid arguments.\n");
		fprintf(stderr, "usage: oxc -p port -h host -c password.\n");
    }else if ((oxfp = connection()) == NULL) {
        fprintf(stderr, "oxc: cannot connect.\n");
    }else {
		fprintf(stderr, "oxc: oxfp = %p, fd = %d\n", oxfp, oxfp->fd);
	    sysinfo_set(20001006, "v2000.10.06", "oxc");
		sm(oxfp);
	}
    return 0;
}
