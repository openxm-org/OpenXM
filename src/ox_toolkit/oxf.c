/* -*- mode: C; coding: euc-japan -*- */
/* $OpenXM: OpenXM/src/ox_toolkit/oxf.c,v 1.24 2016/06/30 01:14:00 ohara Exp $ */

/*
   This module includes functions for sending/receiveng CMO's.
*/

#if defined(_MSC_VER)
#define _CRT_RAND_S
#endif

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <fcntl.h>
#include <time.h>
#include <limits.h>

#if defined(__sun__) || defined(__FreeBSD__)
#include <netdb.h>
#include <sys/types.h>
#include <netinet/in.h>
#elif defined(__linux__)
#include <arpa/inet.h>
#endif

#if defined(__sun__)
#include <synch.h>
#else
#include <inttypes.h>
#endif

#if defined(_MSC_VER)
#include <io.h>
#include <winsock2.h>
#define X_OK 0x01
#define R_OK 0x04
#define MAXHOSTNAMELEN 256
#define srandom(s)  (srand((s)))
static int random()
{
    int r;
    rand_s(&r);
    return r;
}
#define READ(fd,buf,n)   (recv((fd),(buf),(n),0))
#define WRITE(fd,buf,n)  (send((fd),(buf),(n),0))
#else
#include <unistd.h>
#include <sys/file.h>
#include <sys/param.h>
#define READ(fd,buf,n)   (read((fd),(buf),(n)))
#define WRITE(fd,buf,n)  (write((fd),(buf),(n)))
#endif

#include "mysocket.h"
#include "ox_toolkit.h"

static mathcap *oxf_mathcap(OXFILE *oxfp);

static int send_int32_lbo(OXFILE *oxfp, int int32);
static int send_int32_nbo(OXFILE *oxfp, int int32);
static int receive_int32_lbo(OXFILE *oxfp);
static int receive_int32_nbo(OXFILE *oxfp);

static int send_int64_nbo_le(OXFILE *oxfp, double int64);
static int send_int64_lbo(OXFILE *oxfp, double int64);
static double receive_int64_nbo_le(OXFILE *oxfp);
static double receive_int64_lbo(OXFILE *oxfp);

static void pipe_send_info(int fd, char *hostname, int port, char *password);

/* translating double of little endian byte order to one of big endian. */
double htonll_le(double n)
{
    int i;
    double r;
	char *sp = (char *)&n, *dp = (char *)&r + sizeof(double)-1;
    for(i=0; i<sizeof(double); i++) {
		*dp-- = *sp++;
    }
    return r;
}

/* enable write buffering */
int oxf_setbuffer(OXFILE *oxfp, char *buf, int size)
{
    if (buf == NULL && size > 0) {
        buf = MALLOC(size);
    }
    if (oxfp->wbuf != NULL) {
        oxf_flush(oxfp);
    }
    oxfp->wbuf = buf; 
    oxfp->wbuf_size  = size;
    oxfp->wbuf_count = 0;
    return 0;
}

void OX_FD_ZERO(OXFILE_set *s)
{
    memset(s,0,sizeof(OXFILE_set));
}

void OX_FD_SET(OXFILE *oxfp,OXFILE_set *s)
{
    if (oxfp != NULL && oxfp->fd >=0 && oxfp->fd < OX_FD_SETSIZE && !FD_ISSET(oxfp->fd,&(s->fdset))) {
        FD_SET(oxfp->fd,&(s->fdset));
        s->p[oxfp->fd] = oxfp;
        s->count++; 
    }
}

void OX_FD_CLR(OXFILE *oxfp,OXFILE_set *s)
{
    if (oxfp != NULL && oxfp->fd >=0 && oxfp->fd < OX_FD_SETSIZE && FD_ISSET(oxfp->fd,&(s->fdset))) {
        FD_CLR(oxfp->fd,&(s->fdset));
        s->p[oxfp->fd] = NULL;
        s->count--; 
    }
}

int OX_FD_ISSET(OXFILE *oxfp,OXFILE_set *s)
{
    if (oxfp != NULL && oxfp->fd >=0 && oxfp->fd < OX_FD_SETSIZE) {
        return FD_ISSET(oxfp->fd,&(s->fdset));
    }
    return 0;
}

/* The argument `s' is a set of file descriptors for reading */
OXFILE *oxf_select(OXFILE_set *s, struct timeval *tv)
{
    int r;
    r = select(OX_FD_SETSIZE,&(s->fdset),NULL,NULL,tv);
    return (r<0)? NULL: s->p[r];
}

