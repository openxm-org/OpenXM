#
# $OpenXM: OpenXM/src/OpenMath/Makefile,v 1.5 1999/11/16 13:19:14 tam Exp $

make:
	if ! test -d ../../lib/java-support; then \
		mkdir -p ../../lib/java-support; \
	fi
	make clean
	javac -d ../../lib/java-support -O *.java

clean:
	find . -name '*.class' \
		-exec echo rm -f {} \; \
		-exec rm -f {} \;

dist:
	tar cvfz ../OM2OXM.tgz .

distclean: #clean dist
	make clean
	find . -name '*~' \
		-exec echo rm -f {} \; \
		-exec rm -f {} \;

test:
	/usr/local/jdk1.1.8/bin/javac -d . -deprecation -g *.java
