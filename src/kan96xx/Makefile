#  $OpenXM: OpenXM/src/kan96xx/Makefile,v 1.30 2003/09/02 03:31:12 takayama Exp $
#
# Building kan96xx depends on
#   install-gmp, install-gc [install-asir-gc] 
#
OpenXM_HOME=../..
OpenXM_contrib=${OpenXM_HOME}/../OpenXM_contrib
OpenXM_bin=${OpenXM_HOME}/bin
OpenXM_lib=${OpenXM_HOME}/lib

RM=rm

all :   all-sm1 all-kanlib.a

all-sm1 : all-plugin 
	(cd Kan; make all)
all-plugin :  configure
	 (cd plugin ; make all)
all-kanlib.a : all-plugin
	(cd Kan ; make kanlib.a)

### When you make sm1 without the garbage collector, type in make sm1.nogc
sm1-nogc : all-plugin
	(cd Kan; make sm1.nogc)
##########################################################
configure: configure_symbolic_links 
	if test ! -f ./.configure_done  ; \
	then \
	(cd Kan; sh ./configure) ; \
	(cd plugin; sh ./configure) ; \
	fi
	@touch ./.configure_done
configure_symbolic_links :
	@if test ! -f ./.configure_symbolic_links_done  ; \
	then \
	 ${RM} -f lib Doc/var.sm1 ; \
	 ln -f -s ./Doc lib ; \
	 (cd Doc; ln -f -s ../Kan/var.sm1 var.sm1) ; \
	fi
	@touch ./.configure_symbolic_links_done 
###############
install: install-exec install-man install-document
install-exec: clean-for-install all-sm1
	install Kan/sm1 ${OpenXM_bin}
install-man :  
	-cp Doc/sm1.1 ${OpenXM_HOME}/man/man1
install-document:  
	-(cd Doc ; make install-document)
install-for-debug : clean-for-install
	if [ "`uname | sed -e 's/_.*$$//'`" = "CYGWIN" ]; then \
	  ln -f -s ../src/kan96xx/Kan/sm1.exe ${OpenXM_bin}/sm1.exe ; \
	else \
	  ln -f -s ../src/kan96xx/Kan/sm1 ${OpenXM_bin}/sm1 ; \
	fi


##############
clean-for-install :
	${RM} -f ${OpenXM_bin}/sm1
clean-done:
	${RM} -f ./.*done
clean : clean-done
	${RM} -f ./.*done
	(cd Kan; make clean)
	(cd plugin; make clean)
	(cd Doc; make clean)
	${RM} -f sm1 gmp gc Doc/var.sm1 lib



