#!/bin/bash
# $OpenXM: OpenXM/src/polymake/cgi/cgi-polymake.sh,v 1.2 2004/09/27 08:33:37 takayama Exp $ 
PATH=/usr/local/polymake/bin:$PATH
## Replace $OpenXM_HOME by an absolute path of your cite.
source $(OpenXM_HOME)/rc/dot.bashrc
export TMP=/tmp
sm1 -q -s "[(parse) (ox.sm1) pushfile] extension oxNoX [(parse) (cgi.sm1) pushfile] extension cgiPolymake quit "
