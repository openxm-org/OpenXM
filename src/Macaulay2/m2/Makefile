#  $OpenXM$
OpenXM_HOME=../../../../OpenXM
all : ox_m2
	${CC} -o ox_m2  ox_m2.c

install : all
	-mkdir $(OpenXM_HOME)/lib/m2
	cp *.m2 $(OpenXM_HOME)/lib/m2
	cp ox_m2 $(OpenXM_HOME)/bin/ox_m2

install-for-debug : all
	/bin/rm -rf $(OpenXM_HOME)/lib/m2
	(cd $(OpenXM_HOME)/lib ; ln -s ../src/Macaulay2/m2 . )
	(cd $(OpenXM_HOME)/bin ; ln -s ../src/Macaulay2/m2/ox_m2 . )
	(cd $(OpenXM_HOME)/bin ; ln -s $(HOME)/tmp2/Macaulay2/work/Macaulay2/bin/M2 .)
