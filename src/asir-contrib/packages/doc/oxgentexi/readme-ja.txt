Asir contrib $B$N(B texinfo document $B$r:n@=$9$kJ}K!(B.
$B$"$J$?$N(B asir-contrib $B$N(B folder $BL>$,(B xy_zzz $B$@$H$7$^$9(B.

* texinfo $B$rD>@\=q$/>l9g(B.   *** $BJ8;z%3!<%I$OI,$:(B utf8 $B$r;H$C$F2<$5$$(B ***

   mkdir xy_zzz
   xy_zzz $B$K(B texinfo $B%U%!%$%k(B xy_zzz-ja.texi  ($BF|K\8l(B) $B$*$h$S(B
                              xy_zzz-en.texi  ($B1Q8l(B) $B$rMQ0U$7$^$9(B.
   $B$I$A$i$+JRJ}$G$b$+$^$$$^$;$s(B.
   xy_zzz/Makefile $B$O$3$A$i$G=`Hw$7$^$9(B.

   $BNc(B:  todo_parametrize $B$K$O(B todo_parametrize-ja.texi $B$,$"$j$^$9(B.
     $B1Q8l%^%K%e%"%k$O$J$$$N$G(B, $BL5$$$H@k8@$7$F$k%U%!%$%k$r(B Makefile
     $B$G<+F0:n@=$7$F$$$^$9(B.

* oxgentexi $B$rMxMQ$7$FA4(Bdocument$B$r<+F0@8@.$9$k>l9g(B.

    mkdir xy_zzz
    xy_zzz $B$K(B oxgentexi $B$X$NF~NO%U%!%$%k(B xy_zzz.oxg $B$rMQ0U$9$k(B.
    oxgentexi $B$NF~NO%U%!%$%k$OI,$:3HD%;R(B .oxg $B$rIU$1$k(B.

    ---------   xy_zzz.oxg $B$NNc(B  -------------  
    /*&usage-ja
     begin:  myadd(x,y)
       {x} $B$H(B {y} $B$NOB$r5a$a$k(B.
       description:
         $B$3$3$K$O(B texinfo $B$NL?Na$b$+$1$k(B.
       example:
          myadd(1,3)
       example:
          myadd(x+1,y)
     end:
    */


    $B%j%9%H(B GT1 $B$N$h$&$J(B Makefile $B$rMQ0U$9$k(B.
    make $B$G(B xy_zzz-ja.texi $B$,<+F0@8@.$5$l$k(B.
    xy_zzz-en.texi $B$b<+F0@8@.$5$l$k(B. $BCf?H$O(B $B1Q8l$N%^%K%e%"%k$,$J$$$h(B,
   $B$H$$$C$F$k$@$1(B.

    make gen-html-ja  ($BF|K\8l(B html  $B%I%-%e%a%s%H$N@8@.(B)

----------------------------------------------------------
$B%j%9%H(B GT1
#$OpenXM: OpenXM/src/asir-contrib/packages/doc/oxgentexi/readme-ja.txt,v 1.1 2005/04/14 05:10:58 takayama Exp $
# Name of this document.
MYNAME=xy_zzz

SRC = 

ENSRCS = ${SRC:.oxw=.en} 
JASRCS = ${SRC:.oxw=.ja} 
ENAUTO= 
JAAUTO=${ENAUTO:.en=.ja}
GENTEXI_OPT=--noSorting --author "OpenXM.org" --infoName asir-contrib-${MYNAME}
include ../Makefile.common

clean_private: 
	${RM} -f ${MYNAME}-ja.texi ${MYNAME}-en.texi

${MYNAME}-ja.texi: ${MYNAME}.oxg
	@echo "xy_zzz =?ISO-2022-JP?B?GyRCJV4lSyVlJSIlaxsoQg==?= " >title.tmp
# generated by nkf -M
	${NKF} -j ${MYNAME}.oxg | ${OXWEAVE} usage-ja | ${OXGENTEXI} ${GENTEXI_OPT} --title "`nkf -w title.tmp`" >${MYNAME}-ja.texi
	rm -f title.tmp

${MYNAME}-en.texi: 
	@echo "begin: plain|" >auto.en
	@echo "English manual has not yet been written." >>auto.en
	@echo "end:" >>auto.en
	${OXGENTEXI} ${GENTEXI_OPT} --title "xy_zzz Manual" <auto.en >${MYNAME}-en.texi
-------------------------------------------------------------------------
