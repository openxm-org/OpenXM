#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#define BUFSIZE 1024
/* BUG:  it is only for knoppix/math */

int main(int argc, char *argv[]) {
  char *oxhome;
  char browser[BUFSIZE];
  char fname[BUFSIZE];
  char comm[BUFSIZE];
  int i;
  int r;

  fname[0] = 0;
  oxhome = getenv("OpenXM_HOME");
  if (oxhome == NULL) oxhome="/usr/local/OpenXM";
  for (i=1; i<argc; i++) {
    strcpy(fname,argv[i]);
  }
  if (getenv("BROWSER") == NULL) {
    strcpy(browser,"firefox");
  }else{
    strcpy(browser,getenv("BROWSER"));
  }

  if (fname[0] == 0) {
    strcpy(fname,oxhome);
    strcat(fname,"/doc");
  }

  sprintf(comm,"%s %s",browser,fname);
  r=system(comm);

  return 0;

}
