/* $OpenXM: OpenXM/src/Macaulay2/m2/ox_m2.c,v 1.1 2000/10/09 12:04:01 takayama Exp $ */
/* original code from */
/* $Id$ */
#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>

#define OXDIR "/home/mike/M2stuff/takayama/rat/"
int main(int argc, char **argv)
{
  char *c,*s1, *s2, *e;
  e = getenv("OpenXM_HOME");
  c = (char *) malloc(strlen(e)+256);
  s1 = (char *) malloc(strlen(e)+256);
  s2 = (char *) malloc(strlen(e)+256);
  strcpy(c,e); strcat(c,"/bin/M2");
  strcpy(s1,e); strcat(s1,"/lib/m2/oxcommon.m2");
  strcpy(s2,e); strcat(s2,"/lib/m2/oxserver.m2");

  dup2(3,0);
  dup2(4,1);
  /*execl("/usr/local/bin/M2", "M2", "-n", "oxcommon.m2", 
       "oxserver.m2", "-e\"OXserver()\"", NULL);
  */
  /*
  execl("/usr/local/bin/M2", "M2", "-n", "-silent", OXDIR "oxcommon.m2", 
       OXDIR "oxserver.m2", NULL);
	   */
  execl(c, "M2", "-n", "-silent", s1, s2, NULL);
  return 0;
}
