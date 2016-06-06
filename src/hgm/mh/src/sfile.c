/*
  $OpenXM: OpenXM/src/hgm/mh/src/sfile.c,v 1.21 2016/02/13 02:19:00 takayama Exp $
*/
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include "sfile.h"
#define SSIZE 5
int MH_DEBUG = 0;

static int isIntString(char s[]);

void mh_check_intr(int interval) {
  static int intr=0;
  if (intr >= interval) {
    intr = 0;
#ifndef STANDALONE
    R_CheckUserInterrupt();
#endif
  }else intr++;
}

void *mh_malloc(int s) {
  void *p;
  static int total=0;
  total += s;
  if (MH_DEBUG) oxprintf("mh_malloc total allocated memory: %f M\n",(float)total/(float)(1024*1024));
#ifdef STANDALONE
  p = (void*)malloc(s);
#else
  p = (void *)R_alloc(1,s);
#endif
  if (p == NULL) {
    oxprintfe("No memory.\n"); mh_exit(-1);
  }
  return(p);
}
int mh_free(void *p) {
  if (MH_DEBUG) oxprintf("mh_free at %p\n",p);
#ifdef STANDALONE
  free(p); /* free in mh_free */
#endif
  return(0);
}

int mh_exit(int n) {
#ifdef STANDALONE
  static int standalone=0;
  if (n == MH_RESET_EXIT) { standalone=1; return(0);}
  if (standalone) exit(n);
  else {
    oxprintfe("Fatal error mh_exit(%d) in mh-w-n.\n",n);
    return(n);
  }
#else
  error("Error in hgm. Exit from hgm by mh_exit(%d).\n",n);
#endif
}

struct SFILE *mh_fopen(char *name,char *mode,int byFile) {
  struct SFILE *sfp;
  sfp = (struct SFILE *)mh_malloc(sizeof(struct SFILE));
  sfp->byFile=0; sfp->s=NULL; sfp->pt=0; sfp->len=0;sfp->limit=0; sfp->fp=NULL;
  sfp->forRead=1; sfp->copied=0;
  
  if (byFile) {
    sfp->byFile = 1;
    if (strcmp(name,"stdout")==0) {
      sfp->fp = oxstdout;
	}else if (strcmp(name,"stdin")==0) {
	  sfp->fp = oxstdin;
    }else{
      sfp->fp = fopen(name,mode);
    }
    if (sfp->fp == NULL) return(NULL);
    else return(sfp);
  }else if (strcmp(mode,"r")==0) {
    sfp->byFile=0;
    sfp->s=name;
    sfp->pt=0;
    sfp->len=strlen(name);
    sfp->limit=sfp->len+1;
    sfp->forRead=1;
    return(sfp);
  }else if (strcmp(mode,"w")==0) {
    sfp->byFile=0;
    sfp->s=(char *)mh_malloc(SSIZE);
    sfp->s[0]=0;
    sfp->pt=0;
    sfp->len=0;
    sfp->limit= SSIZE;
    sfp->forRead=0;
    return(sfp);
  }else{
    oxprintfe("Error: unknown mode %s in the string mode.\n",mode);
    return NULL;
  }
}

char *mh_fgets(char *str,int size,struct SFILE *sfp) {
  int i,pt,len;
  char *s;
  if (sfp->byFile) return fgets(str,size,sfp->fp);
  s = sfp->s; len = sfp->len; pt = sfp->pt;
  if (s[pt] == 0) return NULL;
  if (pt >= len) return NULL;
  if (size != 0) str[0]=0;
  for (i=0; i<size-1; i++) {
    if (s[pt] != 0) {
      str[i] = s[pt]; str[i+1] = 0;
      pt++; sfp->pt = pt;
      if (s[pt] == 0) return str;
      if (pt >= len) return str;
      if (str[i] == '\n') return str;
    }
  }
  return str;
}
int mh_fputs(char *str,struct SFILE *sfp) {
  int len,limit;
  int inputLen;
  char *s;
  if (sfp->byFile) return fputs(str,sfp->fp);
  s = sfp->s; len = sfp->len;  limit=sfp->limit;
  inputLen=strlen(str);
  if (inputLen+len+1 > limit) {
    limit = (inputLen+len+1)*2;
    s = (char *) mh_malloc(limit);
    if (s == NULL) return(EOF);
    strcpy(s,sfp->s);
    mh_free(sfp->s);
  }
  strcpy(&(s[len]),str);
  len += inputLen;
  /* oxprintf("mh_fputs(%d):[%s]\n",len,s); */
  sfp->s=s; sfp->len=len; sfp->limit=limit;
  return(0);
}

/* Note: copy the resulting string sfp->s before mh_fclose */
int mh_fclose(struct SFILE *sfp) {
  if (!sfp) return(-1);
  if (sfp->byFile) return fclose(sfp->fp);
  if (! (sfp->forRead)) {
    if (!sfp->copied) oxprintfe("Warning in mh_fclose. sfp->s has not been copied, but deallocated.\n"); 
    if (sfp->s != NULL) { mh_free(sfp->s); sfp->s = NULL; }
  }
  mh_free(sfp);
  return(0);
}

