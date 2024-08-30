/* -*- mode: C; coding: euc-japan -*- */
/* $OpenXM: OpenXM/src/ox_toolkit/oxf_old.c,v 1.9 2015/08/27 03:03:34 ohara Exp $ */

/* このモジュールは互換性のためのものです。*/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <fcntl.h>
#include <time.h>

#if defined(_MSC_VER) || defined(__MINGW32__)
#include <io.h>
#define MAXHOSTNAMELEN 256
#else
#include <unistd.h>
#include <sys/file.h>
#include <sys/param.h>
#endif

#if defined(__sun__)
#include <netdb.h>
#include <sys/types.h>
#include <netinet/in.h>
#endif

#include "mysocket.h"
#include "ox_toolkit.h"

OXFILE *oxf_control_set(OXFILE *oxfp, OXFILE *ctl)
{
	oxfp->control = ctl;
	return oxfp;
}

static char *OpenXM_HOME = "/usr/local/OpenXM";
static char *concat_openxm_home_bin(char *s);

OXFILE *         ox_start(char* host, char* prog1, char* prog2);
OXFILE *         ox_start_insecure_nonreverse(char* host, short portControl, short portStream);
OXFILE *         ox_start_remote_with_ssh(char *dat_prog, char* host);

#if 0
/* 将来計画: start 関数は次のような設計であるべきだろう */

OXFILE *oxf_connect_client()
{
    short port   = 0; /* Listen() に決めさせる. */
    int listened = Listen(&port);

    execute_server(port);
    return oxf_connect_passive(listened); /* ここで、認証, バイトオーダの決定 */
}

OXFILE *ox_start_calc(char *cserver)
{
    short port   = 0; /* Listen() に決めさせる. */
    int listened;

    listened = Listen(&port);
    oxc_open(oxfp_ctl, port, cserver); /* ここでpasswordを送る手もあるが... */
    return oxf_connect_passive(listened); /* 認証しない. バイトオーダは決定する. */

}

int ox_start()
{
    OXFILE *oxfp_ctl, *oxfp_calc;

    oxfp_ctl = oxf_connect_client();
    /* mathcap の交換 */
    oxfp_calc = ox_start_calc("ox_sm1");
    return oxf_control_set(oxfp_calc, oxfp_ctl);
}
#endif

static OXFILE *mysocketAccept2(int listened, char *passwd)
{
    OXFILE *oxfp = oxf_connect_passive(listened);
    if(oxf_confirm_client(oxfp, passwd)) {
        oxf_determine_byteorder_client(oxfp);
        return oxfp;
    }
    oxf_close(oxfp);
    return NULL;
}

/* The environment variable OpenXM_HOME must be defined. */
static char *concat_openxm_home_bin(char *s)
{
    char *path;

    /* if s includes '/' then it is not concaticated. */
    if (strchr(s, '/') != NULL) {
        return s;
    }

    path = MALLOC(strlen(OpenXM_HOME)+6+strlen(s));
    sprintf(path, "%s/bin/%s", OpenXM_HOME, s);
    return path;
}

void set_OpenXM_HOME()
{
	char *e;
	if ((e = getenv("OpenXM_HOME")) != NULL
#if defined(__CYGWIN__)
		|| (e = getenv("OPENXM_HOME")) != NULL
#endif
		) {
		OpenXM_HOME = e;
    }
}

#if !defined(_MSC_VER)
void ox_exec_local(char* ctl_prog, char* dat_prog, int portControl, int portStream, char *passwd)
{
    char  ctl[128], dat[128];
    char localhost[MAXHOSTNAMELEN];

    sprintf(ctl, "%d", portControl);
    sprintf(dat, "%d", portStream);

    set_OpenXM_HOME();
    ctl_prog = concat_openxm_home_bin(ctl_prog);
    dat_prog = concat_openxm_home_bin(dat_prog);

    if (gethostname(localhost, MAXHOSTNAMELEN)==0) {
		if (fork() == 0) {
			execlp("oxlog", "oxlog", "xterm", "-icon", "-e", ctl_prog,
				   "-reverse", "-ox", dat_prog,
				   "-data", dat, "-control", ctl, "-pass", passwd,
				   "-host", localhost, NULL);
			exit(1);
		}
	}
}

/*
   (-reverse 版の ox_start)
   ox_start は クライアントが呼び出すための関数である.
   サーバでは使われない.  ctl_prog はコントロールサーバであり,
   -ox, -reverse, -data, -control, -pass, -host
   というオプションを理解することを仮定する. dat_prog は計算サーバである.
   接続時には, ct を先にオープンする.
*/

OXFILE *ox_start(char* host, char* ctl_prog, char* dat_prog)
{
    OXFILE *st, *ct;
    char *passwd = generate_otp();
    int   listen[2];
    int   ports[2] = {0};

	/* host を無視 */
    listen[0] = oxf_listen(&ports[0]);
    listen[1] = oxf_listen(&ports[1]);

    ox_exec_local(ctl_prog, dat_prog, ports[0], ports[1], passwd);
    ct = mysocketAccept2(listen[0], passwd);
    if (ct != NULL) {
        usleep(10);   /* zzz... */
        st = mysocketAccept2(listen[1], passwd);
        if (st != NULL) {
            return oxf_control_set(st, ct);
        }
    }
    return NULL;
}

/*
   (-insecure 版の ox_start)  まだ、中身はありません。
   ox_start_insecure は クライアントが呼び出すための関数である.
   接続時には, ct を先にオープンする.
   既定値:
   portControl = 1200
   portStream  = 1300
*/

OXFILE *ox_start_insecure(char* host, short portControl, short portStream)
{
    OXFILE *ct, *st;
    ct = oxf_connect_active(host, portControl);
    /* ox は insecure のとき byte order の決定が正しくできないようだ... */
    /* oxf_determine_byteorder_client(ct); */
    /* waiting 10 micro second. */
    usleep(10);
    st = oxf_connect_active(host, portStream);
    oxf_determine_byteorder_client(st);
    return oxf_control_set(st, ct);
}

/* ssh -f host oxlog xterm -e ox -ox ox_asir ... */
void ssh_ox_server(char *remote_host, char *ctl_prog, char *dat_prog, short portControl, short portStream)
{
    char buf[2][10];
/*
    ctl_prog = concat_openxm_home_bin(ctl_prog);
    dat_prog = concat_openxm_home_bin(dat_prog);
*/

    if (fork() == 0) {

        sprintf(buf[0], "%hd", portStream);
        sprintf(buf[1], "%hd", portControl);

        execlp("ssh", "ssh", "-f", remote_host, "oxlog", "xterm", "-icon",
              "-e", ctl_prog, "-insecure", "-ox", dat_prog,
              "-data", buf[0], "-control", buf[1],
              "-host", remote_host, NULL);
        exit(1);
    }

    sleep(1); /* wait ssh */
}

OXFILE *ox_start_remote_with_ssh(char *dat_prog, char* remote_host)
{
    ssh_ox_server(remote_host, "ox", dat_prog, 1200, 1300);
    return ox_start_insecure(remote_host, 1200, 1300);
}
#endif
