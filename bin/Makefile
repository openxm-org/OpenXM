# $OpenXM: OpenXM/bin/Makefile,v 1.3 2000/01/09 20:27:28 maekawa Exp $

LIBDIR=../lib
FEP=fep.linux
PHC=lin_phcv2p

configure:
	@if [ -f $(FEP).gz.uuencoded ]; then \
		uudecode -p $(FEP).gz.uuencoded | gzip -dc > $(FEP) ; \
		chmod +x $(FEP) ; \
	fi

	@if [ -f $(PHC).gz.uuencoded ]; then \
		uudecode -p $(PHC).gz.uuencoded | gzip -dc > $(PHC) ; \
		chmod +x $(PHC) ; \
	fi

	@if [ "`uname`" = "Linux" ]; then \
		rm -f fep phc ; \
		ln -f -s $(FEP) fep ; \
		ln -f -s $(PHC) phc ; \
	else \
		echo "We have the binary of phc only for linux." ; \
	fi

	rm -f asir
	ln -f -s $(LIBDIR)/asir/ox_asir asir

distclean:
	rm -f asir fep fep.linux lin_phcv2p phc

uuencode:
	@if [ -f $(FEP) ]; then \
		echo '$OpenXM$' > $(FEP).gz.uuencoded ; \
		gzip -c $(FEP) | uuencode $(FEP).gz >> $(FEP).gz.uuencoded ; \
	fi
	@if [ -f $(PHC) ]; then \
		echo '$OpenXM$' > $(PHC).gz.uuencoded ; \
		gzip -c $(PHC) | uuencode $(PHC).gz >> $(PHC).gz.uuencoded ; \
	fi
