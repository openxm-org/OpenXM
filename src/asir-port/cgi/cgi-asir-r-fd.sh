#!/bin/bash
# $OpenXM: OpenXM/src/asir-port/cgi/cgi-asir-r-fd.sh,v 1.1 2014/07/27 03:18:44 takayama Exp $
## Replace $OpenXM_HOME by an absolute path of your cite.
source $OpenXM_HOME/rc/dot.bashrc
#source /home/taka/OpenXM/rc/dot.bashrc
export TMP=/tmp
export CGI_ASIR_ALLOW="[(version) (asir_contrib_copyright) (r_ahvec)]"
export CGI_ASIR_TLIMIT="3600"
export OX_ASIR_LOAD_FILE=$OpenXM_HOME/src/asir-port/cgi/r-fd.rr
export CGI_ASIR_MSG="<pre>
 Example. r_ahvec(-1,[-2,-3],10,[[1,1/2,1/3],[1,1,1]])
 r_ahvec(A,B,C,P),  P is a 2 by m matrix.
 Contingency table:
 -A,     0,    0,   0
 C-1,-B[0],-B[1],-B[2]
 It returns (Z, d21 Z, d22 Z, ..., d2m Z).
</pre><br>"
WFILE=/tmp/tmp-post-$$.txt
tee /tmp/t.txt | oxdecode_post >${WFILE}
env CONTENT_LENGTH=`wc -m ${WFILE} | awk '{print $1}'`  sm1 -q -s "[(parse) (cgiasir.sm1) pushfile] extension oxNoX cgiAsir quit " <${WFILE}
rm -f ${WFILE}
exit 0

