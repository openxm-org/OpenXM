# $OpenXM: OpenXM/src/asir-contrib/packages/src/Makefile,v 1.9 2001/08/25 04:39:03 takayama Exp $
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
SOURCE2=povray.rr names.rr  taka_print.rr la_solution_guide.rr taka_base.rr datatype.rr taka_poly.rr tags.h tex_symbols.rr
## SOURCE: documents are embedded.
## SOURCE2: documents are NOT embedded.



all :

clean :

install : clean-for-install
	cp $(SOURCE)  $(OpenXM_lib)/asir-contrib		
	cp $(SOURCE2)  $(OpenXM_lib)/asir-contrib		

clean-for-install :
	/bin/rm -rf $(OpenXM_lib)/asir-contrib
	mkdir $(OpenXM_lib)/asir-contrib
	touch $(OpenXM_lib)/asir-contrib/DO_NOT_EDIT_THIS_DIRECTORY._ALL_FILES_ARE_AUTOMATICDALLY_GENERATED.
