/* $OpenXM: OpenXM/src/util/ox_pathfinder.h,v 1.4 2003/07/21 13:36:43 takayama Exp $ */
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
char *getCppPath(void);

