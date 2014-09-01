#!/bin/bash
# $OpenXM: OpenXM/src/asir-port/cgi/cgi-asir-r-fd2.sh,v 1.1 2014/08/31 07:53:57 takayama Exp $
## Replace $OpenXM_HOME by an absolute path of your cite and uncomment
#export OpenXM_HOME=/private/taka/OpenXM
export HOME=/tmp
## Note that OpenXM_tmp is set in the dot.bashrc below as HOME/.OpenXM_tmp
source $OpenXM_HOME/rc/dot.bashrc
#source /home/taka/OpenXM/rc/dot.bashrc
export TMP=/tmp
export CGI_ASIR_ALLOW="[(version) (asir_contrib_copyright) (r_ahvec)]"
export CGI_ASIR_TLIMIT="60"
export OX_ASIR_LOAD_FILE=$OpenXM_HOME/src/asir-port/cgi/r-fd.rr
WFILE=$TMP/tmp2-post-$$.txt
tee /tmp/t2.txt | oxdecode_post >${WFILE}
$OpenXM_HOME/src/asir-port/cgi/webasir2 --settimer ${CGI_ASIR_TLIMIT} --debug 0 --stdin <${WFILE}
rm -f ${WFILE}
exit 0

