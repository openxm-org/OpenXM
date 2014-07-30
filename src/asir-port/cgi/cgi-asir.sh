#!/bin/bash
# $Id$
# $OpenXM: OpenXM/src/asir-port/cgi/cgi-asir.sh,v 1.5 2013/09/26 04:13:40 takayama Exp $
## Replace $OpenXM_HOME by an absolute path of your cite.
export OpenXM_HOME=/private/taka/OpenXM
source $OpenXM_HOME/rc/dot.bashrc
#source /home/taka/OpenXM/rc/dot.bashrc
export TMP=/tmp
export CGI_ASIR_ALLOW="[(version) (asir_contrib_copyright) (fctr) (primadec) (nd_gr_main) (dp_gr_main)]"
export CGI_ASIR_TLIMIT="30"
WFILE=/tmp/tmp-post-$$.txt
tee /tmp/t.txt | oxdecode_post >${WFILE}
env CONTENT_LENGTH=`wc -m ${WFILE} | awk '{print $1}'`  sm1 -q -s "[(parse) (cgiasir.sm1) pushfile] extension oxNoX cgiAsir quit " <${WFILE}
rm -f ${WFILE}
exit 0

