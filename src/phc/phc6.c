/*  phc6.c ,  yama:1999/sm1-prog/phc6.c */
/* $OpenXM: OpenXM/src/phc/phc6.c,v 1.3 2000/10/31 12:22:05 takayama Exp $ */
/* This is a simple C interface to the black-box solver of phc.
** Requirements:
**  1) executable version of phc will be searched in the following order:
**    OpenXM_HOME/bin/, /usr/local/bin/phc, /tmp/phc, your command search path.
**     Here, PHC_LIBDIR is an environment variable.
**  2) user of this program has write permissions to create
**     the files "tmp.input.xx" and "tmp.output.xx" in the directory where
**     this program is executed.  xx are numbers.
*/

#include <stdio.h>
#include <sys/stat.h>
#include <unistd.h>
#include <stdlib.h>

/* Definition of class identifiers. */
#define Snull             0
#define SstringObject     5
#define Sarray            6  
#define SlongdoubleComplex  601

/* Definition of Object */
union cell {
  int ival;
  char *str;
  struct phc_object *op;
  double longdouble;
};
struct phc_object{
  int tag;                /* class identifier */
  union cell lc;          /* left cell */
  union cell rc;          /* right cell */
};

/* Memory allocation function.  
   Use your favorite memory allocation function.
   I recommend not to use malloc and to use gc4.14 for large applications. */
#define sGC_malloc(n)   malloc(n)

/********** macros to use Sarray **************/
/* put to Object Array */
#define phc_putoa(ob,i,cc) {\
                   if ((ob).tag != Sarray) {fprintf(stderr,"Warning: PUTOA is for an array of objects\n");} else \
                     {if ((0 <= (i)) && ((i) < (ob).lc.ival)) {\
                      (ob.rc.op)[i] = cc;\
                    }else{\
                       fprintf(stderr,"Warning: PUTOA, the size is %d.\n",(ob).lc.ival);\
                  }}}
#define phc_getoa(ob,i) ((ob.rc.op)[i])
#define phc_getoaSize(ob) ((ob).lc.ival)

/* prototypes */
struct phc_object phc_newObjectArray(int size);
void phc_printObject(FILE *fp,struct phc_object ob);
char *phc_generateUniqueFileName(char *s);
char *phc_which(char *s);   /* search a path for the file s */
struct phc_object phc_complexTo(double r, double i);


int phc_scan_for_string(FILE *fp, char str[]);
struct phc_object phc_scan_solutions(FILE *fp, int npaths, int dim );
struct phc_object phc_scan_output_of_phc(char *fname);
struct phc_object phc_call_phc(char *sys); 

int phc_verbose = 0;
int phc_overwrite = 1;  /* Always use tmp.input.0 and tmp.output.0
                           for work files. */

main(int argc, char *argv[]) {
  struct phc_object ob;
  int n,i,dim;
#define INPUTSIZE 4096
  char input[INPUTSIZE];
  char fname[INPUTSIZE];
#define A_SIZE 1024
  char a[A_SIZE];
  int message = 0;
  for (i=1; i<argc; i++) {
    if (strcmp(argv[i],"-v") == 0) {
      phc_verbose = 1; message=1;
    }else if (strcmp(argv[i],"-g") == 0) {
      phc_overwrite = 0;
    }else if (strcmp(argv[i],"-file") == 0) {
      /* For debugging. */
      i++;
      strncpy(fname,argv[i],INPUTSIZE-1);
      ob = phc_scan_output_of_phc(fname);
      n = phc_getoaSize(ob);
      printf("[\n");
      for (i=0; i<n; i++) {
        phc_printObject(stdout,phc_getoa(ob,i));
        if (i != n-1) printf(" ,\n"); else printf(" \n");
      }
      printf("]\n");
      exit(0);
    }else if (strcmp(argv[i],"-i") == 0) {
      ob = phc_call_phc(argv[i+1]);
      n = phc_getoaSize(ob);
      printf("[\n");
      for (i=0; i<n; i++) {
        phc_printObject(stdout,phc_getoa(ob,i));
        if (i != n-1) printf(" ,\n"); else printf(" \n");
      }
      printf("]\n");
      exit(0);
    }
  }
  if (message) {
    printf("Input example:\n 2 \n x**2 + y**2 - 1;\n x**2 + y**2 - 8*x - 3;\n");
    printf("Note that input length is limited.\n");
  }
  while (1) {
    if (message) printf("dim= ");
    if (fgets(input,INPUTSIZE,stdin) <= 0) break;
    sscanf(input,"%d",&dim);
    sprintf(input,"%d\n",dim);
    if (message) printf("Input %d equations please.\n",dim);
    for (i=0; i<dim; i++) {
      if (message) {printf("eq[%d] = ",i);  fflush(stdout);}
      do {
        fgets(a,A_SIZE-1, stdin);
      } while (strlen(a) == 0);
      if (strlen(a) >= A_SIZE-3) {
        fprintf(stderr,"Too big input for the input buffer a.\n"); exit(10);
      }
      if (strlen(input)+strlen(a) >= INPUTSIZE) {
        fprintf(stderr,"Too big input for the input buffer input.\n"); exit(10);
      }
      sprintf(input+strlen(input),"%s\n",a);
    }
    ob = phc_call_phc(input);
    if (message) {
      printf("-----------------------------------------------------------\n");
    }
    n = phc_getoaSize(ob);
    printf("[\n");
    for (i=0; i<n; i++) {
      phc_printObject(stdout,phc_getoa(ob,i));
      if (i != n-1) printf(" ,\n"); else printf(" \n");
    }
    printf("]\n");
  }
}

