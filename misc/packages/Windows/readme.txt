$OpenXM$

<1> Put the binary for windows at c:\home\asir\bin
and  c:\home\asir\lib

<2> Build asir and sm1 on cygwin.
    export PATH=/usr/X11R6/bin:$PATH
    cd ~/OpenXM/src
    make install-kan-ox 

<3> Type in 
      make clean
      make build-tgz
      make binary-tgz
   in this directory OpenXM/misc/packages/Windows
   You will get OpenXM-win.tgz
   Copy OpenXM-win to the root of your CD.


