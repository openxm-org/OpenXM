/*
   $OpenXM: OpenXM/src/hgm/mh/src/sfile.h,v 1.16 2016/05/30 00:38:18 takayama Exp $
 */
#include "oxprint.h"
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
  char *message;
  int t_success;
  double series_error;
  double recommended_abserr;
};

struct mh_token {
  int type;
  double dval;
  int ival;
};
 
struct SFILE *mh_fopen(char *name, char *mode, int byFile);
char *mh_fgets(char *str,int size,struct SFILE *sfp);
int mh_fputs(char *s,struct SFILE *sfp);
int mh_fclose(struct SFILE *sfp);
int mh_outstr(char *str,int size,struct SFILE *sfp);
void *mh_malloc(int s);
int mh_free(void *p);
int mh_exit(int n);
void mh_check_intr(int interval);
void mh_error(char *s,int code);
struct mh_token mh_getoken(char s[],int smax,struct SFILE *sfp);
void mh_print_token(struct mh_token tk,char *s);
#define MH_TOKEN_EOF -1
#define MH_TOKEN_ID  1
#define MH_TOKEN_EQ  2
#define MH_TOKEN_DOUBLE 3
#define MH_TOKEN_INT 4


#define MH_SSIZE 1024
#define MH_RESET_EXIT 0x7fffffff

#define myabs(x) ((x)<0?(-(x)):(x))
#define mymax(x,y) ((x)>(y)?(x):(y))
#define mymin(x,y) ((x)<(y)?(x):(y))

/* prototypes in wmain.c */
struct MH_RESULT *mh_main(int argc,char *argv[]);
/* prototypes in rk.c*/
struct MH_RESULT mh_rkmain(double x0,double y0[],double xn);
/* prototype in jack-n.c */
struct MH_RESULT *jk_main(int argc,char *argv[]);

/* Significant digit control used both in wmain.c and jack-n.c */
#define M_ASSIGNED_SERIES_ERROR_DEFAULT 1e-5
#define MH_RELERR_DEFAULT (1e-10)

/* The next value is heuristic */
#define SERIES_ADMISSIBLE_RADIUS_TYPE1 50.0
/* SERIES RADIUS close to 1 is not good. */
#define SERIES_ADMISSIBLE_RADIUS_TYPE2 0.77
int reset_SAR_warning(int n);
