/* -*- mode: C; coding: euc-japan -*- */
/* $OpenXM: OpenXM/src/ox_toolkit/mysocket.c,v 1.6 2000/12/01 16:31:11 ohara Exp $ */
/*
Q: How to get a local port number?
A: You do setsockopt() to set options and do socket(), bind().
An OS set a local port for you.
In order to get the local port, you need to do getsockname().
(See [1] pp. 91, pp. 187 for detail)

Reference
[1] W. Richard Stevens, "UNIX Network Programming", 2nd ed. Vol. 1 (Japanese version)
*/

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <errno.h>
#include <netdb.h>
#include <string.h>
#include <netinet/in.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <fcntl.h>

#if defined(__sun__)
#include <arpa/inet.h>
#endif

#include "mysocket.h"
#include "ox_toolkit.h"

typedef struct ox_sockopt {
    int       level;
    int       option_name;
    void*     option_value;
    int       option_len;
} ox_sockopt;

static int getsocket(struct sockaddr_in *mp, char *host, int port)
{
    struct hostent *ent = gethostbyname(host);

    memset(mp, '\0', sizeof(struct sockaddr_in));
    mp->sin_family = AF_INET;
    mp->sin_port = htons((short)port);
    memcpy((char *)&mp->sin_addr, ent->h_addr, ent->h_length);

    return socket(AF_INET, SOCK_STREAM, 0);
}

static int ox_connect(char *hostname, int port, ox_sockopt *opt)
{
    struct sockaddr_in serv;
    int s = getsocket(&serv, hostname, port);
    if (connect(s, (struct sockaddr *)&serv, sizeof(serv)) != 0) {
        fprintf(ox_stderr, "ox_connect: failed. socket = %d, errno = %d\n", s, errno);
        return -1;
    }
    return s;
}

static int ox_listen(char *hostname, int *port, int backlog, ox_sockopt *opt)
{
    struct sockaddr_in me;
    int s_waiting = getsocket(&me, hostname, *port);

    setsockopt(s_waiting, opt->level, opt->option_name,
               opt->option_value, opt->option_len);
    if (bind(s_waiting, (struct sockaddr *)&me, sizeof(me)) < 0
        || listen(s_waiting, backlog) < 0) {
        fprintf(ox_stderr, "ox_listen: failed.\n");
        return -1;
    }
    return s_waiting;
}

static int ox_getport(int s_waiting)
{
    struct sockaddr_in me;
    int len = sizeof(me);
    if (getsockname(s_waiting, (struct sockaddr *)&me, &len) < 0) {
        fprintf(ox_stderr, "ox_getport: failed.\n");
        return -1;
    }
    return ntohs(me.sin_port);
}

int mysocketAccept(int s_waiting)
{
    /* return ox_accept(s_waiting); */
    return accept(s_waiting, NULL, NULL);
}

int mysocketListen(char *hostname, int *port)
{
    int option     = 1;
    ox_sockopt opt = {SOL_SOCKET, SO_REUSEADDR, &option, sizeof(option)};
    int s_waiting  = ox_listen(hostname, port, 1, &opt);

    if (*port == 0) {
        *port = ox_getport(s_waiting);
    }
    return s_waiting;
}

int mysocketOpen(char* hostname, int port)
{
    return ox_connect(hostname, port, NULL);
}

#if 0
int mypipe(char *program, int fd1, int fd2)
{
    int sockfd[2];
    if (socketpair(AF_UNIX, SOCK_STREAM, 0, sockfd) < 0) {
        fprintf(ox_stderr, "socketpair: fail! errno = %d\n", errno);
    }
    if (fork() == 0) {
        /* child process */
        close(sockfd[0]);
        dup2(sockfd[1], fd1);
        dup2(sockfd[1], fd2);
        execl(program, program, NULL);
  }
  /* parent process */
    close(sockfd[1]);
    return sockfd[0];
}
#endif
