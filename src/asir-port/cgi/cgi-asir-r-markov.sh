#!/bin/bash
# $OpenXM: OpenXM/src/asir-port/cgi/cgi-asir-r-markov.sh,v 1.2 2013/02/18 23:33:13 takayama Exp $
## Replace $OpenXM_HOME by an absolute path of your cite.
source $OpenXM_HOME/rc/dot.bashrc
#source /home/taka/OpenXM/rc/dot.bashrc
export TMP=/tmp
export CGI_ASIR_ALLOW="[(version) (asir_contrib_copyright) (r_markov)]"
export CGI_ASIR_TLIMIT="3600"
export OX_ASIR_LOAD_FILE=$OpenXM_HOME/src/asir-port/cgi/r-markov.rr
export CGI_ASIR_MSG="<pre>
 Example. r_markov([[1,1,1,1],[0,1,2,3]]) 
</pre><br>"
WFILE=/tmp/tmp-post-$$.txt
tee /tmp/t.txt | oxdecode_post >${WFILE}
env CONTENT_LENGTH=`wc -m ${WFILE} | awk '{print $1}'`  sm1 -q -s "[(parse) (cgiasir.sm1) pushfile] extension oxNoX cgiAsir quit " <${WFILE}
rm -f ${WFILE}
exit 0

