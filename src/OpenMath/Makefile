#
# $OpenXM: OpenXM/src/OpenMath/Makefile,v 1.35 2000/09/13 06:44:55 tam Exp $

OpenXM_HOME = ../..
INSTALLDIR = $(OpenXM_HOME)/lib/java-support

#JAVADIR = /usr/local/jdk1.1.8/
#JAVABINDIR = $(JAVADIR)bin/
#JAVAINCLUDEDIR = $(JAVADIR)include
#JAVAINCLUDEDIR = /usr/local/include/kaffe

JAVAC = $(JAVABINDIR)javac
JAVAH = $(JAVABINDIR)javah
JAR = $(JAVABINDIR)jar

PKGSRCS =	\
	ORG/openxm/tam/CMO.java \
	ORG/openxm/tam/CMO_BIGFLOAT.java \
	ORG/openxm/tam/CMO_DATUM.java \
	ORG/openxm/tam/CMO_DISTRIBUTED_POLYNOMIAL.java \
	ORG/openxm/tam/CMO_DMS.java \
	ORG/openxm/tam/CMO_DMS_GENERIC.java \
	ORG/openxm/tam/CMO_ERROR2.java \
	ORG/openxm/tam/CMO_INDETERMINATE.java \
	ORG/openxm/tam/CMO_INT32.java \
	ORG/openxm/tam/CMO_LIST.java \
	ORG/openxm/tam/CMO_MATHCAP.java \
	ORG/openxm/tam/CMO_MONOMIAL32.java \
	ORG/openxm/tam/CMO_NULL.java \
	ORG/openxm/tam/CMO_POLYNOMIAL_IN_ONE_VARIABLE.java \
	ORG/openxm/tam/CMO_QQ.java \
	ORG/openxm/tam/CMO_RATIONAL.java \
	ORG/openxm/tam/CMO_RECURSIVE_POLYNOMIAL.java \
	ORG/openxm/tam/CMO_STRING.java \
	ORG/openxm/tam/CMO_TREE.java \
	ORG/openxm/tam/CMO_ZERO.java \
	ORG/openxm/tam/CMO_ZZ.java \
	ORG/openxm/tam/MathcapViolation.java \
	ORG/openxm/tam/OXbody.java \
	ORG/openxm/tam/OXmessage.java \
	ORG/openxm/tam/OpenXM.java \
	ORG/openxm/tam/OpenXMserver.java \
	ORG/openxm/tam/OpenXMstream.java \
	ORG/openxm/tam/SM.java

PKGTARGETS =	ORG/openxm/tam/CMO.class \
	ORG/openxm/tam/CMO_BIGFLOAT.class \
	ORG/openxm/tam/CMO_DATUM.class \
	ORG/openxm/tam/CMO_DISTRIBUTED_POLYNOMIAL.class \
	ORG/openxm/tam/CMO_DMS.class \
	ORG/openxm/tam/CMO_DMS_GENERIC.class \
	ORG/openxm/tam/CMO_ERROR2.class \
	ORG/openxm/tam/CMO_INDETERMINATE.class \
	ORG/openxm/tam/CMO_INT32.class \
	ORG/openxm/tam/CMO_LIST.class \
	ORG/openxm/tam/CMO_MATHCAP.class \
	ORG/openxm/tam/CMO_MONOMIAL32.class \
	ORG/openxm/tam/CMO_NULL.class \
	ORG/openxm/tam/CMO_POLYNOMIAL_IN_ONE_VARIABLE.class \
	ORG/openxm/tam/CMO_QQ.class \
	ORG/openxm/tam/CMO_RATIONAL.class \
	ORG/openxm/tam/CMO_RECURSIVE_POLYNOMIAL.class \
	ORG/openxm/tam/CMO_STRING.class \
	ORG/openxm/tam/CMO_TREE.class \
	ORG/openxm/tam/CMO_ZERO.class \
	ORG/openxm/tam/CMO_ZZ.class \
	ORG/openxm/tam/MathcapViolation.class \
	ORG/openxm/tam/OpenXM.class \
	ORG/openxm/tam/OpenXMserver.class \
	ORG/openxm/tam/OpenXMstream.class \
	ORG/openxm/tam/OXbody.class \
	ORG/openxm/tam/OXmessage.class \
	ORG/openxm/tam/SM.class

TARGETS = OM2OXM.class OMproxy.class

.SUFFIXES: .java .class

.java.class:
	$(JAVAC) -d . -g -O $<

all:	clean configure OpenXM.jar $(TARGETS)

configure:

clean:
	-find . -name '*.class' | xargs rm -f *.jar

distclean:	clean
	-find . -name '*~' | xargs rm -f

install:	all
	-mkdir -p $(INSTALLDIR)
	find . -name '*.class' | \
	xargs tar cf - $(TARGETS) | \
	(cd $(INSTALLDIR) ; tar xf -)

install2:

test:
	$(JAVAC) -d . -deprecation -g *.java

OpenXM.jar:	$(PKGSRCS)
	-$(JAVAC) -d . -g -O $?
#	-@touch $@
#	-@$(JAR) cf $@ $(PKGTARGETS)
