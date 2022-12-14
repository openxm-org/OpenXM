/*-
 * Copyright (c) 2000 MAEKAWA Masahide <maekawa@math.sci.kobe-u.ac.jp>
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 * 1. Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in the
 *    documentation and/or other materials provided with the distribution.
 *
 * THIS SOFTWARE IS PROVIDED BY THE AUTHOR AND CONTRIBUTORS ``AS IS'' AND
 * ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
 * ARE DISCLAIMED.  IN NO EVENT SHALL THE AUTHOR OR CONTRIBUTORS BE LIABLE
 * FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
 * DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS
 * OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION)
 * HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT
 * LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY
 * OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF
 * SUCH DAMAGE.
 *
 *	$OpenXM: OpenXM/src/ox_socket/ox_getport.c,v 1.7 2000/12/01 17:37:53 maekawa Exp $
 */

#include <sys/types.h>
#include <sys/socket.h>

#include <netinet/in.h>

#include <stdio.h>
#include <unistd.h>
#include <err.h>
#include <netdb.h>

#include "ox_socket.h"

#ifndef HAVE_SOCKADDR_STORAGE
/* Derived from RFC2553 */
#define	_SS_MAXSIZE	128
#define	_SS_ALIGNSIZE	(sizeof(int64_t))

#ifdef HAVE_SOCKADDR_LEN
#define	_SS_PAD1SIZE	(_SS_ALIGNSIZE - sizeof(uint8_t) - sizeof(sa_family_t))
#define	_SS_PAD2SIZE	(_SS_ALIGNSIZE - sizeof(uint8_t) - sizeof(sa_family_t) \
			 - _SS_PAD1SIZE - _SS_ALIGNSIZE)
#else /* HAVE_SOCKADDR_LEN */
#define	_SS_PAD1SIZE	(_SS_ALIGNSIZE - sizeof(sa_family_t))
#define	_SS_PAD2SIZE	(_SS_MAXSIZE - sizeof(sa_family_t) \
			 - _SS_PAD1SIZE - _SS_ALIGNSIZE)
#endif /* HAVE_SOCKADDR_LEN */

struct sockaddr_storage {
#ifdef HAVE_SOCKADDR_LEN
	uint8_t		ss_len;
#endif /* HAVE_SOCKADDR_LEN */
	sa_family_t	ss_family;

	char		__ss_pad1[_SS_PAD1SIZE];
	int64_t		__ss_align;
	char		__ss_pad2[_SS_PAD2SIZE];
};
#endif /* HAVE_SOCKADDR_STORAGE */

int
ox_getport(int sock)
{
	struct sockaddr_storage ss;
	int port;

	if (getsockname(sock, (struct sockaddr *)&ss, NULL) < 0) {
		warn("ox_getport(): getsockname");
		return (-1);
	}

	switch (ss.ss_family) {
	case AF_INET:
	{
		struct sockaddr_in *sin;

		sin = (struct sockaddr_in *)&ss;
		port = ntohs(sin->sin_port);

		break;
	}
#ifdef INET6
	case AF_INET6:
	{
		struct sockaddr_in6 *sin6;

		sin6 = (struct sockaddr_in6 *)&ss;
		port = ntohs(sin6->sin6_port);

		break;
	}
#endif /* INET6 */
	default:
		/* not supported */
		port = -1;
	}

	return (port);
}
