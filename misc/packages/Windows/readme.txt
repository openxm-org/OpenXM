$OpenXM: OpenXM/misc/packages/Windows/readme.txt,v 1.1 2002/03/16 01:26:07 takayama Exp $

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