int oxf_read(void *buffer, size_t size, size_t num, OXFILE *oxfp)
{
    int n = READ(oxfp->fd, buffer, size*num);
    if (n <= 0) {
#if 0
        oxfp->error = 1;
#else
        exit(0);
#endif
    }
    return n;
}

int oxf_write(void *buffer, size_t size, size_t num, OXFILE *oxfp)
{
    size_t sz = size*num;
    if (oxfp->wbuf == NULL) { /* no buffering */
        return WRITE(oxfp->fd, buffer, sz);
    }
    if ((oxfp->wbuf_count + sz) >= oxfp->wbuf_size) {
        oxf_flush(oxfp);
        return WRITE(oxfp->fd, buffer, sz);
    }
    memcpy(oxfp->wbuf + oxfp->wbuf_count, buffer, sz);
    oxfp->wbuf_count += sz;
    return sz;
}

/* sending an object of int64 type with Network Byte Order. */
static int send_int64_nbo_le(OXFILE *oxfp, double int64)
{
    int64 = htonll_le(int64);
    return oxf_write(&int64, sizeof(double), 1, oxfp);
}

/* sending an object of int64 type with Local Byte Order. */
static int send_int64_lbo(OXFILE *oxfp, double int64)
{
    return oxf_write(&int64, sizeof(double), 1, oxfp);
}

/* receiving an object of int64 type with Network Byte Order. */
static double receive_int64_nbo_le(OXFILE *oxfp)
{
    double tag;
    oxf_read(&tag, sizeof(double), 1, oxfp);
    return htonll_le(tag);
}

/* receiving an object of int64 type with Local Byte Order. */
static double receive_int64_lbo(OXFILE *oxfp)
{
    double tag;
    oxf_read(&tag, sizeof(double), 1, oxfp);
    return tag;
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

/* (1) getting the fd by socket(2).
   (2) preparing a buffer by fdopen(fd, "a+"). (not "w+")
   (3) determining the byte order of the OX connection.
   (4) setting function pointers by oxf_setopt().
*/
OXFILE *oxf_open(int fd)
{
    OXFILE *oxfp = (OXFILE *)MALLOC(sizeof(OXFILE));
    oxfp = (OXFILE *)MALLOC(sizeof(OXFILE));
    oxfp->fd = fd;
    oxfp->send_int32    = send_int32_nbo;
    oxfp->receive_int32 = receive_int32_nbo;
    oxfp->serial_number = 0;
    oxfp->control = NULL;
    oxfp->error = 0;
    oxfp->mathcap = NULL;
    oxfp->wbuf = NULL;
    oxfp->wbuf_size = 0;
    oxfp->wbuf_count = 0;
    oxfp->send_double    = send_int64_lbo;
    oxfp->receive_double = receive_int64_lbo;
    return oxfp;
    /* oxfp->fp = fdopen(fd, "a+"); */
    /* return (oxfp->fp != NULL)? oxfp: NULL; */
}

int oxf_fileno(OXFILE *oxfp)
{
    return oxfp->fd;
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

/* If the program is an OX server, then you must use this function. */
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
    if (oxfp->wbuf != NULL) {
        WRITE(oxfp->fd, oxfp->wbuf, oxfp->wbuf_count);
        oxfp->wbuf_count = 0;
    }
}

void oxf_close(OXFILE *oxfp)
{
    oxf_flush(oxfp);
    close(oxfp->fd);
}

#define OXF_SETOPT_NBO  0
#define OXF_SETOPT_LBO  1

void oxf_setopt(OXFILE *oxfp, int mode)
{
    int m = 1;
    if (mode == OXF_SETOPT_NBO && *(char *)&m) {
		/* Little endian architecture. */
		oxfp->send_int32     = send_int32_nbo;
		oxfp->receive_int32  = receive_int32_nbo;
		oxfp->send_double    = send_int64_nbo_le;
		oxfp->receive_double = receive_int64_nbo_le;
	}else {
		oxfp->send_int32     = send_int32_lbo;
		oxfp->receive_int32  = receive_int32_lbo;
		oxfp->send_double    = send_int64_lbo;
		oxfp->receive_double = receive_int64_lbo;
    }
}

int oxf_listen(int *portp)
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
    return (fd < 0)? NULL: oxf_open(fd);
}

OXFILE *oxf_connect_passive(int listened)
{
    int fd = mysocketAccept(listened);
    return (fd < 0)? NULL: oxf_open(fd);
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
    char *buf = ALLOCA(len);
    
    oxf_read(buf, 1, len, oxfp);
    return !strcmp(passwd, buf);
}

