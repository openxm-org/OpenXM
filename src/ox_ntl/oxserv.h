/* $OpenXM: OpenXM/src/ox_ntl/oxserv.h,v 1.1 2003/11/03 03:11:21 iwane Exp $ */

#ifndef __OX_SERVE_H__
#define __OX_SERVE_H__

#include "ox_toolkit.h"

#ifndef OXSERV_FAILURE
#define OXSERV_FAILURE	(-1)
#define OXSERV_SUCCESS  ( 0)
#endif

#ifdef __cplusplus
extern "C" {
#endif

void	 oxserv_delete_cmo	(cmo *);

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
