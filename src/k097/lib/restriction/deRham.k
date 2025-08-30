/* $OpenXM: OpenXM/src/k097/lib/restriction/deRham.k,v 1.1 2006/10/01 05:49:22 takayama Exp $ */

/*
Require: restriction.k, ox.k

generic_bfct(ii,vv,dd,ww);
   ex.
    RingD("x,y");
    generic_bfct([Dx^2+Dy^2-1,Dx*Dy-4],[x,y],[Dx,Dy],[1,1]):
DeRham2(f);    
DeRham3(f);
*/

def demoSendAsirCommand(a) {
  a.executeString("load(\"bfct\")$");
  a.executeString(" def myann(F) { B=ann(eval_str(F)); print(B); return(map(dp_ptod,B,[hoge,x,y,z,s,hh,ee,dx,dy,dz,ds,dhh])); }$ ");
  a.executeString(" def myann0(F) { B=ann0(eval_str(F)); print(B); return(map(dp_ptod,B[1],[hoge,x,y,z,s,hh,ee,dx,dy,dz,ds,dhh])); }$ ");
  a.executeString(" def mybfct(F) { return(rtostr(bfct(eval_str(F)))); }$ ");
  a.executeString(" def mygeneric_bfct(F,VV,DD,WW) { print([F,VV,DD,WW]); return(generic_bfct(F,VV,DD,WW));}$ ");
}

if (Boundp("NoX")) {
  as = Asir.generate(false); 
}else{
  as = Asir.generate();
}

def demoReduction(v) {
  if (v == true) {
    sm1(" [(Verbose) 1] system_variable  [(DebugReductionRed) 1] system_variable Onverbose ");
  }else{
    sm1(" [(Verbose) 0] system_variable  [(DebugReductionRed) 0] system_variable Offverbose ");
  }
}

asssssir = as;
demoSendAsirCommand(as);
RingD("x,y,z,s");

def asirBfunction(a,f) {
  local p,b;
  p = ToString(f);
  Println(p);
  b = a.rpc("mybfct",[p]);
  sm1(" b . /b set ");
  return(b);
}

def asirAnnfsXYZ(a,f) {
  local p,b;
  RingD("x,y,z,s");  /* Fix!! See the definition of myann() */
  p = ToString(f);
  b = a.rpc("myann",[p]);
  return(b);
}


def asir_generic_bfct(a,ii,vv,dd,ww) {
   local ans;
   ans = a.rpc_str("mygeneric_bfct",[ii,vv,dd,ww]);
   return(ans);
}
/* a=startAsir();
   asir_generic_bfct(a,[Dx^2+Dy^2-1,Dx*Dy-4],[x,y],[Dx,Dy],[1,1]): */

def generic_bfct(ii,vv,dd,ww) {
  local ans;
  ans =asir_generic_bfct(asssssir,ii,vv,dd,ww);
  return(ans);
}

HelpAdd(["generic_bfct",
[ "generic_bfct(ii,vv,dd,ww) gives the b-function of the ideal ii for the weight vector ww.",
  "vv is a list of variables and dd is a list of D-variables.",
  "It calls ox_asir and returns a string",
  "Example: RingD(\"x,y\"); f=generic_bfct([Dx^2+Dy^2-1,Dx*Dy-4],[x,y],[Dx,Dy],[1,1]); Println(f);",
  "      RingD(\"s\"); ff=ReParse(f); Factor(ff);"
],
["ii","vv","dd","ww"],
["Factor", "Srestriction", "Sintegration"],
"b-function for a weight vector"
]);


/* usage: misc/tmp/complex-ja.texi */
def ChangeRing(f) {
  local r;
  r = GetRing(f);
  if (Tag(r) == 14) {
    SetRing(r);
    return(true);
  }else{
    return(false);
  }
}

def asir_BfRoots2(G) {
   local bb,ans,ss;
   sm1(" G flatten {dehomogenize} map /G set ");
   ChangeRing(G);
   ss = asir_generic_bfct(asssssir,G,[x,y],[Dx,Dy],[1,1]);
   bb = [ss];
   sm1(" bb 0 get findIntegralRoots { (universalNumber) dc } map /ans set ");
   return([ans, bb]);
}
def asir_BfRoots3(G) {
   local bb,ans,ss;
   sm1(" G flatten {dehomogenize} map /G set ");
   ChangeRing(G);
   ss = asir_generic_bfct(asssssir,G,[x,y,z],[Dx,Dy,Dz],[1,1,1]);
   bb = [ss];
   sm1(" bb 0 get findIntegralRoots { (universalNumber) dc } map /ans set ");
   return([ans, bb]);
}