int phc_scan_for_string(FILE *fp, char s[])
     /*
  **  Scans the file fp for a certain string str of length lenstr+1.
  **  Reading stops when the string has been found, then the variable
  **  on return equals 1, otherwise 0 is returned.
  */
#define BUF_SIZE 1024
{
  char buf[BUF_SIZE];
  int pt,n,c,i;
  pt = 0;
  n=strlen(s);
  if (n > BUF_SIZE-2) {
	fprintf(stderr,"Too long string for scan_for_string.\n");
	exit(1);
  }
  for (i=0; i<n; i++) {
	buf[i] = fgetc(fp); buf[i+1]='\0';
	if (buf[i] == EOF) return 0;
  }
  if (strcmp(s,buf) == 0) return 0;
  while ((c=fgetc(fp)) != EOF) {
	for (i=0; i<n; i++) {
	  buf[i] = buf[i+1];
	}
	buf[n-1]=c; buf[n] = '\0';
	if (strcmp(s,buf) == 0) return 1;
  }
  return 0;
}

int phc_scan_for_string_old(FILE *fp, char str[])
     /*
  **  Scans the file fp for a certain string str of length lenstr+1.
  **  Reading stops when the string has been found, then the variable
  **  on return equals 1, otherwise 0 is returned.
  */
{
#define BUF_SIZE 1024
  char buf[BUF_SIZE];
  char ch;
  int index,i,compare,npaths,dim,found;
  int lenstr;
  lenstr = strlen(str);
  if (lenstr >= BUF_SIZE-1) {
    fprintf(stderr,"Too long string in phc_scan_for_string\n");
    exit(-1);
  }
  index = -1;
  found = 0;
  while (((ch = fgetc(fp))!=EOF) && found == 0)
    {
      if (index == -1 && ch == str[0])
        {
          index = 0;
          buf[index] = ch;
        }
      else
        {
          if (index == lenstr)
            {
              compare = 0;
              for (i=0; str[i] != '\0'; i++)
                {
                  if (buf[i]!=str[i])
                    {
                      compare = compare+1;
                    }
                }
              if (compare == 0)
                {
                  found = 1;
                }
              index = -1;
            }
          else
            if (index > -1 && index < lenstr)
              {
                index = index+1;
                buf[index] = ch;
              }
        }
      if (found == 1) break;
    }
  return found;
}
struct phc_object phc_scan_solutions(FILE *fp, int npaths, int dim )
     /*
  **  Scans the file for the solutions, from a list of length npaths,
  **  of complex vectors with dim entries.
  **  The tolerance for the residual to a solution is set to 1.0E-12.
  **  Returns solutions.
  */
