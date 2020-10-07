/* $OpenXM: OpenXM/src/k097/k2.c,v 1.6 2015/10/10 11:29:46 takayama Exp $ */
/* k2.c main program of k. */

#define DATE "1998,12/15"
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "datatype.h"
#include "stackm.h"
#include "extern.h"
#include "extern2.h"

#include "ki.h"

char *getLOAD_K_PATH();  /* from d.h */

#ifdef CALLASIR
#include "ak0.h"
#endif


extern int DebugCompiler;  /* 0:   , 1: Displays sendKan[ .... ] */
extern int DebugMode;
extern int K00_verbose;

int Startupk2 = 1;


void main(int argc,char *argv[]) {
  extern int Saki;
  extern int Interactive;
  int i;
  char *s;
  char tname[1024];
  FILE *fp;
  if (argc >= 2) {
    for (i=1; i < argc; i++) {
      if (strcmp(argv[i],"-d") == 0) {
	fprintf(stderr,"DebugCompiler = 1\n");
	DebugCompiler = 1;
      }else if (strcmp(argv[i],"-nostartup") == 0) {
	fprintf(stderr,"Do not load startup.k\n");
	Startupk2 = 0;
      }else if (strcmp(argv[i],"-v") == 0) {
	K00_verbose = 1;
      }else if (strcmp(argv[i],"-vv") == 0) {
	K00_verbose = 2;
      }else{
	fprintf(stderr,"Usage: 1: k\n");
	fprintf(stderr,"       2: k -d  (DebugCompiler=1)\n");
	fprintf(stderr,"       3: k -nostartup  (do not load startup.k)\n");
	fprintf(stderr,"       4: k -v (verbose) or k -vv \n");
	exit(1);
      }
    }
  }
#ifdef CALLASIR
  GC_INIT(); /* GC_init(); */
  KasirKanAsirInit();
#endif
  KSstart();   /********  initialize ***********/
  /* Call setjmp(EnvOfStackMachine) for error handling. cf. scanner() in
   Kan/stackmachine.c */

  fprintf(stderr,"This is kan/k0 Version %s",DATE);
  fprintf(stderr,"\n");
  fprintf(stderr,"WARNING: This is an EXPERIMENTAL version\n");
  if (K00_verbose == 1) {
    KSexecuteString(" /K00_verbose 1 def ");
  }else if (K00_verbose == 2) {
    KSexecuteString(" /K00_verbose 2 def ");
  }else {
    KSexecuteString(" /K00_verbose 0 def ");
  }
  execFile("var.sm1"); KSexecuteString(" strictMode ");
  execFile("incmac.sm1");
  execFile("slib.sm1");

  printf("\n\nIn(1)= ");
  s = "startup.k";
  if (Startupk2) {
    strcpy(tname,s);
    if ((fp = fopen(tname,"r")) == (FILE *) NULL) {
      strcpy(tname,getLOAD_K_PATH());
      strcat(tname,s);
      if ((fp = fopen(tname,"r")) == (FILE *) NULL) {
	strcpy(tname,LOAD_K_PATH);
	strcat(tname,s);
	if ((fp = fopen(tname,"r")) == (FILE *) NULL) {
	  fprintf(stderr,"No startup file.\n");
	}
      }
    }
    if (fp != (FILE *)NULL) {
      if (K00_verbose) fprintf(stderr,"Reading startup.k... ");
      Saki = 0;
      parseAfile(fp);
      KCparse();
      if (K00_verbose) fprintf(stderr,"Done.\n");
    }
  }

  clearInop();
  Interactive = 1;
  parseAfile(stdin);
  /*  parseAstring(" sm1(\" 1 1 10 { message } for \" ) "); It works ! */
  KCparse();

  KSstop(); /* closing */
}







