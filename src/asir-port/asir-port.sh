#!/bin/bash
# $OpenXM: OpenXM/src/asir-port/asir-port.sh,v 1.21 2014/07/27 02:33:19 takayama Exp $
# Downloading Risa/Asir for FLL-free systems.
# Risa/Asir is installed under $HOME/.asir-tmp/$asirname
# Symbolic link to $asirname from $OpenXM_HOME/bin/asir must exist
# in the distribution of FLL-free distribution.
# Starting script of Risa/Asir may call this shell script.
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

if [ $# = 1 ]; then
if [ $1 = "--install" ]; then
 rm -rf $HOME/.asir-tmp ;
 rm -f /usr/local/OpenXM/bin/asir ;
fi
fi

if [ -f /usr/local/OpenXM/bin/asir ]; then
  $OpenXM_HOME/bin/fep $OpenXM_HOME/bin/asir $* ; exit ;
fi

_agree() {
	echo "------------------------------------------------------------------"
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
  fi
}

if [ ! -f $HOME/.asir-tmp/$asirnamegunzip ]; then
	_agree ; \
	echo -n "Downloading the binary of asir (1.5M) $asir ... " ; \
	oxfetch.sh $asir $HOME/.asir-tmp ; \
	echo "Done." ; \
	gunzip $HOME/.asir-tmp/$asirname ; \
	chmod +x $HOME/.asir-tmp/$asirnamegunzip ; \
	rm -f $HOME/.asir-tmp/asir ; \
	ln -s $HOME/.asir-tmp/$asirnamegunzip $HOME/.asir-tmp/asir ; \
fi
if [ ! -f $HOME/.asir-tmp/$asirlibname ]; then
	echo -n "Downloading the asir library  $asirlib... " ; \
	oxfetch.sh $asirlib $HOME/.asir-tmp ; \
	echo "Done." ; \
	(cd $HOME/.asir-tmp ; tar xzf $asirlibname) ; \
fi
if [ ! -f $HOME/.asir-tmp/$otname ]; then
	echo -n "Downloading $ot plugin... " ; \
	oxfetch.sh $ot $HOME/.asir-tmp ; \
	echo "Done." ; \
	if [ -f $HOME/.TeXmacs/progs/my-init-texmacs.scm ]; then \
		echo "Warning .TeXmacs/progs/my-init-texmacs.scm exists"; \
		echo "Copy .TeXmacs and .feprc from ~/.asir-tmp/$otname by hand."; \
    else (cd $HOME ; tar xzf $HOME/.asir-tmp/$otname ) ; \
    fi ; \
    if [ -f $HOME/.feprc ]; then \
         echo "Warning .feprc exists"; \
         echo "Copy .feprc from ~/.asir-tmp/$otname by hand."; \
    else (cd $HOME ; touch .feprc.sample ; /bin/cp -n .feprc.sample .feprc) ; \
    fi
fi

_check_install_error

$OpenXM_HOME/bin/fep $OpenXM_HOME/bin/asir $*


