/* $OpenXM$ */

#ifndef __OX_STACK_H__
#define __OX_STACK_H__

#include "ox_toolkit.h"

#ifndef OXSERV_FAILURE
#define OXSERV_FAILURE	(-1)
#define OXSERV_SUCCESS  ( 0)
#endif


#ifdef __cplusplus
extern "C" {
#endif

int	 oxstack_init_stack	(void);
int	 oxstack_extend_stack	(void);
void	 oxstack_dest		(void);

int	 oxstack_push	(cmo *);
cmo	*oxstack_pop	(void);
cmo	*oxstack_get	(int);
cmo	*oxstack_peek	(void);

int	 oxstack_get_stack_pointer	(void);

#ifdef __cplusplus
}
#endif

#endif /* !__OX_STACK_H__ */
