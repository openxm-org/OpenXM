/* $OpenXM: OpenXM/src/ox_ntl/main.cpp,v 1.1 2003/11/03 03:11:21 iwane Exp $ */

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
	if (strcmp(func, "fctr") == 0) {
		ans = ntl_fctr(arg, argc);
		oxstack_push(ans);
		return ;
	}

	oxstack_push((cmo *)new_cmo_error2((cmo *)new_cmo_string("Unknown Function")));
}



int
main(int argc, char *argv[])
{
	int fd = 3;
	int i;
	int ret;

	OXFILE *oxfp;

	oxfp = oxf_open(fd);

	ox_stderr_init(stderr);

	oxserv_set(OXSERV_SET_EXECUTE_FUNCTION, ntl_executeFunction, NULL);
	oxserv_init(oxfp, VERSION, ID_STRING, "ox_ntl", NULL, NULL);

	for (i = 0;; i++) {
		ret = oxserv_receive(oxfp);
		if (ret != OXSERV_SUCCESS)
			break;
	}

	oxserv_dest();
	oxf_close(oxfp);

	return (0);
}


