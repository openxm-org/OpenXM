/* $OpenXM: OpenXM/src/k097/lib/minimal/minimal.k,v 1.27 2000/08/16 22:38:52 takayama Exp $ */
#define DEBUG 1 
Sordinary = false;
/* If you run this program on openxm version 1.1.2 (FreeBSD),
   make a symbolic link by the command
   ln -s /usr/bin/cpp /lib/cpp
*/
#define OFFSET 0 
/* #define OFFSET 20*/
Sverbose = false; /* Be extreamly verbose     */
Sverbose2 = true; /* Don't be quiet and show minimal information */
def Sprintln(s) {
  if (Sverbose) Println(s);
}
def Sprint(s) {
  if (Sverbose) Print(s);
}
def Sprintln2(s) {
  if (Sverbose2) Println(s);
}
def Sprint2(s) {
  if (Sverbose2) Print(s);
  sm1(" [(flush)] extension ");
}

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
    sm1(" [(parse) (new.sm1) pushfile ] extension ");
    sm1(" /k0-tower.sm1.loaded 1 def ");
  }
  sm1(" oxNoX ");
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

def Sinvolutive(f,w) {
  local g,m;
  if (IsArray(f[0])) {
    m = NewArray(Length(f[0]));
  }else{
    m = [0];
  }
  g = Sgroebner(f);
  /* This is a temporary code. */
  sm1(" g 0 get { w m init_w<m>} map /FunctionValue set ");
}



def Error(s) {
  sm1(" s error ");
}

def IsNull(s) {
  if (Stag(s) == 0) return(true);
  else return(false);
}

def MonomialPart(f) {
  sm1(" [(lmonom) f] gbext /FunctionValue set ");
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

def Ord_w_m(f,w,m) {
  sm1(" f  w  m ord_w<m> { (universalNumber) dc } map /FunctionValue set ");
}
HelpAdd(["Ord_w_m",
["Ord_w_m(f,w,m) returns the order of f with respect to w with the shift m.",
 "Note that the order of the ring and the weight w must be the same.",
 "When f is zero, it returns -intInfinity = -999999999.",
 "Example:  Sweyl(\"x,y\",[[\"x\",-1,\"Dx\",1]]); ",
 "          Ord_w_m([x*Dx+1,Dx^2+x^5],[\"x\",-1,\"Dx\",1],[2,0]):"]]);

def Init_w_m(f,w,m) {
  sm1(" f w m init_w<m> /FunctionValue set ");
}
HelpAdd(["Init_w_m",
["Init_w_m(f,w,m) returns the initial of f with respect to w with the shift m.",
 "Note that the order of the ring and the weight w must be the same.",
 "Example:  Sweyl(\"x,y\",[[\"x\",-1,\"Dx\",1]]); ",
 "          Init_w_m([x*Dx+1,Dx^2+x^5],[\"x\",-1,\"Dx\",1],[2,0]):"]]);

def Max(v) {
  local i,t,n;
  n = Length(v);
  if (n == 0) return(null);
  t = v[0];
  for (i=0; i<n; i++) {
    if (v[i] > t) { t = v[i];}
  }
  return(t);
}
HelpAdd(["Max",
["Max(v) returns the maximal element in v."]]);

/*  End of standard functions that should be moved to standard libraries. */
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
      /* sm1("(hoge) message show_ring "); */
}

