#!/bin/bash
# $OpenXM: OpenXM/src/polymake/cgi/cgi-polymake.sh,v 1.3 2004/11/23 04:34:31 takayama Exp $ 
PATH=/usr/local/polymake/bin:$PATH
export TMP=/tmp
# Set OpenXM_HOME with your absolute path
# Ex. OpenXM_HOME=/home/taka/OpenXM
source $(OpenXM_HOME)/rc/dot.bashrc
WFILE=/tmp/tmp-post-$$.txt
oxdecode_post >${WFILE}
env CONTENT_LENGTH=`wc -m ${WFILE} | awk '{print $1}'` sm1 -q -s "[(parse) (cgi.sm1) pushfile] extension cgiPolymake quit " <${WFILE}
rm -f ${WFILE}
exit 0
: rm -f ${WFILE}
