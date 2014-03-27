RSYNC=rsync -Cavz --safe-links

usage:
	@echo "check-hgm, build-hgm"
	@echo "rsync, copy-web"

check-hgm: rsync
	R CMD check hgm
build-hgm: rsync
	R CMD build hgm

rsync: rsync-gsl-t-1 rsync-so3 rsync-mh rsync-orthant
rsync-gsl-t-1: 
	${RSYNC} ../../hgm/gsl-t-1/src/*.c hgm/src/
	${RSYNC} ../../hgm/gsl-t-1/src/*.h hgm/src/
rsync-so3:
	${RSYNC} ../../hgm/so3/src/*.c hgm/src/
rsync-mh:
	${RSYNC} --exclude test1.c --exclude test2.c ../../hgm/mh/src/*.c hgm/src/
	${RSYNC} ../../hgm/mh/src/*.h hgm/src/
rsync-orthant:
	${RSYNC} ../../hgm/orthant/src/hgm_ko_orthant.c hgm/src/

copy-web: build-hgm
	${RSYNC} hgm*gz ${USER}@lemon.math.kobe-u.ac.jp:/home/web/OpenXM/Math/hgm

