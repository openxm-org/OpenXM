#! /bin/sh
# $OpenXM$
LOGFILE=httpd-rpc-8090.log
export LOGFILE
OXWEB_POST=1
export OXWEB_POST
touch $LOGFILE
tail -f $LOGFILE &
while true ; \
do \
  echo "date" >>$LOGFILE ;
  sm1 -s "[(parse) (httpd-rpc.sm1) pushfile] extension ; httpd ; quit " >> $LOGFILE 2>&1 ;
  sleep 3 ; \
done