#define BUFSIZE 1024
{
  struct phc_object rob,sob;
  char ch;
  int fnd,i,j,nsols;
  double res;
  double realpart;
  double imagpart;
  /*  double realparts[npaths][dim];
  double imagparts[npaths][dim]; */
  double *realparts;
  double *imagparts;
  char buf[BUFSIZE];
  nsols = 0;
  realparts = (double *)sGC_malloc(sizeof(double)*npaths*dim+1);
  imagparts = (double *)sGC_malloc(sizeof(double)*npaths*dim+1);
  while ((ch = fgetc(fp)) != EOF) 
    {
      fnd = phc_scan_for_string(fp,"start residual :");
      if (fnd==1)
        {
          fscanf(fp,"%le",&res);
          /* printf(" residual = "); printf("%E\n",res);  */
          if (res < 1.0E-12) {
            nsols = nsols+1;
            if (nsols > npaths) {
              fprintf(stderr,"Something is wrong in phc_scan_solutions\n");
              fprintf(stderr,"npaths=%d, nsols=%d \n",npaths,nsols);
              exit(-1);
            }
          }
          fnd = phc_scan_for_string(fp,"the solution for t :");
          for (i=0;i<dim;i++)
            {
              fnd = phc_scan_for_string(fp,":");
			  fgets(buf,BUFSIZE-1,fp);
			  sscanf(buf,"%le %le",&realpart,&imagpart);
			  if (phc_verbose) fprintf(stderr,"sols in string: %s\n",buf);
              /*fscanf(fp,"%le",&realpart);
              fscanf(fp,"%le",&imagpart);*/
              if (res < 1.0E-12)
                {
                  /*realparts[nsols-1][i] = realpart;
                  imagparts[nsols-1][i] = imagpart;*/
                  realparts[(nsols-1)*npaths+i] = realpart;
                  imagparts[(nsols-1)*npaths+i] = imagpart;
                }
            }
        }
    }
  if(phc_verbose) fprintf(stderr,"  number of solutions = %i\n",nsols); 
  rob = phc_newObjectArray(nsols);
  for (i=0;i<nsols;i++)
    {
      /* fprintf(stderr,"Solution %i :\n",i+1); */
      sob = phc_newObjectArray(dim);
      for (j=0;j<dim;j++)
        {
          /*
            printf("%20.14le",realparts[i][j]); printf("  ");
            printf("%20.14le",imagparts[i][j]); printf("\n");
            */

            printf("%20.14le",realparts[i*npaths+j]); printf("  ");
            printf("%20.14le",imagparts[i*npaths+j]); printf("\n");

          /*phc_putoa(sob,j,phc_complexTo(realparts[i][j],imagparts[i][j]));*/
          phc_putoa(sob,j,phc_complexTo(realparts[i*npaths+j],
                                        imagparts[i*npaths+j]));
        }
      phc_putoa(rob,i,sob);
    }
  return(rob);
}
struct phc_object phc_scan_output_of_phc(char *fname)
     /*
  **  Scans the file "output" of phc in two stages to get
  **   1) the number of paths and the dimension;
  **   2) the solutions, vectors with residuals < 1.0E-12.
  */
{
  FILE *otp;
  char ch;
  int fnd,npaths,dim,i,nsols;
  otp = fopen(fname,"r");
  if (otp == NULL) {
    fprintf(stderr,"The file %d is not found.\n",fname);
    exit(0);
  }
  if (phc_verbose) fprintf(stderr,"Scanning the %s of phc.\n",fname);
  fnd = phc_scan_for_string(otp,"THE SOLUTIONS :");
  fscanf(otp,"%i",&npaths);
  if (phc_verbose) fprintf(stderr,"  number of paths traced = %i\n",npaths);
  fscanf(otp,"%i",&dim);
  if (phc_verbose) fprintf(stderr,"  dimension of solutions = %i\n",dim);
  return(phc_scan_solutions(otp,npaths,dim));
}
struct phc_object phc_call_phc(char *sys)  /* call phc, system as string */
{
  FILE *inp;
  char *f,*outf;
  char cmd[1024];
  char *w;
  struct phc_object phc_NullObject ;
  struct stat statbuf;

