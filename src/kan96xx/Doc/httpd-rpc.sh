#! /bin/sh
# $OpenXM: OpenXM/src/kan96xx/Doc/httpd-rpc.sh,v 1.3 2002/10/29 10:55:19 takayama Exp $
LOGFILE=httpd-rpc-8090.log
export LOGFILE
OXWEB_POST=1
export OXWEB_POST
OXWEB_TAKE_LOG=1
export OXWEB_TAKE_LOG
# OXWEB_DEBUG=1
# export OXWEB_DEBUG
touch $LOGFILE
# tail -f $LOGFILE &
while true ; \
do \
  date >>$LOGFILE ;
  sm1 -s "[(parse) (httpd-rpc.sm1) pushfile] extension ; httpd ; quit " >> $LOGFILE 2>&1 ;
  sleep 3 ; \
done


