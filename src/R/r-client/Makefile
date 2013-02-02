usage:
	@echo "make check"
	@echo "make build-r-package needs to clean everything and checkout again"


check:
	R CMD check Ropenxm 
build-src-package:
	R CMD build Ropenxm
build-binary-package:
	R CMD build --binary Ropenxm
# R CMD INSTALL Ropenxm
# R CMD REMOVE Ropenxm
copy: build-src-package build-binary-package
	scp Ropenxm*gz ${USER}@lemon.math.kobe-u.ac.jp:/home/web/HOME/${USER}/2012/tmp
clean:
	rm -rf Ropenxm.Rcheck