/* $OpenXM$ */

#ifndef __OX_SERVE_H__
#define __OX_SERVE_H__

#include "ox_toolkit.h"

#define OXSERV_FAILURE	(-1)
#define OXSERV_SUCCESS  ( 0)

#ifdef __cplusplus
extern "C" {
#endif

/* c.f. mathcap_init in ox_toolkit */
int	 oxserv_init(OXFILE *, int, char *, char *, int *, int *);
void	 oxserv_dest(void);

int	 oxserv_receive(OXFILE *);

int	 oxserv_set(int mode, void *, void *);

#define OXSERV_SET_EXECUTE_STRING_PARSER	(0x01)
#define OXSERV_SET_EXECUTE_FUNCTION		(0x02)
#define OXSERV_SET_CONVERT_CMO                  (0x03)


#ifdef __cplusplus
}
#endif

#endif /* !__OX_SERVE_H__ */
