/* $OpenXM: OpenXM/src/ox_ntl/main.cpp,v 1.2 2003/11/08 12:34:00 iwane Exp $ */

#include "ox_toolkit.h"
#include "oxserv.h"
#include "oxstack.h"
#include "ntl.h"

#define VERSION 0x00000001
#define ID_STRING "$Revision$"

#if 0
extern "C" {
void dprintf(const char *, ...);
}
#endif


static void
ntl_executeFunction(const char *func, cmo **arg, int argc)
{
	cmo *ans;
	int i;

	struct {
		const char *name;
		cmo *(*func)(cmo **, int);
	} funcs[] = {
		{"fctr", ntl_fctr},
		{"lll", ntl_lll},
		{NULL, NULL},
	};

	for (i = 0; funcs[i].name != NULL; i++) {
		if (strcmp(func, funcs[i].name) == 0) {
			ans = funcs[i].func(arg, argc);
			oxstack_push(ans);
			return ;
		}
	}

	oxstack_push((cmo *)new_cmo_error2((cmo *)new_cmo_string("Unknown Function")));
}



int
main(int argc, char *argv[])
{
	int fd = 3;
	int ret;

	OXFILE *oxfp;

	oxfp = oxf_open(fd);

	ox_stderr_init(stderr);

	oxserv_set(OXSERV_SET_EXECUTE_FUNCTION, ntl_executeFunction, NULL);
	oxserv_set(OXSERV_SET_DELETE_CMO, delete_cmon, NULL);
	oxserv_set(OXSERV_SET_GET_CMOTAG, get_cmon_tag, NULL);
	oxserv_set(OXSERV_SET_CONVERT_CMO, convert_cmon, NULL);
	oxserv_init(oxfp, VERSION, ID_STRING, "ox_ntl", NULL, NULL);

	ret = oxserv_receive(oxfp);

	oxserv_dest();
	oxf_close(oxfp);

	return (0);
}