def asir_BfRoots4(G) {
   local bb,ans,ss;
   sm1(" G flatten {dehomogenize} map /G set ");
   ChangeRing(G);
   ss = asir_generic_bfct(asssssir,G,[x,y,z,vv],[Dx,Dy,Dz,Dvv],[0,0,0,1]);
   bb = [ss];
   sm1(" bb 0 get findIntegralRoots { (universalNumber) dc } map /ans set ");
   return([ans, bb]);
}

def findMinSol(f) {
  sm1(" f (string) dc findIntegralRoots 0 get (universalNumber) dc /FunctionValue set ");
}

def asirAnnXYZ(a,f) {
  local p,b,b0,k1;
  RingD("x,y,z,s");  /* Fix!! See the definition of myann() */
  p = ToString(f);
  b = a.rpc("myann",[p]);
  Print("Annhilating ideal with s is "); Println(b);
  b0 = asirBfunction(a,f);
  Print("bfunction is "); Println(b0);
  k1 = findMinSol(b0);
  Print("Minimal integral root is "); Println(k1);
  sm1(" b { [[(s). k1 (string) dc .]] replace } map /b set ");
  return(b);
}


def nonquasi2(p,q) {
  local s,ans,f;

  sm1("0 set_timer "); sm1(" oxNoX ");
  asssssir.OnTimer();

  f = x^p+y^q+x*y^(q-1);
  Print("f=");Println(f);
  s = ToString(f);
  sm1(" Onverbose ");
  ans = asirAnnfsXYZ(asssssir,f);
  sm1(" ans 0 get (ring) dc ring_def ");
  sm1("[ ans { [[(s). (-1).]] replace } map ] /II set ");
  Println("Step 1: Annhilating ideal (II)"); Println(II);
  sm1(" II 0 get { [(x) (y) (Dx) (Dy) ] laplace0 } map /II set ");
  Sweyl("x,y",[["x",-1,"y",-1,"Dx",1,"Dy",1]]);
  pp = Map(II,"Spoly");
  Res = Sminimal(pp);
  Res0 = Res[0];
  Println("Step2: (-1,1)-minimal resolution (Res0) "); sm1_pmat(Res0);
/*  R = BfRoots1(Res0[0],"x,y"); */
  R = asir_BfRoots2(Res0[0]);
  Println("Step3: computing the cohomology of the truncated complex.");
  Print("Roots and b-function are "); Println(R);
  R0 = R[0];
  Ans=Srestall(Res0, ["x", "y"],  ["x", "y"], R0[Length(R0)-1]);

  Println("Timing data: sm1 "); sm1(" 1 set_timer ");
  Print("     ox_asir [CPU,GC]:  ");Println(asssssir.OffTimer());

  Print("Answer is "); Println(Ans[0]);
  return(Ans);
}

def asirAnn0XYZ(a,f) {
  local p,b,b0;
  RingD("x,y,z,s");  /* Fix!! See the definition of myann() */
  p = ToString(f);
  b = a.rpc("myann0",[p]);
  Print("Annhilating ideal of f^r is "); Println(b);
  return(b);
}


