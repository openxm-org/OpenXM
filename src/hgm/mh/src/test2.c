#include <stdio.h>
#include "sfile.h"
/* $OpenXM: OpenXM/src/hgm/mh/src/test2.c,v 1.1 2013/02/21 07:30:56 takayama Exp $
  test of the --string mode 
  ./test2 Testdata/tmp-idata3.txt >tt.xt
  diff tt.txt Testdata/tmp-data3-out2.txt
*/

struct MH_RESULT *jk_main(int argc, char *argv[]);

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
  // s = (char *) malloc(10); free(s); printf("%x\n",s[0]); // Sample use of Sanitizer.
  mh_exit(MH_RESET_EXIT); /* standalone mode */
  if (argc != 2) { jk_main(argc,argv); return(0);}
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
  rp=jk_main(4,argv2);
  if (rp == NULL) {
	fprintf(stderr,"rp is NULL.\n"); exit(-1);
  }

  sfp = (rp->sfpp)[0];
  if (sfp) {
	printf("0, len=%d\n",sfp->len);
	for (i=0; i<sfp->len; i++) putchar((sfp->s)[i]);
	/* if (sfp->s) printf("%s",sfp->s); */
  }
  fprintf(stderr,"x=%lf, y[0]=%lf\n",rp->x,rp->y[0]);  
  fflush(NULL);

  /* deallocate the memory */
  for (i=0; i<rp->size; i++) mh_fclose((rp->sfpp)[i]);

  /* second try */
  printf("Second try.\n"); 
  jk_main(1,NULL);
  printf("Third try.\n"); 
  rp=jk_main(4,argv2);
  fprintf(stderr,"x=%lf, y[0]=%lf\n",rp->x,rp->y[0]);  
  fflush(NULL);
}
