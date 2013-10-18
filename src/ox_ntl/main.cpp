/* $OpenXM: OpenXM/src/ox_ntl/main.cpp,v 1.6 2008/09/19 10:55:40 iwane Exp $ */

/*
[1208] ox_launch(0, "/home/openxm/OpenXM/src/ox_ntl/ox_ntl");
0
[1210] load("ntl.rr");
[1229] ntl.factor(0, x^2-1);
[[1,1],[x+1,1],[x-1,1]]
[1231] ox_shutdown(0);
0
 */
#include <string.h>

#include "ox_toolkit.h"
#include "oxserv.h"
#include "oxstack.h"
#include "ntl.h"


#define VERSION 0x00000001
#define ID_STRING "$Revision$"


static void
ntl_executeFunction(const char *func, oxstack_node **arg, int argc)
{
	oxstack_node *ans;
	int i;
	char unknown_mes[50] = "Unknown Function";

	enum {
		RET_CMO,
		RET_USR,
	};

	struct {
		const char *name;
		oxstack_node *(*func)(oxstack_node **, int);
		int return_type;
	} funcs[] = {
		{"fctr", ntl_fctr, RET_USR},
		{"lll", ntl_lll, RET_USR},
		{NULL, NULL, 0}
	};

	for (i = 0; funcs[i].name != NULL; i++) {
		if (strcmp(func, funcs[i].name) == 0) {
			ans = funcs[i].func(arg, argc);
			oxstack_push(ans);
			return ;
		}
	}

	oxstack_push_cmo((cmo *)new_cmo_error2((cmo *)new_cmo_string(unknown_mes)));
}



int
main(int argc, char *argv[])
{
	const int fd = 3;
	char oxntl[10] = "ox_ntl";
	char id[30] = ID_STRING;

	OXFILE *oxfp;

	oxfp = oxf_open(fd);

	ox_stderr_init(stderr);

	oxserv_set(OXSERV_SET_EXECUTE_FUNCTION, (void *)(void (*)())ntl_executeFunction, NULL);
	oxserv_set(OXSERV_SET_DELETE_CMO, (void *)(void (*)())delete_cmon, NULL);
	oxserv_set(OXSERV_SET_GET_CMOTAG, (void *)(void (*)())get_cmon_tag, NULL);
	oxserv_set(OXSERV_SET_CONVERT_CMO, (void *)(void (*)())convert_cmon, NULL);
	oxserv_init(oxfp, VERSION, id, oxntl, NULL, NULL);

printf("\n\n");
printf("$MAPLE=%s\n", getenv("MAPLE"));
printf("$PATH=%s\n", getenv("PATH"));
printf("$LD_LIBRARY_PATH=%s\n", getenv("LD_LIBRARY_PATH"));



	oxserv_receive(oxfp);
	oxserv_dest();
	oxf_close(oxfp);

	return (0);
}


