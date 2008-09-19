/* $OpenXM$ */
/* ***********************************************************************
 *********************************************************************** */
#define VERSION   0x00000001
#define ID_STRING "$Revision$"
   
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <limits.h>
   

#include "ox_toolkit.h"
#include "oxserv.h"
#include "oxstack.h"
   
#include "maplec.h"

#include "oxmaple.h"


static void
oxmpl_executeFunction(const char *func, oxstack_node **arg, int argc)
{
        oxstack_node *ans;
        int i;

        enum {
                RET_CMO,
                RET_USR,
        };

        struct {
                const char *name;
                oxstack_node *(*func)(oxstack_node **, int);
                int return_type;
		int argmin;
		int argmax;
        } funcs[] = {
                {"add",  oxmpl_add,  RET_USR, 2, 2},
                {"sleep",  oxmpl_sleep,  RET_USR, 1, 1},
                {"func", oxmpl_func, RET_USR, 1, INT_MAX}, /* 任意の関数を実行 */
                {"whattype", oxmpl_whattype, RET_USR, 1, 1},
                {NULL, NULL, 0}
        };


        for (i = 0; funcs[i].name != NULL; i++) {

                if (strcmp(func, funcs[i].name) == 0) {
			if (argc < funcs[i].argmin || argc > funcs[i].argmax) {
				cmo_error2 *err = new_cmo_error2((cmo *)new_cmo_string("invalid arguments(#)"));
				ans = oxstack_node_init((cmo *)err);
			} else {
                        	ans = funcs[i].func(arg, argc);
			}
                        oxstack_push(ans);
                        return ;
                }
        }

        oxstack_push_cmo((cmo *)new_cmo_error2((cmo *)new_cmo_string("Unknown Function")));


}

int
get_cmon_tag(cmo *p)
{
	return (p->tag);
}
   
int main( int argc, char *argv[] )
{
	const int fd = 3;
	int ret;
	OXFILE *oxfp;

	for (ret = 0; ret < argc; ret++) {
		printf("argv[%2d] = %s\n", ret, argv[ret]);
	}

        oxfp = oxf_open(fd);
        ox_stderr_init(stderr);

	oxserv_set(OXSERV_SET_EXECUTE_STRING_PARSER, (void *)oxmpl_executeStringParser, NULL);
	oxserv_set(OXSERV_SET_EXECUTE_FUNCTION, (void *)oxmpl_executeFunction, NULL);

        oxserv_set(OXSERV_SET_GET_CMOTAG, (void *)(void (*)())get_cmon_tag, NULL);
        oxserv_set(OXSERV_SET_CONVERT_CMO, (void *)convert_maple2cmo_, NULL);
        oxserv_set(OXSERV_SET_CONVERT_STR, (void *)convert_maple2str_, NULL);
        oxserv_init(oxfp, VERSION, ID_STRING, "ox_maple", NULL, NULL);

	ret = oxmpl_init(1, argv);
        ret = oxserv_receive(oxfp);

        oxserv_dest();
        oxf_close(oxfp);

   
	return( 0 );
}

/* EOF */

