# $OpenXM: OpenXM/bin/Makefile,v 1.5 2000/01/19 07:28:37 noro Exp $

LIBDIR=../lib
FEPLINUX=fep.linux
FEPFBSD=fep.fbsd
PHC=lin_phcv2p
RM=rm

configure: $(FEPLINUX) $(FEPFBSD) $(PHC)
	@if [ "`uname`" = "Linux" ]; then \
		rm -f fep phc ; \
		ln -f -s $(FEPLINUX) fep ; \
		ln -f -s $(PHC) phc ; \
	else \
		echo "We have the binary of phc only for linux." ; \
	fi

	rm -f asir
	ln -f -s $(LIBDIR)/asir/ox_asir asir

$(FEPLINUX) : $(FEPLINUX).gz.uuencoded
	@if [ -f $(FEPLINUX).gz.uuencoded ]; then \
		uudecode $(FEPLINUX).gz.uuencoded ; \
		gzip -d $(FEPLINUX).gz ; \
		chmod +x $(FEPLINUX) ; \
	fi

$(FEPFBSD) : $(FEPFBSD).gz.uuencoded
	@if [ -f $(FEPFBSD).gz.uuencoded ]; then \
		uudecode $(FEPFBSD).gz.uuencoded ; \
		gzip -d $(FEPFBSD).gz ; \
		chmod +x $(FEPFBSD) ; \
	fi

$(PHC) : $(PHC).gz.uuencoded
	@if [ -f $(PHC).gz.uuencoded ]; then \
		uudecode $(PHC).gz.uuencoded ; \
		gzip -d $(PHC).gz ; \
		chmod +x $(PHC) ; \
	fi

distclean:
	rm -f asir fep fep.linux lin_phcv2p phc

uuencode:
	@if [ -f $(FEPLINUX) ]; then \
		echo '$OpenXM: OpenXM/bin/Makefile,v 1.5 2000/01/19 07:28:37 noro Exp $' > $(FEPLINUX).gz.uuencoded ; \
		gzip -c $(FEPLINUX) | uuencode $(FEPLINUX).gz >> $(FEPLINUX).gz.uuencoded ; \
	fi
	@if [ -f $(FEPFBSD) ]; then \
		echo '$OpenXM: OpenXM/bin/Makefile,v 1.5 2000/01/19 07:28:37 noro Exp $' > $(FEPFBSD).gz.uuencoded ; \
		gzip -c $(FEPFBSD) | uuencode $(FEPFBSD).gz >> $(FEPFBSD).gz.uuencoded ; \
	fi
	@if [ -f $(PHC) ]; then \
		echo '$OpenXM: OpenXM/bin/Makefile,v 1.5 2000/01/19 07:28:37 noro Exp $' > $(PHC).gz.uuencoded ; \
		gzip -c $(PHC) | uuencode $(PHC).gz >> $(PHC).gz.uuencoded ; \
	fi
