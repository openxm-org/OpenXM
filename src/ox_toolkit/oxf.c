/* -*- mode: C; coding: euc-japan -*- */
/* $OpenXM: OpenXM/src/ox_toolkit/oxf.c,v 1.3 2000/10/11 08:22:58 ohara Exp $ */

/*
   This module includes functions for sending/receiveng CMO's.
   Some commnets is written in Japanese by the EUC-JP coded
   character set.
*/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/file.h>
#include <sys/param.h>
#include <time.h>

#include "mysocket.h"
#include "ox_toolkit.h"

static int send_int32_lbo(OXFILE *oxfp, int int32);
static int send_int32_nbo(OXFILE *oxfp, int int32);
static int receive_int32_lbo(OXFILE *oxfp);
static int receive_int32_nbo(OXFILE *oxfp);

int oxf_read(void *buffer, size_t size, size_t num, OXFILE *oxfp)
{
	int n = read(oxfp->fd, buffer, size*num);
	if (n <= 0) {
		oxfp->error = 1;
	}
    return n;
}

int oxf_write(void *buffer, size_t size, size_t num, OXFILE *oxfp)
{
    return write(oxfp->fd, buffer, size*num);
}

/* sending an object of int32 type with Network Byte Order. 
   (not equal to cmo_int32 type)  */
static int send_int32_nbo(OXFILE *oxfp, int int32)
{
    int32 = htonl(int32);
    return oxf_write(&int32, sizeof(int), 1, oxfp);
}

/* sending an object of int32 type with Local Byte Order. 
   (not equal to cmo_int32 type)  */
static int send_int32_lbo(OXFILE *oxfp, int int32)
{
    return oxf_write(&int32, sizeof(int), 1, oxfp);
}

/* receiving an object of int32 type with Network Byte Order. 
   (not equal to cmo_int32 type)  */
static int receive_int32_nbo(OXFILE *oxfp)
{
    int tag;
    oxf_read(&tag, sizeof(int), 1, oxfp);
    return ntohl(tag);
}

/* receiving an object of int32 type with Local Byte Order. 
   (not equal to cmo_int32 type)  */
static int receive_int32_lbo(OXFILE *oxfp)
{
    int tag;
    oxf_read(&tag, sizeof(int), 1, oxfp);
    return tag;
}

/* socket システムコールなどで socket を開いたのち、
   fdopen(sd, "a+") でバッファリングする(予定)。("w+" ではない)
   バッファリングの後、バイトオーダを決定し、
   oxf_setopt() で関数ポインタを設定し直す。*/

OXFILE *oxf_open(int fd)
{
    OXFILE *oxfp = (OXFILE *)malloc(sizeof(OXFILE));
    oxfp->fd = fd;
    oxfp->send_int32    = send_int32_nbo;
    oxfp->receive_int32 = receive_int32_nbo;
	oxfp->control = NULL;
	oxfp->error = 0;
    return oxfp;
    /* oxfp->fp = fdopen(fd, "a+"); */
    /* return (oxfp->fp != NULL)? oxfp: NULL; */
}

OXFILE *oxf_control(OXFILE *oxfp)
{
	return oxfp->control;
}

/* The function determines a byte order of integer on the OpenXM 
   connection `oxfp'. */
/* REMARKS: 
   we request the byte order of macine integer on a local machine by
   (*(char *)&offer).  The fact depends on OX_BYTE_LITTLE_ENDIAN==1. */
void oxf_determine_byteorder_client(OXFILE *oxfp)
{
    int  offer = OX_BYTE_LITTLE_ENDIAN;
    char receiv;
    int  mode;

    oxf_read(&receiv, 1, 1, oxfp);
    oxf_write(&offer, 1, 1, oxfp);
    mode = (receiv == *(char *)&offer);
    oxf_setopt(oxfp, mode);
}

/* Server 側ではこちらを用いる */
/* いまの実装は dup されていることが前提になっている */
void oxf_determine_byteorder_server(OXFILE *oxfp)
{
    int  offer = OX_BYTE_LITTLE_ENDIAN;
    char receiv;
    int  mode;

    oxf_write(&offer, 1, 1, oxfp);
    oxf_read(&receiv, 1, 1, oxfp);
    mode = (receiv == *(char *)&offer);
    oxf_setopt(oxfp, mode);
}

void oxf_flush(OXFILE *oxfp)
{
    /* fflush(oxfp->fp); */
}

