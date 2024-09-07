# OpenXM library mode interface

This directory contains samples on OpenXM/library mode interface.
For example, you can factor a given multivariable polynomial
over Q or algebraic numbers by linking OpenXM/lib/libasir.a or libasir_X.a
Details on the OpenXM/library mode interface is explained in
Appendix A, OpenXM/doc/asir2000/man-en.dvi (in English)
Appendix A, OpenXM/doc/asir2000/man-ja.dvi (in Japanese)
and
OpenXM/doc/OpenXM-specs/OpenXM-eg.dvi (in English) or
OpenXM/doc/OpenXM-specs/OpenXM-jp.dvi (in Japanese).

## Build
Do a full build of OpenXM before doing below.
```
cd OpenXM/src/asir2018
make clean
make NOX=y
make install    
# libasir.a is installed. It does not contain X11 calls. asir without X11 is installed.

make clean
make
make install  
# Undo the installation of asir without X11

cd ../../doc/oxlib
./configure
make
# It builds test and test3 
```
See the Makefile to build other tests.

## A sample session with test:
```
$ openxm ./test
Input> fctr(x^10-1);
Output> [[1,1],[x-1,1],[x+1,1],[x^4-x^3+x^2-x+1,1],[x^4+x^3+x^2+x+1,1]]
Input> 2^255-19;
Output> 57896044618658097711785492504343953926634992332820282019728792003956564819949
Input> bye
```

## A sample session with test3:
```
  % openxm ./test3
  Input>asir 123;     <- push 123
  Input>pop          <- pop and print
  Output>
  00 00 00 14 00 00 00 01 00 00 00 7b
  Input>push 00 00 00 14 00 00 00 01 00 00 00 0c  <- push 12
  Input>push 00 00 00 14 00 00 00 01 00 00 00 08  <- push 8
  Input>push 00 00 00 02 00 00 00 02              <- push int32 2
  Input>asir "igcd";   <- push a string (function name)
  Input>cmd 269       <- executeFunction
  Input>pop
  Output>
  00 00 00 14 00 00 00 01 00 00 00 04     <- 4 = gcd(12,8)
  Input>asir quit;                        <- execute asir 'quit' command
  %
```
 
$OpenXM: OpenXM/doc/oxlib/README,v 1.3 2009/02/22 17:30:03 ohara Exp $

Memo: Use retext (Ubuntsu) for preview.
