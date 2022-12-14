/* $OpenXM: OpenXM/src/polymake/fake/fake-polymake.c,v 1.2 2004/07/29 02:50:52 takayama Exp $ */
/* This is a fake polymake which works as follows
   (1) By using oxhttpd (OpenXM http service), it calls the real polymake
       and gets an answer.
     [Not implemented]
or
   (2) It only returns stored answers of polymake, i.e., it does not make
       any computation and returns fixed answer no matter the input is.
*/

/*  Note for takayama.
    cf. misc-2003/11/polymake, misc-2004/07/polymake
        orange2:this04/polymake
 */

#include <stdio.h>

main(int argc,char *argv[]) {
  FILE *fp;
  FILE *fp2;
  FILE *rfp;
  int c,i;
  char cube[1024*2];
  char *home;
  if (argc < 3) {
    fprintf(stderr,"This is a fake polymake\n");
    fprintf(stderr,"Usage: polymake filename action\n");
    fprintf(stderr,"Remove files you do not use in tmp/ \n");
    exit(3);
  }
  fp = fopen(argv[1],"w");
  fprintf(fp,"_This is an output of a fake polymake.\n");
  home = (char *)getenv("OpenXM_HOME");
  for (i=0; i<1; i++) {
    if (strcmp(argv[2],"FACET")==0 || strcmp(argv[2],"FACETS")==0) {
      sprintf(cube,"%s/src/polymake/fake/tmp/facet.poly",home);
      if ((rfp = fopen(cube,"r")) != NULL) {
        fclose(rfp); break;
      }
      sprintf(cube,"%s/src/polymake/fake/tmp/ineq2.poly",home);
      if ((rfp = fopen(cube,"r")) != NULL) {
        fclose(rfp);
        printf("FACETS\n");
        printf("0 0 1 1\n");
        printf("0 0 1 1\n");
        printf("0 0 1 1\n");
        printf("0 1 1 0\n");
        printf("0 1 -1 0\n");
        printf("0 0 0 1\n");
		break;
      }
    }else if (strcmp(argv[2],"FACE_LATTICE")==0) {
      sprintf(cube,"%s/src/polymake/fake/tmp/facelat.poly",home);
      if ((rfp = fopen(cube,"r")) != NULL) {
        fclose(rfp); 
        printf("FACE_LATTICE\n\n");
        printf("[ -1 : 3 ]\n");
        printf("{{0 1} {0 2} {1 2}}\n\n");
        printf("[ -2 : 3 ]\n");
        printf("{{0} {1} {2}}\n");
      }
    }else{
      fprintf(stderr,"fake-polymake: ERROR: Unknown method, property, or label\n");
      fprintf(stderr,"facet1.poly  FACET\n");
      fprintf(stderr,"facelat1.poly  FACE_LATTICE\n");
      fclose(fp);
      exit(1);
    }
  }
  fp2 = fopen(cube,"r");
  if (fp2 == NULL) {
    fprintf(stderr,"fake-polymake: ERROR: no data file is found.\n");
    exit(1);
  }
  while ((c=fgetc(fp2)) != EOF) {
     fputc(c,fp);
  }
  fclose(fp);

}


