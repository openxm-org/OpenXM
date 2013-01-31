#!/bin/bash
# $OpenXM$
## Replace $OpenXM_HOME by an absolute path of your cite.
source $OpenXM_HOME/rc/dot.bashrc
#source /home/taka/OpenXM/rc/dot.bashrc
export TMP=/tmp
export CGI_ASIR_ALLOW="[(version) (asir_contrib_copyright) (r_markov)]"
export CGI_ASIR_TLIMIT="3600"
export OX_ASIR_LOAD_FILE=$OpenXM_HOME/src/asir-port/cgi/r-markov.rr
sm1 -q -s "[(parse) (cgiasir.sm1) pushfile] extension oxNoX cgiAsir quit "

