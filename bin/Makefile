# $OpenXM$

LIBDIR=../lib

configure:
	@if [ -f lin_phcv2p.gz.uuencoded ]; then \
		uudecode lin_phcv2p.gz.uuencoded | gzip -d ; \
		chmod +x lin_phcv2p ; \
	fi

	@if [ -f fep.linux.gz.uuencoded ]; then \
		uudecode fep.linux.gz.uuencoded | gzip -d ; \
		chmod +x fep.linux ; \
	fi

	@if [ "`uname`" = "Linux" ]; then \
		rm -f fep phc ; \
		ln -f -s fep.linux fep ; \
		ln -f -s lin_phcv2p phc ; \
	else \
		echo "We have the binary of phc only for linux." ; \
	fi

	rm -f asir
	ln -f -s $(LIBDIR)/asir/ox_asir asir

distclean:
	rm -f asir fep fep.linux lin_phcv2p phc
