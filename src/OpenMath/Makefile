#
# $OpenXM: OpenXM/src/OpenMath/Makefile,v 1.19 2000/01/21 14:50:04 tam Exp $

OpenXM_HOME = ../..
INSTALLDIR = $(OpenXM_HOME)/lib/java-support

#JAVADIR = /usr/local/jdk1.1.8/
#JAVABINDIR = $(JAVADIR)bin/
#JAVAINCLUDEDIR = $(JAVADIR)include
JAVAINCLUDEDIR = /usr/local/include/kaffe

JAVAC = $(JAVABINDIR)javac
JAVAH = $(JAVABINDIR)javah
JAR = $(JAVABINDIR)jar

TAR = tar

PKGSRCS =	JP/ac/kobe_u/math/tam/OpenXM/CMO.java \
	JP/ac/kobe_u/math/tam/OpenXM/CMO_BIGFLOAT.java \
	JP/ac/kobe_u/math/tam/OpenXM/CMO_DATUM.java \
	JP/ac/kobe_u/math/tam/OpenXM/CMO_DISTRIBUTED_POLYNOMIAL.java \
	JP/ac/kobe_u/math/tam/OpenXM/CMO_DMS.java \
	JP/ac/kobe_u/math/tam/OpenXM/CMO_DMS_GENERIC.java \
	JP/ac/kobe_u/math/tam/OpenXM/CMO_ERROR.java \
	JP/ac/kobe_u/math/tam/OpenXM/CMO_ERROR2.java \
	JP/ac/kobe_u/math/tam/OpenXM/CMO_INDETERMINATE.java \
	JP/ac/kobe_u/math/tam/OpenXM/CMO_INT32.java \
	JP/ac/kobe_u/math/tam/OpenXM/CMO_LIST.java \
	JP/ac/kobe_u/math/tam/OpenXM/CMO_MATHCAP.java \
	JP/ac/kobe_u/math/tam/OpenXM/CMO_MONOMIAL32.java \
	JP/ac/kobe_u/math/tam/OpenXM/CMO_NULL.java \
	JP/ac/kobe_u/math/tam/OpenXM/CMO_POLYNOMIAL_IN_ONE_VARIABLE.java \
	JP/ac/kobe_u/math/tam/OpenXM/CMO_QQ.java \
	JP/ac/kobe_u/math/tam/OpenXM/CMO_RATIONAL.java \
	JP/ac/kobe_u/math/tam/OpenXM/CMO_RECURSIVE_POLYNOMIAL.java \
	JP/ac/kobe_u/math/tam/OpenXM/CMO_STRING.java \
	JP/ac/kobe_u/math/tam/OpenXM/CMO_TREE.java \
	JP/ac/kobe_u/math/tam/OpenXM/CMO_ZERO.java \
	JP/ac/kobe_u/math/tam/OpenXM/CMO_ZZ.java \
	JP/ac/kobe_u/math/tam/OpenXM/DebugInputStream.java \
	JP/ac/kobe_u/math/tam/OpenXM/MathcapViolation.java \
	JP/ac/kobe_u/math/tam/OpenXM/OpenXM.java \
	JP/ac/kobe_u/math/tam/OpenXM/OpenXMconnection.java \
	JP/ac/kobe_u/math/tam/OpenXM/SM.java
PKGTARGETS =	JP/ac/kobe_u/math/tam/OpenXM/CMO.class \
	JP/ac/kobe_u/math/tam/OpenXM/CMO_BIGFLOAT.class \
	JP/ac/kobe_u/math/tam/OpenXM/CMO_DATUM.class \
	JP/ac/kobe_u/math/tam/OpenXM/CMO_DISTRIBUTED_POLYNOMIAL.class \
	JP/ac/kobe_u/math/tam/OpenXM/CMO_DMS.class \
	JP/ac/kobe_u/math/tam/OpenXM/CMO_DMS_GENERIC.class \
	JP/ac/kobe_u/math/tam/OpenXM/CMO_ERROR.class \
	JP/ac/kobe_u/math/tam/OpenXM/CMO_ERROR2.class \
	JP/ac/kobe_u/math/tam/OpenXM/CMO_INDETERMINATE.class \
	JP/ac/kobe_u/math/tam/OpenXM/CMO_INT32.class \
	JP/ac/kobe_u/math/tam/OpenXM/CMO_LIST.class \
	JP/ac/kobe_u/math/tam/OpenXM/CMO_MATHCAP.class \
	JP/ac/kobe_u/math/tam/OpenXM/CMO_MONOMIAL32.class \
	JP/ac/kobe_u/math/tam/OpenXM/CMO_NULL.class \
	JP/ac/kobe_u/math/tam/OpenXM/CMO_POLYNOMIAL_IN_ONE_VARIABLE.class \
	JP/ac/kobe_u/math/tam/OpenXM/CMO_QQ.class \
	JP/ac/kobe_u/math/tam/OpenXM/CMO_RATIONAL.class \
	JP/ac/kobe_u/math/tam/OpenXM/CMO_RECURSIVE_POLYNOMIAL.class \
	JP/ac/kobe_u/math/tam/OpenXM/CMO_STRING.class \
	JP/ac/kobe_u/math/tam/OpenXM/CMO_TREE.class \
	JP/ac/kobe_u/math/tam/OpenXM/CMO_ZERO.class \
	JP/ac/kobe_u/math/tam/OpenXM/CMO_ZZ.class \
	JP/ac/kobe_u/math/tam/OpenXM/DebugInputStream.class \
	JP/ac/kobe_u/math/tam/OpenXM/MathcapViolation.class \
	JP/ac/kobe_u/math/tam/OpenXM/OpenXM.class \
	JP/ac/kobe_u/math/tam/OpenXM/OpenXMconnection.class \
	JP/ac/kobe_u/math/tam/OpenXM/SM.class

TARGETS = OM2OXM.class OMproxy.class


all:	configure OpenXM.jar $(TARGETS)


configure:


clean:
	-rm -f *.jar *.class $(PKGTARGETS)


distclean:	clean
	find . -name '*~' \
		-exec echo rm -f {} \; \
		-exec rm -f {} \;


install:	all
	if [ ! -d $(INSTALLDIR) ]; then \
		mkdir -p $(INSTALLDIR); \
	fi
	$(TAR) cf - $(PKGTARGETS) $(TARGETS) | \
		(cd $(INSTALLDIR) ; $(TAR) xf -)


install2:


test:
	$(JAVAC) -d . -deprecation -g *.java


.SUFFIXES:	.java .class


.java.class:
	$(JAVAC) -d . -O $<


OpenXM.jar:	$(PKGSRCS)
	$(JAVAC) -d . -O $?
	@#touch $@
	-@#$(JAR) cf $@ $(PKGTARGETS)
