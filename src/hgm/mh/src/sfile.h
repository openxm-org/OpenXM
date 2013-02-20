/*
   $OpenXM: OpenXM/src/hgm/mh/src/sfile.h,v 1.1 2013/02/19 08:03:14 takayama Exp $
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
 
struct SFILE *mh_fopen(char *name, char *mode, int byFile);
char *mh_fgets(char *str,int size,struct SFILE *sfp);
int mh_fputs(char *s,struct SFILE *sfp);
int mh_fclose(struct SFILE *sfp);
int mh_outstr(char *str,int size,struct SFILE *sfp);
