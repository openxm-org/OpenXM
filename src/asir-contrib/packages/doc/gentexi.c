 /*  $OpenXM: OpenXM/src/asir-contrib/packages/doc/gentexi.c,v 1.5 2002/08/23 03:26:16 noro Exp $  */

#include <stdio.h>
int Debug = 0;
#define VMAX 20
#define LIMIT 1024
#define ITEMMAX 1024
struct item {
  char *category;  /* base */
  char *category2;  /* taka_base */
  char *name;      /* base_replace */
  int argc;
  char *argv[VMAX];  /* A and Rule of base_replace(A,Rule) */
  int optc;
  char *optv[VMAX];
  char *shortDescription;
  char *description;
  char *algorithm;
  char *examplev[VMAX];
  char *exampleDescv[VMAX];
  int examplec;
  int refc;
  char *refv[VMAX];
  char *author;
};
struct item *getItem(void);
char *str(char *key);
char *str2(char *key,int size);
int cmpItem(struct item *it,struct item *it2);

char *S;
int Ssize = 256;
int Sp = 0;
char *Upnode;
char *Category=NULL;
char *Lang="en";
int Include = 0;
int GenExample = 0;

main(int argc,char *argv[]) {
  char *t;
  int p,c,n,i;
  struct item *tt;
  struct item *items[ITEMMAX];

  Upnode = str("UNKNOWN");
  for (i=1; i<argc; i++) {
    if (strcmp(argv[i],"--upnode") == 0) {
      i++; if (i >= argc) { fprintf(stderr,"--upnode node-name\n"); exit(1);}
      Upnode = str(argv[i]);
    }else if (strcmp(argv[i],"--category") == 0) {
      i++; if (i >= argc) { fprintf(stderr,"--category category-name\n"); exit(1);}
      Category = str(argv[i]);
    }else if (strcmp(argv[i],"--en") == 0) {
      Lang = "en";
    }else if (strcmp(argv[i],"--ja") == 0) {
      Lang = "ja";
    }else if (strcmp(argv[i],"--include") == 0) {
      Include = 1;
    }else if (strcmp(argv[i],"--example") == 0) {
      GenExample = 1;
    }else if (strcmp(argv[i],"--debug") == 0) {
      Debug = 1;
    }else {
      fprintf(stderr,"Unknown option\n"); exit(1);
    }
  }
  S = (char *)malloc(Ssize);
  /* Read data from stdin to the string buffer S */
  while ((c=getchar()) != EOF) {
    S[Sp++] = c; S[Sp] = 0;
    if (Sp >= Ssize-3) {
      Ssize = 2*Ssize;
      t = S;
      S = (char *)malloc(Ssize);
      if (S == NULL) {
        fprintf(stderr,"No more memory to allocate S.\n");
        exit(20);
      }
      strcpy(S,t);
    }
  }

  /* Read items */
  n = 0;
  while ((tt = getItem()) != NULL) { 
    if (Debug) printItem(tt);
    if (n >= ITEMMAX) {
      fprintf(stderr,"Too many entries.\n"); exit(1);
    }
    if (Category != NULL) {
      if (strcmp(Category,tt->category) == 0 ||
          strcmp(Category,tt->category2) == 0) {
        items[n++] = tt;
      }
    }else{
      items[n++] = tt;
    }
  }
  if (Debug) fprintf(stderr,"Sorting...\n");
  shell(items,n);
  if (Debug) fprintf(stderr,"Done.\n");
  
  printMenu(stdout,items,n);

  for (i=0; i<n; i++) {
    printTexi(stdout,items[i]);
  }
  exit(0);  
}

genInclude(char *name) {
  char fname[4098];
  FILE *fp;
  int c;
  
  sprintf(fname,"tmp/%s-auto-%s.texi",name,Lang);
  fp = fopen(fname,"r");
  if (fp == NULL) {
    /* fprintf(stderr,"No file %s\n",fname); */
    return 0;
  }
  while ((c=fgetc(fp)) != EOF) {
    putchar(c);
  }
  putchar('\n');
  fclose(fp);
  return 0;
}

cmpItem(struct item *it,struct item *it2) {
  return strcmp(it->name,it2->name);
}
struct item * newItem(){
  struct item *a;
  a = (struct item *)malloc(sizeof(struct item));
  if (a == NULL) {
    fprintf(stderr,"newItem: No more memory.\n");
    exit(20);
  }
  a->argc = 0; a->optc = 0; a->refc=0; a->examplec = 0;
  a->category = a->category2 = a->name = a->shortDescription
    = a->description = a->author = a->algorithm = NULL; 
  return a;
}
  
