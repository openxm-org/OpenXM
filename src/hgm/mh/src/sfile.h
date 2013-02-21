/*
   $OpenXM: OpenXM/src/hgm/mh/src/sfile.h,v 1.4 2013/02/20 05:56:16 takayama Exp $
 */
struct SFILE {
  int byFile;
  char *s;
  int pt;
  int len;
  int limit;
  FILE *fp;
  int forRead;
  int copied;
};

struct MH_RESULT {
  double x;
  double *y;
  int rank;
  struct SFILE **sfpp;  /* sfpp[0], ..., spff[size-1] */
  int size;
};
 
struct SFILE *mh_fopen(char *name, char *mode, int byFile);
char *mh_fgets(char *str,int size,struct SFILE *sfp);
int mh_fputs(char *s,struct SFILE *sfp);
int mh_fclose(struct SFILE *sfp);
int mh_outstr(char *str,int size,struct SFILE *sfp);
void *mh_malloc(int s);
int mh_free(void *p);
int mh_exit(int n);

#define MH_SSIZE 1024
#define MH_RESET_EXIT 0x7fffffff

/* prototypes in wmain.c */
struct MH_RESULT *mh_main(int argc,char *argv[]);
/* prototypes in rk.c*/
struct MH_RESULT mh_rkmain(double x0,double y0[],double xn);
