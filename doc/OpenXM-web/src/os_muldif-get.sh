#!/bin/sh
# $OpenXM: OpenXM/doc/OpenXM-web/src/os_muldif-get.sh,v 1.1 2017/03/31 01:03:51 takayama Exp $
OpenXM_HOME=/home/taka/OX4/OX64/OpenXM
#OpenXM_HOME=/home/nobuki/OX4/OpenXM
OX_MULDIF=${OpenXM_HOME}/src/asir-contrib/packages/src/os_muldif.rr
mkdir -p muldif
if true ; then
#if false ; then
(cd muldif ; curl ftp://akagi.ms.u-tokyo.ac.jp/pub/math/muldif/os_muldif.rr >os_muldif.rr)
(cd muldif ; curl ftp://akagi.ms.u-tokyo.ac.jp/pub/math/muldif/os_muldif.pdf >os_muldif.pdf)
(cd muldif ; curl ftp://akagi.ms.u-tokyo.ac.jp/pub/math/muldif/3DGraph.pdf >3DGraph.pdf)
(cd muldif ; curl ftp://akagi.ms.u-tokyo.ac.jp/pub/math/muldif/integrate.pdf >integrate.pdf)
(cd muldif ; curl ftp://akagi.ms.u-tokyo.ac.jp/pub/math/muldif/conj34f.pdf >conj34f.pdf)
(cd muldif ; curl ftp://akagi.ms.u-tokyo.ac.jp/pub/math/muldif/conj34.pdf >conj34.pdf)
(cd muldif ; curl ftp://akagi.ms.u-tokyo.ac.jp/pub/math/muldif/mtoupper.pdf >mtoupper.pdf)
(cd muldif ; curl ftp://akagi.ms.u-tokyo.ac.jp/pub/math/muldif/mtoupper34.pdf >mtoupper34.pdf)
(cd muldif ; curl ftp://akagi.ms.u-tokyo.ac.jp/pub/math/muldif/mtoupper44.pdf >mtoupper44.pdf)
(cd muldif ; curl ftp://akagi.ms.u-tokyo.ac.jp/pub/math/muldif/mtoupper45.pdf >mtoupper45.pdf)
fi
head -1 ${OX_MULDIF} >muldif/tmp-os_muldif.rr
cat muldif/os_muldif.rr >>muldif/tmp-os_muldif.rr
if diff -B -b -q muldif/tmp-os_muldif.rr ${OX_MULDIF} >/dev/null ; then
  echo "there is no change in os_muldif.rr"
else
  mail -s 'os_muldif_is_updated_check_ox_current_doc_other_docs' takayama@math.kobe-u.ac.jp </dev/null
fi
if true ; then
(cd muldif ; curl ftp://akagi.ms.u-tokyo.ac.jp/pub/math/muldif/changelog.txt >changelog.txt)
fi




 
