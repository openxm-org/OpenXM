/* $OpenXM: OpenXM/src/k097/lib/restriction/demo.k,v 1.1 2000/12/14 13:18:41 takayama Exp $  */

load["restriction.k"];;
load("../ox/ox.k");;

def demoSendAsirCommand(a) {
  a.executeString("load(\"bfct\");");
  a.executeString(" def myann(F) { B=ann(eval_str(F)); print(B); return(map(dp_ptod,B,[hoge,x,y,z,s,hh,ee,dx,dy,dz,ds,dhh])); }; ");
  a.executeString(" def myann0(F) { B=ann0(eval_str(F)); print(B); return(map(dp_ptod,B[1],[hoge,x,y,z,s,hh,ee,dx,dy,dz,ds,dhh])); }; ");
  a.executeString(" def mybfct(F) { return(rtostr(bfct(eval_str(F)))); }; ");
}

as = startAsir();
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
  R = BfRoots1(Res0[0],"x,y");
  Println("Step3: computing the cohomology of the truncated complex.");
  Print("Roots and b-function are "); Println(R);
  R0 = R[0];
  Ans=Srestall(Res0, ["x", "y"],  ["x", "y"], R0[Length(R0)-1]);
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

def DeRham2WithAsir(f) {
  local s;
  s = ToString(f);
  II = asirAnn0XYZ(asssssir,f);
  Print("Step 1: Annhilating ideal (II)"); Println(II);
  sm1(" II  { [(x) (y) (Dx) (Dy) ] laplace0 } map /II set ");
  Sweyl("x,y",[["x",-1,"y",-1,"Dx",1,"Dy",1]]);
  pp = Map(II,"Spoly");
  Res = Sminimal(pp);
  Res0 = Res[0];
  Print("Step2: (-1,1)-minimal resolution (Res0) "); sm1_pmat(Res0);
  R = BfRoots1(Res0[0],"x,y");
  Println("Step3: computing the cohomology of the truncated complex.");
  Print("Roots and b-function are "); Println(R);
  R0 = R[0];
  Ans=Srestall(Res0, ["x", "y"],  ["x", "y"],R0[Length(R0)-1] );
  Print("Answer is ");Println(Ans[0]);
  return(Ans);
}
def DeRham3WithAsir(f) {
  local s;
  s = ToString(f);
  II = asirAnn0XYZ(asssssir,f);
  Print("Step 1: Annhilating ideal (II)"); Println(II);
  sm1(" II  { [(x) (y) (z) (Dx) (Dy) (Dz)] laplace0 } map /II set ");
  Sweyl("x,y,z",[["x",-1,"y",-1,"z",-1,"Dx",1,"Dy",1,"Dz",1]]);
  pp = Map(II,"Spoly");
  Res = Sminimal(pp);
  Res0 = Res[0];
  Print("Step2: (-1,1)-minimal resolution (Res0) "); sm1_pmat(Res0);
  R = BfRoots1(Res0[0],"x,y,z");
  Println("Step3: computing the cohomology of the truncated complex.");
  Print("Roots and b-function are "); Println(R);
  R0 = R[0];
  Ans=Srestall(Res0, ["x", "y", "z"],  ["x", "y", "z"],R0[Length(R0)-1] );
  Print("Answer is ");Println(Ans[0]);
  return(Ans);
}
