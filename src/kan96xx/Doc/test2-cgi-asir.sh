#!/usr/local/bin/bash
#
# test by openxm ./test2-cgi-asir.sh
#Ref: OpenXM/src/asir-port/cgi, misc-2025/A1/barcode-web
#
source $OpenXM_HOME/rc/dot.bashrc
export TMP=/tmp
export CGI_ASIR_ALLOW="[(version) (asir_contrib_copyright) (fctr) (primadec) (nd_gr_main) (dp_gr_main)]"
export CGI_ASIR_TLIMIT="30"
#
# tries
# 
WFILE=/tmp/tmp-post-$$.txt
cat test2.txt >${WFILE}
env CONTENT_LENGTH=`wc -m ${WFILE} | awk '{print $1}'`  sm1 -q -s "[(parse) (cgiasir.sm1) pushfile] extension oxNoX (http) cgiasir_setmode cgiAsir quit " <${WFILE}
#rm -f ${WFILE}
exit 0

## Try 1.
WFILE=test2.txt
env CONTENT_LENGTH=`wc -m ${WFILE} | awk '{print $1}'`  sm1 -q -s "[(parse) (cgiasir.sm1) pushfile] extension oxNoX cgi.test2 quit "

## Try2
WFILE=/tmp/tmp-post-$$.txt
## tee をつかってはだめ.
cat test2.txt | oxdecode_post >${WFILE}
env CONTENT_LENGTH=`wc -m ${WFILE} | awk '{print $1}'`  sm1 -q -s "[(parse) (cgiasir.sm1) pushfile] extension oxNoX (http) cgiasir_setmode cgiAsir quit " <${WFILE}
#rm -f ${WFILE}

