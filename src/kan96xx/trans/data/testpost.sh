WFILE=/tmp/tmp-post-$$.txt
env CONTENT_LENGTH=174 ../oxdecode_post <post1.txt >${WFILE}
env CONTENT_LENGTH=`wc -m ${WFILE} | awk '{print $1}'` sm1 -q -s "[(parse) (cgi.sm1) pushfile] extension cgiPolymake quit " <${WFILE}
exit 0
: rm ${WFILE}
: t.t : oxMessageBody=FACETS++POINTS%0A1+0+0+%0A1+1+0+%0A1+0+1%0A
: cf. src/polymake/cgi/cgi-polymake.sh