def SresolutionFrameWithTower(g,opt) {
  local gbTower, ans, ff, count, startingGB, opts, skelton,withSkel, autof,
        gbasis, nohomog,i,n;
  /* extern Sordinary */
  nohomog = false;
  count = -1;  Sordinary = false; /* default value for options. */
  if (Length(Arglist) >= 2) {
    if (IsArray(opt)) {
      n = Length(opt);
      for (i=0; i<n; i++) {
        if (IsInteger(opt[i])) {
          count = opt[i];
        }
        if (IsString(opt[i])) {
          if (opt[i] == "homogenized") {
            nohomog = true;
          }else if (opt[i] == "Sordinary") {
            Sordinary = true;
          }else{
            Println("Warning: unknown option");
            Println(opt);
          }
        }
      }
    } else if (IsNull(opt)){
    } else {
      Println("Warning: option should be given by an array.");
      Println(opt);
      Println("--------------------------------------------");
    }
  }

  sm1(" setupEnvForResolution ");
  /* If I do not put this macro, homogenization
     make a strange behavior. For example,
     [(2*x*Dx + 3*y*Dy+6) (0)] homogenize returns
     [(2*x*Dx*h + 3*y*Dy*h+6*h^3) (0)].
     4/19, 2000.
  */

  sm1(" (mmLarger) (matrix) switch_function ");
  if (! nohomog) {
    Println("Automatic homogenization.");
    g = Map(g,"Shomogenize");
  }else{
    Println("No automatic homogenization.");
  }
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

  Sprintln(g);
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
 "option: \"homogenized\" (no automatic homogenization) ",
 "Example: Sweyl(\"x,y\");",
 "         a=SresolutionFrameWithTower([x^3,x*y,y^3-1]);"]]);

def SresolutionFrame(f,opt) {
  local ans;
  ans = SresolutionFrameWithTower(f,opt);
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
    { Sverbose {
      (Warning: Homogenization and ReduceLowerTerms options are automatically turned off.) message } { } ifelse
      [(Homogenize) 0] system_variable
      [(ReduceLowerTerms) 0] system_variable
    } {  } ifelse
  ");
}
/* NOTE!!!  Be careful these changes of global environmental variables.
   We should make a standard set of values and restore these values
   after computation and interruption.  August 15, 2000.
*/
def  SturnOnHomogenization() {
  sm1("
    [(Homogenize)] system_variable 0 eq
    { Sverbose {
        (Warning: Homogenization and ReduceLowerTerms options are automatically turned ON.) message } {  } ifelse
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
    Sprint(".");

    t_syz = NewPolynomialVector(gLength);
    t_syz[i] = si;
    t_syz[j] = sj;
    syzAll[k] = t_syz;
  }
  t_syz = syzAll;
  Sprint("Done. betti="); Sprintln(betti);
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
  local d0;
  sm1(" [(grade) f] gbext (universalNumber) dc /d0 set ");
  /* Print("degree of "); Print(f); Print(" is "); Println(d0); */
  return(d0);
}

HelpAdd(["Sord_w",
["Sord_w(f,w) returns the w-order of f",
 "Example: Sord_w(x^2*Dx*Dy,[x,-1,Dx,1]):"]]);
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
  if (Sverbose) {
    sm1_pmat(p); 
    sm1_pmat(SgenerateTable(p[1]));
  }
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
  local i,ww, wd;
  /* extern WeightOfSweyl; */
  ww = WeightOfSweyl;
  f = Init(f);
  if (level <= 1) return(StotalDegree(f));
  i = Degree(f,es);
  return(StotalDegree(f)+Sdegree(tower[level-2,i],tower,level-1)); 
    
}

