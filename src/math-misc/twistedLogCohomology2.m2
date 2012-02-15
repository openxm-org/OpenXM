(J-- source code from Dbasic.m2(B
(J-- puts a module or matrix purely in shift degree 0.(B

(Jzeroize2 = method()(B
(Jzeroize2 Module := M -> ((B
(J     W := ring M;(B
(J     P := presentation M;(B
(J     coker map(W^(numgens target P), W^(numgens source P), P)(B
(J     )(B

(Jzeroize2 Matrix := m -> ((B
(J     W := ring m;(B
(J     map(W^(numgens target m), W^(numgens source m), m)(B
(J     )(B

(J-- source code from Dresolution.m2(B
(J-- modified Dresolution(B
(J-- return hashTable{Dresolution,Transfermap}(B

(Jshifts := (m, w, oldshifts) -> ((B
(J     tempmat := compress leadTerm m;(B
(J     if numgens source tempmat == 0 then newshifts := {}(B
(J     else ((B
(J     (B	(J  expmat := matrix(apply(toList(0..numgens source tempmat - 1), (B
		(J    i -> (k := leadComponent tempmat_i;(B
	(J       (B	(J    (B	(J append((exponents tempmat_(k,i))#0, oldshifts#k))));(B
(J     (B	(J  newshifts = (entries transpose ((B
	(J     (B	(J    expmat*(transpose matrix{ append(w, 1) })) )#0;(B
(J     (B	(J  );(B
(J     newshifts)(B

(Jdebug Core(B

(JkerGB := m -> ((B
(J     -- m should be a matrix which is a GB, and(B
(J     -- whose source has the Schreyer order.(B
(J     -- The resulting map will have the same form.(B
(J     map(ring m, rawKernelOfGB raw m)(B
(J     )(B

(JDresolution2 = method( Options => {Strategy => Schreyer, LengthLimit => infinity} )(B
(JDresolution2 Ideal := options -> I -> ((B
(J     Dresolution2((ring I)^1/I, options)(B
(J     )(B

(JDresolution2 Module := options -> M -> ((B

(J     pInfo (1, "ENTERING Dresolution ... ");(B
(J     (B
(J     W := ring M;(B
(J     N := presentation M;(B

(J     pInfo (1, "Computing usual resolution using Schreyer order ..."); (B
(J     pInfo (2, "\t Degree " | 0 | "...");(B
(J     pInfo (2, "\t\t\t Rank = " | rank target N | "\t time = 0. seconds");(B
(J     pInfo (2, "\t Degree " | 1 | "...");(B
(J     tInfo := toString first timing (m := schreyerOrder gens gb N);(B
(J     pInfo (2, "\t\t\t Rank = " | rank source N | "\t time = " |(B
	(J  tInfo | " seconds");(B

(J     M.cache.resolution = new ChainComplex;(B
(J     M.cache.resolution.ring = W;(B
(J     M.cache.resolution#0 = target m;(B
(J     M.cache.resolution#1 = source m;(B
(J     M.cache.resolution.dd#0 = map(W^0, target m, 0);(B
(J     M.cache.resolution.dd#1 = m;(B

(J     i := 2;(B
(J     while source m != 0 and i <= options.LengthLimit do ((B
	(J  pInfo (2, "\t Degree " | i | "...");(B
	(J  tInfo = toString first timing (m = kerGB m);(B
(J     (B	(J  M.cache.resolution#i = source m;(B
(J     (B	(J  M.cache.resolution.dd#i = m;(B
	(J  pInfo(2, "\t\t\t Rank = " | rank source m | "\t time = " |(B
	(J       tInfo | " seconds");(B
	(J  i = i+1;(B
	(J  );(B
(J     M.cache.resolution.length = i-1;(B
(J     M.cache.resolution(B
(J     )(B

(JDresolution2 (Ideal, List) := options -> (I, w) -> ((B
(J     Dresolution2 ((ring I)^1/I, w, options)(B
(J     )(B

(JDresolution2 (Module, List) := options -> (M, w) -> ((B

(J     pInfo (1, "ENTERING Dresolution ... ");(B
(J     (B
(J     -- ERROR CHECKING:(B
(J     W := ring M;(B
(J     k := options.LengthLimit;(B

(J     -- check that W is a Weyl algebra(B
(J     if W.monoid.Options.WeylAlgebra == {}(B
(J     then error "expected a Weyl algebra";(B
(J     if any(W.monoid.Options.WeylAlgebra, v -> class v =!= Option)(B
(J     then error "expected non-homogenized Weyl algebra";(B
(J     -- check that w is of the form (-u,u)(B
(J     createDpairs W;(B
(J     if #w =!= numgens W(B
(J     then error ("expected weight vector of length " | numgens W);(B
(J     if any( toList(0..#W.dpairInds#0 - 1),(B
	(J  i -> ( w#(W.dpairInds#0#i) + w#(W.dpairInds#1#i) != 0 ) )(B
(J     then error "expected weight vector of the form (-u,u)";(B

(J     -- PREPROCESSING(B
(J     if k == infinity then ((B
	(J  pInfo (1, "Computing adapted free resolution of length infinity using " (B
	(J       | toString options.Strategy | " method...");(B
	(J  if (options.Strategy == Vhomogenize) then(B
(J     (B	(J  pInfo(2, "Warning: resolution via Vhomogenize might not terminate");(B
	(J  )(B
(J     else pInfo (1, "Computing adapted free resolution of length " | k | (B
	(J  " using " | toString options.Strategy | " method...");(B

(J     homVar := symbol homVar;(B
(J     hvw := symbol hvw;(B
(J     if options.Strategy == Schreyer then ((B
(J    (B	(J  -- Make the homogenizing weight vector in HW(B
	(J  Hwt := toList(numgens W + 1:1);(B
(J     (B	(J  -- Make the V-filtration weight vector in HW(B
(J     (B	(J  Vwt := append(w,0);(B
(J     (B	(J  -- Make the homogeneous Weyl algebra(B
(J     (B	(J  HW := (coefficientRing W)(monoid [(entries vars W)#0, homVar,(B
	(J       WeylAlgebra => append(W.monoid.Options.WeylAlgebra, homVar),(B
	(J       MonomialOrder => {Weights=>Hwt, Weights=>Vwt, GRevLex}]);(B
(J     (B	(J  homVar = HW_homVar;(B
(J     (B	(J  WtoHW := map(HW, W, (vars HW)_{0..numgens W - 1});(B
(J     (B	(J  HWtoW := map(W, HW, (vars W)_{0..numgens W - 1} | matrix{{1_W}});(B
	(J  -- Also make the homogenizing Weyl algebra for shifts(B
(J     (B	(J  VW := (coefficientRing W)(monoid [hvw, (entries vars W)#0,(B
	(J       WeylAlgebra => W.monoid.Options.WeylAlgebra,(B
	(J       MonomialOrder => Eliminate 1]);(B
(J     (B	(J  HWtoVW := map(VW, HW, (vars VW)_{1..numgens W} | matrix{{VW_0}});(B
(J     (B	(J  VWtoHW := map(HW, VW, matrix{{homVar}} | (vars HW)_{0..numgens HW - 2});(B
	(J  hvwVar := VW_0;(B
	(J  HVWwt := prepend(-1,w);(B
	(J  VWwt := prepend(0,w);(B
	(J  )(B
(J     else if options.Strategy == Vhomogenize then ((B
(J     (B	(J  Hwt = prepend(-1,w);(B
(J     (B	(J  Vwt = prepend(0,w);(B
	(J  -- make the homogenizing Weyl algebra(B
(J     (B	(J  HW = (coefficientRing W)(monoid [homVar, (entries vars W)#0,(B
	(J       WeylAlgebra => W.monoid.Options.WeylAlgebra,(B
	(J       MonomialOrder => Eliminate 1]);(B
(J     (B	(J  homVar = HW_homVar;(B
(J     (B	(J  WtoHW = map(HW, W, (vars HW)_{1..numgens W});(B
(J     (B	(J  HWtoW = map(W, HW, matrix{{1_W}} | (vars W));(B
	(J  );(B

(J     -- CREATE AND INITIALIZE THE CHAIN COMPLEX(B
(J     --else (B
(J     N := presentation M;(B
(J     --if (isSubmodule M) then N := presentation ((ambient M)/M);(B
(J     -- get the degree shifts right (need to check this against OT paper)(B
(J     if not M.cache.?resolution (B
(J     then M.cache.resolution = new MutableHashTable;(B
(J     M.cache.resolution#w = new ChainComplex;(B
(J     M.cache.resolution#w.ring = W;(B
(J     s := rank source N;(B
(J     t := rank target N;(B
(J     M.cache.resolution#w#0 = target N;(B
(J     M.cache.resolution#w.dd#0 = map(W^0, M.cache.resolution#w#0, 0);(B

(J     -- MAKE THE FIRST STEP OF THE RESOLUTION(B
(J     shiftvec := apply(degrees target N, i -> i#0); (B
(J     tempMap := map(HW^(-shiftvec), HW^(rank source N), WtoHW N);(B
(J     pInfo (2, "\t Degree 0...");(B
(J     pInfo (2, "\t\t\t Rank = " | t | "\t time = 0 seconds");(B
(J     pInfo (3, "\t Degree 1...");(B
(J     tInfo := toString first timing ((B
	(J  Jgb := gb(homogenize(tempMap, homVar, Hwt), ChangeMatrix => true);(B
(J      transfermap := getChangeMatrix Jgb;(B
(J      Jgb = homogenize(tempMap, homVar, Hwt)*transfermap;(B
	(J  if options.Strategy == Schreyer then Jgb = schreyerOrder Jgb;(B
	(J  if options.Strategy == Schreyer then ((B
	(J       tempMat := map(VW^(-shiftvec), VW^(numgens source Jgb), HWtoVW(Jgb));(B
	(J       shiftvec = shifts(homogenize(HWtoVW Jgb, hvwVar, HVWwt),(B
		(J    VWwt, shiftvec);(B
	(J       )(B
	(J  else shiftvec = shifts(Jgb, Vwt, shiftvec);(B
(J      transfermap = HWtoW transfermap;(B
	(J  M.cache.resolution#w#1 = W^(-shiftvec);(B
	(J  M.cache.resolution#w.dd#1 = map(M.cache.resolution#w#0, (B
	(J       M.cache.resolution#w#1, HWtoW Jgb); (B
	(J  );(B
(J     pInfo(2, "\t\t\t Rank = " | #shiftvec | "\t time = " |(B
	(J  tInfo | " seconds");(B
(J     startDeg := 2;(B
	(J  (B
(J     -- COMPUTE REST OF THE RESOLUTION(B
(J     i := startDeg;(B
(J     while i < k+1 and numgens source Jgb != 0 do ((B
	(J  pInfo (2, "\t Degree " | i | "...");(B
	(J  tInfo = toString first timing ((B
	(J       if options.Strategy == Schreyer then Jgb = kerGB Jgb(B
(J     (B	(J       else if options.Strategy == Vhomogenize then ((B
	(J     (B	(J    -- compute the kernel / syzygies(B
(J     (B	(J     (B	(J    Jsyz := syz Jgb;(B
	(J     (B	(J    -- put syzygies in the free module with the correct degree shifts(B
	(J     (B	(J    Jsyzmap := map(HW^(-shiftvec), HW^(numgens source Jsyz), Jsyz);(B
	(J     (B	(J    -- compute an adapted (-w,w)-GB of the syzygies module(B
(J     (B	(J     (B	(J    Jgb = gens gb homogenize(Jsyzmap, homVar, Hwt);(B
	(J     (B	(J    );(B
	(J       if options.Strategy == Schreyer then ((B
	(J       (B	(J    tempMat = map(VW^(-shiftvec), VW^(numgens source Jgb), HWtoVW(Jgb));(B
	(J       (B	(J    shiftvec = shifts(homogenize(tempMat, hvwVar, HVWwt), (B
		(J       (B	(J VWwt, shiftvec);(B
	(J       (B	(J    )(B
	(J       else shiftvec = shifts(Jgb, Vwt, shiftvec);(B
	(J       M.cache.resolution#w#i = W^(-shiftvec);(B
	(J       M.cache.resolution#w.dd#i = map(M.cache.resolution#w#(i-1),(B
	(J       (B	(J    M.cache.resolution#w#i, HWtoW Jgb);(B
	(J       );(B
	(J  pInfo(2, "\t\t\t Rank = " | #shiftvec | "\t time = " |(B
	(J       tInfo | " seconds");(B
	(J  i = i+1;(B
(J     (B	(J  );(B
(J     hashTable {VResolution =>M.cache.resolution#w, (B
(J     TransferMap => transfermap}(B
(J     )(B


(J-- This routine computes a monomial from a list of variables (B
(J-- and an exponent vector(B
(JList ^ List := (Vars, Exps) -> ((B
(J     product (Vars, Exps, (i,j) -> (i^j)) )(B

(J-- This routine takes a weight vector w of strictly positive integers(B
(J-- and returns the exponents beta in N^n such that (B
(J-- ( k_0 < w_1 beta_1 + ... + w_n bet_n <= k_1) (B
(JfindExps := (w, k0, k1) -> ((B
(J     local minimum, local alpha, local tempExps;(B
(J     -- base case: weight vector w has dim 1(B
(J     if #w == 1 and k0 >= 0 then ((B
(J     (B	(J  tempExps = (toList((k0//w#0+1)..k1//w#0) / (i -> {i}) ) )(B
(J     else if #w == 1 and k0 < 0 then ((B
	(J  tempExps = ( toList(0..k1//w#0) / (i -> {i}) ) )(B
(J     else ( -- general case(B
	(J  tempExps = {};(B
	(J  alpha = 0;(B
	(J  while alpha <= k1//w#0 do ((B
	(J       tempExps = join( tempExps,(B
		(J    apply ( findExps( drop(w,1), k0-alpha*(w#0), (B
			(J      k1-alpha*(w#0) ), j -> prepend(alpha,j) ) );(B
	(J       alpha = alpha+1;(B
	(J       );(B
	(J  );(B
(J     tempExps)(B


(J-- modified computeRestriction(B

(JcomputeRestriction2 = (M,wt,n0,n1,output,options) -> ((B

(J-- ERROR CHECKING(B
(J     W := ring M;(B
(J     createDpairs W;(B
(J     -- check weight vector(B
(J     if #wt != #W.dpairInds#0(B
(J     then error ("expected weight vector of length " | #W.dpairInds#0);(B
(J     if any(wt, i -> (i<0))(B
(J     then error "expected non-negative weight vector";(B

(J-- PREPROCESSING(B
(J     local tInfo;(B
(J     local tempvar;(B
(J     tempvar = symbol tempvar;(B
(J     tInfo = "";(B
(J     nW := numgens W;(B
(J     -- make the (-w,w) weight vector(B
(J     w := new MutableList from join(W.dpairInds#0,W.dpairInds#1);(B
(J     i := 0;(B
(J     while i < #W.dpairInds#0 do ((B
	(J  w#(W.dpairInds#0#i) = -wt#i;(B
	(J  w#(W.dpairInds#1#i) = wt#i; (B
	(J  i = i+1;(B
	(J  );(B
(J     w = toList w;(B
(J     d := #positions(w, i->(i>0));(B
(J     -- the variables t_1, ..., t_d(B
(J     negVars := (entries vars W)#0_(positions(w, i->(i<0)));(B
(J     -- the substitution which sets t_1 = ... = t_d = 0(B
(J     resSub := apply( negVars, i -> (i => 0) );(B
(J     -- the variables Dt_1, ..., Dt_d, their indices, and their weights(B
(J     posVars := (entries vars W)#0_(positions(w, i->(i>0)));(B
(J     posInds := positions( w, i->(i>0) );(B
(J     posWeights := select( w, i->(i>0) );(B
(J     diffSub := apply( posVars, i -> (i => 0) );(B
(J     -- the rest of the variables x_1, ..., x_n, D_1, ..., D_n (B
(J     otherVars := (entries vars W)#0_(positions(w, i->(i==0)));(B


(J-- MAKE THE WEYL ALGEBRA "resW" OF THE RESTRICTED SUBSPACE(B
(J     -- Case 1: if restriction to pt, then "resW" a field(B
(J     if #otherVars == 0 then ((B
	(J  resW := coefficientRing W;(B
	(J  WtoresW := map(resW, W, matrix{toList(numgens W: 0_resW)});(B
	(J  resWtoW := map(W, resW)(B
	(J  )(B
(J     -- Case 2: if restriction to coordinate subspace of dim d, then(B
(J     --         resW a Weyl algebra D_d.(B
(J     else (i = 0;(B
(J     (B	(J  resPairsList := {};(B
(J     (B	(J  while i < #otherVars do ((B
	(J       deriv := select(otherVars, j -> (B
		(J    (j*otherVars#i - otherVars#i*j == 1));(B
	(J       if (#deriv == 1) then (B
	(J       resPairsList = append(resPairsList, otherVars#i=>deriv#0);(B
	(J       i = i+1;(B
	(J       );(B
(J     (B	(J  resW = (coefficientRing W)(monoid [otherVars, WeylAlgebra=>resPairsList]);(B
(J     (B	(J  -- make the inclusion ring map "WtoresW" mapping W --> resW(B
(J     (B	(J  counter := 0;(B
(J     (B	(J  tempList := {};(B
	(J  WList := {};(B
	(J  i = 0;(B
(J     (B	(J  while i < numgens W do ((B
	(J       if w#i == 0 then ((B
	(J       (B	(J    tempList = append(tempList, resW_counter);(B
		(J    WList = append(WList, W_i);(B
	(J       (B	(J    counter = counter+1;)(B
	(J       else (tempList = append(tempList, 0_resW));(B
	(J       i = i+1;(B
	(J       );(B
(J     (B	(J  WtoresW = map(resW, W, matrix{tempList});(B
	(J  resWtoW = map(W, resW, matrix{WList});(B
	(J  );(B

(J-- INITIALIZE THE OUTPUT FORMS(B

(J     if member(HomologyModules, output) then homologyList := {};(B
(J     if member(Cycles, output) then kerList := {};(B
(J     if member(Boundaries, output) then imList := {};(B
(J     if member(GenCycles, output) then explicitList := {};(B
(J     if member(RestrictComplex,output) then ((B
	(J  restrictComplex := new ChainComplex;(B
	(J  restrictComplex.ring = resW;(B
	(J  );(B
(J     outputList := {};(B
(J     if member(Cycles, output) or member(Boundaries, output) or(B
(J     member(GenCycles, output) then explicitFlag := true (B
(J     else explicitFlag = false;(B

(J-- GET MIN AND MAX ROOTS OF THE B-FUNCTION(B
(J     --<< "Computing b-function ";(B
(J     if n0 >= d then b := 1_(QQ[tempvar])(B
(J     else if rank ambient M == 1 then ((B
	(J  -- used to use "ideal presentation M" here(B
	(J  --<< "using ideal bFunction ";(B
	(J  pInfo(1, "Computing b-function using ideal bFunction... ");(B
(J     (B	(J  tInfo = toString first timing((B
	(J       b = bFunction(ideal relations M, wt);(B
	(J       );(B
	(J  )(B
(J     else ((B
	(J  pInfo(1, "Computing b-function using module bFunction... ");(B
	(J  tInfo = toString first timing((B
	(J       b = bFunctionM(M, wt, toList(rank ambient M: 0));(B
	(J       );(B
	(J  );(B
(J     (B
(J     if b == 0 then ((B
(J     (B	(J  error "Module not specializable. Restriction cannot be computed.";(B
	(J  );(B

(J     intRoots := getIntRoots b;(B
	(J  (B
(J-- NO INTEGER ROOTS(B
(J     if #intRoots == 0 then ((B
(J     (B	(J  k0 := 0;(B
(J     (B	(J  k1 := 0;(B
(J     (B	(J  pInfo(4, "\t bFunction = " | toString b);(B
(J     (B	(J  pInfo(2, "\t No integer roots");(B
(J     (B	(J  pInfo(3, "\t time = " | tInfo); (B
	(J  if member(RestrictComplex, output) then ((B
	(J       restrictComplex#n0 = resW^0;(B
	(J       restrictComplex#n1 = resW^0;(B
	(J       restrictComplex.dd#n0 = map(resW^0,resW^0,0);(B
	(J       restrictComplex.dd#n1 = map(resW^0,resW^0,0);(B
	(J       i = n0+1;(B
	(J       while i < n1 do ((B
	(J       (B	(J    restrictComplex#i = resW^0;(B
	(J       (B	(J    restrictComplex.dd#i = map(resW^0,resW^0,0);(B
	(J       (B	(J    i = i+1;(B
	(J       (B	(J    );(B
	(J       );(B
	(J  if member(HomologyModules, output) then(B
(J     (B	(J  homologyList = apply (toList(n0+1..n1-1), i ->  (i => resW^0));(B
	(J  if member(Cycles, output) then(B
(J     (B	(J  kerList = apply (toList(n0+1..n1-1), i ->  (i => gens W^0));(B
	(J  if member(Boundaries, output) then(B
(J     (B	(J  imList = apply (toList(n0+1..n1-1), i ->  (i => gens W^0));(B
	(J  if member(GenCycles, output) then(B
(J     (B	(J  explicitList = apply (toList(n0+1..n1-1), i ->  (i => gens W^0));(B
	(J  )(B
(J     (B
(J-- INTEGER ROOTS EXIST(B
(J     else ((B
(J     k0 = min intRoots - 1;(B
(J     k1 = max intRoots;(B
(J     pInfo(4, "\t bFunction = " | toString b);(B
(J     pInfo(2, "\t min root =  " | k0+1 | " , max root =  " | k1);(B
(J     pInfo(3, "\t time = " | tInfo | " seconds");(B
(J     pInfo(2, " ");(B

(J----- SET k0 TO -infinity FOR EXPLICIT COHOMOLOGY CLASSES -----(B
(J     if member(Explicit, output) then k0 = -infinity;(B

(J-- COMPUTE FREE RESOLUTION ADAPTED TO THE WEIGHT VECTOR "w"(B
(J     tInfo = toString first timing ((B
(J     (B	(J  Resolution := Dresolution2 (M, w, LengthLimit => n1, Strategy => options.Strategy);(B
(J          C := Resolution#VResolution;(B
	(J  );(B
(J     pInfo(2, "\t Finished...");(B
(J     pInfo(2, "\t\t\t Total time = " | tInfo | " seconds");(B
(J     pInfo(2, " ");(B


(J-- COMPUTE THE RESTRICTED COMPLEX IN DEGREES "n0" TO "n1" (B
(J     tInfo = toString first timing ((B
(J     pInfo(1, "Computing induced restriction complex in degrees " |(B
	(J  n0 | " to " | n1 | "...");(B

(J     -- INITIALIZE THE ITERATION : making the first differential(B
(J     pInfo(2, "\t Degree " | n0+1 | "...");(B

(J     -- MAKE THE TARGET MODULE AS DIRECT SUM OF D_m MODULES(B
(J     -- "targetGens" is a list of s lists, where the i-th list contains(B
(J     -- the monomial generators {dx^a} (as left D_m-module) of(B
(J     --      F_k1[u_i]((D_n/tD_n) e_i) / F_k0[u_i]((D_n/tD_n) e_i)(B
(J     tInfo = toString first timing ((B
(J     (B	(J  s := numgens target C.dd#(n0+1);(B
(J     (B	(J  targetDeg := degrees target C.dd#(n0+1);(B
(J     (B	(J  targetGens := {};(B
(J     (B	(J  if explicitFlag then targetMat := map(W^0,W^0,0);(B
(J     (B	(J  i = 0;(B
(J     (B	(J  while i < s do ((B
	(J       tempExps := findExps(posWeights, k0-targetDeg#i#0, k1-targetDeg#i#0);(B
	(J       tempGens := apply(tempExps, j -> posVars^j);(B
	(J       targetGens = append(targetGens, tempGens);(B
	(J       if explicitFlag then ((B
	(J       (B	(J    if tempGens == {} then ((B
		(J    (B	(J targetMat = directSum(targetMat, compress matrix{{0_W}}); )(B
	(J       (B	(J    else ((B
		(J    (B	(J targetMat = directSum(targetMat, matrix{tempGens}); );(B
	(J       (B	(J    );(B
	(J       i = i+1;(B	(J  (B
	(J       );(B
(J     (B	(J  targetSize := sum(targetGens, i->#i);  (B
(J          (B
	(J  -- MAKE THE SOURCE MODULE AS DIRECT SUM OF D_m MODULES(B
(J     (B	(J  -- "sourceGens" is a list of r lists, where the i-th list contains(B
(J     (B	(J  -- the monomial generators {dx^a} (as left D_m-module) of(B
(J     (B	(J  --    F_k1[u_i]((D_n/tD_n) e_i) / F_k0[u_i]((D_n/tD_n) e_i)(B
(J     (B	(J  m := C.dd#(n0+1);(B
(J     (B	(J  r := numgens C#(n0+1);(B
(J     (B	(J  sourceDeg := degrees C#(n0+1);(B
(J     (B	(J  sourceGens := {};(B
(J     (B	(J  if explicitFlag then sourceMat := map(W^0,W^0,0);(B
(J     (B	(J  i = 0;(B
(J     (B	(J  while i < r do ((B
	(J       -- Find generators of the current source(B
	(J       --    "F_k1(D_n/tD_n)^r/F_k0(D_n/tD_n)^r"(B
	(J       -- as a left D_m module.(B
	(J       -- They have the form { \prod_{i=1}^n D_i^{a_i} }.(B
	(J       tempExps = findExps(posWeights, k0-sourceDeg#i#0, (B
	(J       (B	(J    k1-sourceDeg#i#0);(B
	(J       tempGens = apply(tempExps, j -> posVars^j);(B
	(J       sourceGens = append(sourceGens, tempGens );(B
	(J       if explicitFlag then ((B
	(J       (B	(J    if tempGens == {} then ((B
		(J    (B	(J sourceMat = directSum(sourceMat, compress matrix{{0_W}}); )(B
	(J       (B	(J    else ((B
		(J    (B	(J sourceMat = directSum(sourceMat, matrix{tempGens}); );(B
	(J       (B	(J    );(B
	(J       i = i+1;(B
	(J       );(B
(J     (B	(J  sourceSize := sum(sourceGens, i -> #i);(B

	(J  (B
	(J  -- MAKE THE DIFFERENTIAL AS MATRIX OF D_m MODULES(B
(J     (B	(J  if sourceSize == 0 and targetSize == 0 then ((B
	(J       oldDiff := map(resW^0,resW^0,0) )(B
(J     (B	(J  else if sourceSize == 0 then ( oldDiff =(B
	(J       compress matrix toList(targetSize:{0_resW}) )(B
(J     (B	(J  else if targetSize == 0 then ( oldDiff =(B
	(J       transpose compress matrix toList(sourceSize:{0_resW}) )(B
(J     (B	(J  else ((B
	(J       -- For each generator j = \prod_i D_i^{a_i}, compute its image(B
	(J       -- j*m_i as an element of the RHS. Get a matrix of image vectors.(B
	(J       imageMat := matrix join toSequence apply( r, a -> (B
	(J       (B	(J    apply(sourceGens#a, b -> substitute(b*m_a, resSub) ) );(B
	(J       -- differentiate with respect to targetGens(B
	(J       oldDiff = transpose compress matrix toList(sourceSize:{0_W});(B
	(J       i = 0;(B
	(J       -- compute the induced image(B
	(J       while i < s do ((B
	(J       (B	(J    if targetGens#i =!= {} (B
	(J       (B	(J    then oldDiff = oldDiff || substitute( (B
		(J    (B	(J contract(transpose matrix{targetGens#i}, imageMat^{i}), (B
		(J    (B	(J diffSub);(B
	(J       (B	(J    i = i+1;(B
	(J       (B	(J    );(B
	(J       oldDiff = WtoresW oldDiff;(B
	(J       );(B
(J     (B	(J  if member(RestrictComplex, output) then ((B
	(J       restrictComplex#n0 = resW^(rank target oldDiff);(B
	(J       restrictComplex#(n0+1) = resW^(rank source oldDiff);(B
	(J       restrictComplex.dd#(n0+1) = map(restrictComplex#n0,(B
	(J       (B	(J    restrictComplex#(n0+1), oldDiff);(B
	(J       );(B
(J     (B	(J  if member(Cycles, output) or member(Boundaries, output)(B
	(J  or member(GenCycles, output) then ((B
	(J       newKernel := zeroize2 mingens kernel oldDiff;(B
	(J       -- newKernel := zeroize2 gens kernel oldDiff;(B
	(J       explicitKernel := compress (sourceMat * resWtoW(newKernel));(B
	(J       );(B
	(J  );(B
(J     pInfo(2, "\t\t\t Rank = " | sourceSize | "\t time = " | tInfo | " seconds" );(B
(J     (B
(J     -- DO THE COMPUTATION IN HIGHER COHOMOLOGICAL DEGREES     (B
(J     s = r;(B
(J     targetGens = sourceGens;(B
(J     targetSize = sourceSize;(B
(J     if explicitFlag then targetMat = sourceMat;(B
(J     targetMat = sourceMat;(B
(J     currDeg := n0 + 2;(B
(J     --newKernel = 0;(B
(J     --explicitKernel = 0;(B
(J     while currDeg <= n1 and C#?(currDeg) do ((B
	(J  -- MAKE THE NEXT SOURCE MODULE(B
	(J  -- "sourceGens" is a list of r lists, where the i-th list contains(B
	(J  -- the monomial generators {dx^a} (as left D_m-module) of(B
	(J  --    F_k1[u_i]((D_n/tD_n) e_i) / F_k0[u_i]((D_n/tD_n) e_i)(B
	(J  pInfo(2, "\t Degree " | currDeg | "...");(B
	(J  tInfo = toString first timing ((B
	(J  r = numgens C#currDeg;(B
	(J  m = C.dd#currDeg;(B
	(J  sourceDeg = degrees C#(currDeg);(B
	(J  sourceGens = {};(B
	(J  if explicitFlag then sourceMat = map(W^0,W^0,0);(B
	(J  i = 0;(B
	(J  while i < r do ((B
	(J       -- Find generators of the current source(B
	(J       --    "F_k1(D_n/tD_n)^r/F_k0(D_n/tD_n)^r"(B
	(J       -- as a left D_m module.(B
	(J       -- They have the form { \prod_{i=1}^n D_i^{a_i} }.(B
	(J       tempExps = findExps(posWeights, k0-sourceDeg#i#0, (B
		(J    k1-sourceDeg#i#0);(B
	(J       tempGens = apply(tempExps, j -> posVars^j);(B
	(J       sourceGens = append(sourceGens, tempGens );(B
	(J       if explicitFlag then ((B
		(J    if tempGens == {} then ((B
(J (B	(J       (B		(J sourceMat = directSum(sourceMat, compress matrix{{0_W}}); )(B
		(J    else ((B
	(J       (B		(J sourceMat = directSum(sourceMat, matrix{tempGens}); );(B
		(J    );(B
	(J       i = i+1;(B
	(J       );(B
	(J  sourceSize = sum(sourceGens, i -> #i);(B
	(J  (B
	(J  -- MAKE THE NEXT DIFFERENTIAL OF D_m MODULES(B	(J       (B
	(J  if sourceSize == 0 and targetSize == 0 then ((B
	(J       newDiff := map(resW^0,resW^0,0) )(B
	(J  else if sourceSize == 0 then ( newDiff =(B
	(J       compress matrix toList(targetSize:{0_resW}) )(B
	(J  else if targetSize == 0 then ( newDiff =(B
	(J       transpose compress matrix toList(sourceSize:{0_resW}) )(B
	(J  else ((B
	(J       -- For each generator j = \prod_i D_i^{a_i}, compute its image(B
	(J       -- j*m_i as an element of the RHS. Get a matrix of image vectors.(B
	(J       imageMat = matrix join toSequence apply( r, a -> (B
		(J    apply(sourceGens#a, b -> substitute(b*m_a, resSub) ) );(B
	(J       -- differentiate with respect to targetGens(B
	(J       newDiff = transpose compress matrix toList(sourceSize:{0_W});(B
	(J       i = 0;(B
	(J       while i < s do ((B
		(J    if targetGens#i =!= {}(B
		(J    then newDiff = newDiff || substitute( (B
			(J contract(transpose matrix{targetGens#i}, imageMat^{i}), (B
			(J diffSub);(B
		(J    i = i+1;(B
		(J    );(B
	(J       newDiff = WtoresW newDiff;(B
	(J       );(B
	(J  (B
	(J  -- UPDATE THE OUTPUT LIST(B
	(J  if member(RestrictComplex, output) then ((B
	(J       restrictComplex#currDeg = resW^(rank source newDiff);(B
	(J       restrictComplex.dd#currDeg = map(restrictComplex#(currDeg-1),(B
		(J    restrictComplex#currDeg, newDiff);(B
	(J       );(B
	(J  if member(HomologyModules, output) then ((B
	(J       tempHomology := homology(zeroize2 oldDiff, zeroize2 newDiff);(B
	(J       if tempHomology =!= null then(B
	(J       tempHomology = cokernel Dprune presentation tempHomology;(B
	(J       if tempHomology === null then tempHomology = resW^0;(B
	(J       homologyList = append(homologyList,(B
		(J    (currDeg-1) => tempHomology);(B
	(J       );(B
	(J  (B
	(J  -- MAKE EXPLICIT COHOMOLOGY CLASSES(B	(J       (B
(J     (B	(J  if member(Cycles, output) or member(Boundaries, output) or(B
	(J  member(GenCycles, output) then ((B
	(J       oldImage := zeroize2 mingens image newDiff;(B
	(J       -- oldImage := zeroize2 gens image newDiff;(B
	(J       if member(GenCycles, output) then ((B
	(J       (B	(J    if #otherVars == 0 then ((B
		(J    (B	(J explicitList = append(explicitList,(B
			(J      (currDeg-1) => targetMat *(B
			(J      resWtoW(mingens subquotient(newKernel, oldImage))) )(B
	(J       (B	(J    else ((B
		(J    (B	(J explicitList = append(explicitList,(B
			(J      (currDeg-1) => explicitKernel);(B
		(J    (B	(J );(B
		(J    );(B
	(J       if member(Cycles, output) then (B
	(J       kerList = append(kerList, (currDeg-1) => explicitKernel);(B
	(J       if member(Boundaries, output) then ((B
		(J    explicitImage := compress (targetMat * resWtoW(oldImage));(B
		(J    imList = append(imList, (currDeg-1) => explicitImage);(B
		(J    );(B
	(J       newKernel = zeroize2 mingens kernel newDiff;(B
	(J       -- newKernel = zeroize2 gens kernel newDiff;(B
	(J       explicitKernel = compress (sourceMat * resWtoW(newKernel));(B
	(J       );(B
	(J  (B
	(J  -- PREPARE FOR NEXT ITERATION(B
	(J  s = r;(B
	(J  targetGens = sourceGens;(B
	(J  targetSize = sourceSize;(B
	(J  if explicitFlag then targetMat = sourceMat;(B
	(J  oldDiff = newDiff;(B
	(J  currDeg = currDeg+1;(B
	(J  );(B
(J     (B	(J  pInfo(2, "\t\t\t Rank = " | targetSize | "\t time = " | tInfo | " seconds");(B
(J     (B	(J  );(B
(J     );(B
(J     pInfo(2, "\t Finished...");(B
(J     pInfo(2, "\t\t\t Total time = " | tInfo | " seconds");(B
(J     pInfo(2, " ");(B
(J  );(B

(J-- OUTPUT FORMAT(B
(Jif member(HomologyModules, output) then outputList = append((B
(J     outputList, HomologyModules => hashTable homologyList);(B
(Jif member(GenCycles, output) then outputList = append((B
(J     outputList, GenCycles => hashTable explicitList);(B
(Jif member(Cycles, output) then outputList = append((B
(J     outputList, Cycles => hashTable kerList);(B
(Jif member(Boundaries, output) then outputList = append((B
(J     outputList, Boundaries => hashTable imList);(B
(Jif member(BFunction, output) then outputList = append((B
(J     outputList, BFunction => factorBFunction b);(B
(Jif member(VResolution, output) then outputList = append((B
(J     outputList, VResolution => C);(B
(Jif member(RestrictComplex, output) then outputList = append((B
(J     outputList, RestrictComplex => restrictComplex);(B
(Jif member(ResToOrigRing, output) then outputList = append((B
(J     outputList, ResToOrigRing => resWtoW);(B
(Jif member(TransferMap,output) then(B
(J     if Resolution =!= null then (B
(J     outputList = append(outputList, TransferMap => Resolution#TransferMap);(B

(JhashTable outputList (B
(J)(B


(JtwistedLogCohomology2 = method( Options => {Strategy => Schreyer} );(B
(JtwistedLogCohomology2 (List, List) := options -> (f, a) -> ((B
	(Jk := #f;(B
	(Jif k != #a(B
	(Jthen error "number of factors is not equal number of parametors";(B
	
	(Ji := 0;(B
	(Jprodf := 1;(B
	(Jwhile i < k do ((B
		(Jprodf = prodf * f_i;(B
		(Ji = i+1;(B
	(J);(B
	
	(JR = ring prodf;(B
	(Jif class R =!= PolynomialRing(B
	(Jthen error "expected element of a polynomial ring";(B
	(Jif R.monoid.Options.WeylAlgebra =!= {}(B
	(Jthen error "expected element of a commutative polynomial ring";(B
	(Jv := generators(R);(B
	(Jn := #v;(B
	(Jif n != 2(B
	(Jthen error "we compute 2-dimention logCohomology";(B
	(Jd := {diff(v#1,prodf),-diff(v#0,prodf),prodf};(B
	(Jsyzf := kernel matrix{d};(B
	(Jm := numgens syzf;(B
	(Jmsyz := gens syzf;(B
	(JW := makeWeylAlgebra( R, SetVariables=>false);(B
	(Jphi := map(W,R);(B
	(Jvw := generators(W);(B
	(Ji = 0; ell := 0; j := 0, sum := 0;(B
	(Jop := {};(B
	(Jwhile i<m do ((B
		(J-- adjoint of differential operators(B
		(Jj = 0; sum = 0;(B
		(Jwhile j < k do ((B
			(Jsum = sum + a_j * (msyz_(1,i) * diff(v#0,f_j) - msyz_(0,i) * diff(v#1,f_j)) // f_j;(B
			(Jj = j+1;(B
		(J);(B
		(Jell = -phi msyz_(1,i) * vw_(2) + phi msyz_(0,i) * vw_(3) - phi msyz_(2,i) + phi sum;(B
	(Jop = join(op,{ell});(B
	(Ji = i+1;(B
	(J);(B
	(Jfop := apply(op,Fourier);(B
	
	(JoutputRequest := {GenCycles, HomologyModules, (B
	(JVResolution, BFunction, Explicit, TransferMap};(B
	(JM := cokernel matrix{fop};(B
	(JoutputTable := computeRestriction2(M, {1,1}, -1, n+1,(B
	(JoutputRequest, options);(B
	(JoutputList := {BFunction => outputTable#BFunction};(B
	(Jif outputTable#VResolution =!= null then(B
	(JoutputList = append(outputList,(B
	(JVResolution => FourierInverse outputTable#VResolution)(B
	(Jelse outputList = append(outputList, VResolution => null);(B
	
	(JoutputList = outputList | {(B
	(JPreCycles => hashTable apply(toList(0..n),(B
		(Ji -> (n-i => FourierInverse outputTable#GenCycles#i)),(B
	(JCohomologyGroups => hashTable apply(toList(0..n),(B
		(Ji -> (n-i => outputTable#HomologyModules#i)) };(B
	
	(JintTable := hashTable outputList;(B
	
	(J-- make basis of H^0(B
	(J-- dim H^0 is -1 or 0(B
	(Jbasis0 := 0;(B
	(Jif dim intTable#CohomologyGroups#0 == 0 then ((B
	(Jsumx := 0; sumy := 0; j = 0;(B
	(Jwhile j < k do ((B
		(Jsumx = sumx + a_j * prodf * diff(v#0,f_j) // f_j;(B
		(Jsumy = sumy + a_j * prodf * diff(v#1,f_j) // f_j;(B
		(Jj = j+1;(B
	(J);(B
	(JI := ideal(phi prodf * vw_(2) + phi sumx, phi prodf * vw_(3) + phi sumy );(B
	(Jbasis0 = matrix{PolySols I};(B
	(J)(B
	(Jelse basis0 = intTable#PreCycles#0;(B
	
	(J-- make basis of H^1(B
	(Jif outputTable#?TransferMap then((B
	(Jtransfermap := transpose Dtransposition FourierInverse outputTable#TransferMap;(B
	
	(Jmat :=  transpose(intTable#PreCycles#1)*transfermap;(B
	(Jl := numRows mat;(B
	(Jmat = mutableMatrix mat;(B
	(Ji = 0;(B
	(Jwhile i < l do((B
		(Jj = 0;(B
		(Jwhile j < m do((B
			(Jmat_(i,j) = substitute(mat_(i,j),{vw_(2) => 0, vw_(3) => 0});(B
			(Jj = j+1;(B
		(J);(B
	(Ji = i+1;(B
	(J);(B
	(Jmat = matrix mat;(B
	(Jmsyz = phi msyz;(B
	
	(JBasis1 := {};(B
	(Ji = 0;(B
	(Jwhile i < l do((B
		(Jbase = 0; j = 0;(B
		(Jwhile j < m do((B
			(Jbase = base + mat_(i,j) * (msyz_(0,j) * vw_(2) + msyz_(1,j) * vw_(3));(B
			(Jj = j+1;(B
		(J);(B
	(JBasis1 = join(Basis1,{base});(B
	(Ji = i+1;(B
	(J);(B
	(JBasis1 = matrix{Basis1};(B
	(J)(B
	(Jelse Basis1 = intTable#PreCycles#1;(B
	
	(Jif intTable#VResolution =!= null then ((B
		(JcreateDpairs W;(B
		(JOmega := Dres ideal W.dpairVars#1;(B
		(JBasis := hashTable {0 => basis0, (B
		(J1 => Basis1,(B
		(J2 => intTable#PreCycles#2 * vw_(2)*vw_(3)};(B
		(JoutputList = outputList | {LogBasis => (B
		(JBasis, OmegaRes => Omega};(B
	(J);(B
	
	(JhashTable outputList(B
(J)(B