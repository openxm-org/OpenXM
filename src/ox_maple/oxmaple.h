/* $OpenXM$ */
#ifndef __OX_MAPLE_H__
#define __OX_MAPLE_H__


#include "ox_toolkit.h"
#include "oxstack.h"
#include "maplec.h"

/*=====================================================*
 * oxmaple.c
 *=====================================================*/
void oxmpl_executeStringParser(char *);

int	oxmpl_init(int argc, char *argv[]);

oxstack_node *oxmpl_add	(oxstack_node **, int );
oxstack_node *oxmpl_whattype	(oxstack_node **, int );
oxstack_node *oxmpl_sleep	(oxstack_node **, int );
oxstack_node *oxmpl_func	(oxstack_node **, int );
oxstack_node *oxmpl_cmo_int32	(oxstack_node **, int );

cmo	*	convert_maple2cmo_(ALGEB);
char	*	convert_maple2str_(ALGEB);

void	oxmpl_debug_callfunc	(const char *, int, ...);


/*=====================================================*
 * m2c.c
 *=====================================================*/
cmo	*	convert_maple2cmo(MKernelVector, ALGEB);
char	*	convert_maple2str(MKernelVector, ALGEB);


/*=====================================================*
 * c2m.c
 *=====================================================*/
ALGEB convert_cmo_to_maple(MKernelVector kv, cmo *, cmo_list *);




#endif

