#!/bin/bash
# $OpenXM$ 
source $(OpenXM_HOME)/rc/dot.bashrc
sm1 -q -s "[(parse) (oxs.sm1) pushfile] extension oxNoX [(parse) (cgi.sm1) pushfile] extension cgiPolymake quit "