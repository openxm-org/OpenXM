# $OpenXM: OpenXM/bin/Makefile,v 1.15 2000/12/21 11:36:15 takayama Exp $

OPENXM_CONTRIB=../../OpenXM_contrib

LIBDIR=../lib
FEPLINUX=fep.linux
FEPFBSD=fep.fbsd
PHCFBSD=fbsd_phcv2p
PHCLINUX=lin_phcv2p
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
		touch ./.configure_done ; \
	fi

clean-for-install:
	$(RM) -f *uuencoded* 

configure-freebsd:
	if [ -f ${PHCFBSD}.gz.uuencoded ]; then \
		uudecode -p ${PHCFBSD}.gz.uuencoded | zcat - > phc ; \
		chmod +x phc ; \
	fi
	if [ -f ${FEPFBSD}.gz.uuencoded ]; then \
		uudecode -p ${FEPFBSD}.gz.uuencoded | zcat - > fep ; \
		chmod +x fep ; \
	fi

configure-linux:
	if [ -f ${PHCLINUX}.gz.uuencoded ]; then \
		uudecode -o /dev/stdout ${PHCLINUX}.gz.uuencoded | zcat - > phc ; \
		chmod +x phc ; \
	fi
	if [ -f ${FEPLINUX}.gz.uuencoded ]; then \
		uudecode -o /dev/stdout ${FEPLINUX}.gz.uuencoded | zcat - > fep ; \
		chmod +x fep ; \
	fi

configure-linux-phc-on-freebsd:
	if [ -f ${PHCLINUX}.gz.uuencoded ]; then \
		uudecode -p ${PHCLINUX}.gz.uuencoded | zcat - > phc ; \
		chmod +x phc ; \
		brandelf -t Linux phc ; \
		echo "You need to turn on the linux emulation. Add linux_enable=\"YES\" to /etc/rc.conf and pkg_add linux_base-(version number)." ; \
	fi

distclean:
	rm -f asir ox_asir fep phc
	@rm -f .configure_done

uuencode:
	@if [ -f $(FEPLINUX) ]; then \
		echo '$OpenXM: OpenXM/bin/Makefile,v 1.15 2000/12/21 11:36:15 takayama Exp $' > $(FEPLINUX).gz.uuencoded ; \
		gzip -c $(FEPLINUX) | uuencode $(FEPLINUX).gz >> $(FEPLINUX).gz.uuencoded ; \
	fi
	@if [ -f $(FEPFBSD) ]; then \
		echo '$OpenXM: OpenXM/bin/Makefile,v 1.15 2000/12/21 11:36:15 takayama Exp $' > $(FEPFBSD).gz.uuencoded ; \
		gzip -c $(FEPFBSD) | uuencode $(FEPFBSD).gz >> $(FEPFBSD).gz.uuencoded ; \
	fi
	@if [ -f $(PHC) ]; then \
		echo '$OpenXM: OpenXM/bin/Makefile,v 1.15 2000/12/21 11:36:15 takayama Exp $' > $(PHC).gz.uuencoded ; \
		gzip -c $(PHC) | uuencode $(PHC).gz >> $(PHC).gz.uuencoded ; \
	fi
