# $OpenXM$
OpenXM_HOME=../../../OpenXM
RM=/bin/rm -rf

install :
	-if [ "`uname`" = "FreeBSD" ]; then \
		(cd ports/FreeBSD/Macaulay2 ; make OpenXM_HOME="`pwd`"/../../../${OpenXM_HOME} install) ; \
	else \
		echo "Install by hand and copy M2 under OpenXM_HOME/bin" ; \
	fi ; \

clean:
	-$(RM) ports/FreeBSD/Macaulay2/work
