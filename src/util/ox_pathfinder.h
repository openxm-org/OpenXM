/* $OpenXM: OpenXM/src/util/ox_pathfinder.h,v 1.11 2004/02/28 12:27:15 takayama Exp $ */
char **catArgv(char **argv1,char **argv2);
char *getOStypes();
char *getOpenXM_HOME();
char *getLOAD_SM1_PATH2();
char *getLOAD_K_PATH2(void);
char **getServerEnv(char *oxServer);
char **debugServerEnv(char *oxServer);
char *cygwinPathToWinPath(char *s);
char *winPathToCygwinPath(char *s);
int   oxForkExec(char **argv);
int   oxForkExecBlocked(char **argv);
int ox_pathfinderNoX(int nox);
int ox_pathfinderVerbose(int flag);
char *generateTMPfileName(char *seed);
char *generateTMPfileName2(char *seed,char *ext,int tmp,int win);
char *getCppPath(void);
char *getCommandPath(char *cmdname);
char *oxWhich(char *cmdname,char *path);
char *oxWhich_unix(char *cmdname,char *path);
char *oxWhich_win(char *cmdname,char *path);
int oxGetPidList(int *pids,int limit);
char *oxEvalEnvVar(char *s);
int oxKillAll(void);
int oxKillPid(int pid);
int oxDeleteFile(char *s);
void oxResetRedirect(void);
void ox_pathfinder_quiet(void);
char *oxTermWhich_unix(int *typep);
#define T_OXTERM 2
#define T_RXVT   1
#define T_XTERM  0

