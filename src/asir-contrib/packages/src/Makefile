# $OpenXM: OpenXM/src/asir-contrib/packages/src/Makefile,v 1.3 2000/02/28 10:47:47 takayama Exp $
# targets
# all : 
#     do nothing
# install :
#     copy all files to $(OpenXM_lib/asir-contrib
# clean :

OpenXM_ROOT=../../../../../OpenXM
OpenXM_lib=$(OpenXM_ROOT)/lib
## If you add a new source to be installed, add in the list below.
SOURCE=Matrix    fourier-0 m   sets Diff   bench-1   gnuplot   om     sm1  Dmodule   beta   igraph    phc    tigers longname dsolv


all :

clean :

install : clean-for-install
	cp $(SOURCE)  $(OpenXM_lib)/asir-contrib		

clean-for-install :
	/bin/rm -rf $(OpenXM_lib)/asir-contrib
	mkdir $(OpenXM_lib)/asir-contrib
	touch $(OpenXM_lib)/asir-contrib/DO_NOT_EDIT_THIS_DIRECTORY._ALL_FILES_ARE_AUTOMATICDALLY_GENERATED.
