#!/bin/sh
# $OpenXM$
# Downloading Risa/Asir for FLA-free systems.
# Risa/Asir is installed under $HOME/.asir-tmp/$asirname
# Symbolic link to $asirname from $OpenXM_HOME/bin/asir must exist
# in the distribution of FLA-free 
# Starting script of Risa/Asir may call this shell script.
os=`uname -s`
md=`cat $OpenXM_HOME/lib/asir/distinfo`
# For testing
asir="ftp://ftp.math.kobe-u.ac.jp/pub/asir/gzip.exe"
asirname="gzip.exe"
# 
#asir="ftp://ftp.math.kobe-u.ac.jp/pub/asir/binaries/asir-$os-$md"
#asirname="asir-$os-$md"

#echo $asir
_agree() {
	echo "------------------------------------------------------------------"
	echo "Risa/Asir is distributed with no warranty for non-commercial use."
	echo "Do you agree with the license agreement at $OpenXM_HOME/Copyright/Copyright.asir?"
	echo "------------------------------------------------------------------"
	echo "y: agree, n: do not agree, v: read the detail of the agreement."
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

if [ ! -f $HOME/.asir-tmp/$asirname ]; then
	_agree ; \
	oxfetch.sh $asir $HOME/.asir-tmp ; \
	ln -s $HOME/.asir-tmp/$asirname $HOME/.asir-tmp/asir ; \
fi

#fep asir



