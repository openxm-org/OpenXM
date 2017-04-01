RSYNC=rsync -Cavz --safe-links

usage:
	@echo "check-hgm, build-hgm"
	@echo "build-hgmFB"
	@echo "rsync, copy-web, check-hgm-for-cran"

check-hgm: rsync
	R CMD check hgm
check-hgm-for-cran: rsync
	R CMD check --as-cran hgm
build-hgm: rsync
	rm -f hgm/src/@dont-edit-files
	R CMD build hgm
	touch hgm/src/@dont-edit-files

rsync: rsync-gsl-t-1 rsync-so3 rsync-mh rsync-orthant
rsync-gsl-t-1: 
	${RSYNC} ../../hgm/gsl-t-1/src/*.c hgm/src/
	${RSYNC} ../../hgm/gsl-t-1/src/*.h hgm/src/
rsync-so3:
	${RSYNC} ../../hgm/so3/src/*.c hgm/src/
rsync-mh:
	${RSYNC} --exclude test1.c --exclude test2.c --exclude genusage.c ../../hgm/mh/src/*.c hgm/src/
	${RSYNC} ../../hgm/mh/src/*.h hgm/src/
rsync-orthant:
	${RSYNC} ../../hgm/orthant/src/hgm_ko_orthant*.c  hgm/src/

check-hgmFB:
	R CMD check hgm_fb
build-hgmFB: 
	R CMD build hgm_fb
copy-web: copy-web-hgm copy-web-hgmFB
copy-web-hgm: build-hgm
	${RSYNC} hgm_*gz ${USER}@lemon.math.kobe-u.ac.jp:/home/web/OpenXM/Math/hgm
	${RSYNC} hgm.Rcheck/hgm-manual.pdf ${USER}@lemon.math.kobe-u.ac.jp:/home/web/OpenXM/Math/hgm

copy-web-hgmFB: build-hgmFB
	${RSYNC} hgmFB_*gz ${USER}@lemon.math.kobe-u.ac.jp:/home/web/OpenXM/Math/hgm
	${RSYNC} hgm_fb.Rcheck/hgmFB-manual.pdf ${USER}@lemon.math.kobe-u.ac.jp:/home/web/OpenXM/Math/hgm

clean:
	rm -f hgm/src/*.o hgm/src/*.so
