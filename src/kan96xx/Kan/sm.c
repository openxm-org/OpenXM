/* sm.c   $OpenXM: OpenXM/src/kan96xx/Kan/sm.c,v 1.29 2015/09/29 01:52:14 takayama Exp $ */
#define DATE "2005/06/15"
#define RELEASE "3.050615"    /* This "string" must be an increasing seq.*/
/* You should write the same string in global.c
                     VersionString for ox_sm1.
                 Change also OxVersion in plugin/oxmisc.c
                 if you install new openXM protocol.*/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
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
//extern unsigned int GC_version;

int main(argc,argv) 
     int argc;
     char *argv[];
{
  int i;
  int gc_version;
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

  stackmachine_init(); /* GC is also initialized here */
  gc_version = GC_get_version();
  if (!Quiet) {
    fprintf(Fstack,"\n");
    fprintf(Fstack,"Kan/StackMachine1");
    fprintf(Fstack,"                         1991 April --- 2004.\n");
    fprintf(Fstack,"                              Release %s (C) N. Takayama\n",
            RELEASE);
    fprintf(Fstack,"gc %d.%d-%d (C) Boehm, Demers, Xerox, SGI, HP,\nGNU MP 4.1 (C) Free Software Foundation,\n",gc_version >>16, (gc_version >> 8)&0xff,gc_version & 0xff);
    fprintf(Fstack,"OpenXM RFC100, 101, 103 (C) OpenXM developing team. \n");
    fprintf(Fstack,"This software may be freely distributed  ");
    fprintf(Fstack,"with no warranty expressed. \nSee OpenXM/Copyright/Copyright.generic\n");
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
  return(0);
}



