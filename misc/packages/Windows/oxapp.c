/* $OpenXM: OpenXM/misc/packages/Windows/oxapp.c,v 1.2 2002/01/10 03:55:10 takayama Exp $ */
#include <stdio.h>
#define LINESIZE 4096

int ThereIsLoad = 0;
main(int argc,char *argv[]) {
  char s[LINESIZE];
  int i;
  int removeSharp = 0;
  int checkLoad = 0;
  extern ThereIsLoad;
  for (i=1; i<argc; i++) {
    if (strcmp(argv[i],"--removeSharp") == 0) {
      removeSharp = 1;
    }else if (strcmp(argv[i],"--load") == 0) {
      checkLoad = 1;
    }
  }
  while (fgets(s,LINESIZE,stdin) != NULL) {
    if (strlen(s) >= LINESIZE-1) {
      fprintf(stderr,"Too long line.\n");
      exit(20);
    }
    edit(s);
    if (removeSharp && s[0] == '#') {
      printf("\n");
    }else{
      printf("%s",s);
    }
  }
  if (removeSharp == 0) {
    printf("end$\n");
  }
  if (checkLoad) {
    fprintf(stderr,"checkLoad status = %d\n",ThereIsLoad);
    if (ThereIsLoad) exit(0);
    else exit(1);
  }
}

edit(char s[]) {
  int k,i,j;
  char t[LINESIZE*2];
  extern int ThereIsLoad;
  if (strlen(s) == 0) return;
  t[0] = 0;
  if ((k = find(s,"load(")) >= 0) {
    if (k > 0 && not_separator(s[k-1]) ) return; /* bload */
    /* Heuristic 1 */
    if (k > 0) {
      if (s[0] == 'i' && s[1] == 'f') return; /* if ... load */
    }
    /* Heuristic 2.A.    load(User_asirrc)$ */
    if (find(s,"load(User_asirrc)")>=0) {
      s[0] = '\n'; s[1]=0;
      return;
    }
    /* Heuristic 2 */
    if (('A' <= s[k+5]) && (s[k+5] <= 'Z')) {
      return; /* load(User_asirrc) */
    }  
    /* Heuristic 3. load("./"+Fname) in phc */
    if (find(s,"+Fname")>=0) return;

    for (i=0; i<k; i++) {
      t[i] = s[i]; t[i+1] = 0;
    }
    strcat(t,"#include ");
    ThereIsLoad = 1;
    j=k+5;
    for (i=strlen(t); i<LINESIZE*2-1; ) {
      if (s[j] == 0) break;
      if (s[j] == ')' || s[j] == ';' || s[j] == '$')  {
        j += 1;
      }else{
        t[i] = s[j]; t[i+1] = 0;
        i++; j++;
      }
    }
    if (strlen(t) >= LINESIZE-1) {
      fprintf(stderr,"Too long string %s\n",t);
    }
    strcpy(s,t);
  }
  if ((k = find(s,"end$")) >= 0) {
    if (k > 0 && not_separator(s[k-1])) return;
    strcpy(s,"\n");
  }
}

find(char *s,char *substr) {
  int n,m,i,j,k,k0;
  n = strlen(s);
  m = strlen(substr);
  k0 = -1; k = -1;
  for (i=0; i<n-m+1; i++) {
    k = i;
    for (j=0; j<m; j++) {
      if (s[i+j] != substr[j]) {
        k = -1; break;
      }
    }
    if (k >= 0 && k0 >= 0) {
      fprintf(stderr,"More than one appearances of %s\n",substr);
      exit(20);
    }
    if (k >= 0) k0 = k;
  }
  return k0;
}

not_separator(c) {
  if (c >='A' && c <='Z') return 1;
  if (c >='a' && c <='z') return 1;
  if (c >='0' && c <='9') return 1;
  if (c =='_') return 1;
  return 0;
}

