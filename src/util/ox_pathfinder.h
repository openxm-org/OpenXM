/* $OpenXM: OpenXM/src/util/ox_pathfinder.h,v 1.2 2003/07/21 11:56:34 takayama Exp $ */
char **catArgv(char **argv1,char **argv2);
char *getOStypes();
char *getOpenXM_HOME();
char *getLOAD_SM1_PATH2();
char *getLOAD_K_PATH2(void);
char **getServerEnv(char *oxServer);
char **debugServerEnv(char *oxServer);
char *cygwinPathToWinPath(char *s);
int   oxForkExec(char **argv);
int ox_pathfinderNoX(int nox);
