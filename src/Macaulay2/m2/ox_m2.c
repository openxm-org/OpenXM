/* $OpenXM$ */
/* original code from */
/* $Id$ */
#include <stdio.h>
#include <unistd.h>

#define OXDIR "/home/mike/M2stuff/takayama/rat/"
int main(int argc, char **argv)
{
  dup2(3,0);
  dup2(4,1);
  /*execl("/usr/local/bin/M2", "M2", "-n", "oxcommon.m2", 
       "oxserver.m2", "-e\"OXserver()\"", NULL);
  */
  execl("/usr/local/bin/M2", "M2", "-n", "-silent", OXDIR "oxcommon.m2", 
       OXDIR "oxserver.m2", NULL);
  return 0;
}
