# $OpenXM$

TIGERS = ../../../OpenXM_contrib/TiGERS_0.9
CURDIR = ../../OpenXM/src/tigers
BINDIR = ../../bin

all: patch
	@if [ ! -f ./.make_done ]; then \
		(cd $(TIGERS) ; make all) \
	fi
	@touch ./.make_done

install: all
	install -c -s $(TIGERS)/tigers $(BINDIR)/tigers

clean: patch-clean
	(cd $(TIGERS) ; rm -f *.o tigers)
	@rm -f ./.make_done

patch:
	@if [ ! -f ./.patch_done ]; then \
		(cd $(TIGERS) ; patch < $(CURDIR)/tigers.patch) \
	fi
	@touch ./.patch_done

patch-clean:
	@if [ -f ./.patch_done ]; then \
		(cd $(TIGERS) ; patch -R < $(CURDIR)/tigers.patch) \
	fi
	@rm -f ./.patch_done
