#
# $OpenXM$

make:
	#javac -nowarn -O OMproxy.java
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
	javac -d . -deprecation -O *.java
