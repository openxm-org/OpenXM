/*$OpenXM$*/
/* cc -I/usr/X11R6/include -L/usr/X11R6/lib xt.c -lXt -lX11 */
/* a.out -geometry 300x200 */
#include <X11/StringDefs.h>
#include <X11/Intrinsic.h>

main(int argc,char **argv) {
  Widget toplevel ;

  toplevel = XtInitialize(argv[0],"Pixel",NULL,0,&argc,argv);

  XtRealizeWidget(toplevel);
  XtMainLoop();
}

  
