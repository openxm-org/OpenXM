#!/bin/bash
# $Id$
# $OpenXM: OpenXM/src/asir-port/cgi/cgi-asir.sh,v 1.3 2005/02/27 07:08:11 takayama Exp $
## Replace $OpenXM_HOME by an absolute path of your cite.
#source $(OpenXM_HOME)/rc/dot.bashrc
source /home/taka/OpenXM/rc/dot.bashrc
export TMP=/tmp
export CGI_ASIR_ALLOW="[(version) (asir_contrib_copyright) (fctr) (primadec) (nd_gr_main) (dp_gr_main)]"
export CGI_ASIR_TLIMIT="30"
sm1 -q -s "[(parse) (cgiasir.sm1) pushfile] extension oxNoX cgiAsir quit "
