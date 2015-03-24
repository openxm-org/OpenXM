# This Makefile is based on ~/OpenXM/src/hgm/so3/src/Makefile
#CC=gcc
HEADERS=config.h odeiv_util.h t-gsl_errno.h \
        t-gsl_math.h t-gsl_odeiv.h t-gsl_sys.h \
        t-gsl_types.h
hgm_ko_orthant: make-link hgm_ko_orthant.o 
	${CC} -o hgm_ko_orthant hgm_ko_orthant.o ../../gsl-t-1/src/gsl-t-1.a -lm -lblas -llapack
hgm_ko_orthant.o : hgm_ko_orthant.c
	${CC} -c -D_STANDALONE hgm_ko_orthant.c -D_VERBOSE

make-link: make-link-p
	-if [ ! -f .link_done ] ; then \
	for i in ${HEADERS}; do \
		ln -s ../../gsl-t-1/src/$$i . ; \
	done ; \
	touch .link_done ; \
	fi
make-link-p:
	-if [ ! -f .link_done_p ] ; then \
	ln -s ../../oxprint/oxprint.h . ; \
	touch .link_done_p ; \
	fi
clean:
	rm -f *.o *~ hgm_so3_nc  .link_done* 
	for i in ${HEADERS}; do \
		rm -f $$i ; \
	done 

