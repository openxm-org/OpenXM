RSYNC=rsync -Cavz

usage:
	@echo "check-hgm, build-hgm"
	@echo "rsync, copy-web"

check-hgm: rsync-gsl-t-1 rsync-so3
	R CMD check hgm
build-hgm: rsync-gsl-t-1 rsync-so3
	R CMD build hgm

rsync: rsync-gsl-t-1 rsync-so3
rsync-gsl-t-1: 
	${RSYNC} ../../hgm/gsl-t-1/src/*.c hgm/src/
	${RSYNC} ../../hgm/gsl-t-1/src/*.h hgm/src/
rsync-so3:
	${RSYNC} ../../hgm/so3/src/*.c hgm/src/

copy-web: build-hgm
	scp hgm*gz ${USER}@lemon.math.kobe-u.ac.jp:/home/web/HOME/${USER}/2012/tmp

