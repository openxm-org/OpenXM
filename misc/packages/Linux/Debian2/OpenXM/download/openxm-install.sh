#!/bin/sh
# $OpenXM: OpenXM/misc/packages/Linux/Debian2/OpenXM/download/openxm-install.sh,v 1.3 2010/07/20 08:06:05 takayama Exp $
#Set OpenXM_HOME properly to use this script.  See debian/openxm.postinst
version=`cat $OpenXM_HOME/download/version.txt`
os=`dpkg --print-architecture`
md=`cat $OpenXM_HOME/download/distinfo-openxm-binary.md5`
openxmBinaryName=openxm-binary-$os-$version.tar.gz
DISTDIR=$HOME/OpenXM_tmp

if [ $# = 1 ]; then
if [ $1 = "--force" ]; then
 rm -rf $OpenXM_HOME/bin/ox_sm1
 rm -rf $DISTDIR/$openxmBinaryName
fi
fi

_agree() {
	echo "------------------------------------------------------------------"
	echo "This script install OpenXM binary files under /usr/local/OpenXM"
	echo "from internet."
	echo "Will you go ahead?"
	echo "------------------------------------------------------------------"
	echo "y: yes, n: no"
    if [ -f "/tmp/i-agree-with-asir-license" ]; then
      ans="y" 
    else
  	 read  -p "(y/n/v)" ans
    fi

	if [ $ans = "y" ]; then
		return
	fi
	if [ $ans = "n" ]; then
		echo "Aborting the installation." ; \
		exit 
        fi
	_agree
}

_check_install_error() {
  if [ ! -f $OpenXM_HOME/bin/ox_sm1 ]; then
     echo "Installation of OpenXM binary from the network seems to be failed."; \
     echo "Please check if your computer is connected to the internet. "; \
     sleep 60 ; \
     exit ; \
  fi
}

if [ ! -f $DISTDIR/$openxmBinaryName ]; then
	_agree ; \
       (cd $OpenXM_HOME/download ; make install-big ; make clean-big) ; \
       (cd $OpenXM_HOME/rc ; make install; make clean) ; \
fi

_check_install_error

echo "Installation is completed."



