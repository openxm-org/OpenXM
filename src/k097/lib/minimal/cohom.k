/* $OpenXM: OpenXM/src/k097/lib/minimal/cohom.k,v 1.4 2000/11/19 05:50:30 takayama Exp $ */

/* k0 interface functions for cohom.sm1 */
def Boundp(a) {
   local b;
   sm1("[(parse) [(/) ",a," ( load tag 0 eq 
                          { /FunctionValue 0 def }
                          { /FunctionValue 1 def } ifelse )] cat ] extension");
}

def load_cohom() {
  if (Boundp("cohom.sm1.loaded")) {
  }else{
    sm1(" [(parse) (k0-cohom.sm1) pushfile ] extension ");
  }
}

load_cohom();

def sm1_deRham(a,b) {
  local aa,bb;
  aa = ToString(a); 
  if (IsArray(b)) {
     bb = Map(b,"ToString");
  }else{
     bb = ToString(b);
  }
  sm1("[", aa,bb, " ]  deRham /FunctionValue set ");
}
HelpAdd(["sm1_deRham",
["sm1_deRham(f,v) computes the dimension of the deRham cohomology groups",
 "of C^n - V(f)",
 "This function does not use (-w,w)-minimal free resolution.",
  "Example:  sm1_deRham(\"x^3-y^2\",\"x,y\");"
]]);


def Weyl(v,w,p) {
  local a,L;
  L=Length(Arglist);
  if (L == 1) {
    a=RingD(v);
  } else if (L == 2) {
    a=RingD(v,w);
  }else if (L == 3) {
    a=RingD(v,w,p);
  }else{
    Println("Error: argument mismatch");
    return(null);
  }
  sm1(" define_ring_variables ");
  return(a);
}
HelpAdd(["Weyl",
[ "Weyl(v,w) defines the Weyl algebra (the ring of differential operators)",
  "with the weight vector w.",
  "Example:  Weyl(\"x,y\",[[\"x\",-1,\"Dx\",1]]); "
]]);
/* (  and  ) must match  in HelpAdd. */

def sm1_pmat(a) {
  sm1(a," pmat ");
}

Weyl("x,y");
/*  See page 8, (2.2).  */
cech=[ 
  [ [x*Dx], 
    [y*Dy]
  ],
  [[ y*Dy, -x*Dx]]
];

def sm1_v_string(V) {
  if (IsArray(V)) {
    V = Map(V,"ToString");
  }else {
    V = ToString(V);
  }
  return(V);
}

def sm1_syz(A,V,W) {
  local L,P;
  L=Length(Arglist);
  if (L == 1) {
    P = [A];
  }else if (L==2) {
    V = sm1_v_string(V);
    P = [A,V];
  }else if (L==3) {
    P = [A,V,W];
  }else {
    Println("sm1_syz: Argument mismatch");
    return(null);
  }
  sm1(P," syz /FunctionValue set");
}
/*  
  cf.  Kernel() 
  sm1_syz([x*Dx,y*Dy],[x,y]):
  We want to syz_h, too.
  Step 1: Control by global variable ?  syz ==> syz_generic
  Step 2: syz and syz_h
*/

def sm1_resol1(I,V,W) {
  local P,L;
  L=Length(Arglist);
  if (L == 1) {
    P = [I];
  }else if (L==2) {
    V = sm1_v_string(V);
    P = [I,V];
  }else if (L==3) {
    P = [I,V,W];
  }else {
    Println("sm1_syz: Argument mismatch");
    return(null);
  }
  sm1(P," resol1 /FunctionValue set ");
}
/*  sm1_resol1([x^2,x*y],[x,y]):  */

def sm1_res_solv(A,B,C) {
  local P,L;
  L=Length(Arglist);
  if (L == 2) {
    P = [A,B];
    sm1(P," res-solv /FunctionValue set");
  }else if (L==3) {
    C = sm1_v_string(C);
    P = [[A,B], C];
    sm1(P," res*solv /FunctionValue set ");
  }else{
    Println("Error: argument mismatch");
    return(null);
  }
}
/*
 sm1_res_solv(
  [[x*Dx + 2, 0], 
   [Dx+3,    x^3],
   [3,      x],
   [Dx*(x*Dx + 3) - (x*Dx + 2)*(x*Dx -4), 0]],
   [1, 0], [x,y]):

 sm1_res_solv([x,1],1,"x"):
 sm1_res_solv([x,y],y,"x,y"):
*/

