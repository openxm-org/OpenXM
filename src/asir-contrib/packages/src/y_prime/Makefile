#$OpenXM$
#
# A temporary Makefile to download pdata. It is for unix/cygwin/MacOSX
# Todo: how to automatically download on windows.

OpenXM_HOME=../../../../..
PREFIX=${OpenXM_HOME}
DISTDIR=${OpenXM_HOME}/../OpenXM_dist
RM = /bin/rm
MASTER_SITE=ftp://ftp.math.kobe-u.ac.jp/pub2/OpenXM_data/
# Original. http://www.math.kyushu-u.ac.jp
DISTNAME=y_prime-pdata-2005-11-12
DISTFILES=${DISTNAME}.tar.gz
WRKSRC=work/${DISTNAME}
PATCHFILES = 

usage:
	@echo "make install-pdata to install pdata (prime number table of y_prime project"

install-pdata: extract

fetch:
	-mkdir work
	@if [ ! -f work/.fetch_done ]; then \
		${OpenXM_HOME}/bin/oxfetch.sh ${MASTER_SITE}${DISTFILES} ${DISTDIR}; \
	fi
	@touch work/.fetch_done

extract: fetch
	@if [ ! -f work/.extract_done ]; then \
	    cat ${DISTDIR}/${DISTFILES} | tar xzvf - ; \
	fi
	@touch work/.extract_done

clean:
	-$(RM) -rf work

distclean: clean
	-$(RM) -rf pdata
