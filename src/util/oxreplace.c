/* $OpenXM: OpenXM/src/util/oxreplace.c,v 1.3 2003/05/31 03:05:00 takayama Exp $ */
/* cf. fb/src/misc/nan-tfb2.c */
#include <stdio.h>
#include <time.h>
int ReplaceLine=0;

char *readAsString(FILE *fp) {
  static char *s = NULL;
  static int size= 102400;
  int p = 0;
  char *s2;
  int c;
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

matches(char *word,char *s,int j) {
  int i;
  for (i=0; i<j; i++) {
    if (word[i] != s[i]) return 0;
  }
  return 1;
}

isHexc(int c) {
  if ((c >= '0') && (c<='9')) return 1;
  if ((c >= 'A') && (c<='F')) return 1;
  if ((c >= 'a') && (c<='f')) return 1;
  return 0;
}
char *hex2str(char hstr[]) {
  int i,n,c,k;
  char *s;
  char ts[3];
  /* printf("%s\n",hstr); */
  n = strlen(hstr); 
  s = (char *) malloc(n/2+2);
  if (s == NULL) {fprintf(stderr,"Memory exhausted.\n"); exit(10);}
  k = 0; i = 0;
  s[k] = 0;
  while (i<n) {
	while ((!isHexc(hstr[i])) && (i<n)) i++;
    if (isHexc(hstr[i]) && (i<n)) ts[0] = hstr[i]; else ts[0] = 0; 
    i++;
	while ((!isHexc(hstr[i])) && (i<n)) i++;
    if (isHexc(hstr[i]) && (i<n)) ts[1] = hstr[i]; else ts[1] = 0; 
    i++; ts[2] = 0;
    if (ts[0] != 0) {
	  sscanf(ts,"%xd",&c);
      s[k] = c; k++; s[k] = 0;
	}
  }
  if (strcmp(s,"") == 0) return NULL;
  else return s;
}

main(int argc, char *argv[]) {
  int i;
  char *old = NULL;
  char *new = NULL;
  if (argc < 2) {usage(); exit(0); }
  for (i=1; i<argc; i++) {
	if (strcmp(argv[i],"--old") == 0) {
	  old = argv[i+1]; i++;
	} else if (strcmp(argv[i],"--new") == 0) {
	  new = argv[i+1]; i++;
	} else if (strcmp(argv[i],"--oldx") == 0) {
	  old = hex2str(argv[i+1]); i++;
	} else if (strcmp(argv[i],"--newx") == 0) {
	  new = hex2str(argv[i+1]); i++;
    } else if (strcmp(argv[i],"--replaceLine") == 0) {
      ReplaceLine = 1;
    } else if (strcmp(argv[i],"--f") == 0) {
	  fprintf(stderr,"--f option (rule file) has not yet been implemented.\n");
	  exit(10);
	}else {
	  if ((old != NULL) && (new != NULL)) {
		replaceOneWord(argv[i],old,new);
	  }else{
		fprintf(stderr,"--old and --new option or --f option are not given.\n");
		usage();
		exit(10);
	  }
	}
  }
  exit(0);
}
	
#define DEBUG 1

replaceOneWord(char *fname,char *old, char *new) {
  FILE *fp;
  FILE *fpOrig;
  char *s;
  char *fnameBackup;
  char *comm;
  int i,j,mm;
#ifdef DEBUG
  fprintf(stderr,"fname=%s, old=%s, new=%s, ReplaceLine=%d\n",fname,old,new,ReplaceLine);
#endif
  fp = fopen(fname,"r");
  if (fp == NULL) {
	fprintf(stderr,"File %s is not found.\n",fname);
	return -1;
  }
  fclose(fp);

  fnameBackup = (char *)malloc(strlen(fname)+200);
  comm = (char *)malloc(strlen(fname)+300);
  if (fnameBackup == NULL) {
	fprintf(stderr,"No more memory.\n"); exit(10);
  }
  sprintf(fnameBackup,"%s.%ld.oxreplace-tmp.old",fname,(long) time(NULL));
  sprintf(comm,"cp %s %s",fname,fnameBackup);
  system(comm);

  fpOrig = fopen(fnameBackup,"r");
  fp = fopen(fname,"w");
  s = readAsString(fpOrig);
  if (ReplaceLine) {
    for (i=0; i<strlen(s); i++) {
      /* Look for \n */
      mm = 0;
	  for (j = i; j<strlen(s); j++) {
        if (matches(old,s+j,strlen(old))) mm = 1;
		if (s[j] == '\n') { break;}
      }
      if (mm) {
		i = j;
		fprintf(fp,"%s\n",new);
	  }else{
		for ( ; i < j; i++) {
		  putc(s[i],fp);
		}
		putc('\n',fp);
	  }
	}
  }else{
    for (i=0; i<strlen(s); i++) {
      if (!matches(old,s+i,strlen(old))) {
        fputc(s[i],fp);
      }else{
        fprintf(fp,"%s",new);
        i += strlen(old)-1;
      }
    }
  }
  fclose(fp); fclose(fpOrig);
  free(comm); free(fnameBackup);
  return 0;
}


usage() {
  fprintf(stderr,"oxreplace [--old oword --new nword --f rule_file_name --replaceLine] \n");
  fprintf(stderr,"          [file1 file2 ... ] \n");
  fprintf(stderr,"    Use --oldx or --newx to give a word in hexadecimal codes\n");
}

