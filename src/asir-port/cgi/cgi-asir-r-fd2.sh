#!/bin/bash
# $OpenXM: OpenXM/src/asir-port/cgi/cgi-asir-r-fd2.sh,v 1.4 2015/02/25 04:47:50 takayama Exp $
## Replace $OpenXM_HOME by an absolute path of your cite and uncomment
export OpenXM_HOME=/private/taka/OpenXM
export HOME=/tmp
## Note that OpenXM_tmp is set in the dot.bashrc below as HOME/.OpenXM_tmp
source "$OpenXM_HOME/rc/dot.bashrc"
#source /home/taka/OpenXM/rc/dot.bashrc
export TMP=/tmp
export CGI_ASIR_ALLOW="[(version) (asir_contrib_copyright) (a_ahvec) (r_ahvec) (a_expect) (r_expect) (r_ahmat) (a_ahmat) (r_log_ahmat) (a_log_ahmat)]"
export CGI_ASIR_TLIMIT="60"
export OX_ASIR_LOAD_FILE="$OpenXM_HOME/src/asir-port/cgi/r-fd.rr"
export CGI_ASIR_MSG="<pre>
 Example. r_ahvec(-1,[-2,-3],10,[[1,1/2,1/3],[1,1,1]])
 r_ahvec(A,B,C,P),  P is a 2 by m matrix.
 Contingency table:
 -A,     0,    0,   0
 C-1,-B[0],-B[1],-B[2]
 It returns (Z, d21 Z, d22 Z, ..., d2m Z).
 a_expect(A,B,C,P) or r_expect(A,B,C,P) returns
  [E[U_21],E[U_22], ..., E[U_2m]]
 BUG: decimal to rational transformation function might not work properly.
          Then, double-check the R outputs.
</pre><br>"
WFILE=$TMP/tmp2-post-$$.txt
tee /tmp/t2.txt | oxdecode_post >${WFILE}
"$OpenXM_HOME/src/asir-port/cgi/webasir2" --settimer ${CGI_ASIR_TLIMIT} --debug 0 --stdin <${WFILE}
rm -f ${WFILE}
exit 0

