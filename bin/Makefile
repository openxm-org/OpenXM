# $OpenXM: OpenXM/bin/Makefile,v 1.4 2000/01/09 21:04:04 maekawa Exp $

LIBDIR=../lib
FEPLINUX=fep.linux
FEPFBSD=fep.fbsd
PHC=lin_phcv2p

configure:
	@if [ -f $(FEPLINUX).gz.uuencoded ]; then \
		uudecode $(FEPLINUX).gz.uuencoded ; \
		gzip -d $(FEPLINUX).gz ; \
		chmod +x $(FEPLINUX) ; \
	fi

	@if [ -f $(FEPFBSD).gz.uuencoded ]; then \
		uudecode $(FEPFBSD).gz.uuencoded ; \
		gzip -d $(FEPFBSD).gz ; \
		chmod +x $(FEPFBSD) ; \
	fi

	@if [ -f $(PHC).gz.uuencoded ]; then \
		uudecode $(PHC).gz.uuencoded ; \
		gzip -d $(PHC).gz ; \
		chmod +x $(PHC) ; \
	fi

	@if [ "`uname`" = "Linux" ]; then \
		rm -f fep phc ; \
		ln -f -s $(FEPLINUX) fep ; \
		ln -f -s $(PHC) phc ; \
	else \
		echo "We have the binary of phc only for linux." ; \
	fi

	rm -f asir
	ln -f -s $(LIBDIR)/asir/ox_asir asir

distclean:
	rm -f asir fep fep.linux lin_phcv2p phc

uuencode:
	@if [ -f $(FEPLINUX) ]; then \
		echo '$OpenXM: OpenXM/bin/Makefile,v 1.4 2000/01/09 21:04:04 maekawa Exp $' > $(FEPLINUX).gz.uuencoded ; \
		gzip -c $(FEPLINUX) | uuencode $(FEPLINUX).gz >> $(FEPLINUX).gz.uuencoded ; \
	fi
	@if [ -f $(FEPFBSD) ]; then \
		echo '$OpenXM: OpenXM/bin/Makefile,v 1.4 2000/01/09 21:04:04 maekawa Exp $' > $(FEPFBSD).gz.uuencoded ; \
		gzip -c $(FEPFBSD) | uuencode $(FEPFBSD).gz >> $(FEPFBSD).gz.uuencoded ; \
	fi
	@if [ -f $(PHC) ]; then \
		echo '$OpenXM: OpenXM/bin/Makefile,v 1.4 2000/01/09 21:04:04 maekawa Exp $' > $(PHC).gz.uuencoded ; \
		gzip -c $(PHC) | uuencode $(PHC).gz >> $(PHC).gz.uuencoded ; \
	fi
