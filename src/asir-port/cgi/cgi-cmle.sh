#!/bin/bash
# $OpenXM$
## Replace $OpenXM_HOME by an absolute path of your cite.
export OpenXM_HOME=/private/taka/OpenXM
export HOME=/tmp
source $OpenXM_HOME/rc/dot.bashrc
#source /home/taka/OpenXM/rc/dot.bashrc
export TMP=/tmp
export CGI_ASIR_ALLOW="[(version) (asir_contrib_copyright) (gtt_ekn.cmle_3) (gtt_ekn.expectation) (number_eval)]"
export CGI_ASIR_TLIMIT="3600"
export OX_ASIR_LOAD_FILE=$OpenXM_HOME/src/asir-contrib/packages/src/gtt_ekn.rr
export CGI_ASIR_MSG="<pre>
 Example. gtt_ekn.cmle_3([[3,3,1],[9,1,1]])
</pre><br>"
WFILE=/tmp/tmp-post-$$.txt
tee /tmp/t.txt | oxdecode_post >${WFILE}
env CONTENT_LENGTH=`wc -m ${WFILE} | awk '{print $1}'`  sm1 -q -s "[(parse) (cgiasir.sm1) pushfile] extension oxNoX cgiAsir quit " <${WFILE}
rm -f ${WFILE}
exit 0

