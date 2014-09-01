#!/bin/bash
# r_ahvec(-1,[-3,-5],10,[[1,1/2,1/3],[1,1,1]])
# encode by sm1, (cgi.sm1) run [(cgiStringToUrlEncoding) (string)] extension
# Length is obtained by the wc command.
CONTENT_LENGTH=101 ./cgi-asir-r-fd2.sh <test-r-fd.txt