def SgenerateTable(tower) {
  local height, n,i,j, ans, ans_at_each_floor;

  /*
  Sprint("SgenerateTable: tower=");Sprintln(tower);  
  sm1(" print_switch_status "); */
  height = Length(tower);
  ans = NewArray(height);
  for (i=0; i<height; i++) {
    n = Length(tower[i]);
    ans_at_each_floor=NewArray(n);
    for (j=0; j<n; j++) {
      ans_at_each_floor[j] = Sdegree(tower[i,j],tower,i+1)-(i+1)
                            + OFFSET;
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
def ScopyArray(a) {
  local n, i,ans;
  n = Length(a);
  ans = NewArray(n);
  for (i=0; i<n; i++) {
    ans[i] = a[i];
  }
  return(ans);
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


def SlaScala(g,opt) {
  local rf, tower, reductionTable, skel, redundantTable, bases,
        strategy, maxOfStrategy, height, level, n, i,
        freeRes,place, f, reducer,pos, redundant_seq,bettiTable,freeResV,ww,
        redundantTable_ordinary, redundant_seq_ordinary,
        reductionTable_tmp;
  /* extern WeightOfSweyl; */
  ww = WeightOfSweyl;
  Sprint("WeightOfSweyl="); Sprintln(WeightOfSweyl);
  rf = SresolutionFrameWithTower(g,opt);  
  Sprint("rf="); if (Sverbose) {sm1_pmat(rf);}
  redundant_seq = 1;   redundant_seq_ordinary = 1;
  tower = rf[1];

  Sprintln("Generating reduction table which gives an order of reduction.");
  Sprint("WeghtOfSweyl="); Sprintln(WeightOfSweyl);
  Sprint2("tower="); Sprintln2(tower);
  reductionTable = SgenerateTable(tower);
  Sprint2("reductionTable="); 
  if (Sverbose || Sverbose2) {sm1_pmat(reductionTable);}

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
      reductionTable_tmp = ScopyArray(reductionTable[level]); 
      while (SthereIs(reductionTable_tmp,strategy)) {
        i = SnextI(reductionTable_tmp,strategy,redundantTable,
                   skel,level,freeRes);
        Sprintln([level,i]);
        reductionTable_tmp[i] = -200000;
        if (reductionTable[level,i] == strategy) {
           Sprint("Processing [level,i]= "); Sprint([level,i]);
           Sprint("   Strategy = "); Sprintln(strategy);
           Sprint2(strategy);
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
if (Sordinary) {
                  redundantTable[level-1,place] = redundant_seq;
                  redundant_seq++;
}else{
                  if (f[4] > f[5]) {
                    /* Zero in the gr-module */
                    Sprint("v-degree of [org,remainder] = ");
                    Sprintln([f[4],f[5]]);
                    Sprint("[level,i] = "); Sprintln([level,i]);
                    redundantTable[level-1,place] = 0;
                  }else{
                    redundantTable[level-1,place] = redundant_seq;
                    redundant_seq++;
                  }
}
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
  Sprintln2(" ");
  n = Length(freeRes);
  freeResV = SnewArrayOfFormat(freeRes);
  for (i=0; i<n; i++) {
    bases = freeRes[i];
    bases = Sbases_to_vec(bases,bettiTable[i]);
    freeResV[i] = bases;
  }
  return([freeResV, redundantTable,reducer,bettiTable,redundantTable_ordinary,rf]);
}

def SthereIs(reductionTable_tmp,strategy) {
  local n,i;
  n = Length(reductionTable_tmp);
  for (i=0; i<n; i++) {
    if (reductionTable_tmp[i] == strategy) {
      return(true);
    }
  }
  return(false);
}

def SnextI(reductionTable_tmp,strategy,redundantTable,
                                  skel,level,freeRes)
{
   local ii,n,p,myindex,i,j,bases;
   n = Length(reductionTable_tmp);
   if (level == 0) {
     for (ii=0; ii<n; ii++) {
       if (reductionTable_tmp[ii] == strategy) {
          return(ii);
        }
      }
   }else{
     for (ii=0; ii<n; ii++) {
       if (reductionTable_tmp[ii] == strategy) {
         p = skel[level,ii];
         myindex = p[0];
         i = myindex[0]; j = myindex[1];
         bases = freeRes[level-1];
         if (IsNull(bases[i]) || IsNull(bases[j])) {
            
         }else{
           return(ii);
         }
       }
     }
   }
   Sprint("reductionTable_tmp=");
   Sprintln(reductionTable_tmp);
   Sprintln("See also reductionTable, strategy, level,i");
   Error("SnextI: bases[i] or bases[j] is null for all combinations.");
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
  Sprintln([f,tower]);
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


/* WARNING:
  When you use SwhereInTower, you have to change gbList
  as below. Ofcourse, you should restrore the gbList
  SsetTower(StowerOf(tower,level));
  pos = SwhereInTower(syzHead,tower[level]);
*/
def SwhereInTower(f,tower) {
  local i,n,p,q;
  if (f == Poly("0")) return(-1);
  n = Length(tower);
  for (i=0; i<n; i++) {
    p = MonomialPart(tower[i]);
    q = MonomialPart(f);
    if (p == q) return(i);
  }
  Sprintln([f,tower]);
  Error("[f,tower]: f could not be found in the tower.");      
}

def Stag(f) {
  sm1(f," tag (universalNumber) dc /FunctionValue set");
}

def SpairAndReduction(skel,level,ii,freeRes,tower,ww) {
  local i, j, myindex, p, bases, tower2, gi, gj,
       si, sj, tmp, t_syz, pos, ans, ssp, syzHead,pos2,
       vdeg,vdeg_reduced;
  Sprintln("SpairAndReduction:");

  if (level < 1) Error("level should be >= 1 in SpairAndReduction.");
  p = skel[level,ii];
  myindex = p[0];  
  i = myindex[0]; j = myindex[1];
  bases = freeRes[level-1];
  Sprintln(["p and bases ",p,bases]);
  if (IsNull(bases[i]) || IsNull(bases[j])) {
    Sprintln([level,i,j,bases[i],bases[j]]);
    Error("level, i, j : bases[i], bases[j]  must not be NULL.");
  }

  tower2 = StowerOf(tower,level-1);
  SsetTower(tower2);  
  Sprintln(["level=",level]);
  Sprintln(["tower2=",tower2]);
  /** sm1(" show_ring ");   */

  gi = Stoes_vec(bases[i]);
  gj = Stoes_vec(bases[j]);

  ssp = Sspolynomial(gi,gj); 
  si = ssp[0,0];  
  sj = ssp[0,1];
  syzHead = si*es^i;
  /* This will be the head term, I think. But, double check. */
  Sprintln([si*es^i,sj*es^j]);

  Sprint("[gi, gj] = "); Sprintln([gi,gj]);
  sm1(" [(Homogenize)] system_variable  ");
  Sprint("Reduce the element "); Sprintln(si*gi+sj*gj);
  Sprint("by  "); Sprintln(bases);

  tmp = Sreduction(si*gi+sj*gj, bases);

  Sprint("result is "); Sprintln(tmp);

  /* This is essential part for V-minimal resolution. */
  /* vdeg = SvDegree(si*gi+sj*gj,tower,level-1,ww); */
  vdeg = SvDegree(si*gi,tower,level-1,ww); 
  vdeg_reduced = SvDegree(tmp[0],tower,level-1,ww);  
  Sprint("vdegree of the original = "); Sprintln(vdeg);
  Sprint("vdegree of the remainder = "); Sprintln(vdeg_reduced);

  t_syz = tmp[2];
  si = si*tmp[1]+t_syz[i];
  sj = sj*tmp[1]+t_syz[j];
  t_syz[i] = si; 
  t_syz[j] = sj;

  SsetTower(StowerOf(tower,level));
  pos = SwhereInTower(syzHead,tower[level]);

  SsetTower(StowerOf(tower,level-1));
  pos2 = SwhereInTower(tmp[0],tower[level-1]);
  ans = [tmp[0],t_syz,pos,pos2,vdeg,vdeg_reduced];
  /* pos is the place to put syzygy at level. */
  /* pos2 is the place to put a new GB at level-1. */   
  Sprintln(ans);
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

HelpAdd(["Sminimal",
["It constructs the V-minimal free resolution by LaScala's algorithm",
 "option: \"homogenized\" (no automatic homogenization)",
 "      : \"Sordinary\"   (no (u,v)-minimal resolution)",
 "Options should be given as an array.",
 "Example:  Sweyl(\"x,y\",[[\"x\",-1,\"y\",-1,\"Dx\",1,\"Dy\",1]]);", 
 "          v=[[2*x*Dx + 3*y*Dy+6, 0],",
 "             [3*x^2*Dy + 2*y*Dx, 0],",
 "             [0,  x^2+y^2],",
 "             [0,  x*y]];",
 "         a=Sminimal(v);",
 "         Sweyl(\"x,y\",[[\"x\",-1,\"y\",-1,\"Dx\",1,\"Dy\",1]]);", 
 "         b = ReParse(a[0]); sm1_pmat(b); ",
 "         IsExact_h(b,[x,y]):",
 "Note:  a[0] is the V-minimal resolution. a[3] is the Schreyer resolution."]]);

def Sminimal(g,opt) {
  local r, freeRes, redundantTable, reducer, maxLevel,
        minRes, seq, maxSeq, level, betti, q, bases, dr,
        betti_levelplus, newbases, i, j,qq, tminRes,bettiTable, ansSminimal;
  if (Length(Arglist) < 2) {
     opt = null;
  }
  /* Sordinary is set in SlaScala(g,opt) --> SresolutionFrameWithTower */

  ScheckIfSchreyer("Sminimal:0");
  r = SlaScala(g,opt);
  /* Should I turn off the tower?? */
  ScheckIfSchreyer("Sminimal:1");
  freeRes = r[0];
  redundantTable = r[1];
  reducer = r[2];
  bettiTable = SbettiTable(redundantTable);
  Sprintln2("BettiTable ------");
  if (Sverbose || Sverbose2) {sm1_pmat(bettiTable);}
  minRes = SnewArrayOfFormat(freeRes);
  seq = 0;
  maxSeq = SgetMaxSeq(redundantTable);
  maxLevel = Length(freeRes);
  for (level = 0; level < maxLevel; level++) {
    minRes[level] = freeRes[level];
  }
  seq=maxSeq+1;
  while (seq > 1) {
    seq--;  Sprint2(seq);
    for (level = 0; level < maxLevel; level++) {
      betti = Length(freeRes[level]);
      for (q = 0; q<betti; q++) {
        if (redundantTable[level,q] == seq) {
          Sprint("[seq,level,q]="); Sprintln([seq,level,q]);
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
            Sprintln(["level, q =", level,q]);
            Sprintln("bases="); if (Sverbose) {sm1_pmat(bases); }
            Sprintln("dr="); if (Sverbose) {sm1_pmat(dr);}
            Sprintln("newbases="); if (Sverbose) {sm1_pmat(newbases);}
            minRes[level+1] = newbases;
            freeRes = minRes;
#ifdef DEBUG
            for (qq=0; qq<betti; qq++) {
              if ((redundantTable[level,qq] >= seq) && 
                  (redundantTable[level,qq] <= maxSeq)) {
                for (i=0; i<betti_levelplus; i++) {
                  if (!IsZero(newbases[i,qq])) {
                    Println(["[i,qq]=",[i,qq]," is not zero in newbases."]);
                    Sprint("redundantTable ="); sm1_pmat(redundantTable[level]);
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
   tminRes = Stetris(minRes,redundantTable);
   ansSminimal = [SpruneZeroRow(tminRes), tminRes,
                  [ minRes, redundantTable, reducer,r[3],r[4]],r[0],r[5]];
   Sprintln2(" ");
   Println("------------ Note -----------------------------");
   Println("To get shift vectors, use Reparse and SgetShifts(resmat,w)");
   Println("To get initial of the complex, use Reparse and Sinit_w(resmat,w)");
   Println("0: minimal resolution, 3: Schreyer resolution ");
   Println("------------ Resolution Summary  --------------");
   Print("Betti numbers : "); 
   Println(Join([Length(ansSminimal[0,0,0])],Map(ansSminimal[0],"Length")));
   Print("Betti numbers of the Schreyer frame: ");
   Println(Join([Length(ansSminimal[3,0,0])],Map(ansSminimal[3],"Length")));
   Println("-----------------------------------------------");

   sm1(" restoreEnvAfterResolution ");
   Sordinary = false;

   return(ansSminimal);
  /* r[4] is the redundantTable_ordinary */
  /* r[0] is the freeResolution */
  /* r[5] is the skelton */
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
    Sprintln(["level=", level]);
    if (Sverbose){
      sm1_pmat(bases);
      sm1_pmat(newbases);
    }

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
  f = Init(f);
  if (level <= 0) {
    return(Sord_w(f,w));
  }
  i = Degree(f,es);
  ans = Sord_w(f,w) +
        SvDegree(tower[level-1,i],tower,level-1,w);
  return(ans);
}

def Sannfs(f,v) {
  local f2;
  f2 = ToString(f);
  if (IsArray(v)) {
     v = Map(v,"ToString");
  }
  sm1(" [f2 v] annfs /FunctionValue set ");
}

/* Sannfs2("x^3-y^2"); */
def Sannfs2(f) {
  local p,pp;
  p = Sannfs(f,"x,y");
  sm1(" p 0 get { [(x) (y) (Dx) (Dy)] laplace0 } map /p set ");
  Sweyl("x,y",[["x",-1,"y",-1,"Dx",1,"Dy",1]]); 
  pp = Map(p,"Spoly");
  return(Sminimal(pp)); 
}

HelpAdd(["Sannfs2",
["Sannfs2(f) constructs the V-minimal free resolution for the weight (-1,1)",
 "of the Laplace transform of the annihilating ideal of the polynomial f in x,y.",
 "See also Sminimal, Sannfs3.",
 "Example: a=Sannfs2(\"x^3-y^2\");",
 "         b=a[0]; sm1_pmat(b);",
 "         b[1]*b[0]:",
 "Example: a=Sannfs2(\"x*y*(x-y)*(x+y)\");",
 "         b=a[0]; sm1_pmat(b);",
 "         b[1]*b[0]:"
]]);
/* Some samples.
  The betti numbers of most examples are 2,1. (0-th and 1-th).
  a=Sannfs2("x*y*(x+y-1)"); ==> The betti numbers are 3, 2. 
  a=Sannfs2("x^3-y^2-x");    
  a=Sannfs2("x*y*(x-y)");    
*/


def Sannfs3(f) {
  local p,pp;
  p = Sannfs(f,"x,y,z");
  sm1(" p 0 get { [(x) (y) (z) (Dx) (Dy) (Dz)] laplace0 } map /p set ");
  Sweyl("x,y,z",[["x",-1,"y",-1,"z",-1,"Dx",1,"Dy",1,"Dz",1]]);
  pp = Map(p,"Spoly");
  return(Sminimal(pp));
}

HelpAdd(["Sannfs3",
["Sannfs3(f) constructs the V-minimal free resolution for the weight (-1,1)",
 "of the Laplace transform of the annihilating ideal of the polynomial f in x,y,z.",
 "See also Sminimal, Sannfs2.",
 "Example: a=Sannfs3(\"x^3-y^2*z^2\");",
 "         b=a[0]; sm1_pmat(b);",
 "         b[1]*b[0]: b[2]*b[1]:"]]);

     

/* Sannfs2("x*y*(x-y)*(x+y)"); is a test problem */
/* x y (x+y-1)(x-2),  x^3-y^2, x^3 - y^2 z^2,
   x y z (x+y+z-1) seems to be interesting, because the first syzygy
  contains 1.
*/

def CopyArray(m) {
  local ans,i,n;
  if (IsArray(m)) {
     n = Length(m);
     ans = NewArray(n);
     for (i=0; i<n; i++) {
       ans[i] = CopyArray(m[i]);
     }
     return(ans);
  }else{
     return(m);
  }
}
HelpAdd(["CopyArray",
["It duplicates the argument array recursively.",
 "Example: m=[1,[2,3]];",
 "         a=CopyArray(m); a[1] = \"Hello\";",
 "         Println(m); Println(a);"]]);

def IsZeroVector(m) {
  local n,i;
  n = Length(m);
  for (i=0; i<n; i++) {
    if (!IsZero(m[i])) { 
      return(false);
    }
  }
  return(true);
}

def SpruneZeroRow(res) {
  local minRes, n,i,j,m, base,base2,newbase,newbase2, newMinRes;

  minRes = CopyArray(res);
  n = Length(minRes);
  for (i=0; i<n; i++) {
    base = minRes[i];
    m = Length(base);
    if (i != n-1) {
      base2 = minRes[i+1];
      base2 = Transpose(base2);
    }
    newbase = [ ];
    newbase2 = [ ];
    for (j=0; j<m; j++) {
      if (!IsZeroVector(base[j])) {
        newbase = Append(newbase,base[j]);
        if (i != n-1) {
          newbase2 = Append(newbase2,base2[j]);
        }
      }
    }
    minRes[i] = newbase;
    if (i != n-1) {
      if (newbase2 == [ ]) {
        minRes[i+1] = [ ];
      }else{
        minRes[i+1] = Transpose(newbase2);
      }
    }
  }
  
  newMinRes = [ ];
  n = Length(minRes);
  i = 0;
  while (i < n ) {
    base = minRes[i];
    if (base == [ ]) {
      i = n; /* break; */
    }else{
      newMinRes = Append(newMinRes,base);
    }
    i++;
  }
  return(newMinRes);
}

def testAnnfs2(f) {
  local a,i,n;
  a = Sannfs2(f);
  b=a[0];
  n = Length(b);
  Println("------ V-minimal free resolution -----");
  sm1_pmat(b);
  Println("----- Is it complex?  ---------------");
  for (i=0; i<n-1; i++) {
    Println(b[i+1]*b[i]);
  }
  return(a);
}
def testAnnfs3(f) {
  local a,i,n;
  a = Sannfs3(f);
  b=a[0];
  n = Length(b);
  Println("------ V-minimal free resolution -----");
  sm1_pmat(b);
  Println("----- Is it complex?  ---------------");
  for (i=0; i<n-1; i++) {
    Println(b[i+1]*b[i]);
  }
  return(a);
}

def ToString_array(p) {
  local ans;
  if (IsArray(p)) {
    ans = Map(p,"ToString_array");
  }else{
    ans = ToString(p);
  }
  return(ans);
}

/* sm1_res_div([[x],[y]],[[x^2],[x*y],[y^2]],[x,y]): */

def sm1_res_div(I,J,V) {
  I = ToString_array(I);
  J = ToString_array(J);
  V = ToString_array(V);
  sm1(" [[ I J]  V ] res*div /FunctionValue set ");
}

/* It has not yet been working */
def sm1_res_kernel_image(m,n,v) {
  m = ToString_array(m);
  n = ToString_array(n);
  v = ToString_array(v);
  sm1(" [m n v] res-kernel-image /FunctionValue set ");
}
def Skernel(m,v) {
  m = ToString_array(m);
  v = ToString_array(v);
  sm1(" [ m v ] syz /FunctionValue set ");
}


def sm1_gb(f,v) {
  f =ToString_array(f);
  v = ToString_array(v);
  sm1(" [f v] gb /FunctionValue set ");
}


def SisComplex(a) {
  local n,i,j,k,b,p,q;
  n = Length(a);
  for (i=0; i<n-1; i++) {
    if (Length(a[i+1]) != 0) {
      b = a[i+1]*a[i];
      p = Length(b); q = Length(b[0]);
      for (j=0; j<p; j++) {
        for (k=0; k<q; k++) {
          if (!IsZero(b[j,k])) {
             Print("Is is not complex at ");
             Println([i,j,k]);
             return(false);
          }
        }
      }
    }
  }
  return(true);
}

def IsExact_h(c,v) {
  local a;
  v = ToString_array(v);
  a = [c,v];
  sm1(a," isExact_h /FunctionValue set ");
}
HelpAdd(["IsExact_h",
["IsExact_h(complex,var): bool",
 "It checks the given complex is exact or not in D<h> (homogenized Weyl algebra)",
 "cf. ReParse"
]]);

def IsSameIdeal_h(ii,jj,v) {
  local a;
  v = ToString_array(v);
  a = [ii,jj,v];
  sm1(a," isSameIdeal_h /FunctionValue set ");
}
HelpAdd(["IsSameIdeal_h",
["IsSameIdeal_h(ii,jj,var): bool",
 "It checks the given ideals are the same or not in D<h> (homogenized Weyl algebra)",
 "cf. ReParse"
]]);

def ReParse(a) {
  local c;
  if (IsArray(a)) {
    c = Map(a,"ReParse");
  }else{
    sm1(a," toString . /c set");
  }
  return(c);
}
HelpAdd(["ReParse",
["Reparse(obj): obj",
 "It parses the given object in the current ring.",
 "Outputs from SlaScala, Sschreyer may cause a trouble in other functions,",
 "because it uses the Schreyer order.",
 "In this case, ReParse the outputs from these functions.",
 "cf. IsExaxt_h"
]]);

def ScheckIfSchreyer(s) {
  local ss;
  sm1(" (report) (grade) switch_function /ss set ");
  if (ss != "module1v") {
     Print("ScheckIfSchreyer: from "); Println(s);
     Error("grade is not module1v");
  }
  /*
  sm1(" (report) (mmLarger) switch_function /ss set ");
  if (ss != "tower") {
     Print("ScheckIfSchreyer: from "); Println(s);
     Error("mmLarger is not tower");
  }
  */
  sm1(" [(Schreyer)] system_variable (universalNumber) dc /ss set ");
  if (ss != 1) {
     Print("ScheckIfSchreyer: from "); Printl(s);
     Error("Schreyer order is not set.");
  }
  /* More check will be necessary. */
  return(true);
}

def SgetShift(mat,w,m) {
  local omat;
  sm1(" mat { w m ord_w<m> {(universalNumber) dc}map } map /omat set");
  return(Map(omat,"Max")); 
}
HelpAdd(["SgetShift",
["SgetShift(mat,w,m) returns the shift vector of mat with respect to w with the shift m.",
 "Note that the order of the ring and the weight w must be the same.",
 "Example:  Sweyl(\"x,y\",[[\"x\",-1,\"Dx\",1]]); ",
 "          SgetShift([[x*Dx+1,Dx^2+x^5],[Poly(\"0\"),x],[x,x]],[\"x\",-1,\"Dx\",1],[2,0]):"]]);

def SgetShifts(resmat,w) {
  local i,n,ans,m0;
  n = Length(resmat);
  ans = NewArray(n+1);
  m0 = NewArray(Length(resmat[0,0]));
  ans[0] = m0;
  for (i=0; i<n; i++) {
    ans[i+1] = SgetShift(resmat[i],w,m0);
    m0 = ans[i+1];
  }
  return(ans);
}
HelpAdd(["SgetShifts",
["SgetShifts(resmat,w) returns the shift vectors of the resolution resmat",
 " with respect to w with the shift m.",
 "Note that the order of the ring and the weight w must be the same.",
 "Zero row is not allowed.",
 "Example:   a=Sannfs2(\"x^3-y^2\");",
 "           b=a[0]; w = [\"x\",-1,\"y\",-1,\"Dx\",1,\"Dy\",1];",
 "           Sweyl(\"x,y\",[w]); b = Reparse(b);",
 "           SgetShifts(b,w):"]]);

def Sinit_w(resmat,w) {
  local shifts,ans,n,i,m,mat,j;
  shifts = SgetShifts(resmat,w);
  n = Length(resmat);
  ans = NewArray(n);
  for (i=0; i<n; i++) {
    m = shifts[i];
    mat = ScopyArray(resmat[i]);
    for (j=0; j<Length(mat); j++) {
      mat[j] = Init_w_m(mat[j],w,m);
    }
    ans[i] = mat;
  }
  return(ans);
}
HelpAdd(["Sinit_w",
["Sinit_w(resmat,w) returns the initial of the complex resmat with respect to the weight w.",
 "Example:   a=Sannfs2(\"x^3-y^2\");",
 "           b=a[0]; w = [\"x\",-1,\"y\",-1,\"Dx\",1,\"Dy\",1];",
 "           Sweyl(\"x,y\",[w]); b = Reparse(b);",
 "           c=Sinit_w(b,w); c:"
]]);

/* This method does not work, because we have zero rows. 
   Think about it later. */
def SbettiTable(rtable) {
  local ans,i,j,pp;
  ans = SnewArrayOfFormat(rtable);
  for (i=0; i<Length(rtable); i++) {
    pp = 0;
    for (j=0; j<Length(rtable[i]); j++) {
       if (rtable[i,j] != 0) {pp = pp+1;}
    }
    ans[i] = pp;
  }
  return(ans);
}