#!/bin/sh
# $OpenXM: OpenXM/src/asir-mode/use-asir-mode.sh,v 1.1 2016/02/12 02:39:26 takayama Exp $

_ok_local() {
	echo "------------------------------------------------------------------"
	echo "This script install $OpenXM_HOME/share/emacs/asir-mode.el"
    echo "to your .emacs.d and modify your .emacs.d/init.el to enable asir-mode.el"
	echo -n "y: go ahead, n: stop  : "
    read ans
	if [ $ans = "y" ]; then
		return
	fi
	if [ $ans = "n" ]; then
		echo "Aborting use-asir-mode.sh" ; \
		exit 
    fi
}

_install_local() {
	mkdir -p $HOME/.emacs.d/OpenXM
	cp $OpenXM_HOME/share/emacs/asir-mode.el $HOME/.emacs.d/OpenXM
	cat $OpenXM_HOME/share/emacs/use-asir-mode-local.txt >>$HOME/.emacs.d/init.el
}

_install() {
	cat $OpenXM_HOME/share/emacs/use-asir-mode-local.txt >>$HOME/.emacs.d/init.el
}

if [ $# = 0 ]; then
	echo "Usage: use-asir-mode.sh --local" ; \
	echo "       use-asir-mode.sh --local-yes" ; \
	echo "       --use-site-lisp has not been implemented." ; \
fi

if [ $# = 1 ]; then
if [ $1 = "--local" ]; then
 _ok_local ; \
 _install_local ; \
fi
if [ $1 = "--local-yes" ]; then
 _install_local ; \
fi
fi






