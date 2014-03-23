dyn.load("hgm_ko_orthant.so")

hgm.ko.ncorthant <- function(x,y,rk_step_size=1e-3)
      .C("hgm_ko_orthant_R",
	as.integer(nrow(x)),
	as.double(t(x)), 
	as.double(y),
	as.double(rk_step_size),
	result=double(1))$result

HgmKoNcorthant <- hgm.ko.ncorthant