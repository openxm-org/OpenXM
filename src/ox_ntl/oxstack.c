/* $OpenXM$ */

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
static cmo **G_ox_stack = NULL;


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
	G_ox_stack = (cmo **)malloc(G_ox_stack_size * sizeof(cmo *));
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
	cmo **stack2 = (cmo **)malloc(size2 * sizeof(cmo *));
	if (stack2 == NULL) {
		DPRINTF(("server: %d: %s\n", errno, strerror(errno)));
		return (OXSERV_FAILURE);
	}

	memcpy(stack2, G_ox_stack, G_ox_stack_size * sizeof(cmo *));
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
oxstack_push(cmo *m)
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

/*****************************************************************************
 * remove thd CMO at the top of this stack and
 * returns that cmo as the value of this function.
 *
 * PARAM   : NONE
 * RETURN  : CMO at the top of the stack.
 *****************************************************************************/
cmo *
oxstack_pop(void)
{
	cmo *c;
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
cmo *
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
cmo *
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



