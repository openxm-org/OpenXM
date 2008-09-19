/* $OpenXM: OpenXM/src/ox_ntl/oxstack.h,v 1.1 2003/11/08 12:34:01 iwane Exp $ */

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

typedef struct oxstack_node_t {
	int tag;
	int user;
	int reserve;
	cmo *c;
	void *p; /* user node */
} oxstack_node;

int	 oxstack_init_stack	(void);
int	 oxstack_extend_stack	(void);
void	 oxstack_dest		(void);

oxstack_node	*oxstack_node_init(cmo *);
int	 oxstack_push	(oxstack_node *);
int	 oxstack_push_cmo	(cmo *);
oxstack_node	*oxstack_pop	(void);
oxstack_node	*oxstack_get	(int);
oxstack_node	*oxstack_peek	(void);

int	 oxstack_get_stack_pointer	(void);

#ifdef __cplusplus
}
#endif

#endif /* !__OX_STACK_H__ */
