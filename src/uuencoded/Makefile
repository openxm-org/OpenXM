# $OpenXM: OpenXM/src/uuencoded/Makefile,v 1.3 2003/09/02 14:26:06 takayama Exp $

OPENXM=../../../OpenXM
OPENXM_CONTRIB=../../../OpenXM_contrib
OPENXM_BIN=$(OPENXM)/bin
OPENXM_UU=$(OPENXM_CONTRIB)/uuencoded

LIBDIR=${OPENXM}/lib
FEPLINUX=fep.linux
FEPFBSD=fep.fbsd
PHCFBSD=fbsd_phcv2p
PHCLINUX=lin_phcv2p
RM=rm

install:
	@if [ ! -f .install_done ] ; then \
		rm -f $(OPENXM_BIN)/phc ; \
		if [ "`which gnatmake`" ]; then \
			(cd ${OPENXM_CONTRIB}/PHC/Objects; mkdir tmp; ${MAKE} phc; strip tmp/phc); \
			cp -p ${OPENXM_CONTRIB}/PHC/Objects/tmp/phc ${OPENXM_BIN}/phc; \
		elif [ "`uname`" = "FreeBSD" ]; then \
			make install-freebsd ; \
		elif [ "`uname`" = "Linux" ]; then \
			make install-linux ; \
		fi ; \
		(cd $(OPENXM_BIN) ; rm -f ox_asir ; ln -f -s ./asir ox_asir) ; \
		touch ./.install_done ; \
	fi

install-freebsd:
	if [ -f $(OPENXM_UU)/${PHCFBSD}.gz.uuencoded ]; then \
		uudecode -p $(OPENXM_UU)/${PHCFBSD}.gz.uuencoded | zcat - > $(OPENXM_BIN)/phc ; \
		chmod +x $(OPENXM_BIN)/phc ; \
	fi

install-linux:
	if [ -f $(OPENXM_UU)/${PHCLINUX}.gz.uuencoded ]; then \
		uudecode -o /dev/stdout $(OPENXM_UU)/${PHCLINUX}.gz.uuencoded | zcat - > $(OPENXM_BIN)/phc ; \
		chmod +x $(OPENXM_BIN)/phc ; \
	fi

install-linux-phc-on-freebsd:
	if [ -f $(OPENXM_UU)/${PHCLINUX}.gz.uuencoded ]; then \
		uudecode -p $(OPENXM_UU)/${PHCLINUX}.gz.uuencoded | zcat - > $(OPENXM_BIN)/phc ; \
		chmod +x $(OPENXM_BIN)/phc ; \
		brandelf -t Linux $(OPENXM_BIN)/phc ; \
		echo "You need to turn on the linux emulation. Add linux_enable=\"YES\" to /etc/rc.conf and pkg_add linux_base-(version number)." ; \
	fi

distclean:
	(cd $(OPENXM_BIN) ; rm -f asir ox_asir phc)
	@rm -f .install_done

uuencode:
	@if [ -f $(OPENXM_BIN)/$(PHC) ]; then \
		echo -n '$Open' > $(OPENXM_UU)/$(PHC).gz.uuencoded ; \
		echo -n 'XM$' >> $(OPENXM_UU)/$(PHC).gz.uuencoded ; \
		gzip -c $(OPENXM_BIN)/$(PHC) | uuencode $(PHC).gz >> $(OPENXM_UU)/$(PHC).gz.uuencoded ; \
	fi
