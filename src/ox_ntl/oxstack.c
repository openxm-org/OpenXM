/* $OpenXM: OpenXM/src/ox_ntl/oxstack.c,v 1.1 2003/11/08 12:34:01 iwane Exp $ */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>

#include "oxstack.h"


#define DPRINTF(x)	printf x; fflush(stdout)

#define OXSERV_INIT_STACK_SIZE 2048
#define OXSERV_EXT_STACK_SIZE  2048

/*===========================================================================*
 * Global Variables.
 *===========================================================================*/
/* cmo stack */
static int G_ox_stack_size = 0;
static int G_ox_stack_pointer = 0;
static oxstack_node **G_ox_stack = NULL;


/*===========================================================================*
 * CMO STACK FUNCTIONs
 *===========================================================================*/
/*****************************************************************************
 * return the number of cmo in the stack.
 *
 * PARAM   : NONE
 * RETURN  : the number of cmo in the stack.
 *****************************************************************************/
int
oxstack_get_stack_pointer()
{
	return (G_ox_stack_pointer);
}


/*****************************************************************************
 * initialize stack.
 *
 * PARAM   : NONE
 * RETURN  : if success return OXSERV_SUCCESS, else OXSERV_FAILURE.
 *****************************************************************************/
int
oxstack_init_stack(void)
{
	free(G_ox_stack);

	G_ox_stack_pointer = 0;
	G_ox_stack_size = OXSERV_INIT_STACK_SIZE;
	G_ox_stack = (oxstack_node **)malloc(G_ox_stack_size * sizeof(oxstack_node *));
	if (G_ox_stack == NULL) {
		DPRINTF(("server: %d: %s\n", errno, strerror(errno)));
		return (OXSERV_FAILURE);
	}

	return (OXSERV_SUCCESS);
}

/*****************************************************************************
 * 
 * PARAM   : NONE
 * RETURN  : if success return OXSERV_SUCCESS, else OXSERV_FAILURE.
 *****************************************************************************/
int
oxstack_extend_stack(void)
{
	int size2 = G_ox_stack_size + OXSERV_EXT_STACK_SIZE;
	oxstack_node **stack2 = (oxstack_node **)malloc(size2 * sizeof(oxstack_node *));
	if (stack2 == NULL) {
		DPRINTF(("server: %d: %s\n", errno, strerror(errno)));
		return (OXSERV_FAILURE);
	}

	memcpy(stack2, G_ox_stack, G_ox_stack_size * sizeof(oxstack_node *));
	free(G_ox_stack);

	G_ox_stack = stack2;
	G_ox_stack_size = size2;

	return (OXSERV_SUCCESS);
}

/*****************************************************************************
 * push a cmo onto the topof the stack.
 *
 * PARAM   : m  : the cmo to be pushed on the stack.
 * RETURN  : if success return OXSERV_SUCCESS, else OXSERV_FAILURE.
 *****************************************************************************/
int
oxstack_push(oxstack_node *m)
{
	int ret;

	if (G_ox_stack_pointer >= G_ox_stack_size) {
		ret = oxstack_extend_stack();
		if (ret != OXSERV_SUCCESS)
			return (ret);
	}

	G_ox_stack[G_ox_stack_pointer] = m;
	G_ox_stack_pointer++;

	return (OXSERV_SUCCESS);
}

int
oxstack_push_cmo(cmo *c)
{
	return (oxstack_push(oxstack_node_init(c)));
}


/*****************************************************************************
 * remove thd CMO at the top of this stack and
 * returns that cmo as the value of this function.
 *
 * PARAM   : NONE
 * RETURN  : CMO at the top of the stack.
 *****************************************************************************/
oxstack_node *
oxstack_pop(void)
{
	oxstack_node *c;
	if (G_ox_stack_pointer > 0) {
		G_ox_stack_pointer--;
		c = G_ox_stack[G_ox_stack_pointer];
		G_ox_stack[G_ox_stack_pointer] = NULL;
		return (c);
	}
	return (NULL);
}

/*****************************************************************************
 * return the cmo at the specified position in the stack without removing it from the stack.
 *
 * PARAM : i : position in the stack.
 * RETURN: thd cmo at the specified position in the stack.
 *****************************************************************************/
oxstack_node *
oxstack_get(int i)
{
	if (i < G_ox_stack_pointer && i >= 0) {
		return (G_ox_stack[i]);
	}
	return (NULL);
}


/*****************************************************************************
 * return the cmo at the top of the stack without removing it from the stack.
 * PARAM : NONE
 * RETURN: the cmo at the top of the stack.
 *****************************************************************************/
oxstack_node *
oxstack_peek(void)
{
	return (oxstack_get(G_ox_stack_pointer - 1));
}



/*****************************************************************************
 * destroy
 * 
 * PARAM : NONE
 * RETURN: NONE
 *****************************************************************************/
void
oxstack_dest(void)
{
	free(G_ox_stack);
}


/*****************************************************************************
 * destroy
 * 
 * PARAM : NONE
 * RETURN: NONE
 *****************************************************************************/
oxstack_node	*
oxstack_node_init(cmo *c)
{
	oxstack_node *p;

	p = malloc(sizeof(oxstack_node));
	memset(p, 0, sizeof(*p));
	
	p->c = c;
	return (p);
}


