/* -*- mode: C -*- */
/* $OpenXM$ */

#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>

#include <stdio.h>
#include <stdlib.h>
#include <mathlink.h>
#include <unistd.h>
#include <signal.h>

#define FNAME "/tmp/inttest00"

static int signum = 0;

static void handler()
{
    print("handler(): signal received.");
}

static int block0(int sig_no)
{
    sigset_t mask;
    sigemptyset(&mask);
    sigaddset(&mask, sig_no);
    sigprocmask(SIG_SETMASK, &mask, NULL);
}

static int block(int sig_no)
{
    int i;
    sigset_t mask;
    sigemptyset(&mask);
    for(i=1;i<sig_no;i++) {
        sigaddset(&mask, i);
    }
    sigprocmask(SIG_SETMASK, &mask, NULL);
}

int print(char *s)
{
    FILE *fp = fopen(FNAME, "w+");
    fprintf(fp, "%s\n", s);
    fclose(fp);
    return 0;
}

int INT_addto(int x, int y)
{
    while (!MLAbort) {
        sleep(3);
    }
    print("INT_addto(): MLAbort is set.");
    return x+y;
}

int INT_addtoo(int x, int y)
{
    return x+y;
}

int INT_signal(int n)
{
    signal(n, handler);
    signum=n;
    return n;
}

int INT_block(int n)
{
    block(n);
    return n;
}

int main(int argc, char *argv[])
{
    ox_stderr_init(NULL);
    MLMain(argc, argv);
}
