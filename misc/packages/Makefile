# $OpenXM: OpenXM/misc/packages/Makefile,v 1.4 2000/03/10 14:21:43 takayama Exp $

# Modify below for priting.
PS=a2ps-j
LPR=lpr -Pdousan

usage :
	@echo "Targets "
	@echo "  clean "
	@echo "  print "

clean :
	-(cd Generic/openxm ; make clean-work)
	-(cd FreeBSD ; make clean)
	-(cd Linux/RPM ; make clean)

generate-tar-balls:
	make clean
	(cd Generic/openxm ; make tar-ball)
	(cd FreeBSD/openxm-1.1.2 ; make make-tar-ball-for-openxm-port)
	(cd Linux/RPM ; make make-directory)
	(cd Linux/RPM ; make link-for-real-openxm)

print:
	$(PS) Generic/openxm/Makefile | $(LPR)	
	$(PS) Linux/RPM/Makefile | $(LPR)	
	$(PS) Linux/RPM/openxm-*spec.in | $(LPR)	
	$(PS) FreeBSD/openxm-ports/openxm-1.1.2/Makefile | $(LPR)	
