/* sm.c   $OpenXM: OpenXM/src/kan96xx/Kan/sm.c,v 1.13 2000/07/26 02:21:30 takayama Exp $ */
#define DATE "2000/12/03"
#define RELEASE "3.001203"    /* This "string" must be an increasing seq.*/
                              /* You should write the same string in global.c
			         VersionString for ox_sm1.
				 Change also OxVersion in plugin/oxmisc.c
				 if you install new openXM protocol.*/

#include <stdio.h>
#include "datatype.h"
#include "stackm.h"
#include "extern.h"
/*  Release 2.981108 (1998, 11/8 --Sunday) ftp, source, binary-linux 
                     packages; bfunction, hol, gkz, appell.
    Release 2.981117 (1998, 11/17 --Tuesday)
                     packages; resol0, r-interface (new), oxasir(new)
                     bugfix : KdefaultPolyRing, hilbert.c
    Release 2.981123 bugfix : flush for solaris. bugfix : lockctrlCForOx().
    Release 2.981217 Rather stable release.
    New project on CMO starts.                      		     

*/

extern int StandardMacros;
extern int StartAFile;
extern char *StartFile;
extern int StartAString;
extern char *StartString;
extern int Quiet;
extern char *VersionString;

main(argc,argv) 
int argc;
char *argv[];
{
  int i;
  StandardMacros = 1;
  VersionString = RELEASE;
  if (argc > 1) {
    for (i=1; i<argc; i++) {
      if (strcmp(argv[i],"-standardmacros") == 0) {
	StandardMacros = 0; /* disable of the standard macros */
      }else if (strcmp(argv[i],"-f") == 0) {
	StartAFile = 1;
	i++;
	if (i >= argc) {
	  fprintf(stderr,"Usage: 1. sm1\n2. sm1 -f filename\n");
	  fprintf(stderr,"3. sm1 -standardmacros\n");
	  exit(15);
	}
	StartFile = argv[i];
      }else if (strcmp(argv[i],"-s") == 0) {
	StartAString = 1;
	i++;
	if (i >= argc) {
	  fprintf(stderr,"Usage: 1. sm1\n2. sm1 -f filename\n");
	  fprintf(stderr,"3. sm1 -standardmacros\n");
	  fprintf(stderr,"4. sm1 -q  (quiet)\n");
	  fprintf(stderr,"5. sm1 -s \"command strings\" \n");
	  exit(15);
	}
	StartString = argv[i];
      } else if (strcmp(argv[i],"-q") == 0) {
	Quiet = 1;
      }else{
	fprintf(stderr,"Usage: 1. sm1\n2. sm1 -f filename\n");
	fprintf(stderr,"3. sm1 -standardmacros\n");
	fprintf(stderr,"4. sm1 -q  (quiet)\n");
	fprintf(stderr,"5. sm1 -s \"command strings\" \n");
	exit(15);
      }
    }
  }


  stackmachine_init();
  if (!Quiet) {
    fprintf(Fstack,"\n");
    fprintf(Fstack,"Kan/StackMachine1");
    fprintf(Fstack,"                         1991 April --- 2000.\n");
    fprintf(Fstack,"                              Release %s (C) N. Takayama\n",
	    RELEASE);
    fprintf(Fstack,"gc 4.14 (C) Boehm, Demers, GNU MP 2.0.2 (C) Free Software Foundation,\n");
    fprintf(Fstack,"OpenXM (C) OpenXM developing team. \n");
    fprintf(Fstack,"This software may be freely distributed as is ");
    fprintf(Fstack,"with no warranty expressed.\n");
    fprintf(Fstack,"Info: http://www.math.kobe-u.ac.jp/KAN, kan@math.kobe-u.ac.jp. ? for help.\n");
#ifdef NOGC
    fprintf(Fstack,"No garbage collector is installed.\n");
#endif
  }
  KinitKan();
  if (!Quiet) {
    fprintf(Fstack,"\nReady\n\n");
  }
  scanner();
  Kclose();  stackmachine_close();
}



