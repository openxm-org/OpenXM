# $OpenXM: OpenXM/misc/packages/Makefile,v 1.2 2000/03/09 09:26:24 takayama Exp $

# Modify below for priting.
PS=a2ps-j
LPR=lpr -Pdousan

usage :
	@echo "Targets "
	@echo "  clean "
	@echo "  print "

clean :
	-(cd Generic/openxm ; make clean)
	-(cd FreeBSD ; make clean)
	-(cd Linux/RPM ; make clean)

print:
	$(PS) Generic/openxm/Makefile | $(LPR)	
	$(PS) Linux/RPM/Makefile | $(LPR)	
	$(PS) Linux/RPM/openxm-*spec.in | $(LPR)	
	$(PS) FreeBSD/openxm-ports/openxm-1.1.2/Makefile | $(LPR)	
