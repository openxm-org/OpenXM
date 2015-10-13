$OpenXM: OpenXM/misc/packages/Windows/readme.txt,v 1.6 2005/10/15 06:25:30 takayama Exp $


THIS IS A NOTE FOR DEVELOPPERS.

In order to build asir on cygwin, set the following path.
    export PATH=/usr/X11R6/bin:$PATH

Set OpenXM_HOME for oxpp.

For Java (in case of jdk1.4)
export PATH=/cygdrive/c/j2sdk1.4.2_08/bin:$PATH
export CLASSPATH='.;c:\j2sdk1.4.2_08\lib\tools.jar'
export JAVA_HOME='c:\j2sdk1.4.2_08'
   

<1> Type in 
      make clean
      make all
     [ make remove-experimental ]
   in this directory OpenXM/misc/packages/Windows
   Copy OpenXM-win to the root of your CD.


   NOTE: make ASIR_LANG=en  all (generating English version of CD)
         asirbinary for windows will be downloaded from ftp.math.kobe-u.ac.jp
<2>
   make build-tgz
   make binary-tgz
   You will get OpenXM-win.tgz

<3>  pub/OpenXM/Head/OpenXM-win-en.tgz
  make clean-work   ## To remove windows binary at the work area.
  make clean
  make ASIR_LANG=en all ; make ASIR_LANG=en copy-to-ftp-area

<3'>  pub/OpenXM/Head/OpenXM-win-ja.tgz
  # make clean-work  ## To remove windows binary at the work area.
  make clean
  make all ; make copy-to-ftp-area

<3''>  pub/OpenXM/asir-book/asir-book-ja.tgz
  ## make clean-work  ## To remove windows binary at the work area.
  ## make clean
  make all
  make copy-asir-book-to-ftp-area


2015.10.13
  To generate ox_pari for asirgui, run
  make clean ; make oxpari-gen-zip
  cd OpenXM-win\bin
  Set OpenXM_HOME.
    Example: set OpenXM_HOME=c:\cygwin64\home\nobuki\OX4\OpenXM
  Test:
  sm1
  (ox.sm1) run ; sm1connectr
  sm1
  (oxpari.sm1) run ;  pariconnectr
