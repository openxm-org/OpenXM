$OpenXM: OpenXM/src/util/readme-ja.txt,v 1.1 2005/04/05 13:12:06 takayama Exp $

$B;H$$$+$?$N4JC1$J2r@b$r$H$j$"$($:$3$N%U%!%$%k$K=q$/(B.

oxgentexi:   $B4X?t$N;EMM$+$i(B texi $B%U%!%$%k$r@8@.$9$k%D!<%k(B.
             asir-contrib $B$N%^%K%e%"%k:n@.$K;H$o$l$F$$$k(B.
oxreplace:   $BJ8;zNs$NCV$-49$($r9T$J$&%D!<%k(B.
             latex2html $B$G:n$i$l$?%U%!%$%k$K:Y9)$9$k$N$K;H$o$l$F$$$k(B.
oxweave:     $B0l$D$N%U%!%$%k$K1Q8l$N%I%-%e%a%s%H$dF|K\8l$N%I%-%e%a%s%H=q$$$F(B
            $B$*$$$F$=$l$r(B oxweave $B$GJ,2r$9$k(B. oxgentexi $B$HAH$G$b;H$&(B.
            specification $B$d(B asir-contrib $B$N%^%K%e%"%k$G;H$C$F$k(B.

[oxgentexi]
  oxgentexi $B$X$NF~NO$O(B item $B$H$$$&C10L$G4IM}$5$l$F$$$k(B.
  $B0l$D$N(B item $B$O(B $B%-!<%o!<%I(B begin:  $B$G$O$8$^$j(B,
  end: $B$G$*$o$k(B.
  begin: ~ end: $B$N30$N%G!<%?$O$9$Y$FFI$_Ht$P$5$l$k(B.
  : ($B%3%m%s(B) $B$G$*$o$kC18l$O(B oxgentexi $B$G$O%-!<%o!<%I$H$J$k(B.
  $B%3%m%s$rF~NO$9$k$K$O(B @colon $B$rMQ$$$k(B.

  $B%3%m%s$G$*$o$k%-!<%o!<%I$N<!$K$O$+$J$i$:6uGr$+2~9T$r$$$l$k$3$H(B.

  item $B$O8=:_$N$H$3$m(B 
    (1) asir$B4X?t(Bitem 
  $B$H(B 
    (2) verbatim item 
  $B$N(B2$B<oN`$,$"$k(B.

  verbatim item $B$O<!$N$h$&$K=q$/(B.
  begin:  key|
    texinfo $B7A<0$NF~NO(B.
  end:

  key $B$O1Q?t;z$NJB$S$G$"$k(B. key $B$K$D$$$F$O8e=R(B.
  | $B$N$"$H$K(B texinfo $B7A<0$NF~NO$r=q$/$H$=$l$,$=$N$^$^=PNO$5$l$k(B.

  asir$B4X?t(Bitem $B$O<!$N$h$&$K=q$/(B.
  begin:  fname(arg1,arg2,...)
     short description of the function fname.
  description:
     explain about fname. It accepts texinfo inputs.
  example:
     X=fname(2,3);
     Y=fctr(X);
  exampleDescription:
    explain about the example.
  ref:
    xxx,yyy
  end:
  
  example: $B$NCf$G$O(B { } $B$d(B @ $B$O(B texinfo $B$NFC<l5-9f$H$7$F$O07$o$l$J$$(B.  

  item $B$O(B key $B$K$h$j(B $B%=!<%H$5$l$F$+$i=PNO$5$l$k(B.
  asir$B4X?t(Bitem $B$N>l9g$O(B fname $B$,(B key $B$H$J$k(B.
  key $B$rJQ99$9$k$K$O(B sortKey: $B%-!<%o!<%I$G;XDj$9$k(B.
  sortKey: $B%-!<%o!<%I$O(B short description $B0J9_$K=q$+$J$$$H$$$1$J$$(B.

  ref: $B$N8e$m$K$O;2>H$9$Y$-4X?tL>(B (asir$B4X?t(Bitem $B$NL>A0(B)$B$r(B $B%3%s%^$G6h@Z$C$F(B
  $BF~NO$9$k(B.

  short description $B0J9_$K;H$($k$=$NB>$N%-!<%o!<%I(B.
  algorithm:  $B%"%k%4%j%:%`$N2r@b$r=q$/(B.
  author:     $B%W%m%0%i%`$N:n<T$r=q$/(B.
  

[$B%*%W%7%g%s(B|oxgentexi]

  --noSorting    key $B$G%=!<%H$7$J$$(B.
  --title        texinfo $B%U%!%$%k$N(B title $B$r;XDj$9$k(B. $B$3$l$N;XDj$,$"$k$H(B,
                 $BI=Bj$b=PNO$7$F40A4$J(B texinfo $B%U%!%$%k$r=PNO$9$k(B.
  --author       $BI=Bj$K$"$i$o$l$kCx<TL>(B.
  --infoFileName  info $B$N%U%!%$%kL>(B.
  --upnode
  --category

[$BNc(B1|oxgentexi]
  $B0J2<$NNc$O(B
  nkf -e readme-ja.txt |oxweave ja| oxgentexi --title 'Test of oxgentexi' >test.texi
  ptex test.texi
  $B$H=hM}(B.
/*&ja

  begin:  igcd(F,G)
    igcd $B$O@0?t(B {F} $B$H(B {G} $B$N:GBg8xLs?t$r7W;;$9$k(B.
  example:
    igcd(5,3);
  example_description:
    5 $B$H(B 3 $B$N(B gcd $B$N7W;;(B. $BEz$($O(B 1.
  example:
    igcd(10,15);
  example_description:
    10 $B$H(B 15 $B$N(B gcd $B$N7W;;(B. $BEz$($O(B 5.

  end:

  begin:  fctr(F)
    fctr $B$OB?9`<0(B{F} $B$r0x?tJ,2r$9$k(B.
  description:
   @table @b
   @item $B%"%k%4%j%:%`(B
    $B0x?tJ,2r$K$O(B Berlkamp $B$N%"%k%4%j%:%`$rMQ$$$k(B.
   @item $BCm0U(B
    $B%J%C%W%5%C%/%"%k%4%j%:%`$O$^$@<BAu$7$F$J$$(B.
   @end table
  example:
   fctr(x^10-1);

  end:

*/

[$BNc(B2|oxgentexi]
  $B0J2<$NNc$O(B
  nkf -e readme-ja.txt |oxweave ja2| oxgentexi --noSorting --title 'Test 2 of oxgentexi' >test2.texi

  ptex test2.texi
  $B$H=hM}(B.
/*&ja2
  begin: start|
  @section $B4X?t$N>R2p(B.

   $B$3$N@a$G$O=iJbE*$J4X?t$r>R2p$9$k(B.

  end:

  begin:  igcd(F,G)
    igcd $B$O@0?t(B {F} $B$H(B {G} $B$N:GBg8xLs?t$r7W;;$9$k(B.
  example:
    igcd(5,3);
  example_description:
    5 $B$H(B 3 $B$N(B gcd $B$N7W;;(B. $BEz$($O(B 1.
  example:
    igcd(10,15);
  example_description:
    10 $B$H(B 15 $B$N(B gcd $B$N7W;;(B. $BEz$($O(B 5.

  end:

  begin:  fctr(F)
    fctr $B$OB?9`<0(B{F} $B$r0x?tJ,2r$9$k(B.
  description:
   @table @b
   @item $B%"%k%4%j%:%`(B
    $B0x?tJ,2r$K$O(B Berlkamp $B$N%"%k%4%j%:%`$rMQ$$$k(B.
   @item $BCm0U(B
    $B%J%C%W%5%C%/%"%k%4%j%:%`$O$^$@<BAu$7$F$J$$(B.
   @end table
  example:
   fctr(x^10-1);

  end:

  begin: owari|

  @section $B=*$j$N>O(B
  $B$3$l$G$*$7$^$$(B.

  end:
*/


[oxweave]
 
oxweave [--plain] [--source] [--recursive] [key1 key2 ...]
oxweave reads bytes from the standarndard input stream 
and outputs them if one of the following conditions
is satisfied:
(1) hogehoge is output if hogehoge is enclosed 
    like /*&xyz  hogehoge */ and xyz matches with one of the keys
(2) hogehoge is output if hogehoge comes after //&xyz 
    followed with the newline and xyz matches with one of the keys
(3) If the option --plain is given, all texts out of /*&xyz */ and //&xyz 
    are output.
(4) If the option --recursive is given, the symbol */ is treated recursively.
(5) --debug is used to find unmatched /* and */
Example 1: oxweave --source ja <oxweave.c >t.tex

Example 2: oxweave  ja <oxweave.c >t.tex
  ja $B$,(B $B%-!<$J$N$G(B  /*&ja  $B$H(B */ $B$G0O$^$l$?ItJ,$,<h$j=P$5$l$k(B.

Example 3: oxweave --plain  <oxweave.c >t.tex

[oxreplace]

oxreplace $B$G$O%U%!%$%k(B file1 file2 ... $B$,(B overwrite $B$5$l$k(B. 

oxreplace [--old oword --new nword --f rule_file_name] 
          [file1 file2 ... ] 
    Use --oldx or --newx to give a word in hexadeciam codes

