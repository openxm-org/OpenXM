#!/bin/bash
# $OpenXM: OpenXM/src/asir-port/cgi/cgi-cmle.sh,v 1.1 2018/03/19 06:14:47 takayama Exp $
## Replace $OpenXM_HOME by an absolute path of your cite.
export OpenXM_HOME=/private/taka/OpenXM
export HOME=/tmp
source $OpenXM_HOME/rc/dot.bashrc
#source /home/taka/OpenXM/rc/dot.bashrc
export TMP=/tmp
export CGI_ASIR_ALLOW="[(version) (asir_contrib_copyright) (gtt_ekn.cmle_3) (gtt_ekn.expectation) (number_eval) (gtt_ekn.nc)]"
export CGI_ASIR_TLIMIT="3600"
export OX_ASIR_LOAD_FILE=$OpenXM_HOME/src/asir-contrib/packages/src/gtt_ekn.rr
export CGI_ASIR_MSG="<pre>
 Example. gtt_ekn.cmle_3([[3,3,1],[9,1,1]])
          gtt_ekn.nc([[7,11],[12,4,2]],[[1,1/2,1/3],[1,1,1]])
          gtt_ekn.expectation([[7,11],[12,4,2]],[[1,1/2,1/3],[1,1,1]])
</pre><br>  <a href=\"http://www.math.kobe-u.ac.jp/OpenXM/Current/doc/asir-contrib/ja/gtt_ekn-html/gtt_ekn-ja.html\" target=\"_blank\"> manual (ja)</a><br>"
WFILE=/tmp/tmp-post-$$.txt
tee /tmp/t.txt | oxdecode_post >${WFILE}
env CONTENT_LENGTH=`wc -m ${WFILE} | awk '{print $1}'`  sm1 -q -s "[(parse) (cgiasir.sm1) pushfile] extension oxNoX cgiAsir quit " <${WFILE}
rm -f ${WFILE}
exit 0