def DeRham2(f) {
  local s;

  sm1("0 set_timer "); sm1(" oxNoX ");
  asssssir.OnTimer();

  s = ToString(f);
  II = asirAnn0XYZ(asssssir,f);
  Print("Step 1: Annhilating ideal (II)"); Println(II);
  sm1(" II  { [(x) (y) (Dx) (Dy) ] laplace0 } map /II set ");
  Sweyl("x,y",[["x",-1,"y",-1,"Dx",1,"Dy",1]]);
  pp = Map(II,"Spoly");
  Res = Sminimal(pp);
  Res0 = Res[0];
  Print("Step2: (-1,1)-minimal resolution (Res0) "); sm1_pmat(Res0);
  /* R = BfRoots1(Res0[0],"x,y"); */
  R = asir_BfRoots2(Res0[0]);
  Println("Step3: computing the cohomology of the truncated complex.");
  Print("Roots and b-function are "); Println(R);
  R0 = R[0];
  Ans=Srestall(Res0, ["x", "y"],  ["x", "y"],R0[Length(R0)-1] );

  Println("Timing data: sm1 "); sm1(" 1 set_timer ");
  Print("     ox_asir [CPU,GC]:  ");Println(asssssir.OffTimer());

  Print("Answer is ");Println(Ans[0]);
  return(Ans);
}
def DeRham3(f) {
  local s;

  sm1("0 set_timer "); sm1(" oxNoX ");
  asssssir.OnTimer();

  s = ToString(f);
  II = asirAnn0XYZ(asssssir,f);
  Print("Step 1: Annhilating ideal (II)"); Println(II);
  sm1(" II  { [(x) (y) (z) (Dx) (Dy) (Dz)] laplace0 } map /II set ");
  Sweyl("x,y,z",[["x",-1,"y",-1,"z",-1,"Dx",1,"Dy",1,"Dz",1]]);
  pp = Map(II,"Spoly");
  Res = Sminimal(pp);
  Res0 = Res[0];
  Print("Step2: (-1,1)-minimal resolution (Res0) "); sm1_pmat(Res0);
  /* R = BfRoots1(Res0[0],"x,y,z");  */
  R = asir_BfRoots3(Res0[0]);
  Println("Step3: computing the cohomology of the truncated complex.");
  Print("Roots and b-function are "); Println(R);
  R0 = R[0];
  Ans=Srestall(Res0, ["x", "y", "z"],  ["x", "y", "z"],R0[Length(R0)-1] );

  Println("Timing data: sm1 "); sm1(" 1 set_timer ");
  Print("     ox_asir [CPU,GC]:  ");Println(asssssir.OffTimer());

  Print("Answer is ");Println(Ans[0]);
  return(Ans);
}


/*  test data 

   NoX=true;
   nonquasi2(4,5);
   nonquasi2(4,6);
   nonquasi2(4,7);
   nonquasi2(4,8);
   nonquasi2(4,9);
   nonquasi2(4,10);

   nonquasi2(5,6);
   nonquasi2(6,7);
   nonquasi2(7,8);
   nonquasi2(8,9);
   nonquasi2(9,10);
*/

   P2 = [
     "x^3-y^2",
     "y^2-x^3-x-1",
     "y^2-x^5-x-1",
     "y^2-x^7-x-1",
     "y^2-x^9-x-1",
     "y^2-x^11-x-1"
   ];

   P3 = [
     "x^3-y^2*z^2",
     "x^2*z+y^3+y^2*z+z^3",
     "y*z^2+x^3+x^2*y^2+y^6",
     "x*z^2+x^2*y+x*y^3+y^5"
   ];


def diff_tmp(ff,xx) {
  local g;
  g = xx*ff;
  return( Replace(g,[[xx,Poly("0")],[h,Poly("1")]]));
}

def Localize3WithAsir(I,f) {
  local s;

  sm1("0 set_timer "); sm1(" oxNoX ");
  asssssir.OnTimer();
  
  RingD("x,y,z,vv");  
  /* BUG: use of RingD("x,y,z,v") causes an expected error.
    [x2,x3,x4,x4] in mygeneric_bfct.  (should be [x2,x3,x4,x5]).
  */
  f = ReParse(f);
  I = ReParse(I);
  /* Test data. */
  /*
  f = x^3-y^2*z^2;
  I = [f^2*Dx-3*x^2, f^2*Dy-2*y*z^2, f^2*Dz-2*y^2*z];

  f = x^3-y^2;
  I = [f^2*Dx-diff_tmp(f,Dx), f^2*Dy-diff_tmp(f,Dy), Dz];
  */

  r1 = Dx-vv^2*diff_tmp(f,Dx)*Dvv;
  r2 = Dy-vv^2*diff_tmp(f,Dy)*Dvv;
  r3 = Dz-vv^2*diff_tmp(f,Dz)*Dvv;
  II = ReParse(I);
  sm1(" II { [[(Dx). r1] [(Dy). r2] [(Dz). r3]] replace } map dehomogenize /II set ");

  Print("Step 1: phi(J)"); Println(II);
  II = Join(II,[vv*f-1]);
  Print("Step 2: <phi(J),vf-1>"); Println(II);

  Println("Step3: computing the integral.");
  sm1(" II  { [(x) (y) (z) (vv) (Dx) (Dy) (Dz) (Dvv)] laplace0 } map /JJ set ");
  Sweyl("x,y,z,vv",[["vv",-1,"Dvv",1]]);
  pp = Map(JJ,"Spoly");
  R = asir_BfRoots4(pp);
  Print("Roots and b-function are "); Println(R);

  R0 = R[0];
  k1 = R0[Length(R0)-1];
  sm1(" [(parse) (intw.sm1) pushfile] extension /intw.verbose 1 def ");
  sm1(" [II [(vv) (x) (y) (z)] [(vv) -1 (Dvv) 1] k1 (integer) dc] integral-k1 /Ans set ");

  Println("Timing data: sm1 "); sm1(" 1 set_timer ");
  Print("     ox_asir [CPU,GC]:  ");Println(asssssir.OffTimer());

  Print("Answer is ");Println(Ans);
  return(Ans);
}


