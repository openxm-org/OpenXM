/* $OpenXM$ */

#include "ox_toolkit.h"
#include "oxserv.h"
#include "ntl.h"

#define VERSION 0x00000001
#define ID_STRING "$Revision$"

#if 1
extern "C" {
void dprintf(const char *, ...);
}
#endif

static cmo *
ntl_executeFunction(const char *func, cmo **arg, int argc)
{
	if (strcmp(func, "fctr") == 0) {
		return (ntl_fctr(arg, argc));
	}

	return (NULL);
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


