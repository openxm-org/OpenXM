# $OpenXM: OpenXM/src/asir-mode/Makefile,v 1.5 2019/09/30 06:11:05 takayama Exp $
BINDIR=../../bin
SHAREDIR=../../share
OpenXM_contrib2=../../../OpenXM_contrib2
install:
	-mkdir -p ${SHAREDIR}/emacs ${BINDIR}
	install ${OpenXM_contrib2}/windows/post-msg-asirgui/asir-mode.el ${SHAREDIR}/emacs
	install ${OpenXM_contrib2}/windows/post-msg-asirgui/asir-mode-v26.el.diff ${SHAREDIR}/emacs
	install ${OpenXM_contrib2}/windows/post-msg-asirgui/asir-mode-v28.el ${SHAREDIR}/emacs
	install ${OpenXM_contrib2}/windows/post-msg-asirgui/asir-mode-v29.el ${SHAREDIR}/emacs
	install ./use-asir-mode-local.txt ${SHAREDIR}/emacs
	install ./use-asir-mode.txt ${SHAREDIR}/emacs
	install ./use-asir-mode.sh ${BINDIR}

# build distribution for emacs/asir on Mac
# We assume that "make install" is done under OpenXM/src
# If OpenXM/src is built by make install-for-debug", the following script causes an error.
build-for-mac:
	rm -rf work
	mkdir work
	-cp install.txt work
	(cd work ; (cd ${OpenXM_HOME}/.. ; \
		  tar cf - OpenXM/lib/libgc* OpenXM/lib/libgmp* OpenXM/rc \
		    OpenXM/bin \
			OpenXM/lib/cfep OpenXM/lib/sm1/bin \
			OpenXM/src/jupyter OpenXM/src/cfep/Samples OpenXM/share)  | tar xvf -)
	(cd work ; (cd ${OpenXM_HOME}/.. ; \
		  tar cf - OpenXM/lib/asir  )  | tar xvf -)
	(cd work/OpenXM/rc ; make clean)
	mkdir work/OpenXM/lib/asir-contrib
	(cd work/OpenXM/lib/asir-contrib ; (cd ${OpenXM_HOME}/src/asir-contrib/packages/src ; tar cf - * )  | tar xvf -)
	rm -f work/OpenXM/lib/asir-contrib/noro_rewrite.rr
	(cp ${OpenXM_HOME}/src/asir-contrib/testing/noro/new_rewrite.rr work/OpenXM/lib/asir-contrib/noro_rewrite.rr)
	rm -f work/OpenXM/bin/ox work/OpenXM/bin/asir
	(cp ${OpenXM_HOME}/bin/ox work/OpenXM/bin)
	(cp ${OpenXM_HOME}/bin/asir work/OpenXM/bin)
	(cp ${OpenXM_HOME}/bin/ox_sm1 work/OpenXM/bin)
	(cp ${OpenXM_HOME}/bin/ox_texmacs work/OpenXM/bin)
	(cp ${OpenXM_HOME}/bin/ox100start work/OpenXM/bin)
	(cp ${OpenXM_HOME}/bin/ox work/OpenXM/bin)
	(cp ${OpenXM_HOME}/bin/oxlog work/OpenXM/bin)
	-(cp ${OpenXM_HOME}/bin/nkf work/OpenXM/bin)
	(cp ${OpenXM_HOME}/bin/sm1 work/OpenXM/bin)
	(cp ${OpenXM_HOME}/bin/ox_k0 work/OpenXM/bin)
	(cp ${OpenXM_HOME}/bin/ox_cpp work/OpenXM/bin)
	(cp ${OpenXM_HOME}/bin/ox_pari work/OpenXM/bin)
	(mkdir -p work/OpenXM/lib/sm1 ; cp -r ${OpenXM_HOME}/lib/sm1/*.sm1 work/OpenXM/lib/sm1)
	mkdir -p work/Samples
	(cp ${OpenXM_HOME}/src/cfep/Samples/circle*.rr work/Samples)
	@echo "  "
	@echo "  "
	@echo "Generate diskimage by hand. Name should be emacs-asir.dmg.gz"
	@echo "Copy work/OpenXM work/Samples to the image."	

clean:
	rm -rf work

