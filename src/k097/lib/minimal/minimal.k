/* $OpenXM$ */
#define DEBUG 1 
/* #define ORDINARY 1 */
/* Test sequences. 
   Use load["minimal.k"];;

   a=Sminimal(v);
   b=a[0];
   b[1]*b[0]:
   b[2]*b[1]:

   a = test0();
   b = a[0];
   b[1]*b[0]:
   b[2]*b[1]:
   a = Sminimal(b[0]);

   a = test1();
   b=a[0];
   b[1]*b[0]:
   b[2]*b[1]:
     
*/


load("cohom.k");
def load_tower() {
  if (Boundp("k0-tower.sm1.loaded")) {
  }else{
    sm1(" [(parse) (k0-tower.sm1) pushfile ] extension ");
    sm1(" /k0-tower.sm1.loaded 1 def ");
  }
}
load_tower();
SonAutoReduce = true;
def Factor(f) {
   sm1(f, " fctr /FunctionValue set");
}
def Reverse(f) {
   sm1(f," reverse /FunctionValue set");
}
def Sgroebner(f) {
   sm1(" [f] groebner /FunctionValue set");
}
def test0() {
  local f;
  Sweyl("x,y,z");
  f = [x^2+y^2+z^2, x*y+x*z+y*z, x*z^2+y*z^2, y^3-x^2*z - x*y*z+y*z^2,
       -y^2*z^2 + x*z^3 + y*z^3, -z^4];
  frame=SresolutionFrame(f);
  Println(frame);
  /* return(frame); */
  return(SlaScala(f));
}
def test1() {
  local f;
  Sweyl("x,y,z");
  f = [x^2+y^2+z^2, x*y+x*z+y*z, x*z^2+y*z^2, y^3-x^2*z - x*y*z+y*z^2,
       -y^2*z^2 + x*z^3 + y*z^3, -z^4];
  return(Sminimal(f));
}

  

def Sweyl(v,w) {
  /* extern WeightOfSweyl ; */
  local ww,i,n;
  if(Length(Arglist) == 1) {
    sm1(" [v s_ring_of_differential_operators 0 [(schreyer) 1]] define_ring ");
    sm1(" define_ring_variables ");

    sm1(" [ v to_records pop ] /ww set ");
    n = Length(ww);
    WeightOfSweyl = NewArray(n*4);
    for (i=0; i< n; i++) {
      WeightOfSweyl[2*i] = ww[i];
      WeightOfSweyl[2*i+1] = 1;
    }
    for (i=0; i< n; i++) {
      WeightOfSweyl[2*n+2*i] = AddString(["D",ww[i]]);
      WeightOfSweyl[2*n+2*i+1] = 1;
    }
      
  }else{
    sm1(" [v s_ring_of_differential_operators w s_weight_vector 0 [(schreyer) 1]] define_ring ");
    sm1(" define_ring_variables ");
    WeightOfSweyl = w[0];
  }
}


def Spoly(f) {
  sm1(f, " toString tparse /FunctionValue set ");
}

def SreplaceZeroByZeroPoly(f) {
  if (IsArray(f)) {
     return(Map(f,"SreplaceZeroByZeroPoly"));
  }else{
     if (IsInteger(f)) {
       return(Poly(ToString(f)));
     }else{
       return(f);
     }
  }
}
def Shomogenize(f) {
  f = SreplaceZeroByZeroPoly(f);
  if (IsArray(f)) {
    sm1(f," sHomogenize2  /FunctionValue set "); 
    /* sm1(f," {sHomogenize2} map  /FunctionValue set ");  */
    /* Is it correct? Double check.*/
  }else{
    sm1(f, " sHomogenize /FunctionValue set ");
  }
}

def StoTower() {
  sm1("  [(AvoidTheSameRing)] pushEnv [ [(AvoidTheSameRing) 0] system_variable (mmLarger) (tower) switch_function ] pop popEnv ");
}