def Ltest2() {
  RingD("x,y,z");
  f = x^3-y^2;
  I = [f^2*Dx-diff_tmp(f,Dx), f^2*Dy-diff_tmp(f,Dy), Dz];
  return( Localize3WithAsir(I,f) );
}

/* The polynomial variable name w cannot be used. Why? ww also causes a trouble, because
   perhaps there is a local variable ww. ww --> www.  2014.11.27 */
def asir_BfRoots4www(G) {
   local bb,ans,ss;
   sm1(" G flatten {dehomogenize} map /G set ");
   ChangeRing(G);
   ss = asir_generic_bfct(asssssir,G,[x,y,z,www],[Dx,Dy,Dz,Dwww],[1,1,1,1]);
   bb = [ss];
   sm1(" bb 0 get findIntegralRoots { (universalNumber) dc } map /ans set ");
   return([ans, bb]);
}
def asirAnn0XYZWWW(a,f) {
  local p,b,b0;
  RingD("x,y,z,www,s");  /* Fix!! See the definition of myann0www() */
  p = ToString(f);
  a.executeString(" def myann0www(F) { B=ann0(eval_str(F)); print(B); return(map(dp_ptod,B[1],[hoge,x,y,z,www,s,hh,ee,dx,dy,dz,dwww,ds,dhh])); }$ ");
  b = a.rpc("myann0www",[p]);
  Print("Annhilating ideal of f^r is "); Println(b);
  return(b);
}

def DeRham4(f) {
  local s;

  sm1("0 set_timer "); sm1(" oxNoX ");
  asssssir.OnTimer();

  s = ToString(f);
  II = asirAnn0XYZWWW(asssssir,f);
  Print("Step 1: Annhilating ideal (II)"); Println(II);
  sm1(" II  { [(x) (y) (z) (www) (Dx) (Dy) (Dz) (Dwww)] laplace0 } map /II set ");
  Sweyl("x,y,z,www",[["x",-1,"y",-1,"z",-1,"www",-1,"Dx",1,"Dy",1,"Dz",1,"Dwww",1]]);
  pp = Map(II,"Spoly");
  Res = Sminimal(pp);
  Res0 = Res[0];
  Print("Step2: (-1,1)-minimal resolution (Res0) "); sm1_pmat(Res0);

  R = asir_BfRoots4www(Res0[0]);
  Println("Step3: computing the cohomology of the truncated complex.");
  Print("Roots and b-function are "); Println(R);
  R0 = R[0];
  Ans=Srestall(Res0, ["x", "y", "z","www"],  ["x", "y", "z","www"],R0[Length(R0)-1] );

  Println("Timing data: sm1 "); sm1(" 1 set_timer ");
  Print("     ox_asir [CPU,GC]:  ");Println(asssssir.OffTimer());

  Print("Answer is ");Println(Ans[0]);
  return(Ans);
}

HelpAdd(["DeRham4",
[ "DeRham4(f) gives the dimensions of de Rham cohomology groups of C^4-V(f).",
  "f (string) is a polynomial in Z[x,y,z,www].",
  "DeRham2 is for Z[x,y], DeRham3 is for Z[x,y,z].",
  "Example: c=DeRham4(\"x^3+y^2+z^2+www^2-1\"); c:",
  "         It will take more than 10 minutes. Answer will be [2,0,0,1,1]",
  "                           standing for dim of [H^4,H^3,H^2,H^1,H^0]."
],
["f"],
["ToString", "Sintegration"],
"de Rham cohomology groups of the complement of the zero set of a polynomial"
]);