int oxf_confirm_server(OXFILE *oxfp, char *passwd)
{
    return oxf_write(passwd, 1, strlen(passwd)+1, oxfp);
}

__inline__
static mathcap *oxf_mathcap(OXFILE *oxfp)
{
    if (oxfp->mathcap == NULL) {
        oxfp->mathcap = new_mathcap();
    }
	return oxfp->mathcap;
}

cmo_mathcap *oxf_cmo_mathcap(OXFILE *oxfp)
{
	return mathcap_get(oxf_mathcap(oxfp));
}

void oxf_mathcap_update(OXFILE *oxfp, cmo_mathcap *ob)
{
    mathcap_update(oxf_mathcap(oxfp), ob);
}

/* example: which("xterm", getenv("PATH")); */
char *which(char *exe, const char *env)
{
    char *tok;
    char *path;
    char delim[] = ":";
    char *e = ALLOCA(strlen(env)+1);
    strcpy(e, env);
    tok = strtok(e, delim);
    while (tok != NULL) {
        path = MALLOC(strlen(tok)+strlen(exe)+2);
        sprintf(path, "%s/%s", tok, exe);
        if (access(path, X_OK&R_OK) == 0) {
            return path;
        }
        FREE(path);
        tok = strtok(NULL, delim);
    }
    return NULL;
}

#if !defined(_MSC_VER)
/* Remarks: ssh determines remote host by his name, i.e. by arg[0]. */
int oxc_start(char *remote_host, int port, char *passwd)
{
    char localhost[MAXHOSTNAMELEN];
    char ports[128];
    int pid = 0;
    char *cmd = "oxc";

    if (gethostname(localhost, MAXHOSTNAMELEN)==0) {
        if ((pid = fork()) == 0) {
            sprintf(ports, "%d", port);
            ox_printf("oxf.c:: oxc_start() does %s(ssh) -f %s -h %s -p %s -c %s\n", remote_host, cmd, localhost, ports, passwd);
            execlp("ssh", remote_host, "-f", cmd,
                  "-h", localhost, "-p", ports,"-c", passwd, NULL);
        }
    }
    return pid;
}

/* Remarks: ssh determines remote host by his name, i.e. by arg[0]. */
int oxc_start_with_pipe(char *remote_host, int port, char *passwd)
{
    char localhost[MAXHOSTNAMELEN];
    int  pid = 0;
    char *cmd = "oxc";
	int  pipefd[2];

    if (gethostname(localhost, MAXHOSTNAMELEN)==0) {
		if (pipe(pipefd) < 0) {
			return -1;
		}
        if ((pid = fork()) == 0) {
			dup2(pipefd[1], 0);
			close(pipefd[0]);
			close(pipefd[1]);
            execlp("ssh", remote_host, cmd, NULL);
			exit(1);
        }
		close(pipefd[1]);
		pipe_send_info(pipefd[0], localhost, port, passwd);
    }
    return pid;
}
#endif

static void pipe_send_string(int fd, char *s)
{
	int len  = strlen(s);
	int lenN = htonl(len);
	write(fd, &lenN, sizeof(int));
	write(fd, s, len+1);
}

static char *pipe_read_string()
{
	int len;
	char *s;
	read(0, &len, sizeof(int));
	len = ntohl(len)+1;
	s = MALLOC(len);
	read(0, s, len);
	return s;
}

/* The data format used by pipe_send_info() is defined in OX-RFC-101. */
void pipe_send_info(int fd, char *hostname, int port, char *password)
{
	port = htonl(port);
	write(fd, &port, sizeof(int));
	pipe_send_string(fd, hostname);
	pipe_send_string(fd, password);
}

int pipe_read_info(char **hostname, int *port, char **password)
{
	if (read(0, port, sizeof(int)) > 0) {
		*port = ntohl(*port);
		*hostname = pipe_read_string();
		*password = pipe_read_string();
		return 0;
	}
	return -1;
}

/*  Example: oxf_execute_cmd(oxfp, "ox_sm1"); */
OXFILE *oxf_execute_cmd(OXFILE *oxfp, char *cmd)
{
    int port = 0;
    int listened;

    if ((listened = oxf_listen(&port)) != -1) {
		cmo_list *args =  list_appendl(NULL, list_append(new_cmo_list(), (cmo *)new_cmo_int32(port)), new_cmo_string(cmd), NULL);
		send_ox_cmo(oxfp, (cmo *)args);
        send_ox_command(oxfp, SM_control_spawn_server);
        return oxf_connect_passive(listened);
    }
    return NULL;
}