int mh_outstr(char *str,int size,struct SFILE *sfp) {
  int i;
  if (sfp->byFile) {
    oxprintfe("Error in mh_outstr. mh_outstr is called in the file i/o mode.\n");
    return 0;
  }
  if (size) str[0] = 0;
  for (i = 0; (i<size-1) && (i<sfp->len); i++) {
    str[i] = (sfp->s)[i]; str[i+1] = 0;
  }
  sfp->copied=1;
  return(i);
}

void mh_error(char *s,int code) {
  oxprintfe("Error: %s\n",s);
  mh_exit(code);
}

struct mh_token mh_getoken(char s[],int smax,struct SFILE *sfp) {
  static int w0=0;
  static int wn=0;
  static char work[MH_SSIZE];
  char *r;
  int t,t2;
  struct mh_token token;
  token.type = MH_TOKEN_EOF;
  token.ival=0; token.dval=0.0;
  while (1) {
    if (w0 >= wn) {
      r = mh_fgets(work,MH_SSIZE-1,sfp);
      if (r == NULL) {
        token.type = MH_TOKEN_EOF;
        return token;
      }
      /* oxprintf("work=%s\n",work); */
      w0 = 0; wn=strlen(work);
    }
    t=w0;
    while ((work[t] <= ' ') && (t < wn)) t++;
    if (t == wn) {
      w0=wn;
      continue;
    }
    if (work[t] == ',') {w0=t+1; continue; }
    t2 = t;
    while((work[t2] > ' ') && (work[t2] != '=')
          && (work[t2] != ',')) t2++;
    /* work[t] ... work[t2-1] is a token */
    /* %abc=123#comment is not allowed. but %abc=123,#  or %abc=123 # is OK. */
    if ((t == t2) && (work[t] == '=')) {
      token.type=MH_TOKEN_EQ;
      w0=t2+1;
      return token;
    }
    if ((work[t] == '%') && (work[t+1] == '%')) {
      /* comment */
      w0 =wn;
      continue;
    }
    if (work[t] == '#') {
      /* comment */
      w0=wn;
      continue;
    }
    if (work[t] == '%') {
      if (t2-t-2 >= smax) {
        oxprintfe("s is too small in mh_getoken.\n");
        mh_exit(-1);
      }
      strncpy(s,&(work[t+1]),t2-(t+1)); s[t2-(t+1)] = 0;
      token.type=MH_TOKEN_ID;
      w0 = t2;
      return token;
    }
    /* The case of double or int */
    strncpy(s,&(work[t]),t2-t); s[t2-t]=0;
    if (isIntString(s)) {
      token.type=MH_TOKEN_INT;
      sscanf(s,"%d",&(token.ival));
      sscanf(s,"%lg",&(token.dval));
    }else{
      token.type=MH_TOKEN_DOUBLE;
      sscanf(s,"%lg",&(token.dval));
    }
    w0 = t2;
    return token;
  }
}

static int isIntString(char s[]) {
  int i;
  for (i=0; i<strlen(s); i++) {
    if (isdigit(s[i]) || (s[i]=='-')) continue;
    else return(0);
  }
  return(1);
}

void mh_print_token(struct mh_token tk,char *s) {
  int type;
  type = tk.type;
  oxprintf("type=%d\n",type);
  switch(type) {
  case MH_TOKEN_ID:
    oxprintf("ID=%s\n",s); break;
  case MH_TOKEN_EQ:
    oxprintf("MH_TOKEN_EQ\n"); break;
  default:
    oxprintf("NUM=%s, ival=%d, dval=%lg\n",s,tk.ival,tk.dval); break;
  }
}

#ifdef TEST
/* for debugging */
dump(struct SFILE *sfp) {
  oxprintf("byFile=%d\n",sfp->byFile);
  if (sfp->s) oxprintf("sfp->s=%s\n",sfp->s);
  oxprintf("pt=%d\n",sfp->pt);
  oxprintf("len=%d\n",sfp->len);
  oxprintf("limit=%d\n",sfp->limit);
  oxprintf("fp=%p\n",sfp->fp);
}


#define TESTSIZE 1024
main() {
  struct SFILE *sfp;
  char str[TESTSIZE];
  int i;
  struct mh_token tk;
  sfp = mh_fopen("%%abc\n%abs=1.234\n  %abs = \n 1.235\n%abs=\n1.236\n\n%abs=1.237\n%ival=-234,#comment\n","r",0);
  while ((tk=mh_getoken(str,TESTSIZE,sfp)).type != MH_TOKEN_EOF) {
    mh_print_token(tk,str);
  }
  return 0;
  /*
    sfp = mh_fopen("hoge\nafo\nbho\ncat\ndot\ndolphin\n","r",0);
    while (mh_fgets(str,1024,sfp)) {
    oxprintf(str);
    }
    mh_fclose(sfp);

    sfp = mh_fopen("sfile.h","r",1);
    while (mh_fgets(str,1024,sfp)) {
    oxprintf(str);
    }
    mh_fclose(sfp);
  */
  sfp = mh_fopen("","w",0);
  for (i=0; i<3; i++) {
    mh_fputs("hoge\n",sfp);
    mh_fputs("hage\n",sfp);
    dump(sfp);
  }
  mh_fputs("end end\n",sfp);
  oxprintf("result=%s\n",sfp->s);
  mh_outstr(str,TESTSIZE,sfp);
  mh_fclose(sfp);
}
#endif
