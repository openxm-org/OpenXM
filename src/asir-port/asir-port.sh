#!/bin/sh
# $OpenXM: OpenXM/src/asir-port/asir-port.sh,v 1.5 2004/03/02 09:51:45 takayama Exp $
# Downloading Risa/Asir for FLL-free systems.
# Risa/Asir is installed under $HOME/.asir-tmp/$asirname
# Symbolic link to $asirname from $OpenXM_HOME/bin/asir must exist
# in the distribution of FLL-free distribution.
# Starting script of Risa/Asir may call this shell script.
os=`uname -s`
md=`cat $OpenXM_HOME/lib/asir/distinfo-asir`
libmd=`cat $OpenXM_HOME/lib/asir/distinfo-lib`
# For testing
#asir="ftp://ftp.math.kobe-u.ac.jp/pub/asir/gzip.exe"
#asirname="gzip.exe"
#asirlib="ftp://ftp.math.kobe-u.ac.jp/pub/asir/tar.exe"
#asirlibname="tar.exe"
# 
asir="ftp://ftp.math.kobe-u.ac.jp/pub/asir/knoppix/asir-$os-$md.gz"
asirnamegunzip="asir-$os-$md"
asirname="asir-$os-$md.gz"
asirlib="ftp://ftp.math.kobe-u.ac.jp/pub/asir/knoppix/asirlib-$os-$libmd.tar.gz"
asirlibname="asirlib-$os-$libmd.tar.gz"
ot="ftp://ftp.math.kobe-u.ac.jp/pub/asir/knoppix/ox-texmacs-$os.tar.gz"
otname="ox-texmacs-$os.tar.gz"

if [ $# = 1 ]; then
if [ $1 = "--install" ]; then
 rm -rf $HOME/.asir-tmp
fi
fi

_agree() {
	echo "------------------------------------------------------------------"
	echo "Risa/Asir is distributed with no warranty for non-commercial use."
	echo "OpenXM subcomponents are distributed with no warranty under BSD license or GPL."
	echo "Do you agree with the licenses under $OpenXM_HOME/Copyright?"
	echo "------------------------------------------------------------------"
	echo "y: agree, n: do not agree, v: read the detail of the asir license."
	read -e -p "(y/n/v)" ans
	if [ $ans = "y" ]; then
		return
	fi
	if [ $ans = "n" ]; then
		echo "Aborting the installation." ; \
		exit 
    fi
	if [ $ans = "v" ]; then
		more $OpenXM_HOME/Copyright/Copyright.asir ; \
		_agree ; \
		return 
	fi
	_agree
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
		echo "Copy from ~/.asir-tmp/$otname by hand."; \
    else (cd $HOME ; tar xzf $HOME/.asir-tmp/$otname) ; \
    fi
fi


$OpenXM_HOME/bin/fep $OpenXM_HOME/bin/asir $*


