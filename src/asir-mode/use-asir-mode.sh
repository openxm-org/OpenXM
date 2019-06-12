#!/bin/sh
# $OpenXM: OpenXM/src/asir-mode/use-asir-mode.sh,v 1.2 2016/02/12 02:43:33 takayama Exp $

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

_check() {
    if [ -f $HOME/.emacs ]; then
	grep 'emacs\.d/init\.el' $HOME/.emacs ;
	if [ $? -eq 0 ]; then
	    echo "OK ...";
	else
	    echo "Warning! init.el is not loaded from .emacs. Check if you load init.el";
	    echo "  Example:  Add the following line ";
	    echo "       (load \"~/.emacs.d/init.el\") ";
	    echo "  to $HOME/.emacs";
        fi
    fi
}

_install() {
	cat $OpenXM_HOME/share/emacs/use-asir-mode-local.txt >>$HOME/.emacs.d/init.el
}
_check ;
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






