#!/bin/bash
# $OpenXM: OpenXM/src/asir-port/asir-install.sh,v 1.18 2013/02/15 05:06:21 takayama Exp $
# Downloading Risa/Asir for FLL-free systems
# under $HOME/.asir-tmp and installs asir to $OpenXM_HOME/bin
#
# In the Debian package, /usr/local/OpenXM/bin/asir is the symbolic link to
# /home/knoppix/.asir-tmp/asir.
# So, on non-knoppix system, execute
# (export OpenXM_HOME=/usr/local/OpenXM; openxm asir-install.sh)
# as the root in the bash to install the asir under /usr/local/OpenXM/bin.
#  /root/.asir-tmp will be a work-directory in this case.
#
version=`cat $OpenXM_HOME/lib/version.txt`
os=`dpkg --print-architecture`
md=`cat $OpenXM_HOME/lib/asir/distinfo-asir.md5`
libmd=`cat $OpenXM_HOME/lib/asir/distinfo-asirlib.md5`
# For testing
#asir="http://www.math.kobe-u.ac.jp/pub/asir/gzip.exe"
#asirname="gzip.exe"
#asirlib="http://www.math.kobe-u.ac.jp/pub/asir/tar.exe"
#asirlibname="tar.exe"
# 
asir="https://www.math.kobe-u.ac.jp/pub/OpenXM/head/knoppix/asir-$os-$version.gz"
asirnamegunzip="asir-$os-$version"
asirname="asir-$os-$version.gz"
asirlib="https://www.math.kobe-u.ac.jp/pub/OpenXM/head/knoppix/asirlib-$os-$version.tar.gz"
asirlibname="asirlib-$os-$version.tar.gz"
ot="https://www.math.kobe-u.ac.jp/pub/OpenXM/head/knoppix/ox-texmacs-$os-$version.tar.gz"
otname="ox-texmacs-$os-$version.tar.gz"

ASIR_LIB_FILES="alph bfct bgk const cyclic defs.h dmul fctrdata fctrtest \
               fff gr ifplot katsura mat nf num primdec \
               ratint robot solve sp sturm xm"


if [ $# = 1 ]; then
if [ $1 = "--force" ]; then
 rm -rf $HOME/.asir-tmp
fi
fi

_agree() {
	echo "------------------------------------------------------------------"
	echo "This script install Risa/Asir under $OpenXM_HOME"
	echo "Risa/Asir is distributed with no warranty for non-commercial use."
	echo "OpenXM subcomponents are distributed with no warranty under BSD license or GPL."
	echo "Do you agree with the licenses under $OpenXM_HOME/Copyright?"
	echo "------------------------------------------------------------------"
	echo "y: agree, n: do not agree, v: read the detail of the asir license."
    if [ -f "/tmp/i-agree-with-asir-license" ]; then
      ans="y" 
    else
  	 read  -p "(y/n/v)" ans
    fi
	if [ $ans = "y" ]; then
		return
	fi
	if [ $ans = "n" ]; then
		echo "Aborting the installation of asir." ; \
		exit 
    fi
	if [ $ans = "v" ]; then
		more $OpenXM_HOME/Copyright/Copyright.asir ; \
		_agree ; \
		return 
	fi
	_agree
}

_check_install_error() {
  if [ ! -f $HOME/.asir-tmp/$asirnamegunzip ]; then
     echo "Installation of Risa/Asir from the network seems to be failed."; \
     echo "Please check if your computer is connected to the internet "; \
     echo "  and you can download files from www.math.kobe-u.ac.jp  "; \
     echo "  by http."; \
     sleep 60 ; \
     exit ; \
  fi
}

if [ ! -f $HOME/.asir-tmp/$asirnamegunzip ]; then
	_agree ; \
	echo -n "Downloading the binary of asir (1.5M) $asir ... " ; \
	oxfetch.sh $asir $HOME/.asir-tmp ; \
	echo "Done." ; \
	gunzip $HOME/.asir-tmp/$asirname ; \
	chmod +x $HOME/.asir-tmp/$asirnamegunzip ; \
        rm -f $OpenXM_HOME/bin/asir ; \
	install -m 755 $HOME/.asir-tmp/$asirnamegunzip $OpenXM_HOME/bin/asir ; \
fi
if [ ! -f $HOME/.asir-tmp/$asirlibname ]; then
	echo -n "Downloading the asir library  $asirlib... " ; \
	oxfetch.sh $asirlib $HOME/.asir-tmp ; \
	echo "Done." ; \
	(cd $HOME/.asir-tmp ; tar xzf $asirlibname) ; \
	(cd $OpenXM_HOME/lib/asir ; rm -f $ASIR_LIB_FILES) ; \
	(cd $HOME/.asir-tmp ; install -m 644 $ASIR_LIB_FILES $OpenXM_HOME/lib/asir) ; \
fi
if [ ! -f $HOME/.asir-tmp/$otname ]; then
	echo -n "Downloading the $ot plugin... " ; \
	oxfetch.sh $ot $HOME/.asir-tmp ; \
	echo "Done." ; \
	if [ -f $HOME/.TeXmacs/progs/my-init-texmacs.scm ]; then \
		echo "Warning .TeXmacs/progs/my-init-texmacs.scm exists"; \
		echo "Copy .TeXmacs and .feprc from ~/.asir-tmp/$otname by hand."; \
    else (cd $HOME ; tar xzf $HOME/.asir-tmp/$otname ; /bin/cp -n .feprc.sample .feprc) ; \
    fi
fi

_check_install_error

echo "Installation is completed."



