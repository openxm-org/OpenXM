/*$OpenXM: OpenXM/src/kxx/oxlog.c, v 1.7 2000/02/09 12:24:52 takayama Exp $*/
#include <stdio.h>
#include <signal.h>
#include <string.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
/* In some distributions of linux you need to include the following file
#include <linux/sched.h>
*/
#include <unistd.h> 

/* oxlog /usr/X11R6/bin/xterm -icon -e /home/nobuki/kxx/ox
    0     1                    2     3  4
         -ox /home/nobuki/kxx/ox_asir
          5   6
*/
/* oxlog /home/nobuki/kxx/ox
    0     1     
         -ox /home/nobuki/kxx/ox_asir  >& /dev/null
          2     3
*/
static int Debug_which = 1;

/*which("xterm", getenv("PATH"));*/
char *which(char *prog, char *path_env)
{
    extern int Debug_which ;
    char *tok;
    char *path;
    char delim[] = ":";
    char *e = alloca(strlen(path_env)+1);
    strcpy(e, path_env);
    tok = strtok(e, delim);
    if (Debug_which) {
        fprintf(stderr, "PATH=%s\n", path_env);
    }
    while (tok != NULL) {
        char *path = malloc(strlen(tok)+strlen(prog)+2);
        sprintf(path, "%s/%s", tok, prog);
        if (access(path, X_OK&R_OK) == 0) {
            if (Debug_which) {
                fprintf(stderr, "Found: %s\n", path);
            }
            return path;
        }
        free(path);
        tok = strtok(NULL, delim);
    }
    return NULL;
}

char *toFullPath(char *s)
{
    extern int Debug_which;
    char *path = "/bin:/usr/bin:/usr/local/bin:/usr/X11R6/bin:/usr/openwin/bin:/usr/X/bin";
    char *s2;
    if (strlen(s) == 0 || strchr(s, '/') != NULL) {
        return s;
    }
    s2 = which(s, getenv("PATH"));
    if (s2 == NULL) {
        if (Debug_which) {
            fprintf(stderr, "Could not find arg[1] in your search path.\n");
            fprintf(stderr, "You path is %s.\n", getenv("PATH"));
            fprintf(stderr, "Trying to find in the path: %s\n", path);
        }
        s2 = which(s, path);
        /* Why? */
    }
    return s2;
}

int main(int argc, char *argv[])
{
    sigset_t sss;
    int i;
    char *oxname;
    /* char *env[2];
       env[0] = NULL;*/
    sigemptyset(&sss);
    sigaddset(&sss, SIGINT);
    sigprocmask(SIG_BLOCK, &sss, NULL);
    if (argc >= 2) {
        oxname = toFullPath(argv[1]);
        if (oxname == NULL) {
            oxname = "NULL";
        } /* Why? */
        execv(oxname, argv+1);

        fprintf(stderr, "\nError in oxlog: Failed to start the process.\n");
        fprintf(stderr, "oxname=%s\n", oxname);
        for (i=0; i<argc; i++) {
            fprintf(stderr, "argv[%d]=%s ", i, argv[i]);
        }
        fprintf(stderr, "\n");
        exit(20);
    }else {
        fprintf(stderr, "Error in oxlog: cannot handle argc=%d\n", argc);
        fprintf(stderr, "oxname=???\n");
        for (i=0; i<argc; i++) {
            fprintf(stderr, "argv[%d]=%s ", i, argv[i]);
        }
        fprintf(stderr, "\n");
        fprintf(stderr, "Usage example 1: oxlog /usr/X11R6/bin/xterm -icon -e /home/nobuki/kxx/ox -ox /home/nobuki/kxx/ox_asir\n");
        fprintf(stderr, "      example 2: oxlog /home/nobuki/kxx/ox -ox /home/nobuki/kxx/ox_asir >& /dev/null\n");
        exit(10);
    }
}
