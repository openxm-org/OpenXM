#! /bin/sh
# $OpenXM: OpenXM/src/kan96xx/Doc/httpd-rpc.sh,v 1.1 2002/10/24 07:45:03 takayama Exp $
LOGFILE=httpd-rpc-8090.log
export LOGFILE
OXWEB_POST=1
export OXWEB_POST
touch $LOGFILE
tail -f $LOGFILE &
while true ; \
do \
  date >>$LOGFILE ;
  sm1 -s "[(parse) (httpd-rpc.sm1) pushfile] extension ; httpd ; quit " >> $LOGFILE 2>&1 ;
  sleep 3 ; \
done


