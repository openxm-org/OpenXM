/* $OpenXM: OpenXM/src/ox_ntl/oxserv.h,v 1.2 2003/11/08 12:34:00 iwane Exp $ */

#ifndef __OX_SERVE_H__
#define __OX_SERVE_H__

#if HAVE_CONFIG_H
#include "config.h"
#endif

#include <signal.h>
#include "ox_toolkit.h"

#ifndef OXSERV_FAILURE
#define OXSERV_FAILURE	(-1)
#define OXSERV_SUCCESS  ( 0)
#endif

#ifdef __cplusplus
extern "C" {
#endif

/* signal */
extern sigset_t	G_oxserv_sigusr1;

#define	BLOCK_INPUT()	do { \
	sigprocmask(SIG_BLOCK, &G_oxserv_sigusr1, NULL); \
} while(0)

#define	UNBLOCK_INPUT()	do { \
	sigprocmask(SIG_UNBLOCK, &G_oxserv_sigusr1, NULL); \
} while(0)

/* c.f. mathcap_init in ox_toolkit */
int	 oxserv_init	(OXFILE *, int, char *, char *, int *, int *);
void	 oxserv_dest	(void);

int	 oxserv_receive	(OXFILE *);

int	 oxserv_set	(int mode, void *, void *);

#define OXSERV_SET_EXECUTE_STRING_PARSER        (0x01)
#define OXSERV_SET_EXECUTE_FUNCTION             (0x02)
#define OXSERV_SET_CONVERT_CMO                  (0x03)
#define OXSERV_SET_DELETE_CMO                   (0x04)
#define OXSERV_SET_GET_CMOTAG                   (0x05)


#ifdef __cplusplus
}
#endif

#endif /* !__OX_SERVE_H__ */
