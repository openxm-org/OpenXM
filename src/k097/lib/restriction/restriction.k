/*  $OpenXM$ */
load["lib/minimal/minimal-test.k"];;
Load_sm1(["Srestall_s.sm1","lib/restriction/Srestall_s.sm1"],"Srestall_s.sm1.loaded");

def Srestall(gg,ttxx,tt,k1) {
  local cohom, gg, ttxx, tt, k1, cohom0, cohomd,ans;
  sm1("gg dehomogenize /gg set");
  gg = ToString_array(gg);
  sm1(" [(x) ring_of_differential_operators [[(x) 1]] weight_vector 0] define_ring ]");
  sm1("[(Homogenize_vec) 1] system_variable");
  Println([Tag(gg),Tag(ttxx),Tag(tt)]);
  sm1("gg ttxx tt k1 (integer) dc Srestall1  /cohom0 set"); 
  sm1(" cohom0 {deRham.simp} map /cohomd set ");
  ans = [cohomd,cohom0];
  return(ans);
}
HelpAdd(["Srestall",
[ "Srestall(gg,v,rv,k1) evaluates the dimensions of all restictions of",
  "gg along the list of variables rv. Here, v is a list of variables and",
  " k1 is the maximal integral root of the b-function of gg.",
  "Srestall uses the function Sminimal to get a (-w,w)-minimal free resolution.",
  "cf. Bfroots1(ii,vv)"
]]);

def DeRham2(f) {
  local s;
  s = ToString(f);
  II = Sannfs(f,"x,y");
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
  Ans=Srestall(Res0, ["x", "y"], ["x", "y"],R0[Length(R0)-1]);
  Print("Answer is "); Println(Ans[0]);
  return(Ans);
}

def DeRham3(f) {
  local s;
  s = ToString(f);
  II = Sannfs(f,"x,y,z");
  Print("Step 1: Annhilating ideal (II)"); Println(II);
  sm1(" II 0 get { [(x) (y) (z) (Dx) (Dy) (Dz)] laplace0 } map /II set ");
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

def DeRham1(f) {
  local s;
  s = ToString(f);
  II = Sannfs(f,"x");
  Println("Step 1: Annhilating ideal (II)"); Println(II);
  sm1(" II 0 get { [(x) (Dx) ] laplace0 } map /II set ");
  Sweyl("x",[["x",-1,"Dx",1]]);
  pp = Map(II,"Spoly");
  Res = Sminimal(pp);
  Res0 = Res[0];
  Println("Step2: (-1,1)-minimal resolution (Res0) "); sm1_pmat(Res0);
  R = BfRoots1(Res0[0],"x");
  Println("Step3: computing the cohomology of the truncated complex.");
  Print("Roots and b-function are "); Println(R);
  R0 = R[0];
  Ans=Srestall(Res0, ["x"], ["x"],R0[Length(R0)-1]);
  Print("Answer is "); Println(Ans[0]);
  return(Ans);
}

/*  Demo for non-quasi */
def nonquasi(p,q) {
  local s,ans,f;
  f = x^p+y^q+x*y^(q-1);
  Print("f=");Println(f);
  s = ToString(f);
  sm1(" Onverbose ");
  sm1(" s [(s) (x) (y)] genericAnn /ans set ");
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

