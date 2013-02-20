#include <stdio.h>
#include "sfile.h"
/* $OpenXM$ 
  test of the --string mode 
  ./test1 Testdata/tmp-idata2.txt >tt.xt
  diff tt.txt Testdata/tmp-data2-out.txt
*/

char *readAsString(FILE *fp) {
  static char *s = NULL;
  static int size= 102400;
  int p = 0;
  char *s2;
  int c;
  if (fp == NULL) { s=NULL; return(NULL); }
  if (s == NULL) {
    s = (char *) malloc(size);
    if (s == NULL) {
      fprintf(stderr,"no more memory.\n"); exit(10);
    }
  }
  s[0] = 0;
  while ((c=fgetc(fp)) != EOF) {
    s[p++] = c; s[p] = 0;
    if (p >= size-2) {
      s2 = s;
      size = 2*size;
      s = (char *) malloc(size);
      if (s == NULL) {
        fprintf(stderr,"no more memory.\n"); exit(10);
      }
      strcpy(s,s2);
      free(s2);     
    }
  }
  return s;
}

main(int argc,char *argv[]) {
  int i;
  char *s;
  FILE *fp;
  struct MH_RESULT *rp;
  struct SFILE *sfp;
  char *argv2[10];
  mh_exit(0x7fffffff); /* standalone mode */
  if (argc != 2) { mh_main(argc,argv); return(0);}
  fp = fopen(argv[1],"r");
  if (fp == NULL) {
	fprintf(stderr,"File %s is not found.\n",argv[1]); return(-1);
  }
  s = readAsString(fp);
  fclose(fp);
  argv2[0] = "dummy";
  argv2[1] = "--bystring";
  argv2[2] = "--idata";
  argv2[3] = s;
  argv2[4] = "--dataf";
  argv2[5] = "aa";
  rp=mh_main(6,argv2);

  sfp = (rp->sfpp)[0];
  if (sfp) {
	printf("0, len=%d\n",sfp->len);
	if (sfp->s) printf("%s",sfp->s);
  }

  sfp = (rp->sfpp)[1];
  if (sfp) {
	printf("1, len=%d\n",sfp->len);
	if (sfp->s) printf("1:%s",sfp->s);
  }
  fflush(NULL);

  /* deallocate the memory */
  for (i=0; i<rp->size; i++) mh_fclose((rp->sfpp)[i]);
  mh_freeWorkArea();

  /* second try */
  rp=mh_main(6,argv2);
}
