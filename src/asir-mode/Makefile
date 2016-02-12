BINDIR=../../bin
SHAREDIR=../../share
OpenXM_contrib2=../../../OpenXM_contrib2
install:
	-mkdir -p ${SHAREDIR}/emacs ${BINDIR}
	install ${OpenXM_contrib2}/windows/post-msg-asirgui/asir-mode.el ${SHAREDIR}/emacs
	install ./use-asir-mode-local.txt ${SHAREDIR}/emacs
	install ./use-asir-mode.txt ${SHAREDIR}/emacs
	install ./use-asir-mode.sh ${BINDIR}
