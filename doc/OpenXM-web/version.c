/*  $OpenXM$ */
#include <stdio.h>
#include <sys/types.h>
#include <sys/stat.h>

#define SIZE  1024
main(int argc, char *argv[]) {
  int c,i,j;
  int counter = 0;
  int line = 0;
  char s[SIZE];
  struct stat sb;
  FILE *fp;
  FILE *fp2;
  char fin[SIZE];
  char oldversion[SIZE];
  char newversion[SIZE];
  char oldversion2[SIZE];
  char newversion2[SIZE];
  char comm[SIZE];
  oldversion[0] = newversion[0] = fin[0] = 0;

  fp = fopen(".tmp.txt","w");
  if (fp == NULL) exit(10);

  for (i=1;  i<argc; i++) {
    if (strcmp(argv[i],"--oldversion")==0) {
	  i++;
      strcpy(oldversion,argv[i]);
      strcpy(oldversion2,argv[i]);
	  for (j=0; j<strlen(oldversion2); j++)
		if (oldversion2[j] == '.') oldversion2[j] = '_';
	}else if (strcmp(argv[i],"--newversion")==0) {
	  i++;
	  strcpy(newversion,argv[i]);
	  strcpy(newversion2,argv[i]);
	  for (j=0; j<strlen(newversion2); j++)
		if (newversion2[j] == '.') newversion2[j] = '_';
	}else if (strcmp(argv[i],"--file") == 0) {
      i++;
	  strcpy(fin,argv[i]);
	}
  }

  if (fin[0] == 0 || oldversion[0] == 0 || newversion[0] == 0) {
	fprintf(stderr,"options are not valid.\n");
    usage();
	exit(10);
  }

  fp2 = fopen(fin,"r");
  if (fp2 == NULL) {
	fprintf(stderr,"File is not found.\n");
	usage();
	exit(10);
  }

  while (fgets(s,SIZE,fp2) != NULL) {
	if (substr(s,oldversion) && !substr(s,"Current")) {
      replace(s,oldversion,newversion,fp);
	}else if (substr(s,oldversion2)) {
      replace(s,oldversion2,newversion2,fp);
	}else{
	  fputs(s,fp);
	}
  }
  fclose(fp);
  sprintf(comm,"rm -f %s",fin);
  system(comm);
  sprintf(comm,"mv .tmp.txt %s",fin);
  system(comm);
}

usage() {
  fprintf(stderr,"version --oldversion x.y.z  --newversion p.q.r --file finename\n");
}

there(char s[],int c) {
  int i;
  int n;
  n = strlen(s);
  for (i=0; i<n; i++) {
	if (s[i] == c) return i;
  }
  return -1;
}

substr(char s[],char a[]) {
  int n,m,i,j;
  n = strlen(s);
  m = strlen(a);
  for (i=0; i<n-m; i++) {
	for (j=0; j<m; j++) {
	  if (s[i+j] != a[j]) break;
	  if (j == m-1) return 1;
	}
  }
  return 0;
}


replace(char s[],char a[],char newversion[],FILE *fp) {
  int n,m,i,j,k;
  n = strlen(s);
  m = strlen(a);
  for (i=0; i<n-m; i++) {
	for (j=0; j<m; j++) {
	  if (s[i+j] != a[j]) break;
	  if (j == m-1) {
		for (k=0; k<i; k++) {
		  fputc(s[k],fp);
		}
		fputs(newversion,fp);
		for (k=i+m; k<n; k++) {
		  fputc(s[k],fp);
		}
		return 1;
	  }
	}
  }
  return 0;
}

