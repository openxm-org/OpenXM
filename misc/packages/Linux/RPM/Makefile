# $OpenXM$
#
# Preparations to use build redhat package for OpenXM/
# 1. Generate ~/.rpmrc and set the TopDir for the work.
#    Here is my .rpmrc
#    TopDir: home/taka/OpenXM/misc/packages/Linux/RPM/work
#  [ To extract a source from *src.rpm to SOURCES dir, 
#    type in "make make-directory" and "rpm -i your.src.rpm" ]

usage :

make-directory: clean
	mkdir work
	mkdir work/SOURCES work/SPECS work/BUILD work/RPMS 
	mkdir work/RPMS/i386 work/SPRMS

clean :
	-/bin/rm -rf work
