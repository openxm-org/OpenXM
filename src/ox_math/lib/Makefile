# $OpenXM$

OpenXM_HOME = ../../..
LIBRARIES = oxclient.m beta.m primadec.m

all:

install: install-binary

install-binary: ${LIBRARIES}
	-mkdir -p ${OpenXM_HOME}/lib/math
	cp -f ${LIBRARIES} ${OpenXM_HOME}/lib/math

install-for-debug: ${LIBRARIES}
	-mkdir -p ${OpenXM_HOME}/lib/math
	for i in ${LIBRARIES}; do \
		ln -s -f ../../src/ox_math/lib/$$i ${OpenXM_HOME}/lib/math; \
	done

clean-for-debug:
	-for i in ${OpenXM_HOME}/lib/math/*; do \
		test -L $$i && rm -f $$i; \
	done

clean:
	-rm -f *~
