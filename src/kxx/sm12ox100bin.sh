#!/bin/sh
case $# in
0)   echo 'Usage: sm12ox100bin.sh output-file-name-for-ox100-bin' 1>&2 ; \
     echo 'Example of the intput:' 1>&2 ; \
     echo '   oxsm1.ccc (hello) oxpushcmo ; ' 1>&2 ; \
     echo '   oxsm1.ccc oxpopcmo ; ' 1>&2 ; \
     echo '   ((x-1)^2). [(class) (recursivePolynomial)] dc /ff set ' 1>&2 ; \
     echo '   oxsm1.ccc ff oxpushcmo ; ' 1>&2 ; exit 1
esac
tmp_prog=.prog.ox100bin.$$
tmp_in=.in.ox100bin.$$
tmp_out=$1
rm -f ${tmp_prog}
echo " [(parse) (ox.sm1) pushfile] extension  sm1connectr " >${tmp_prog}
echo " (${tmp_in}) (w) file /ii.tmp set " >>${tmp_prog}
echo " (${tmp_out}) (w) file /oo.tmp set " >>${tmp_prog}
echo "  [(oxLog) oxsm1.ccc ii.tmp oo.tmp] extension [(oxWatch) oxsm1.ccc ] extension " >>${tmp_prog}
cat >>${tmp_prog}
echo "[(oxLogStop) oxsm1.ccc ] extension " >>${tmp_prog}
echo "quit" >>${tmp_prog}
sm1 <${tmp_prog}
rm -f ${tmp_prog} ${tmp_in}
