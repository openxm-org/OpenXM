# $OpenXM: OpenXM/bin/Makefile,v 1.8 2000/01/21 09:51:08 noro Exp $

LIBDIR=../lib
FEPLINUX=fep.linux
FEPFBSD=fep.fbsd
PHC=lin_phcv2p
RM=rm

configure:
	@if [ -f .configure_done ] ; then \
		echo "OpenXM/bin is already configured." ; \
	else \
		make configure-real ; \
		$(RM) -f *uuencoded* ; \
	fi

configure-real: $(FEPLINUX) $(FEPFBSD) $(PHC)
	@if [ "`uname`" = "Linux" ]; then \
		rm -f fep phc ; \
		ln -f -s $(FEPLINUX) fep ; \
		ln -f -s $(PHC) phc ; \
	    echo "Linux binary generation: fep, phc" ; \
	else \
		echo "Not Linux" ; \
	fi
	-@if [ "`uname`" = "FreeBSD" ]; then \
		rm -f fep phc ; \
		ln -f -s $(FEPFBSD) fep ; \
		ln -f -s $(PHC) phc ; \
		brandelf -t Linux ./phc ; \
		echo "FreeBSD binary generation: fep and phc. Linux Emulation is requied to execute phc." ; \
	else \
	     echo "Not FreeBSD" ; \
	fi

	rm -f ox_asir
	ln -f -s ./asir ox_asir
	@touch .configure_done

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
	rm -f asir ox_asir fep fep.linux fep.fbsd lin_phcv2p phc .configure_done

uuencode:
	@if [ -f $(FEPLINUX) ]; then \
		echo '$OpenXM: OpenXM/bin/Makefile,v 1.8 2000/01/21 09:51:08 noro Exp $' > $(FEPLINUX).gz.uuencoded ; \
		gzip -c $(FEPLINUX) | uuencode $(FEPLINUX).gz >> $(FEPLINUX).gz.uuencoded ; \
	fi
	@if [ -f $(FEPFBSD) ]; then \
		echo '$OpenXM: OpenXM/bin/Makefile,v 1.8 2000/01/21 09:51:08 noro Exp $' > $(FEPFBSD).gz.uuencoded ; \
		gzip -c $(FEPFBSD) | uuencode $(FEPFBSD).gz >> $(FEPFBSD).gz.uuencoded ; \
	fi
	@if [ -f $(PHC) ]; then \
		echo '$OpenXM: OpenXM/bin/Makefile,v 1.8 2000/01/21 09:51:08 noro Exp $' > $(PHC).gz.uuencoded ; \
		gzip -c $(PHC) | uuencode $(PHC).gz >> $(PHC).gz.uuencoded ; \
	fi
