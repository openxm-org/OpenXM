# $OpenXM: OpenXM/rc/Makefile,v 1.32 2020/08/25 15:29:21 fujimoto Exp $
# make install-under-prefix  [ prefix=/home/hoge ]  ; Default is /usr/local
prefix=/usr/local
# CC=gcc
# SHELL_SCRIPTS=asir sm1 oxgp k0 oxmath oxMathematica ox
SHELL_SCRIPTS=asir sm1 oxmath oxMathematica ox openxm webasir
SHELL_SCRIPTS_FREE= sm1 oxmath oxMathematica ox openxm webasir
INSTALL_DIR=${prefix}/bin
MAN_DIR=../man/man1
KDE_MENU_DIR=/usr/lib/menu

# SHELL_SCRIPTS are generated by "gen-shell-scripts" 
all-scripts :  dot.bashrc  dot.cshrc .done_gen-shell-scripts

dot.bashrc : bashrc repl 
	rm -f dot.bashrc 
	echo "# DO NOT EDIT THIS FILE" >dot.bashrc
	./repl bash <bashrc >>dot.bashrc

dot.cshrc : cshrc repl
	rm -f dot.cshrc 
	echo "# DO NOT EDIT THIS FILE" >dot.cshrc
	./repl csh <cshrc >>dot.cshrc

repl : repl.c 
	rm -f repl
	${CC} -o ../bin/rc.repl repl.c
	ln -s ../bin/rc.repl repl

.done_gen-shell-scripts : dot.cshrc Makefile gen-shell-scripts
	./gen-shell-scripts
	-install openxm ../bin
	touch .done_gen-shell-scripts

#For the backword compatibility.
install: install-scripts
install-scripts : all-scripts  install-man
	-install -d ${INSTALL_DIR}
	-install $(SHELL_SCRIPTS) ${INSTALL_DIR}
install-man :
	-install openxm.1 ${MAN_DIR}
install-kde-menu-knoppix:
	install kde/openxm-binary-knoppix ${KDE_MENU_DIR}/openxm-binary

clean :
	rm -rf dot.bashrc dot.cshrc repl openxm $(SHELL_SCRIPTS) .done_gen-shell-scripts work global .done_global

## New targets.  Relocate OpenXM under ${prefix}
## They are designed to be a top level Makefile
all:
	(cd ../src ; make all)
all-non-free:
	(cd ../src ; make all-asir)
all-global-shell-scripts: repl
	@if [ ! -f .done_global ] ; then \
	echo "Generating shell scripts in global" ; \
	rm -rf global ; mkdir global ; \
	(cd global ; ../repl bash --prefix ${prefix} <../bashrc >dot.bashrc); \
	(cd global ; ../repl csh --prefix ${prefix} <../cshrc >dot.cshrc); \
	(cd global ; ../gen-shell-scripts) ; \
	touch .done_global ; \
	fi
install-non-free:
	(cd ../src ; make install-asir)
install-free:
	(cd ../src ; make install)
install-under-prefix: all-global-shell-scripts   install-free
	make copy-free
	make copy-non-free
install-global-scripts: all-global-shell-scripts
	(cd global ; install ${SHELL_SCRIPTS} ${INSTALL_DIR})
install-global-scripts-non-free: all-global-shell-scripts
	(cd global ; install asir ${INSTALL_DIR})
install-global-scripts-free: all-global-shell-scripts
	(cd global ; install ${SHELL_SCRIPTS_FREE} ${INSTALL_DIR})

clean-under-prefix:
	-(cd ${prefix} ; rm -rf ${prefix}/OpenXM )
# Relocate only asir
#    make install-non-free ;  make clean-under-prefix ; 
#    make install-global-scripts-non-free ; make copy-non-free
# Relocate only free
#    make install-free ; make clean-under-prefix ;
#    make install-global-scripts-free ; make copy-free
#
# junk-list is not a collection of real junk files. 
# Files in it are not necessary for the binary package. They are important.
copy-free: all-global-shell-scripts
	-mkdir ${prefix}
	-(cd ../.. ; tar czf - --exclude-from OpenXM/rc/non-free-list --exclude OpenXM/misc --exclude OpenXM/src --exclude-from OpenXM/rc/junk-list OpenXM) | (cd ${prefix} ; tar xvzf -)
# During installation OpenXM/bin/openxm must be overridden by global/openxm
	install global/openxm ${prefix}/OpenXM/bin

copy-non-free: all-global-shell-scripts
	-mkdir ${prefix}
	-(cd ../.. ; tar czf - --exclude-from OpenXM/rc/junk-list `cat OpenXM/rc/non-free-list`) | (cd ${prefix} ; tar xvzf -)

install-emacs-asir-on-mac: all-scripts
	-mkdir -p ~/bin
	cp ../bin/openxm ~/bin
	if type "openxm" >/dev/null 2>&1; then \
		echo "openxm is already in the path."; \
	else \
		echo "export PATH=~/bin:$$PATH" >>~/.bash_profile ; \
		echo "~/bin is added to the path."; \
	fi
	~/bin/openxm use-asir-mode.sh --local-yes
	@echo "Installation completed. Login again or do source ./dot.bashrc to start emacs/asir"

norc-fix:
	pushd ../bin ; mv openxm openxm.orig ; sed -e 's/ASIR_CONFIG/#ASIR_CONFIG/g' openxm.orig >openxm ; chmod +x openxm ; popd
	cp ../bin/openxm ~/bin


