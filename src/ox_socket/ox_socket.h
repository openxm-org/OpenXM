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
 *	$OpenXM$
 */

#ifndef _OX_SOCKET_H_
#define	_OX_SOCKET_H_

#define	OX_MAX_LISTEN_SOCKS	32

struct ox_sockarray {
	int	sock[OX_MAX_LISTEN_SOCKS];
	int	nsocks;
};

struct ox_sockopt {
	int		level;
	int		option_name;
	void		*option_value;
	socklen_t	option_len;
};

int ox_connect(const char *hostname, int port, struct ox_sockopt *opt);
struct ox_sockarray *ox_listen(const char *hostname, int *port, int backlog,
			       struct ox_sockopt *opt);
int ox_accept(struct ox_sockarray *socks);
int ox_getsockname(int socket, struct sockaddr **addr, socklen_t *addrlen);
int ox_getport(int socket);

#endif /* !_OX_SOCKET_H_ */