def SsetTower(tower) {
sm1(" [(AvoidTheSameRing)] pushEnv 
      [ [(AvoidTheSameRing) 0] system_variable 
        [(gbListTower) tower (list) dc] system_variable
      ] pop popEnv ");
}

def SresolutionFrameWithTower(g,opt) {
  local gbTower, ans, ff, count, startingGB, opts, skelton,withSkel, autof,
        gbasis;
  if (Length(Arglist) >= 2) {
    if (IsInteger(opt)) count = opt;
  }else{
    count = -1;
  }

  sm1(" setupEnvForResolution ");
  /* If I do not put this macro, homogenization
     make a strange behavior. For example,
     [(2*x*Dx + 3*y*Dy+6) (0)] homogenize returns
     [(2*x*Dx*h + 3*y*Dy*h+6*h^3) (0)].
     4/19, 2000.
  */

  sm1(" (mmLarger) (matrix) switch_function ");
  g = Map(g,"Shomogenize");
  if (SonAutoReduce) {
    sm1("[ (AutoReduce) ] system_variable /autof set ");
    sm1("[ (AutoReduce) 1 ] system_variable ");
  }
  gbasis = Sgroebner(g);
  g = gbasis[0];
  if (SonAutoReduce) {
    sm1("[ (AutoReduce) autof] system_variable  "); 
  }

  g = Init(g);

/*  sm1(" setupEnvForResolution-sugar "); */
  /* -sugar is fine? */
  sm1(" setupEnvForResolution "); 

  Println(g);
  startingGB = g;
  /* ans = [ SzeroMap(g) ];  It has not been implemented. see resol1.withZeroMap */
  ans = [ ];
  gbTower = [ ];
  skelton = [ ];
  while (true) {
    /* sm1(g," res0Frame /ff set "); */
    withSkel = Sres0FrameWithSkelton(g);
    ff = withSkel[0];
    ans = Append(ans, ff[0]);
    gbTower = Join([ ff[1] ], gbTower);
    skelton = Join([ withSkel[1] ], skelton);
    g = ff[0];
    if (Length(g) == 0) break;
    SsetTower( gbTower );
    if (count == 0) break;
    count = count - 1;
  }
  return([ans,Reverse(gbTower),Join([ [ ] ], Reverse(skelton)),gbasis]);
}
HelpAdd(["SresolutionFrameWithTower",
["It returs [resolution of the initial, gbTower, skelton, gbasis]",
 "Example: Sweyl(\"x,y\");",
 "         a=SresolutionFrameWithTower([x^3,x*y,y^3-1]);"]]);

def SresolutionFrame(f,opt) {
  local ans;
  ans = SresolutionFrameWithTower(f);
  return(ans[0]);
}
/* ---------------------------- */
def ToGradedPolySet(g) {
  sm1(g," (gradedPolySet) dc /FunctionValue set ");
}

def NewPolynomialVector(size) {
  sm1(size," (integer) dc newPolyVector /FunctionValue set ");
}

def  SturnOffHomogenization() {
  sm1("
    [(Homogenize)] system_variable 1 eq
    { (Warning: Homogenization and ReduceLowerTerms options are automatically turned off.) message
      [(Homogenize) 0] system_variable
      [(ReduceLowerTerms) 0] system_variable
    } {  } ifelse
  ");
}
def  SturnOnHomogenization() {
  sm1("
    [(Homogenize)] system_variable 0 eq
    { (Warning: Homogenization and ReduceLowerTerms options are automatically turned ON.) message
      [(Homogenize) 1] system_variable
      [(ReduceLowerTerms) 1] system_variable
    } {  } ifelse
  ");
}

def SschreyerSkelton(g) {
  sm1(" [(schreyerSkelton) g] gbext /FunctionValue set ");
}
def Stoes(g) {
  if (IsArray(g)) {
    sm1(g," {toes} map /FunctionValue set ");
  }else{
    sm1(g," toes /FunctionValue set ");
  }
}
def Stoes_vec(g) {
    sm1(g," toes /FunctionValue set ");
}

def Sres0Frame(g) {
  local ans;
  ans = Sres0FrameWithSkelton(g);
  return(ans[0]);
}
def Sres0FrameWithSkelton(g) {
  local t_syz, nexttower, m, t_gb, skel, betti,
        gg, k, i, j, pair, tmp, si, sj, grG, syzAll, gLength;

  SturnOffHomogenization();

  g = Stoes(g);
  skel = SschreyerSkelton(g);
  /* Print("Skelton is ");
  sm1_pmat(skel); */
  betti = Length(skel);
  
  gLength = Length(g);
  grG = ToGradedPolySet(g);
  syzAll = NewPolynomialVector(betti);
  for (k=0; k<betti; k++) {
    pair = skel[k];
    i = pair[0,0];
    j = pair[0,1];
    si = pair[1,0];
    sj = pair[1,1];
    /* si g[i] + sj g[j] + \sum tmp[2][k] g[k] = 0 in res0 */
    Print(".");

    t_syz = NewPolynomialVector(gLength);
    t_syz[i] = si;
    t_syz[j] = sj;
    syzAll[k] = t_syz;
  }
  t_syz = syzAll;
  Print("Done. betti="); Println(betti);
  /* Println(g);  g is in a format such as 
    [e_*x^2 , e_*x*y , 2*x*Dx*h , ...]
    [e_*x^2 , e_*x*y , 2*x*Dx*h , ...]
    [y-es*x , 3*es^4*y*Dy-es^5*x , 3*es^5*y*Dy-es^6*x , ...]
    [3*es^3*y*Dy-es^5*x ] 
  */
  nexttower = Init(g);
  SturnOnHomogenization();
  return([[t_syz, nexttower],skel]);
}


def StotalDegree(f) {
  sm1(" [(grade) f] gbext (universalNumber) dc /FunctionValue set ");
}

/* Sord_w(x^2*Dx*Dy,[x,-1,Dx,1]); */
def Sord_w(f,w) {
  local neww,i,n;
  n = Length(w);
  neww = NewArray(n);
  for (i=0; i<n; i=i+2) {
    neww[i] = ToString(w[i]);
  }
  for (i=1; i<n; i=i+2) {
    neww[i] = IntegerToSm1Integer(w[i]);
  }
  sm1(" f neww ord_w (universalNumber) dc /FunctionValue set ");
}


/* This is not satisfactory. */
def SinitOfArray(f) {
  local p,pos,top;
  if (IsArray(f)) {
     sm1(f," toes init /p set ");
     sm1(p," (es). degree (universalNumber) dc /pos set ");
     return([Init(f[pos]),pos]);
  } else {
     return(Init(f));
  }
}

def test_SinitOfArray() {
  local f, frame,p,tower,i,j,k;
  Sweyl("x,y,z");
  f = [x^2+y^2+z^2, x*y+x*z+y*z, x*z^2+y*z^2, y^3-x^2*z - x*y*z+y*z^2,
       -y^2*z^2 + x*z^3 + y*z^3, -z^4];
  p=SresolutionFrameWithTower(f);
  sm1_pmat(p); 
  sm1_pmat(SgenerateTable(p[1]));
  return(p);
  frame = p[0];
  sm1_pmat(p[1]);
  sm1_pmat(frame);
  sm1_pmat(Map(frame[0],"SinitOfArray"));
  sm1_pmat(Map(frame[1],"SinitOfArray"));
  return(p);
}

/* f is assumed to be a monomial with toes. */
def Sdegree(f,tower,level) {
  local i;
  if (level <= 1) return(StotalDegree(f));
  i = Degree(f,es);
  return(StotalDegree(f)+Sdegree(tower[level-2,i],tower,level-1));
}

def SgenerateTable(tower) {
  local height, n,i,j, ans, ans_at_each_floor;
  height = Length(tower);
  ans = NewArray(height);
  for (i=0; i<height; i++) {
    n = Length(tower[i]);
    ans_at_each_floor=NewArray(n);
    for (j=0; j<n; j++) {
      ans_at_each_floor[j] = Sdegree(tower[i,j],tower,i+1)-(i+1);
      /* Println([i,j,ans_at_each_floor[j]]); */
    }
    ans[i] = ans_at_each_floor;
  }
  return(ans);
}
Sweyl("x,y,z");
v=[[2*x*Dx + 3*y*Dy+6, 0],
   [3*x^2*Dy + 2*y*Dx, 0],
   [0,  x^2+y^2],
   [0,  x*y]];
/*  SresolutionFrameWithTower(v); */

def SnewArrayOfFormat(p) {
  if (IsArray(p)) {
     return(Map(p,"SnewArrayOfFormat"));
  }else{
     return(null);
  }
}
def SminOfStrategy(a) {
  local n,i,ans,tt;
  ans = 100000; /* very big number */
  if (IsArray(a)) {
    n = Length(a);
    for (i=0; i<n; i++) {
      if (IsArray(a[i])) {
        tt = SminOfStrategy(a[i]);
        if (tt < ans) ans = tt;
      }else{
        if (a[i] < ans) ans = a[i];
      }
    }
  }else{
     if (a < ans) ans = a;
  }
  return(ans);
}  
def SmaxOfStrategy(a) {
  local n,i,ans,tt;
  ans = -100000; /* very small number */
  if (IsArray(a)) {
    n = Length(a);
    for (i=0; i<n; i++) {
      if (IsArray(a[i])) {
        tt = SmaxOfStrategy(a[i]);
        if (tt > ans) ans = tt;
      }else{
        if (a[i] > ans) ans = a[i];
      }
    }
  }else{
     if (a > ans) ans = a;
  }
  return(ans);
}  


def SlaScala(g) {
  local rf, tower, reductionTable, skel, redundantTable, bases,
        strategy, maxOfStrategy, height, level, n, i,
        freeRes,place, f, reducer,pos, redundant_seq,bettiTable,freeResV,ww,
        redundantTable_ordinary, redundant_seq_ordinary;
  /* extern WeightOfSweyl; */
  ww = WeightOfSweyl;
  Print("WeghtOfSweyl="); Println(WeightOfSweyl);
  rf = SresolutionFrameWithTower(g);
  redundant_seq = 1;   redundant_seq_ordinary = 1;
  tower = rf[1];
  reductionTable = SgenerateTable(tower);
  skel = rf[2];
  redundantTable = SnewArrayOfFormat(rf[1]);
  redundantTable_ordinary = SnewArrayOfFormat(rf[1]);
  reducer = SnewArrayOfFormat(rf[1]);
  freeRes = SnewArrayOfFormat(rf[1]);
  bettiTable = SsetBettiTable(rf[1],g);

  strategy = SminOfStrategy( reductionTable );
  maxOfStrategy = SmaxOfStrategy( reductionTable );
  height = Length(reductionTable);
  while (strategy <= maxOfStrategy) {
    for (level = 0; level < height; level++) {
      n = Length(reductionTable[level]);
      for (i=0; i<n; i++) {
        if (reductionTable[level,i] == strategy) {
           Print("Processing "); Print([level,i]);
           Print("   Strategy = "); Println(strategy);
           if (level == 0) {
             if (IsNull(redundantTable[level,i])) {
               bases = freeRes[level];
               /* Println(["At floor : GB=",i,bases,tower[0,i]]); */
               pos = SwhereInGB(tower[0,i],rf[3,0]);
               bases[i] = rf[3,0,pos];
               redundantTable[level,i] = 0;
               redundantTable_ordinary[level,i] = 0;
               freeRes[level] = bases;
               /* Println(["GB=",i,bases,tower[0,i]]); */
             }
           }else{ /* level >= 1 */
             if (IsNull(redundantTable[level,i])) {
               bases = freeRes[level];
               f = SpairAndReduction(skel,level,i,freeRes,tower,ww);
               if (f[0] != Poly("0")) {
                  place = f[3];
                  /* (level-1, place) is the place for f[0], 
                     which is a newly obtained  GB. */
#ifdef ORDINARY
                  redundantTable[level-1,place] = redundant_seq;
                  redundant_seq++;
#else
                  if (f[4] > f[5]) {
                    /* Zero in the gr-module */
                    Print("v-degree of [org,remainder] = ");
                    Println([f[4],f[5]]);
                    Print("[level,i] = "); Println([level,i]);
                    redundantTable[level-1,place] = 0;
                  }else{
                    redundantTable[level-1,place] = redundant_seq;
                    redundant_seq++;
                  }
#endif
                  redundantTable_ordinary[level-1,place]
                     =redundant_seq_ordinary;
                  redundant_seq_ordinary++;
                  bases[i] = SunitOfFormat(place,f[1])-f[1];  /* syzygy */
                  redundantTable[level,i] = 0;
                  redundantTable_ordinary[level,i] = 0;
                  /* i must be equal to f[2], I think. Double check. */
                  freeRes[level] = bases;
                  bases = freeRes[level-1];
                  bases[place] = f[0];
                  freeRes[level-1] = bases;
                  reducer[level-1,place] = f[1];
               }else{
                  redundantTable[level,i] = 0;
                  bases = freeRes[level];
                  bases[i] = f[1];  /* Put the syzygy. */
                  freeRes[level] = bases;
               }
             }
           } /* end of level >= 1 */
        }
      }
    }
    strategy++;
  }
  n = Length(freeRes);
  freeResV = SnewArrayOfFormat(freeRes);
  for (i=0; i<n; i++) {
    bases = freeRes[i];
    bases = Sbases_to_vec(bases,bettiTable[i]);
    freeResV[i] = bases;
  }
  return([freeResV, redundantTable,reducer,bettiTable,redundantTable_ordinary]);
}

def SsetBettiTable(freeRes,g) {
  local level,i, n,bases,ans;
  ans = NewArray(Length(freeRes)+1);
  n = Length(freeRes);
  if (IsArray(g[0])) {
    ans[0] = Length(g[0]);
  }else{
    ans[0] = 1;
  }
  for (level=0; level<n; level++) {
    bases = freeRes[level];
    if (IsArray(bases)) {
      ans[level+1] = Length(bases);
    }else{
      ans[level+1] = 1;
    }
  }
  return(ans);
}

def SwhereInGB(f,tower) {
  local i,n,p,q;
  n = Length(tower);
  for (i=0; i<n; i++) {
    p = MonomialPart(tower[i]);
    q = MonomialPart(f);
    if (p == q) return(i);
  }
  Println([f,tower]);
  Error("whereInGB : [f,myset]: f could not be found in the myset.");      
}
def SunitOfFormat(pos,forms) {
  local ans,i,n;
  n = Length(forms);
  ans = NewArray(n);
  for (i=0; i<n; i++) {
    if (i != pos) {
      ans[i] = Poly("0");
    }else{
      ans[i] = Poly("1");
    }
  }
  return(ans);
}

def Error(s) {
  sm1(" s error ");
}

def IsNull(s) {
  if (Stag(s) == 0) return(true);
  else return(false);
}

def StowerOf(tower,level) {
  local ans,i;
  ans = [ ];
  if (level == 0) return([[]]);
  for (i=0; i<level; i++) {
    ans = Append(ans,tower[i]);
  } 
  return(Reverse(ans));
}

def Sspolynomial(f,g) {
  if (IsArray(f)) {
    f = Stoes_vec(f);
  }
  if (IsArray(g)) {
    g = Stoes_vec(g);
  }
  sm1("f g spol /FunctionValue set");
}

def MonomialPart(f) {
  sm1(" [(lmonom) f] gbext /FunctionValue set ");
}

def SwhereInTower(f,tower) {
  local i,n,p,q;
  if (f == Poly("0")) return(-1);
  n = Length(tower);
  for (i=0; i<n; i++) {
    p = MonomialPart(tower[i]);
    q = MonomialPart(f);
    if (p == q) return(i);
  }
  Println([f,tower]);
  Error("[f,tower]: f could not be found in the tower.");      
}

def Stag(f) {
  sm1(f," tag (universalNumber) dc /FunctionValue set");
}

def SpairAndReduction(skel,level,ii,freeRes,tower,ww) {
  local i, j, myindex, p, bases, tower2, gi, gj,
       si, sj, tmp, t_syz, pos, ans, ssp, syzHead,pos2,
       vdeg,vdeg_reduced;
  Println("SpairAndReduction:");

  if (level < 1) Error("level should be >= 1 in SpairAndReduction.");
  p = skel[level,ii];
  myindex = p[0];  
  i = myindex[0]; j = myindex[1];
  bases = freeRes[level-1];
  Println(["p and bases ",p,bases]);
  if (IsNull(bases[i]) || IsNull(bases[j])) {
    Println([level,i,j,bases[i],bases[j]]);
    Error("level, i, j : bases[i], bases[j]  must not be NULL.");
  }

  tower2 = StowerOf(tower,level-1);
  SsetTower(tower2);  
  /** sm1(" show_ring ");   */

  gi = Stoes_vec(bases[i]);
  gj = Stoes_vec(bases[j]);

  ssp = Sspolynomial(gi,gj); 
  si = ssp[0,0];  
  sj = ssp[0,1];
  syzHead = si*es^i;
  /* This will be the head term, I think. But, double check. */
  Println([si*es^i,sj*es^j]);

  Print("[gi, gj] = "); Println([gi,gj]);
  sm1(" [(Homogenize)] system_variable message ");
  Print("Reduce the element "); Println(si*gi+sj*gj);
  Print("by  "); Println(bases);

  tmp = Sreduction(si*gi+sj*gj, bases);

  Print("result is "); Println(tmp);

  vdeg = SvDegree(si*gi+sj*gj,tower,level-1,ww);
  vdeg_reduced = SvDegree(tmp[0],tower,level-1,ww);  
  Print("vdegree of the original = "); Println(vdeg);
  Print("vdegree of the remainder = "); Println(vdeg_reduced);

  t_syz = tmp[2];
  si = si*tmp[1]+t_syz[i];
  sj = sj*tmp[1]+t_syz[j];
  t_syz[i] = si; 
  t_syz[j] = sj;
  pos = SwhereInTower(syzHead,tower[level]);
  pos2 = SwhereInTower(tmp[0],tower[level-1]);
  ans = [tmp[0],t_syz,pos,pos2,vdeg,vdeg_reduced];
  /* pos is the place to put syzygy at level. */
  /* pos2 is the place to put a new GB at level-1. */   
  Println(ans);
  return(ans);
}

def Sreduction(f,myset) {
  local n, indexTable, set2, i, j, tmp, t_syz;
  n = Length(myset);
  indexTable = NewArray(n);
  set2 = [ ];
  j = 0;
  for (i=0; i<n; i++) {
    if (IsNull(myset[i])) {
      indexTable[i] = -1;
/*    }else if (myset[i] == Poly("0")) {
      indexTable[i] = -1;  */
    }else{
      set2 = Append(set2,Stoes_vec(myset[i]));
      indexTable[i] = j;
      j++;
    }
  }
  sm1(" f toes set2 (gradedPolySet) dc reduction /tmp set ");
  t_syz = NewArray(n);
  for (i=0; i<n; i++) {
    if (indexTable[i] != -1) {
      t_syz[i] = tmp[2, indexTable[i]];
    }else{
      t_syz[i] = Poly("0");
    }
  }
  return([tmp[0],tmp[1],t_syz]);
}

def Warning(s) {
  Print("Warning: ");
  Println(s);
}
def RingOf(f) {
  local r;
  if (IsPolynomial(f)) {
    if (f != Poly("0")) {
      sm1(f," (ring) dc /r set ");
    }else{
      sm1(" [(CurrentRingp)] system_variable /r set ");
    }
  }else{
    Warning("RingOf(f): the argument f must be a polynomial. Return the current ring.");
    sm1(" [(CurrentRingp)] system_variable /r set ");
  }
  return(r);
}

def Sfrom_es(f,size) {
  local c,ans, i, d, myes, myee, j,n,r,ans2;
  if (Length(Arglist) < 2) size = -1;
  if (IsArray(f)) return(f);
  r = RingOf(f);
  myes = PolyR("es",r);
  myee = PolyR("e_",r);
  if (Degree(f,myee) > 0 && size == -1) {
    if (size == -1) {
       sm1(f," (array) dc /ans set");
       return(ans);
    }
  }

/*
    Coefficients(x^2-1,x):
    [    [    2 , 0 ]  , [    1 , -1 ]  ] 
*/
  if (Degree(f,myee) > 0) {
    c = Coefficients(f,myee);
  }else{
    c = Coefficients(f,myes);
  }
  if (size < 0) {
    size = c[0,0]+1;
  }
  ans = NewArray(size);
  for (i=0; i<size; i++) {ans[i] = 0;}
  n = Length(c[0]);
  for (j=0; j<n; j++) {
    d = c[0,j];
    ans[d] = c[1,j];
  }
  return(ans);
}

def Sbases_to_vec(bases,size) {
  local n, giveSize, newbases,i;
  /*  bases = [1+es*x, [1,2,3*x]] */
  if (Length(Arglist) > 1) {
    giveSize = true;
  }else{
    giveSize = false;
  }
  n = Length(bases);
  newbases = NewArray(n);
  for (i=0; i<n; i++) {
     if (giveSize) {
       newbases[i] = Sfrom_es(bases[i], size);
     }else{
       newbases[i] = Sfrom_es(bases[i]);
     }
  }
  return(newbases);
}

def Sminimal(g) {
  local r, freeRes, redundantTable, reducer, maxLevel,
        minRes, seq, maxSeq, level, betti, q, bases, dr,
        betti_levelplus, newbases, i, j,qq;
  r = SlaScala(g);
  /* Should I turn off the tower?? */
  freeRes = r[0];
  redundantTable = r[1];
  reducer = r[2];
  minRes = SnewArrayOfFormat(freeRes);
  seq = 0;
  maxSeq = SgetMaxSeq(redundantTable);
  maxLevel = Length(freeRes);
  for (level = 0; level < maxLevel; level++) {
    minRes[level] = freeRes[level];
  }
  seq=maxSeq+1;
  while (seq > 1) {
    seq--;
    for (level = 0; level < maxLevel; level++) {
      betti = Length(freeRes[level]);
      for (q = 0; q<betti; q++) {
        if (redundantTable[level,q] == seq) {
          Print("[seq,level,q]="); Println([seq,level,q]);
          if (level < maxLevel-1) {
            bases = freeRes[level+1];
            dr = reducer[level,q];
            dr[q] = -1;
            newbases = SnewArrayOfFormat(bases);
            betti_levelplus = Length(bases);
            /*
               bases[i,j] ---> bases[i,j]+bases[i,q]*dr[j]
            */
            for (i=0; i<betti_levelplus; i++) {
              newbases[i] = bases[i] + bases[i,q]*dr;
            }
            Println(["level, q =", level,q]);
            Println("bases="); sm1_pmat(bases); 
            Println("dr="); sm1_pmat(dr);
            Println("newbases="); sm1_pmat(newbases);
            minRes[level+1] = newbases;
            freeRes = minRes;
#ifdef DEBUG
            for (qq=0; qq<betti; qq++) {
              if ((redundantTable[level,qq] >= seq) && 
                  (redundantTable[level,qq] <= maxSeq)) {
                for (i=0; i<betti_levelplus; i++) {
                  if (!IsZero(newbases[i,qq])) {
                    Println(["[i,qq]=",[i,qq]," is not zero in newbases."]);
                    Print("redundantTable ="); sm1_pmat(redundantTable[level]);
                    Error("Stop in Sminimal for debugging.");
                  }
                }
              }
            }
#endif
          }
        }
      }
    }
   }
   return([Stetris(minRes,redundantTable),
          [ minRes, redundantTable, reducer,r[3],r[4]]]);
  /* r[4] is the redundantTable_ordinary */
}  


def IsZero(f) {
  if (IsPolynomial(f)) {
    return( f == Poly("0"));
  }else if (IsInteger(f)) {
    return( f == 0);
  }else if (IsSm1Integer(f)) {
    return( f == true );
  }else if (IsDouble(f)) {
    return( f == 0.0 );
  }else if (IsRational(f)) {
    return(IsZero(Denominator(f)));
  }else{
    Error("IsZero: cannot deal with this data type.");
  }
}
def SgetMaxSeq(redundantTable) {
   local level,i,n,ans, levelMax,bases;
   levelMax = Length( redundantTable );
   ans = 0;
   for (level = 0; level < levelMax; level++) {
     bases = redundantTable[level];
     n = Length(bases);
     for (i=0; i<n; i++) {
       if (IsInteger( bases[i] )) {
          if (bases[i] > ans) {
             ans = bases[i];
          }
       }
     }
   }
   return(ans);
}

def Stetris(freeRes,redundantTable) {
  local level, i, j, resLength, minRes,
        bases, newbases, newbases2;
  minRes = SnewArrayOfFormat(freeRes);
  resLength = Length( freeRes );
  for (level=0; level<resLength; level++) {
    bases = freeRes[level];
    newbases = SnewArrayOfFormat(bases);
    betti = Length(bases); j = 0;
    /* Delete rows */
    for (i=0; i<betti; i++) {
      if (redundantTable[level,i] < 1) {
         newbases[j] = bases[i];
         j++;
      }
    }
    bases = SfirstN(newbases,j);
    if (level > 0) {
      /* Delete columns */
      newbases = Transpose(bases);
      betti = Length(newbases); j = 0;
      newbases2 = SnewArrayOfFormat(newbases);
      for (i=0; i<betti; i++) {
        if (redundantTable[level-1,i] < 1) {
           newbases2[j] = newbases[i];
           j++;
        }
      }
      newbases = Transpose(SfirstN(newbases2,j));
    }else{
      newbases = bases;
    }
    Println(["level=", level]);
    sm1_pmat(bases);
    sm1_pmat(newbases);

    minRes[level] = newbases;
  }
  return(minRes);    
}

def SfirstN(bases,k) {
   local ans,i;
   ans = NewArray(k);
   for (i=0; i<k; i++) {
     ans[i] = bases[i];
   }
   return(ans);
}


/* usage:  tt is tower. ww is weight.
    a = SresolutionFrameWithTower(v);
    tt = a[1];    
    ww = [x,1,y,1,Dx,1,Dy,1];
    SvDegree(x*es,tt,1,ww):

In(17)=tt:
[[2*x*Dx , e_*x^2 , e_*x*y , 3*x^2*Dy , e_*y^3 , 9*x*y*Dy^2 , 27*y^2*Dy^3 ]  ,
 [es*y , 3*es^3*y*Dy , 3*es^5*y*Dy , 3*x*Dy , es^2*y^2 , 9*y*Dy^2 ]  ,
 [3*es^3*y*Dy ]  ] 
In(18)=SvDegree(x*es,tt,1,ww):
3
In(19)=SvDegree(x*es^3,tt,1,ww):
4
In(20)=SvDegree(x,tt,2,ww):
4

*/
def SvDegree(f,tower,level,w) {
  local i,ans;
  if (IsZero(f)) return(null);
  if (level <= 0) {
    return(Sord_w(f,w));
  }
  i = Degree(f,es);
  ans = Sord_w(f,w) +
        SvDegree(tower[level-1,i],tower,level-1,w);
  return(ans);
}

