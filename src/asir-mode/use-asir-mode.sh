#!/bin/sh
# $OpenXM: OpenXM/src/asir-mode/use-asir-mode.sh,v 1.3 2019/06/12 06:14:20 takayama Exp $

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
	ver=$(emacs --batch --eval="(princ emacs-major-version)")
	if [  ${ver} -le 27 ]; then
	echo "emacs ver <= 27" ; sed -e 's/(insert "openxm asir")/(insert "openxm asir -nofep")/g' $OpenXM_HOME/share/emacs/asir-mode.el >$HOME/.emacs.d/OpenXM/asir-mode.el
	elif [ ${ver} -eq 28]; then
	echo "emacs ver == 28" ; sed -e 's/(insert "asir -nofep")/(insert "openxm asir -nofep")/g' $OpenXM_HOME/share/emacs/asir-mode-v28.el >$HOME/.emacs.d/OpenXM/asir-mode.el
	else 
	echo "emacs ver == 29" ; sed -e 's/(insert "asir -nofep")/(insert "openxm asir -nofep")/g' $OpenXM_HOME/share/emacs/asir-mode-v29.el >$HOME/.emacs.d/OpenXM/asir-mode.el
	echo "The dollar symbol is no longer supported. Use ;; instead."
	fi
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