nextToken(char *key,int n) {
  static int pos = 0;
  int i = 0;
  if (pos >= Ssize) return -1;
  while (S[pos] <= ' ') {
    pos++;
    if (pos >= Ssize) return -1;
  }
  while (S[pos] > ' ') {
    key[i++] = S[pos++]; key[i] = 0;
    if (i >= n-1) {
      fprintf(stderr,"Too big key word.\n");
      fprintf(stderr,"key=%s\n",key);
      exit(10);
    }
    if (S[pos-1] == '(' ||
        S[pos-1] == ')' ||
        S[pos-1] == ',' ||
        S[pos-1] == '{' ||
        S[pos-1] == '}' ||
        S[pos-1] == '|' ) {
      return pos;
    }
    if (S[pos] == '(' ||
        S[pos] == ')' ||
        S[pos] == ',' ||
        S[pos] == '{' ||
        S[pos] == '}' ||
        S[pos] == '|' ) {
      return pos;
    }
    
  }
  if (Debug) fprintf(stderr,"token=%s\n",key);
  return pos;
}

printItem(struct item *it) {
  int i;
  if (it == NULL) return;
  if (it->category != NULL) 
    printf("category=%s\n",it->category);
  if (it->category2 != NULL) 
    printf("category2=%s\n",it->category2);
  if (it->name != NULL)
    printf("name=%s\n",it->name);
  for (i=0; i<it->argc; i++)
    printf("  argv[%d]=%s\n",i,it->argv[i]);
  for (i=0; i<it->optc; i++)
    printf("  optv[%d]=%s\n",i,it->optv[i]);
  if (it->shortDescription != NULL)
    printf("shortDescription=%s\n",it->shortDescription);
  if (it->description != NULL)
    printf("description=%s\n",it->description);
  if (it->algorithm != NULL)
    printf("algorithm=%s\n",it->algorithm);
  for (i=0; i <it->examplec; i++) 
    printf("examplev[%d]=%s\n",i,it->examplev[i]);
  for (i=0; i <it->examplec; i++) 
    printf("exampleDescv[%d]=%s\n",i,it->exampleDescv[i]);
  for (i=0; i<it->refc; i++)
    printf("  refv[%d]=%s\n",i,it->refv[i]);
  if (it->author != NULL)
    printf("author=%s\n",it->author);
  printf("\n");
}

char *str(char *key) {
  char *s;
  s = (char *)malloc(strlen(key)+1);
  if (s == NULL) {
    fprintf(stderr,"str: No more memory.\n");
    exit(20);
  }
  strcpy(s,key);
  return s;
}
char *str2(char *key,int size) {
  char *s;
  int i;
  s = (char *)malloc(size+1);
  if (s == NULL) {
    fprintf(stderr,"str2: No more memory.\n");
    exit(20);
  }
  for (i=0; i<size; i++) {
    s[i] = key[i]; s[i+1] = 0;
  }
  return s;
}
char *getCategory(char *key) {
  int i,n;
  char *s;
  s = str(key);
  for (i=0; i<strlen(s); i++) {
    if (s[i] == '_') {
      s[i] = 0;
      return s;
    }
  }
  return s;
}
char *getCategory2(char *key) {
  int i,n;
  char *s;
  int count;
  s = str(key);
  for (i=0; i<strlen(s); i++) {
    if (s[i] == '_') count++;
    if (count == 2) {
      s[i] = 0; return s;
    }
  }
  return s;
}
  

struct item *getItem() {
  char key[LIMIT];
  char key2[LIMIT];
  struct item *it;
  int p;
  int pp,pOld;
  int argc;
  int examplec = 0;
  it = newItem();
  do {
    p = nextToken(key,LIMIT);
    /* printf("%s\n",key); */
    if (strcmp(key,"begin:") == 0) break;
  }while (p >= 0);
  if (p < 0) {
    fprintf(stderr,"gentexi: End of input file.\n");
    return NULL;
  }
  p = nextToken(key,LIMIT);
  it->name = str(key);
  it->category = getCategory(key);
  it->category2 = getCategory2(key);
  nextToken(key,LIMIT);
  if (strcmp(key,"(") != 0) {
    fprintf(stderr," ( is expected at %s\n",it->name);
    exit(10);
  }
  argc = 0; 
  while ((pp=nextToken(key,LIMIT)) >= 0) {
    if (strcmp(key,"|") == 0) {
      /* options */
      argc = 0;
      while ((pp=nextToken(key,LIMIT)) >= 0) {
        if (strcmp(key,")") == 0) {
          break;
        }
        if (strcmp(key,",") != 0) {
          it->optv[argc] = str(key);
          argc++; it->optc = argc;
        }
        if (argc >+ VMAX -1) {
          fprintf(stderr,"Too many opt args at %s\n",it->name);
          exit(10);
        }
      }
    }
    if (strcmp(key,")") == 0) {
      break;
    }else if (strcmp(key,",") != 0) {
      it->argv[argc] = str(key); 
      argc++; it->argc=argc;
    }
    if (argc >= VMAX-1) {
      fprintf(stderr,"Too many args at %s\n",it->name);
      exit(10);
    }
  }

