#
# $OpenXM: OpenXM/src/OpenMath/Makefile,v 1.3 1999/11/02 16:16:10 tam Exp $

make:
	if ! test -d ../../lib/java-support; then \
		mkdir -p ../../lib/java-support; \
	fi
	javac -d ../../lib/java-support -O *.java

clean:
	find . -name '*.class' \
		-exec echo rm -f {} \; \
		-exec rm -f {} \;

dist:
	tar cvfz ../OM2OXM.tgz .

dist-clean: #clean dist
	make clean
	find . -name '*~' \
		-exec echo rm -f {} \; \
		-exec rm -f {} \;

test:
	javac -d . -deprecation -g *.java
