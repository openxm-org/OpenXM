#!/bin/bash
# $OpenXM: OpenXM/src/polymake/cgi/cgi-polymake.sh,v 1.1 2004/09/27 01:20:46 takayama Exp $ 
PATH=/usr/local/polymake/bin:$PATH
## Replace $OpenXM_HOME by an absolute path of your cite.
source $(OpenXM_HOME)/rc/dot.bashrc
export TMP=/tmp
sm1 -q -s "[(ox.sm1) pushfile] extension oxNoX [(parse) (cgi.sm1) pushfile] extension cgiPolymake quit "
