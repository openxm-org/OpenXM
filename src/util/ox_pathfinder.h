/* $OpenXM: OpenXM/src/util/ox_pathfinder.h,v 1.1 2003/07/21 11:36:10 takayama Exp $ */
char **catArgv(char **argv1,char **argv2);
char *getOStypes();
char *getOpenXM_HOME();
char *getLOAD_SM1_PATH2();
char **getServerEnv(char *oxServer);
char **debugServerEnv(char *oxServer);
char *cygwinPathToWinPath(char *s);
int   oxForkExec(char **argv);
int ox_pathfinderNoX(int nox);
