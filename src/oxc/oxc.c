/* -*- mode: C -*- */
/* $OpenXM: OpenXM/src/oxc/oxc.c,v 1.13 2004/12/01 17:42:46 ohara Exp $ */

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <ctype.h>
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
    pid_t pid;
    if ((pid = fork()) == 0) {
        oxf_connect_dup(remote_host, port);
        ox_printf("oxc: oxc_open(%s, %d)\n", cmd, port);
        execlp(cmd, cmd, NULL);
    }
    return pid; /* if error, pid == 0 */
}

#define MAX_RETRY  2000

OXFILE *connection()
{
    OXFILE *oxfp;
    int counter = MAX_RETRY;
    while((oxfp = oxf_connect_active(remote_host, port)) == NULL) {
        if (--counter > 0) {
            usleep(100); /* spends 100 micro seconds */
        }else {
            ox_printf("oxc: cannot connect.\n");
            return NULL;
        }
    }
    oxf_confirm_server(oxfp, password);
    oxf_determine_byteorder_server(oxfp);
    return oxfp;
}

__inline__
static char *sskip(char *s)
{
    while (isspace(*s)) {
        s++;
    }
    return s;
}

__inline__
static char *wskip(char *s)
{
    while (!isspace(*s) && *s != '\0') {
        s++;
    }
    return s;
}

static int wc(char *s)
{
    int n = 0;
    s = sskip(s);
    while(*s != '\0') {
        s = wskip(s);
        s = sskip(s);
        n++;
    }
    return n;
}

static void word(char *str, int argc, char *argv[])
{
    int i;
    char *s = strcpy(malloc(strlen(str)+1), str);
    for(i=0; i<argc; i++) {
        s = sskip(s);
        argv[i] = s;
        s = wskip(s);
        *s++ = '\0';
    }
    argv[i] = NULL;
}

__inline__
static int arglen(char *args[])
{
    int len;
    for(len = 0; args[len] != NULL; len++) {
        ;
    }
    return len;
}

/* xterm, kterm, rxvt, gnome-terminal, ... */
static char **makeargs(char **oldargs)
{
    char *e = getenv("OpenXM_XTERM");
    int argc;
    char **newargs;
    int len = arglen(oldargs);
    if (e == NULL) {
        argc = 1;
        newargs = malloc((len+1+argc)*sizeof(char *));
        newargs[0] = "xterm";
    }else {
        argc = wc(e);
        newargs = malloc((len+1+argc)*sizeof(char *));
        word(e, argc, newargs);
    }
    memcpy(newargs+argc, oldargs, (len+1)*sizeof(char *));
    return newargs;
}

/* We assume that data has the following format:
   LENGTH hostname '\0' port '\0' password '\0'
   where LENGTH is an integer with network byte order and its value
   equals to the sum of the length of three data above.
*/

void pipe_read_info(char **hostname, int *port, char **password);

int main(int argc, char *argv[])
{
    OXFILE *oxfp;
    char *port_s = "";
    char *myname = argv[0];
    int oxlog = 0;
    int c;
    int delay = 0;
    char *delay_s = "0";
	int quiet = 0;

    while ((c = getopt(argc, argv, "c:d:h:p:qx")) != -1) {
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
        case 'q':
            quiet = 1;
            break;
        case 'x':
            if (getenv("DISPLAY") != NULL) {
                oxlog = 1;
            }
            break;
        case 'd':
            delay_s = optarg;
            delay = atoi(optarg);
            break;            
        default:
            ;
        }
    }
    argc -= optind;
    argv += optind;

    if (!quiet) {
        ox_stderr_init(stderr);
	}

    if (strlen(remote_host) == 0) {
        pipe_read_info(&remote_host, &port, &password);
        port_s = malloc(32);
        sprintf(port_s, "%d", port);
    }
    if (oxlog) {
        char *common_args[] = {"-e", myname, "-d", delay_s,
                          "-h", remote_host, "-p", port_s, "-c", 
                          password, NULL};
        char **args = makeargs(common_args);
        execvp(args[0], args);
    }

    ox_printf("start connection!\n");
    usleep(delay);
    if ((oxfp = connection()) != NULL) {
        ox_printf("oxc: oxfp = %p, fd = %d\n", oxfp, oxfp->fd);
        mathcap_init("v2000.10.06", "oxc");
        sm(oxfp);
    }
    return 0;
}
