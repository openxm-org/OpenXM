# $OpenXM$
# targets
# all : 
#     do nothing
# install :
#     copy all files to $(OpenXM_lib/asir-contrib

OpenXM_ROOT=../../../../../OpenXM
OpenXM_lib=$(OpenXM_ROOT)/lib
SOURCE=Matrix    fourier-0 m   sets Diff   bench-1   gnuplot   om     sm1  Dmodule   beta   igraph    phc    tigers


all :

install : clean-for-install
	cp $(SOURCE)  $(OpenXM_lib)/asir-contrib		

clean-for-install :
	/bin/rm -rf $(OpenXM_lib)/asir-contrib
	mkdir $(OpenXM_lib)/asir-contrib
	touch $(OpenXM_lib)/asir-contrib/DO_NOT_EDIT_THIS_DIRECTORY._ALL_FILES_ARE_AUTOMATICDALLY_GENERATED.
