/* load["lib/all.k"];;  
*/
/* $OpenXM$
   The original was in misc-2007/11/logc2/logc2.kk
   It is put under the cvs repository of openxm.org
*/

def Syz0(f) {
  local ans;
  sm1(" f syz /ans set ");
  return(ans);
}
HelpAdd(["Syz0",
["Syz0 calls syz (sm1).",
 "Example: Syz0([[\"z^2-1\",\"z-1\"], \"z\"]); "
]]);

def Syz0_xy(f) {
  local ans;
  sm1(" [(x,y) ring_of_differential_operators 0] define_ring f { . homogenize} map message ");
  return( [1,1,1] );
}

/* Some test functions */
def logc2_pq(p,q) {
  local f,ans;
  RingD("x,y");
  f = x^p+y^q+x*y^(q-1);
  ans = Logc2(f);
  return(ans);
}

/* cf. mail from Paco in Jan, 2007 
   logc2_pqab(4,7,1,1);
   logc2_pqab(4,7,2,3); --> need minimal syzygy
*/ 
def logc2_pqab(p,q,a,b) {
  local f,ans;
  RingD("x,y");
  f = (x^p+y^q+x*y^(q-1))*(x^a-y^b);
  ans = Logc2(f);
  return(ans);
}

HelpAdd(["Logc2",
["Logc2(f) [f a polynomial in x and y] computes dimensions",
 "of the logarithmic cohomology groups.",
 "load[\"lib/all.k\"];; is required to use it.",
 "See Castro, Takayama: The Computation of the Logarithmic Cohomology for Plane Curves.",
 "Example: Logc2(\"x*y*(x-y)\"): "
]]);
def Logc2(f) {
  local s,ans,f,II,sss,pp,fx,fy;

  sm1("0 set_timer "); sm1(" oxNoX ");
  asssssir.OnTimer();

  RingD("x,y");
  /* f = x^p+y^q+x*y^(q-1); */
  f = ReParse(f);
  Print("f=");Println(f);
  fx = Dx*f; fx = Replace(fx,[[Dx,Poly("0")],[h,Poly("1")]]);
  fy = Dy*f; fy = Replace(fy,[[Dy,Poly("0")],[h,Poly("1")]]);

  pp = [f,fx,fy];
  Println(pp);
  sss = Syz0([pp]);
  sss = sss[0];
  Println(sss);
  if (Length(sss) != 2) Error("You need to use a function for Quillen-Suslin Theorem.");

  p1 = -sss[0,0]+sss[0,1]*Dx+sss[0,2]*Dy;
  p2 = -sss[1,0]+sss[1,1]*Dx+sss[1,2]*Dy;
  SSS=sss;
  Println([p1,p2]);
  sm1(" [p1,p2] { [(x) (y) (Dx) (Dy) ] laplace0 } map /II set ");

  Sweyl("x,y",[["x",-1,"y",-1,"Dx",1,"Dy",1]]);
  pp = Map(II,"ReParse");
  Res = Sminimal(pp);
  Res0 = Res[0];
  Println("Step1: minimal resolution (Res0) "); sm1_pmat(Res0);

  R = asir_BfRoots2(Res0[0]);
  Println("Step2: computing the cohomology of the truncated complex.");
  Print("Roots and b-function are "); Println(R);
  R0 = R[0];
  Ans=Srestall(Res0, ["x", "y"],  ["x", "y"], R0[Length(R0)-1]);

  Println("Timing data: sm1 "); sm1(" 1 set_timer ");
  Print("     ox_asir [CPU,GC]:  ");Println(asssssir.OffTimer());

  Print("Answer is "); Println(Ans[0]);
  return(Ans);

}

