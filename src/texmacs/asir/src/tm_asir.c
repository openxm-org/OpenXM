/* -*- mode: C -*- */
/* $OpenXM: OpenXM/src/texmacs/asir/src/tm_asir.c,v 1.1 2004/02/25 14:45:28 ohara Exp $ */

/*
usage: tm_asir
       tm_asir -d [logfile]
*/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/types.h>
#include <sys/socket.h>

#if defined(__sun__)
#include <arpa/inet.h>
#endif

#define DATA_BEGIN   2
#define DATA_END     5
#define DATA_ESCAPE  27

#define OX_COMMAND 513
#define OX_DATA    514
#define CMO_INT32  2
#define CMO_STRING 4
#define SM_popString                  263
#define SM_executeStringByLocalParser 268
#define SM_executeFunction            269

#define SIZE_BUFFER 2048
#define DEFAULT_LOG_FILE "/tmp/tm_asir.log"

#define WRITE(ptr,size)  fwrite((ptr),1,(size),ox_fp)
#define READ(ptr,size)   fread((ptr),1,(size),ox_fp)
#define FLUSH()          fflush(ox_fp)
#define PIPE(fd)         socketpair(AF_UNIX,SOCK_STREAM,0,(fd))

int dat_string[3]      = {OX_DATA,    0, CMO_STRING};
int dat_int32[3]       = {OX_DATA,    0, CMO_INT32};
int cmd_exeFunction[3] = {OX_COMMAND, 0, SM_executeFunction};
int cmd_exeString[3]   = {OX_COMMAND, 0, SM_executeStringByLocalParser};
int cmd_popString[3]   = {OX_COMMAND, 0, SM_popString};

FILE *ox_fp;
FILE *logfp;
static int counter;

char *chomp(char *s)
{
    char *t = s+strlen(s)-1;
    *t = (*t != '\n')? *t: 0;
    return s;
}

void put_log(char *format, char *mesg)
{
    fprintf(logfp, "[%d]", counter++);
    fprintf(logfp, format);
    fprintf(logfp, ":%s\n", mesg);
    fflush(logfp);
}

void open_log(char *logfile)
{
    logfp = fopen(logfile, "a");
}

void load_asir()
{
    int pid;
    int fd[2];
    int offer;
    if (PIPE(fd) == 0) {
        if ((pid = fork()) > 0) {
            close(fd[0]);
            ox_fp = fdopen(fd[1], "w+");
            /* deteriming byte order */
            READ(&offer, 1);
            offer=1;
            WRITE(&offer, 1); FLUSH();
            return;
        }else if (pid == 0) {
            close(fd[1]);
            dup2(fd[0], 3);
            dup2(3, 4);
            dup2(open("/dev/null", O_APPEND|O_RDWR, 0644), 2); /* stderr */
            execlp("ox_asir", "ox_asir", "-quiet", NULL);
        }
    }
    perror("load_asir() :");
    exit(1);
}

void send_int(int n)
{
    WRITE(dat_int32, sizeof(int)*3);
    WRITE(&n, sizeof(int));
}

void send_string(char *s)
{
    int len = strlen(s);
    WRITE(dat_string, sizeof(int)*3);
    WRITE(&len, sizeof(int));
    WRITE(s, len);
}

void send_cmd(int *cmd)
{
    WRITE(cmd, sizeof(int)*3);
}

char *recv_string()
{
    static char *s;
    int len;
    int null[3];
    READ(null, sizeof(int)*3);
    READ(&len, sizeof(int));
    if ((s = realloc(s, len+1)) != NULL) {
        s[len] = 0;
        READ(s, len);
        return s;
    }
    perror("recv_string(): ");
    exit(1);
}

char *asir(char *cmd)
{
    send_string(cmd);
    send_cmd(cmd_exeString);
    send_int(1);
    send_string("print_tex_form");
    send_cmd(cmd_exeFunction);
    send_cmd(cmd_popString);
    return recv_string();
}

char *asir_copyright()
{
    send_string("copyright();");
    send_cmd(cmd_exeString);
    send_cmd(cmd_popString);
    return recv_string();
}

void output_escape(char *s)
{
    while (*s != '\0') {
        if (*s == DATA_BEGIN || *s == DATA_END || *s == DATA_ESCAPE) {
            fputc(DATA_ESCAPE, stdout);
        }
        fputc(*s, stdout);
        s++;
    }
}

void output_latex(char *s)
{
    fputs("\2latex:$", stdout);
    output_escape(s);
    fputs("$\5", stdout);
    fflush(stdout);
}

void output_verbatim(char *s)
{
    fputs("\2verbatim:", stdout);
    output_escape(s);
    fputs("\5", stdout);
    fflush(stdout);
}

int main(int argc, char *argv[])
{
    char buffer[SIZE_BUFFER];
    int debug = 0;
    char *logfile = DEFAULT_LOG_FILE;
    if (argc > 1 && strcmp(argv[1], "-d") == 0) {
        debug = 1;
        if (argc > 2) {
            logfile = argv[2];
        }
    }
    if (!debug) {
        load_asir();
        output_verbatim(asir_copyright());
    }else {
        open_log(logfile);
        sprintf(buffer, "This is Plugin Debugger with logging to %s.", logfile);
        output_verbatim(buffer);
		put_log("verbatim", buffer);
    }
    while(fgets(buffer, SIZE_BUFFER, stdin) != NULL) {
        if (!debug) {
            output_latex(asir(buffer));
        }else {
            output_verbatim(chomp(buffer));
			put_log("verbatim", buffer);
        }
    }
    return 0;
}
