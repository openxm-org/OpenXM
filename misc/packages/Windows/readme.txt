$OpenXM: OpenXM/misc/packages/Windows/readme.txt,v 1.2 2002/07/28 08:10:24 takayama Exp $


THIS IS A NOTE FOR DEVELOPPERS.

In order to build asir on cygwin, set the following path.
    export PATH=/usr/X11R6/bin:$PATH

<1> Type in 
      make clean
      make all
     [ make remove-experimental ]
   in this directory OpenXM/misc/packages/Windows
   Copy OpenXM-win to the root of your CD.


   NOTE: make ASIR_LANG=en    (generating English version of CD)
         asirbinary for windows will be downloaded from ftp.math.kobe-u.ac.jp
<2>
   make build-tgz
   make binary-tgz
   You will get OpenXM-win.tgz