def sm1_res_solv_h(A,B,C) {
  local P;
  P = [[A,B], C];
  sm1(P," res*solv*h /FunctionValue set ");
}

def Reparse(A) {
  if (IsArray(A)) {
    return(Map(A,"Reparse"));
  }else if (IsPolynomial(A) || IsInteger(A)) {
    return(Poly(ToString(A)));
  }else{
    return(A);
  }
}

def sm1_res_sub2Q(I,V) {
  local L,P;
  L = Length(Arglist);
  if (L == 1) {
    P = I;
  }else if ( L == 2) {
    V = sm1_v_string(V);
    if (IsArray(V)) {
      sm1(V," from_records /V set ");
    }
    Weyl(V);
    P = Reparse(I);
  }
  sm1(P," res-sub2Q /FunctionValue set ");
}

/*
   sm1_res_sub2Q([x*Dx,Dy]):
   M res-sub2Q =: J,   M \simeq D^p/J
*/

def ex2_9() {
  Weyl("x,y,z");
  I = [ x*Dx+y*Dy+z*Dz+6,
        z^2*Dy-y^2*Dz,
        z^2*Dx-x^2*Dz,
        y^2*Dx-x^2*Dy,
        x^3*Dz+y^3*Dz+z^3*Dz+6*z^2,
        x^3*Dy+y^3*Dy+y^2*z*Dz+6*y^2];
  a = sm1_resol1(I,"x,y,z");
  return(a);
}

def to_int0(A) {
   local i,c,n,r;
   if (IsArray(A)) {
     n = Length(A);
     r = NewArray(n);
     for (i=0; i<n; i++) {
       r[i] = to_int0(A[i]);
     }
     return(r);
   } else if (IsInteger(A)) {
     return(IntegerToSm1Integer(A));
   } else {
     return(A);
   }
}
HelpAdd(["Translate.to_int0",
 ["to_int0(a) :  as same as sm1_push_int0."]]);


def GKZ(A,B) {
  /* we need sm1_rat_to_p in a future. */
  local c;
  c = to_int0([A,B]);
  sm1(c," gkz /FunctionValue set ");
}
HelpAdd(["GKZ.GKZ",
  ["GKZ(a,b) returns the GKZ systems associated to the matrix a and the vector b",
   "The answer is given by strings.",
   "Example: GKZ([[1,1,1,1],[0,1,3,4]],[0,2]);"]]);

def ToricIdeal(A) {
  /* we need sm1_rat_to_p in a future. */
  local c,B,i,n,pp;
  n = Length(A);
  B = NewArray(n);
  for (i=0; i<n; i++) {B[i] = 0;}
  c = to_int0([A,B]);
  sm1(c," gkz 0 get /pp set ");
  for (i=0; i<n; i++) { pp = Rest(pp); }
  return(pp);
}
HelpAdd(["ToricIdeal",
  ["ToricIdeal(a) returns the affine toric ideal associated to the matrix a",
   "The answer is given by a list of strings.",
   "Example: ToricIdeal([[1,1,1,1],[0,1,3,4]]);"]]);

def Rest(a) {
  sm1(a," rest /FunctionValue set ");
}
HelpAdd(["Rest",
["Rest(a), list a; "]]);

def Annfs(f,v) {
  local fs;
  fs = ToString(f);
  sm1(" [fs v] annfs /FunctionValue set ");
}
HelpAdd(["Annfs",
["Annfs(f,v) computes the annihilating ideal of f^r and the Bernstein-Sato",
 "  polynomial b(s) of f",
 "Return value: [Ann(f^r), r, b(s)] where r is the minimal integral root of",
 "              b(s) = 0.",
 "Example:  Annfs(x^2+y^2,\"x,y\"): "
]]);
