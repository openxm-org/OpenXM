# $OpenXM: OpenXM/src/uuencoded/Makefile,v 1.1 2001/08/21 00:39:13 takayama Exp $

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

configure:
	@if [ -f .configure_done ] ; then \
		echo "OpenXM/bin is already configured." ; \
	else \
		rm -f $(OPENXM_BIN)/phc ; \
		if [ "`uname`" = "FreeBSD" ]; then \
			make configure-freebsd ; \
		fi ; \
		if [ "`uname`" = "Linux" ]; then \
			make configure-linux ; \
		fi ; \
		(cd $(OPENXM_BIN) ; rm -f ox_asir ; ln -f -s ./asir ox_asir) ; \
		touch ./.configure_done ; \
	fi


configure-freebsd:
	if [ -f $(OPENXM_UU)/${PHCFBSD}.gz.uuencoded ]; then \
		uudecode -p $(OPENXM_UU)/${PHCFBSD}.gz.uuencoded | zcat - > $(OPENXM_BIN)/phc ; \
		chmod +x $(OPENXM_BIN)/phc ; \
	fi

configure-linux:
	if [ -f $(OPENXM_UU)/${PHCLINUX}.gz.uuencoded ]; then \
		uudecode -o /dev/stdout $(OPENXM_UU)/${PHCLINUX}.gz.uuencoded | zcat - > $(OPENXM_BIN)/phc ; \
		chmod +x $(OPENXM_BIN)/phc ; \
	fi

configure-linux-phc-on-freebsd:
	if [ -f $(OPENXM_UU)/${PHCLINUX}.gz.uuencoded ]; then \
		uudecode -p $(OPENXM_UU)/${PHCLINUX}.gz.uuencoded | zcat - > $(OPENXM_BIN)/phc ; \
		chmod +x $(OPENXM_BIN)/phc ; \
		brandelf -t Linux $(OPENXM_BIN)/phc ; \
		echo "You need to turn on the linux emulation. Add linux_enable=\"YES\" to /etc/rc.conf and pkg_add linux_base-(version number)." ; \
	fi

distclean:
	(cd $(OPENXM_BIN) ; rm -f asir ox_asir phc)
	@rm -f .configure_done

uuencode:
	@if [ -f $(OPENXM_BIN)/$(PHC) ]; then \
		echo -n '$Open' > $(OPENXM_UU)/$(PHC).gz.uuencoded ; \
		echo -n 'XM$' >> $(OPENXM_UU)/$(PHC).gz.uuencoded ; \
		gzip -c $(OPENXM_BIN)/$(PHC) | uuencode $(PHC).gz >> $(OPENXM_UU)/$(PHC).gz.uuencoded ; \
	fi
