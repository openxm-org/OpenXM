/* -*- mode: C; coding: euc-japan -*- */
/* $OpenXM: OpenXM/src/ox_toolkit/mysocket.c,v 1.3 2000/01/13 07:57:09 ohara Exp $ */
/* 
Q: How to get a local port number?
A: You do setsockopt() to set options and do socket(), bind().
An OS set a local port for you.
In order to get the local port, you need to do getsockname().
(See [1] pp. 91, pp. 187 for detail)

Reference
[1] W. Richard Stevens, "UNIX Network Programming", 2nd ed. Vol. 1
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

static int getsocket(struct sockaddr_in *mp, char *host, short port)
{
    struct hostent *ent = gethostbyname(host);
  
    memset(mp, '\0', sizeof(struct sockaddr_in));
    mp->sin_family = AF_INET;
    mp->sin_port = htons(port);
    memcpy((char *)&mp->sin_addr, ent->h_addr, ent->h_length);

    return socket(AF_INET, SOCK_STREAM, 0);
}

int mysocketAccept(int s_waiting)
{
    return accept(s_waiting, NULL, NULL);
}

int mysocketListen(char *hostname, short *portp)
{
    int option;
    int tmp;
    struct sockaddr_in me;
 
    int s_waiting = getsocket(&me, hostname, *portp);

    option = 1;
    setsockopt(s_waiting, SOL_SOCKET, SO_REUSEADDR, &option, sizeof(option));

    if (bind(s_waiting, (struct sockaddr *)&me, sizeof(me)) < 0) {
        fprintf(stderr, "bind: failed.\n");
        exit(1);
    }

    tmp = sizeof(me);
    if (getsockname(s_waiting, (struct sockaddr *)&me, &tmp) < 0) {
        fprintf(stderr, "getsockname is failed.\n");
        exit(1);
    }

    *portp = ntohs(me.sin_port);
    
    if (listen(s_waiting, 1) < 0) {
        fprintf(stderr, "listen: failed.\n");
        exit(1);
    } 

    return s_waiting;
}

int mysocketOpen(char* hostname, short port)
{
    struct sockaddr_in serv;
    int s = getsocket(&serv, hostname, port);

    fprintf(stderr, "get socket address for port number %d.\n", port);
    if (connect(s, (struct sockaddr *)&serv, sizeof(serv)) != 0) {
        fprintf(stderr, "connect: fail! socket = %d, errno = %d\n", s, errno);
        exit(-1);
    }
    return s;
}

#if 0
int mypipe(char *program, int fd1, int fd2)
{
    int sockfd[2];
    if (socketpair(AF_UNIX, SOCK_STREAM, 0, sockfd) < 0) {
        fprintf(stderr, "socketpair: fail! errno = %d\n", errno);
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