void oxf_close(OXFILE *oxfp)
{
    close(oxfp->fd);
    /* fclose(oxfp->fp); */
}

#define OXF_SETOPT_NBO  0
#define OXF_SETOPT_LBO  1

void oxf_setopt(OXFILE *oxfp, int mode)
{
    if (mode == OXF_SETOPT_LBO) {
        oxfp->send_int32    = send_int32_lbo;
        oxfp->receive_int32 = receive_int32_lbo;
    }else if (mode == OXF_SETOPT_NBO) {
        oxfp->send_int32    = send_int32_nbo;
        oxfp->receive_int32 = receive_int32_nbo;
    }
}

int oxf_listen(short *portp)
{
    char localhost[MAXHOSTNAMELEN];
    if (gethostname(localhost, MAXHOSTNAMELEN)==0) {
		return mysocketListen(localhost, portp);
	}
	return -1;
}

OXFILE *oxf_connect_active(char *hostname, short port)
{
    int fd = mysocketOpen(hostname, port);
    return oxf_open(fd);
}

OXFILE *oxf_connect_passive(int listened)
{
    int fd = mysocketAccept(listened);
    return oxf_open(fd);
}

#define LENGTH_OF_ONETIME_PASSWORD 64

/* a password generator. */
char *generate_otp()
{
	static char crypto[] = "%.,^_+-=/@0123456789ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz";
    static char otp[LENGTH_OF_ONETIME_PASSWORD+1] = {0};
    int i;

    srandom(time(NULL));
    for(i=0; i<LENGTH_OF_ONETIME_PASSWORD; i++) {
        otp[i] = crypto[random() % (sizeof(crypto)-1)];
    }
    return otp;
}

/* proceeding a one time password. */
/* if the password is right,
   then the function returns 1, if otherwise, then 0. */
int oxf_confirm_client(OXFILE *oxfp, char *passwd)
{
    int len = strlen(passwd)+1;
    char *buf = alloca(len);
    
    oxf_read(buf, 1, len, oxfp);
    return !strcmp(passwd, buf);
}

int oxf_confirm_server(OXFILE *oxfp, char *passwd)
{
    return oxf_write(passwd, 1, strlen(passwd)+1, oxfp);
}

/* example: which("xterm", getenv("PATH")); */
char *which(char *exe, const char *env)
{
    char *tok;
    char *path;
    char delim[] = ":";
    char *e = alloca(strlen(env)+1);
    strcpy(e, env);
    tok = strtok(e, delim);
    while (tok != NULL) {
        path = malloc(strlen(tok)+strlen(exe)+2);
        sprintf(path, "%s/%s", tok, exe);
        if (access(path, X_OK&R_OK) == 0) {
            return path;
        }
        free(path);
        tok = strtok(NULL, delim);
    }
    return NULL;
}

/* Remarks: ssh determines remote host by his name, i.e. by arg[0]. */
int oxc_start(char *remote_host, short port, char *passwd)
{
    char localhost[MAXHOSTNAMELEN];
    char ports[128];
    int pid = 0;
    char *cmd = "oxc";

    if (gethostname(localhost, MAXHOSTNAMELEN)==0) {
        if ((pid = fork()) == 0) {
            sprintf(ports, "%d", port);
#ifdef DEBUG
            fprintf(stderr, "oxf.c:: oxc_start() does %s(ssh) -f %s -h %s -p %s -c %s\n", remote_host, cmd, localhost, ports, passwd);
#endif
            execlp("ssh", remote_host, "-f", cmd,
                  "-h", localhost, "-p", ports,"-c", passwd, NULL);
        }
    }
    return pid;
}

/*  Example: oxf_execute_cmd(oxfp, "ox_sm1"); */
OXFILE *oxf_execute_cmd(OXFILE *oxfp, char *cmd)
{
	short port = 0;
	int listened;

	if ((listened = oxf_listen(&port)) != -1) {
		send_ox_cmo(oxfp, (cmo *)new_cmo_int32(port));
		send_ox_cmo(oxfp, (cmo *)new_cmo_string(cmd));
		send_ox_cmo(oxfp, (cmo *)new_cmo_int32(2));  /* number of arguments */
		send_ox_cmo(oxfp, (cmo *)new_cmo_string("oxc_open"));
		send_ox_command(oxfp, SM_executeFunction);
		return oxf_connect_passive(listened);
	}
	return NULL;
}
