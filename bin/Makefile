# $OpenXM: OpenXM/bin/Makefile,v 1.11 2000/09/04 03:17:22 takayama Exp $

OPENXM_CONTRIB=../../OpenXM_contrib

LIBDIR=../lib
FEPLINUX=fep.linux
FEPFBSD=fep.fbsd
PHC=lin_phcv2p
RM=rm

configure:
	@if [ -f .configure_done ] ; then \
		echo "OpenXM/bin is already configured." ; \
	else \
		rm -f fep phc ; \
		if [ "`uname`" = "FreeBSD" ]; then \
			make configure-freebsd ; \
		fi ; \
		if [ "`uname`" = "Linux" ]; then \
			make configure-linux ; \
		fi ; \
		rm -f ox_asir ; \
		ln -f -s ./asir ox_asir ; \
		@touch ./configure_done ; \
	fi

clean-for-install:
	$(RM) -f *uuencoded* 

configure-freebsd:
	(cd ${OPENXM_CONTRIB}/PHC/Objects/ ; make phc)
	install /tmp/phc .
	if [ -f ${FEPFBSD}.gz.uuencoded ]; then \
		uudecode -p ${FEPFBSD}.gz.uuencoded | zcat - > fep ; \
		chmod +x fep ; \
	fi

configure-linux:
	if [ -f ${PHC}.gz.uuencoded ]; then \
		uudecode -p ${PHC}.gz.uuencoded | zcat - > phc ; \
		chmod +x phc ; \
	fi
	if [ -f ${FEPLINUX}.gz.uuencoded ]; then \
		uudecode -p ${FEPLINUX}.gz.uuencoded | zcat - > fep ; \
		chmod +x fep ; \
	fi

distclean:
	if [ "`uname`" = "FreeBSD" ]; then \
		(cd ${OPENXM_CONTRIB}/PHC/Objects/ ; make clean) \
	fi
	rm -f asir ox_asir fep phc
	@rm -f .configure_done

uuencode:
	@if [ -f $(FEPLINUX) ]; then \
		echo '$OpenXM: OpenXM/bin/Makefile,v 1.11 2000/09/04 03:17:22 takayama Exp $' > $(FEPLINUX).gz.uuencoded ; \
		gzip -c $(FEPLINUX) | uuencode $(FEPLINUX).gz >> $(FEPLINUX).gz.uuencoded ; \
	fi
	@if [ -f $(FEPFBSD) ]; then \
		echo '$OpenXM: OpenXM/bin/Makefile,v 1.11 2000/09/04 03:17:22 takayama Exp $' > $(FEPFBSD).gz.uuencoded ; \
		gzip -c $(FEPFBSD) | uuencode $(FEPFBSD).gz >> $(FEPFBSD).gz.uuencoded ; \
	fi
	@if [ -f $(PHC) ]; then \
		echo '$OpenXM: OpenXM/bin/Makefile,v 1.11 2000/09/04 03:17:22 takayama Exp $' > $(PHC).gz.uuencoded ; \
		gzip -c $(PHC) | uuencode $(PHC).gz >> $(PHC).gz.uuencoded ; \
	fi
