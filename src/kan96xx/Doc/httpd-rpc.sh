#! /bin/sh
# $OpenXM: OpenXM/src/kan96xx/Doc/httpd-rpc.sh,v 1.5 2002/11/03 12:43:02 takayama Exp $
# OpenXM/rc/dot.bashrc must be read before execution of this script.
# source ${OpenXM_HOME}/rc/dot.bashrc
#
# Output verbose messages  
# OXWEB_DEBUG=1
# export OXWEB_DEBUG
#
# Port number for the httpd service
OXWEB_PORT=8090
export OXWEB_PORT
#
# asirrc is executed when risa/asir starts. Override it if necessary.
# You can also define your own command in this file.
#ASIR_CONFIG=httpd-rpc-asirrc.rr
#ASIR_CONFIG=${OpenXM_HOME}/rc/asirrc
#export ASIR_CONFIG
#
# Set your own httpd-rpc file if necessary.
# OXWEB_HTTPD_RPC=httpd-rpc-${OXWEB_PORT}.sm1  
OXWEB_HTTPD_RPC=httpd-rpc.sm1
export OXWEB_HTTPD_RPC
#
#
#LOGFILE=httpd-rpc-${OXWEB_PORT}.log
LOGFILE=/dev/tty
export LOGFILE
OXWEB_POST=1
export OXWEB_POST
OXWEB_TAKE_LOG=1
export OXWEB_TAKE_LOG
#
touch $LOGFILE
# tail -f $LOGFILE &
#
#
#  Main loop
while true ; \
do \
  date >>$LOGFILE ;
  sm1 -s "[(parse) (${OXWEB_HTTPD_RPC}) pushfile] extension ; httpd ; quit " >> $LOGFILE 2>&1 ;
  sleep 3 ; \
done