  phc_NullObject.tag = Snull;
  f = phc_generateUniqueFileName("tmp.input");
  if (phc_verbose) fprintf(stderr,"Creating file with name %s.\n",f);
  outf = phc_generateUniqueFileName("tmp.output");
  if (stat(outf,&statbuf) == 0) {
    sprintf(cmd,"/bin/rm -f %s",outf);
    system(cmd);
  }
  inp = fopen(f,"w");
  fprintf(inp,sys);
  fclose(inp);
  if ((w = phc_which("phc")) != NULL) {
    sprintf(cmd,"%s -b %s %s",w,f,outf);
  }else{
    sprintf(cmd,"phc -b %s %s",f,outf);
  }
  if (phc_verbose)fprintf(stderr,"Calling %s, black-box solver of phc.\n",cmd);
  system(cmd);
  if (stat(outf,&statbuf) < 0) {
    fprintf(stderr,"execution error of phc.\n");
    return(phc_NullObject);
  }else{
    if (phc_verbose) fprintf(stderr,"See the file %s for results.\n",outf);
    return(phc_scan_output_of_phc(outf));
  }
}


struct phc_object phc_newObjectArray(size) 
     int size;
{
  struct phc_object rob;
  struct phc_object *op;
  if (size > 0) {
    op = (struct phc_object *)sGC_malloc(size*sizeof(struct phc_object));
    if (op == (struct phc_object *)NULL) {fprintf(stderr,"No memory\n");exit(1);}
  }else{
    op = (struct phc_object *)NULL;
  }
  rob.tag = Sarray;
  rob.lc.ival = size;
  rob.rc.op = op;
  return(rob);
}

void phc_printObject(FILE *fp,struct phc_object ob)
{
  int n,i;
  if (ob.tag == Snull) {
    fprintf(fp,"null");
  }else if (ob.tag == SstringObject) {
    fprintf(fp,"%s",ob.lc.str);
  }else if (ob.tag == Sarray) {
    n = phc_getoaSize(ob);
    fprintf(fp,"[");
    for (i=0; i<n; i++) {
      phc_printObject(fp,phc_getoa(ob,i));
      if (i <n-1) fprintf(fp," , ");
    }
    fprintf(fp,"]");
  }else if (ob.tag == SlongdoubleComplex) {
    /* Try your favorite way to print complex numbers. */
    /*fprintf(fp,"(%20.14le)+I*(%20.14le)",ob.lc.longdouble,ob.rc.longdouble);*/
    fprintf(fp,"[%lf , %lf]",ob.lc.longdouble,ob.rc.longdouble);
  }else{
    fprintf(stderr,"Unknown phc_object tag %d",ob.tag);
  }
}
    

char *phc_generateUniqueFileName(char *s)
{
  char *t;
  int i;
  struct stat statbuf;
  t = (char *)sGC_malloc(sizeof(char)*strlen(s)+4+2);
  for (i=0; i<1000; i++) {
    /* Give up if we failed for 1000 names. */
    sprintf(t,"%s.%d",s,i);
    if (phc_overwrite) return(t);
    if (stat(t,&statbuf) < 0) {
      return(t);
    }
  }
  fprintf(stderr,"Could not generate a unique file name.");
  return(NULL);
}

char *phc_which(char *s) {
  struct stat statbuf;
  char *cmd,*a;
  a = getenv("OpenXM_HOME");
  if (a != NULL) {
    cmd = (char *) sGC_malloc(sizeof(char)*(strlen(s)+strlen(a)
                                            +strlen("/usr/local/bin/")+1));
    if (cmd == NULL) {fprintf(stderr,"No more memory.\n"); exit(1);}
    strcpy(cmd,a); strcat(cmd,"/bin/"); strcat(cmd,s);
    if (stat(cmd,&statbuf) == 0) {
      return(cmd);
    }
  }
  cmd = (char *) sGC_malloc(sizeof(char)*(strlen(s)
                                          +strlen("/usr/local/bin/")+1));
  if (cmd == NULL) {fprintf(stderr,"No more memory.\n"); exit(1);}
  strcpy(cmd,"/usr/local/bin/"); strcat(cmd,s);
  if (stat(cmd,&statbuf) == 0) {
    return(cmd);
  }
  strcpy(cmd,"/tmp/"); strcat(cmd,s);
  if (stat(cmd,&statbuf) == 0) {
    return(cmd);
  }
  return(NULL);
}

   
struct phc_object phc_complexTo(double r, double i)
{
  struct phc_object rob;
  rob.tag = SlongdoubleComplex;
  rob.lc.longdouble = r;
  rob.rc.longdouble = i;
  return(rob);
}