  /* Getting the short Description */
  p = pp;
  do {
    pOld = p;
    p = nextToken(key,LIMIT);
    /* printf("%s\n",key); */
    if (key[strlen(key)-1] == ':') break; /* Next keyword. */
  }while (p >= 0);
  it->shortDescription = str2(&(S[pp]),pOld-pp);


  do {
    /* Get Description or Examples */
    if (strcmp(key,"end:") == 0) break;
    if (strcmp(key,"description:") == 0 ||
        strcmp(key,"algorithm:") == 0 ||
        strcmp(key,"author:") == 0 ||
        strcmp(key,"example:") == 0 ||
		strcmp(key,"example_description:") ==0 ) {
      pp = p;
      strcpy(key2,key);
      do {
        pOld = p;
        p = nextToken(key,LIMIT);
        /* printf("%s\n",key); */
        if (key[strlen(key)-1] == ':') break; /* Next keyword. */
      }while (p >= 0);
      if (strcmp(key2,"description:") == 0) {
        it->description = str2(&(S[pp]),pOld-pp);
      }
      if (strcmp(key2,"example:") == 0) {
        it->examplev[examplec++] = str2(&(S[pp]),pOld-pp);
        it->exampleDescv[examplec-1] = "";
        it->examplec = examplec;
        if (examplec > VMAX-1) {
          fprintf(stderr,"Too many examples. \n");
          exit(20);
        }
      }
      if (strcmp(key2,"example_description:") == 0) {
        it->exampleDescv[examplec-1] = str2(&(S[pp]),pOld-pp);
      }
      if (strcmp(key2,"author:") == 0) {
        it->author = str2(&(S[pp]),pOld-pp);
      }
      if (strcmp(key2,"algorithm:") == 0) {
        it->algorithm = str2(&(S[pp]),pOld-pp);
      }
    }else if (strcmp(key,"ref:") == 0) {
      argc = 0;
      while ((pp=nextToken(key,LIMIT)) >= 0) {
        p = pp;
        if (key[strlen(key)-1] == ':') break;
        if (strcmp(key,",") != 0) {
          it->refv[argc] = str(key); 
          argc++; it->refc = argc;
        }
        if (argc >= VMAX-1) {
          fprintf(stderr,"Too many args for Ref at %s\n",it->name);
          exit(10);
        }
      }
    }else{
      fprintf(stderr,"Warning: unknown keyword << %s >> at %s. Ignored.\n",key, it->name);
      p = nextToken(key,LIMIT);
    }
  }while (p >= 0);

  return it;
}

shell(struct item *v[],int n) {
  int gap,i,j;
  struct item *temp;
  
  for (gap = n/2; gap > 0; gap /= 2) {
    for (i = gap; i<n; i++) {
      for (j=i-gap ; j>=0 && cmpItem(v[j],v[j+gap])>0 ; j -= gap) {
        temp = v[j];
        v[j] = v[j+gap];
        v[j+gap] = temp;
      }
    }
  }
}

printMenu(FILE *fp, struct item **it, int n) {
  int i;

  fprintf(fp,"@menu\n");
  for ( i = 0; i < n; i++ )
  	fprintf(fp,"* %s::\n",it[i]->name);
  fprintf(fp,"@end menu\n");
}

