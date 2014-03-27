#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <string.h>
#include <stdarg.h>

#define KO_TIME_TAB_LEN 10
#define KO_TIME_NAME_LEN 20

struct ko_time{
  char name[KO_TIME_NAME_LEN];
  clock_t start, end;
};

struct ko_time ko_time_tab[KO_TIME_TAB_LEN];

static int 
ko_time_getindex(char *s)
{
  int i;
  for(i=0; i<KO_TIME_TAB_LEN; i++)
    if (!strcmp(ko_time_tab[i].name, s))
      return i;
  fprintf(stderr, "Error: In ko_time_getindex,");
  fprintf(stderr, " unknown name `%s`\n", s);  
  exit(EXIT_FAILURE);
}

void 
ko_time_init(int n, ...)
{
  va_list ap;
  char *s;
  double *v;
  int i,j;

  if (n > KO_TIME_TAB_LEN -1){
    fprintf(stderr, "Error: in ko_time_init\n");
    exit(EXIT_FAILURE);
  }

  va_start(ap, n);
  for(i= 0; i<n; i++){
    s = va_arg(ap, char*);
#ifdef _DEBUG
    fprintf(stderr,"ko_time_tab[%d].name=%s\n",i,s);
#endif
    if(strlen(s) < KO_TIME_NAME_LEN )
      strcpy(ko_time_tab[i].name, s);
    else{
      fprintf(stderr, "Error: in ko_time_init\n");
      exit(EXIT_FAILURE);
    }
  }
  va_end(ap);

  for( ; i<KO_TIME_TAB_LEN; i++)
    strcpy(ko_time_tab[i].name, "");
}

void
ko_time_print(void)
{
  int i,j, len, maxlen=0;
  for(i=0; i<KO_TIME_TAB_LEN; i++)
    if((len = strlen(ko_time_tab[i].name)) > maxlen)
      maxlen = len;

  printf("time\n");
  for(i=0; i<KO_TIME_TAB_LEN && (len=strlen(ko_time_tab[i].name)); i++){
    printf("\t%s",ko_time_tab[i].name);
    for(j=len; j<maxlen; j++)
      putchar(' ');
    printf(": %f sec\n", (double)(ko_time_tab[i].end-ko_time_tab[i].start)/CLOCKS_PER_SEC);
  }
}

void 
ko_time_start(char *s)
{
  int i = ko_time_getindex(s);
  ko_time_tab[i].start = clock();
}

void 
ko_time_end(char *s)
{
  int i = ko_time_getindex(s);
  ko_time_tab[i].end = clock();
}