printTexi(FILE *fp, struct item *it) {
  int i;
  fprintf(fp,"@c DO NOT EDIT THIS FILE. Generated by gentexi.\n");
  if (it == NULL) {
    fprintf(fp,"@c item is NULL.\n");
    return ;
  }
  if (it->name == NULL) {
    fprintf(fp,"@c item name is missing.\n");
    return ;
  }

#if 0
  fprintf(fp,"@menu\n");
  fprintf(fp,"* %s::\n",it->name);
  fprintf(fp,"@end menu\n");
#endif
  fprintf(fp,"@node %s,,, %s\n",it->name,Upnode);
  fprintf(fp,"@subsection @code{%s}\n",it->name);
  fprintf(fp,"@findex %s\n",it->name);
  fprintf(fp,"@table @t\n");
  fprintf(fp,"@item %s(",it->name);
  for (i=0; i<it->argc; i++) {
    fprintf(fp,"@var{%s}",it->argv[i]);
    if (i != it->argc-1) fprintf(fp,",");
  }
  fprintf(fp,")\n");
  if (it->shortDescription != NULL) {
    fprintf(fp,": ");
    for (i=0; i<strlen(it->shortDescription); i++) {
      if (it->shortDescription[i] == '{') {
        fprintf(fp,"@var{");
      }else {
        fprintf(fp,"%c",it->shortDescription[i]);
      }
    }
    fprintf(fp," \n");
  }
  if (it->optc > 0) {
    fprintf(fp,"@item %s(",it->name);
    for (i=0; i<it->argc; i++) {
      fprintf(fp,"@var{%s}",it->argv[i]);
      if (i != it->argc-1) fprintf(fp,",");
    }
    fprintf(fp," | ");
    for (i=0; i<it->optc; i++) {
      fprintf(fp,"@var{%s}=key%d",it->optv[i],i);
      if (i != it->optc-1) fprintf(fp,",");
    }
    fprintf(fp,")\n");
    fprintf(fp,": This function allows optional variables \n  ");
    for (i=0; i<it->optc; i++) {
      fprintf(fp,"@var{%s}",it->optv[i]);
      if (i != it->optc-1) fprintf(fp,", ");
    }
    fprintf(fp,"\n");
  }
  fprintf(fp,"@end table\n");

  /* include file */
  if (Include) {
    if (genInclude(it->name)) 
      fprintf(fp,"@c @include tmp/%s-auto-en.texi\n",it->name);
  }
  fprintf(fp,"@c @itemize @bullet \n");
  fprintf(fp,"@c @item \n");
  fprintf(fp,"@c @end itemize\n");
  
  if (it->description != NULL) {
    fprintf(fp,"%s\n\n",it->description);
  }

  if (it->algorithm != NULL) {
    fprintf(fp,"\n\n@noindent\nAlgorithm: \n");
    fprintf(fp,"%s\n\n",it->algorithm);
  }

  if (it->examplec > 0) {
    for (i=0; i<it->examplec; i++) {
	  if (it->examplec == 1) {
        fprintf(fp,"Example:\n");
	  }else{
        fprintf(fp,"Example %d:\n",i);
	  }
      fprintf(fp,"@example\n");
      fprintf(fp,"%s\n",it->examplev[i]);
      if (GenExample) {
        outputOfExample(it->examplev[i]);
      }
      fprintf(fp,"@end example\n");
	  if (it->exampleDescv[i] != NULL && strlen(it->exampleDescv[i]) > 0) {
		fprintf(fp,"%s\n\n",it->exampleDescv[i]);
	  }
    }
  }
  if (it->author != NULL) {
    fprintf(fp,"Author : %s\n\n",it->author);
  }
  if (it->refc > 0) {
    fprintf(fp,"@table @t\n");
    fprintf(fp,"@item References\n");
    for (i=0; i <it->refc; i++) {
      fprintf(fp,"@code{%s} ",it->refv[i]);
      if (i != it->refc-1) fprintf(fp,", ");
    }
    fprintf(fp,"\n@end table\n");
  }
  fprintf(fp,"\n");
}

outputOfExample(char *com) {
  FILE *fp2;
  int c;
  fp2 = fopen("gentexi-in.tmp","w");
  if (fp2 == NULL) {
    fprintf(stderr,"Cannot open tentexi-in.tmp\n");
    exit(10);
  }
  system("rm -f gentexi-out.tmp");
  fprintf(fp2,"output(\"gentexi-out.tmp\")$\n");
  fprintf(fp2,"%s\n",com);
  fprintf(fp2,"output()$\n");
  fprintf(fp2,"quit;");
  fclose(fp2);
  system("asir <gentexi-in.tmp >/dev/null");

  fp2 = fopen("gentexi-out.tmp","r");
  if (fp2 == NULL) {
    fprintf(stderr,"Cannot open tentexi-in.tmp\n");
    exit(10);
  }
  while ((c=fgetc(fp2)) != EOF) {
    putchar(c);
  }
  putchar('\n');
}
