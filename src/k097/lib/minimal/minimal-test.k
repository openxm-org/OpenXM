/* $OpenXM: OpenXM/src/k097/lib/minimal/minimal-test.k,v 1.20 2000/08/22 05:34:06 takayama Exp $ */
load["minimal.k"];
def sm1_resol1(p) {
  sm1(" p resol1 /FunctionValue set ");
}

def test8() {
  local p,pp,ans,b,c,cc,ww,ww2;
  f = "x^3-y^2*z^2";
  p = Sannfs(f,"x,y,z");
  ww = [["x",1,"y",1,"z",1,"Dx",1,"Dy",1,"Dz",1,"h",1],
                 ["x",-1,"y",-1,"z",-1,"Dx",1,"Dy",1,"Dz",1]];
  ww2 = [["x",-1,"y",-1,"z",-1,"Dx",1,"Dy",1,"Dz",1]];
  sm1(" p 0 get { [(x) (y) (z) (Dx) (Dy) (Dz)] laplace0 } map /p set ");
  Sweyl("x,y,z",ww);
  pp = Map(p,"Spoly");
  /* return(pp); */
  /* pp =
     [y*Dy-z*Dz , -2*x*Dx-3*y*Dy+1 , 2*x*Dy*Dz^2-3*y*Dx^2 , 
      2*x*Dy^2*Dz-3*z*Dx^2 , 2*x*z*Dz^3-3*y^2*Dx^2+4*x*Dz^2 ] 
  */
  ans = sm1_resol1([pp,"x,y,z",ww]);
  /* Schreyer is in ans. */

  v = [x,y,z];
  b = ans;
  Println("------ ker=im for Schreyer ?------------------");
  c = Skernel(b[0],v);
  c = c[0];
  sm1_pmat([c,b[1],v]);
  cc = sm1_res_div(c,b[1],v);
  sm1_pmat(sm1_gb(cc,v));
  c = Skernel(b[1],v);
  c = c[0];
  cc = sm1_res_div(c,b[2],v);
  sm1_pmat(sm1_gb(cc,v));
  return(ans);
}
/*
   a = test8();
   SisComplex(a):
*/

def test11() {
  local  a;
  a = test_ann3("x^3-y^2*z^2");
  return(a);
}
/* f should be a string. */
/* a=test_ann3("x^3+y^3+z^3");
It returns the following resolution in 1.5 hours.  June 14, 2000.
 [ 
  [ 
    [    x*Dx+y*Dy+z*Dz-3*h^2 ] 
    [    -z*Dy^2+y*Dz^2 ] 
    [    -z*Dx^2+x*Dz^2 ] 
    [    -y*Dx^2+x*Dy^2 ] 
  ]
  [ 
    [    0 , -x , y , -z ] 
    [    z*Dx^2-x*Dz^2 , x*Dy , x*Dx+z*Dz-3*h^2 , z*Dy ] 
    [    y*Dx^2-x*Dy^2 , -x*Dz , y*Dz , x*Dx+y*Dy-3*h^2 ] 
    [    0 , Dx^2 , -Dy^2 , Dz^2 ] 
    [    z*Dy^2-y*Dz^2 , x*Dx+y*Dy+z*Dz-2*h^2 , 0 , 0 ] 
  ]
  [ 
    [    -x*Dx+3*h^2 , y , -z , 0 , -x ] 
    [    Dy^3+Dz^3 , Dy^2 , -Dz^2 , x*Dx+y*Dy+z*Dz , -Dx^2 ] 
  ]
 ]
*/
def test_ann3(f) {
  local a,v,ww2,ans2;
  a = Sannfs3(f);
  ans2 = a[0];
  v = [x,y,z];
  ww2 = [["x",-1,"y",-1,"z",-1,"Dx",1,"Dy",1,"Dz",1]];
  Sweyl("x,y,z",ww2);
  ans2 = ReParse(ans2);
  r= IsExact_h(ans2,[x,y,z]);
  Println(r);
  return([r,ans2,a]);
}
def test11a() {
  local a,v,ww2,ans2;
/* constructed by test11.
  ans2 =
       [[[y*Dy-z*Dz] , [-2*x*Dx-3*z*Dz+h^2] , [2*x*Dy*Dz^2-3*y*Dx^2*h] , [2*x*Dy^2*Dz-3*z*Dx^2*h]] , 
        [[3*Dx^2*h , 0 , Dy , -Dz] , 
         [6*x*Dy*Dz^2-9*y*Dx^2*h , -2*x*Dy*Dz^2+3*y*Dx^2*h , -2*x*Dx-3*y*Dy , 0] , 
         [0 , 2*x*Dy^2*Dz-3*z*Dx^2*h , 0 , 2*x*Dx+3*z*Dz] , 
         [2*x*Dx+3*z*Dz-h^2 , y*Dy-z*Dz , 0 , 0] , 
         [0 , 0 , 0 , 0] , 
         [2*x*Dy*Dz , 0 , z , -y] , 
         [0 , 0 , 0 , 0] , 
         [0 , 0 , 0 , 0] , 
         [0 , 0 , 0 , 0]] , 
  [[0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0] , 
   [0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0] , 
   [0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0] , 
   [-2*x*Dx-3*y*Dy-3*z*Dz-6*h^2 , -Dy , -Dz , 3*Dx^2*h , 3*Dy^2 , 3*Dy*Dz , -2*x*Dy , 2*x*Dz , 0] , 
   [0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0] , 
   [3*y*z , z , y , -2*x*Dy*Dz , -3*z*Dy , 2*x*Dx , 2*x*z , -2*x*y , 0] , 
   [0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0] , 
   [0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0] , 
   [0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0]] , 
   [[0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0] , 
    [0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0] , 
    [0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0]]]
*/
  ans2 =
       [[[y*Dy-z*Dz] , [-2*x*Dx-3*z*Dz+h^2] , [2*x*Dy*Dz^2-3*y*Dx^2*h] , [2*x*Dy^2*Dz-3*z*Dx^2*h]] , 
        [[3*Dx^2*h , 0 , Dy , -Dz] , 
         [6*x*Dy*Dz^2-9*y*Dx^2*h , -2*x*Dy*Dz^2+3*y*Dx^2*h , -2*x*Dx-3*y*Dy , 0] , 
         [0 , 2*x*Dy^2*Dz-3*z*Dx^2*h , 0 , 2*x*Dx+3*z*Dz] , 
         [2*x*Dx+3*z*Dz-h^2 , y*Dy-z*Dz , 0 , 0] , 
         [2*x*Dy*Dz , 0 , z , -y]], 
  [[-2*x*Dx-3*y*Dy-3*z*Dz-6*h^2 , -Dy , -Dz , 3*Dx^2*h , 3*Dy*Dz ] , 
   [3*y*z , z , y , -2*x*Dy*Dz , 2*x*Dx]]];

  sm1_pmat( ans2[1]*ans2[0] );
  sm1_pmat( ans2[2]*ans2[1] );
  ww2 = [["x",-1,"y",-1,"z",-1,"Dx",1,"Dy",1,"Dz",1]];
  Sweyl("x,y,z",ww2);
  ans2 = ReParse(ans2);
  r= IsExact_h(ans2,[x,y,z]);
  Println(r);
  return([r,ans2]);
}

def test12() {
  local a,v,ww2,ans2;
  a = Sannfs3("x^3-y^2*z^2");
  ans2 = a[0];
  v = [x,y,z];
  ww2 = [["x",-1,"y",-1,"z",-1,"Dx",1,"Dy",1,"Dz",1]];
  Sweyl("x,y,z",ww2); 
  ans2 = ReParse(ans2); /* DO NOT FORGET! */
  r= IsExact_h(ans2,[x,y,z]);
  Println(r);
  return([r,ans2]);
}

def test13() {
  Println("test13 try to construct a minimal free resolution");
  Println("of a GKZ system [[1,2]]. 6/12, 2000.");
  ans2 = GKZ([[1,2]],[0]); 
   /* Be careful!! It resets the grade to module1, not module1v */
  ww2 = [["x1",-1,"x2",-1,"Dx1",1,"Dx2",1]];
  Sweyl("x1,x2",ww2);
  ans2 = ReParse(ans2[0]);
  Println(ans2);
  return(Sminimal(ans2));
}

def test14() {
  Println("test14 try to construct a minimal free resolution");
  Println("of a GKZ system [[1,2,3]]. 6/12, 2000.");
  ans2 = GKZ([[1,2,3]],[0]);
     /* It stops by the strategy error.
        July 26, 2000. It works fine after fixing a bug in resol.c */
  ww2 = [["x1",-1,"x2",-1,"x3",-1,"Dx1",1,"Dx2",1,"Dx3",1]];
  Sweyl("x1,x2,x3",ww2);
  ans2 = ReParse(ans2[0]);
  return(Sminimal(ans2));
}
def test14a() {
  Println("test14a try to construct a minimal free resolution");
  Println("of a GKZ system [[1,2,3]]. 6/12, 2000.");
  Println("Without automatic homogenization.");
  ww2 = [["x1",-1,"x2",-1,"x3",-1,"Dx1",1,"Dx2",1,"Dx3",1]];
  Sweyl("x1,x2,x3",ww2);
  ans2 = [x1*Dx1+2*x2*Dx2+3*x3*Dx3 , Dx1^2-Dx2*h , -Dx1*Dx2+Dx3*h , 
          Dx2^2-Dx1*Dx3 ];
  ans2 = ReParse(ans2);
  return(Sminimal(ans2,["homogenized"]));
}

def test15() {
  Println("test15 try to construct a minimal free resolution");
  Println("of a GKZ system [[1,2,3]] by the order filt. 6/12, 2000.");
  ww2 = [["Dx1",1,"Dx2",1,"Dx3",1]];
  ans2 = GKZ([[1,2,3]],[0]);  
  Sweyl("x1,x2,x3",ww2);
  ans2 = ReParse(ans2[0]);
  a = Sminimal(ans2);
  Println("Minimal Resolution is "); sm1_pmat(a[0]);
  Sweyl("x1,x2,x3");
  ans3 = ReParse(a[0]);
  r= IsExact_h(ans3,[x1,x2,x3]);
  Println(r);
  return(a);
}

def test15b() {
  Println("test15b try to construct a minimal free resolution");
  Println("of toric [[1,2,3]] by the order filt. 6/12, 2000.");
  ww2 = [["Dx1",1,"Dx2",1,"Dx3",1]];
  Sweyl("x1,x2,x3",ww2);
  ans2 = [Dx1^2-Dx2*h , -Dx1*Dx2+Dx3*h , Dx2^2-Dx1*Dx3 ];
  ans2 = ReParse(ans2);
  return(Sminimal(ans2,["homogenized"]));
}

def test15c() {
  Println("test15c try to construct a minimal free resolution ");
  Println("of a GKZ system [[1,2,3]] by -1,1");
  ww2 = [["Dx1",1,"Dx2",1,"Dx3",1,"x1",-1,"x2",-1,"x3",-1]];
  ans2 = GKZ([[1,2,3]],[0]);  
  Sweyl("x1,x2,x3",ww2);
  ans2 = ReParse(ans2[0]);
  a = Sminimal(ans2);
  Println("Minimal Resolution is "); sm1_pmat(a[0]);
  Sweyl("x1,x2,x3");
  ans3 = ReParse(a[0]);
  r= IsExact_h(ans3,[x1,x2,x3]);
  Println(r);
  return(a);
}
def test16() {
  Println("test16 try to construct a minimal free resolution");
  Println("of a GKZ system [[1,2,3,5]] by the order filt. 6/12, 2000.");
  ww2 = [["Dx1",1,"Dx2",1,"Dx3",1,"Dx4",1]];
  Sweyl("x1,x2,x3,x4",ww2);
  ans2 = GKZ([[1,2,3,5]],[0]);  
  ans2 = ReParse(ans2[0]);
  return(Sminimal(ans2));
}

def test16b() {
  Println("test16b try to construct a minimal free resolution");
  Println("of a toric [[1,2,3,5]] by the order filt. 6/12, 2000.");
  ww2 = [["Dx1",1,"Dx2",1,"Dx3",1,"Dx4",1]];
  Sweyl("x1,x2,x3,x4",ww2);
  ans2 = GKZ([[1,2,3,5]],[0]);  
  ans3 = Rest(ans2[0]);
  ans3 = ReParse(ans3);
  Println("Toric variety:");
  Println(ans3);
  return(Sminimal(ans3));
}


def test17() {
   a=Sannfs3("x^3-y^2*z^2");
   b=a[0]; w = ["x",-1,"y",-1,"z",-1,"Dx",1,"Dy",1,"Dz",1];
   Sweyl("x,y,z",[w]); b = Reparse(b);
   c=Sinit_w(b,w); 
   Println("Resolution (b)----");
   sm1_pmat(b);
   Println("Initial (c)----");
   sm1_pmat(c);
   Println(IsExact_h(c,"x,y,z"));
}          

def test_if_v_strict(resmat,w,v) {
   local b,c,g;
   Sweyl(v,[w]); b = Reparse(resmat);
   Println("Degree shifts ");
   Println(SgetShifts(b,w));
   c=Sinit_w(b,w); 
   Println("Resolution (b)----");
   sm1_pmat(b);
   Println("Initial (c)----");
   sm1_pmat(c);
   Println("Exactness of the resolution ---");
   Println(IsExact_h(b,v));
   Println("Exactness of the initial complex.---");
   Println(IsExact_h(c,v));
   g = Sinvolutive(b[0],w);
   /* Println("Involutive basis ---");
      sm1_pmat(g); 
      Println(Sinvolutive(c[0],w));
      sm1(" /gb.verbose 1 def "); */
   Println("Is same ideal?");
   Println(IsSameIdeal_h(g,c[0],v));
}
def test17b() {
   a=Sannfs3("x^3-y^2*z^2");
   b=a[0]; w = ["x",-1,"y",-1,"z",-1,"Dx",1,"Dy",1,"Dz",1];
   test_if_v_strict(b,w,"x,y,z");
   return(a);
}          

def test18() {
   a=Sannfs2("x^3-y^2");
   b=a[0]; w = ["x",-1,"y",-1,"Dx",1,"Dy",1];
   test_if_v_strict(b,w,"x,y");
   return(a);
}          

def test19() {
  Println("test19 try to construct a minimal free resolution and check if it is v-strict.");
  Println("of a GKZ system [[1,2,3]] by -1,1");
  ww2 = ["Dx1",1,"Dx2",1,"Dx3",1,"x1",-1,"x2",-1,"x3",-1];
  ans2 = GKZ([[1,2,3]],[0]);  
  Sweyl("x1,x2,x3",[ww2]);
  ans2 = ReParse(ans2[0]);
  a = Sminimal(ans2);
  Println("Minimal Resolution is "); sm1_pmat(a[0]);
  b = a[0];
  test_if_v_strict(b,ww2,"x1,x2,x3");
  return(a);
}

/* Need more than 100M memory. 291, 845, 1266, 1116, 592 : Schreyer frame.
   I've not yet tried to finish the computation. */
def test20() {
  w = ["Dx1",1,"Dx2",1,"Dx3",1,"Dx4",1,"x1",-1,"x2",-1,"x3",-1,"x4",-1];
  ans2 = GKZ([[1,1,1,1],[0,1,3,4]],[0,0]);  
  Sweyl("x1,x2,x3,x4",[w]);
  ans2 = ReParse(ans2[0]);
  a = Sminimal(ans2);
  Println("Minimal Resolution is "); sm1_pmat(a[0]);
  b = a[0];
  /* test_if_v_strict(b,w,"x1,x2,x3,x4"); */
  return(a);
}
def test20b() {
  w = ["Dx1",1,"Dx2",1,"Dx3",1,"Dx4",1,"x1",-1,"x2",-1,"x3",-1,"x4",-1];
  ans2 = GKZ([[1,1,1,1],[0,1,3,4]],[1,2]);  
  Sweyl("x1,x2,x3,x4",[w]);
  ans2 = ReParse(ans2[0]);
  a = Sminimal(ans2);
  Println("Minimal Resolution is "); sm1_pmat(a[0]);
  b = a[0];
  /* test_if_v_strict(b,w,"x1,x2,x3,x4"); */
  return(a);
}

def test21() {
   a=Sannfs3("x^3-y^2*z^2+y^2+z^2");
   /* a=Sannfs3("x^3-y-z");  for debug */
   b=a[0]; w = ["x",-1,"y",-1,"z",-1,"Dx",1,"Dy",1,"Dz",1];
   test_if_v_strict(b,w,"x,y,z");
   Println("Degree shifts of Schreyer resolution ----");
   Println(SgetShifts(Reparse(a[3]),w));
   return(a);
}          
def test21b() {
  local i,j,n,sss, maxR, ttt,ans,p, euler;
  Println("The dimensions of linear spaces -----");
  /* sss is the SgetShifts of the Schreyer resol. */
  sss=[    [    0 ]  , [    2 , 2 , 2 , 2 , 2 , 2 , 2 , 3 , 3 , 2 , 1 , 3 , 2 ]  , [    1 , 1 , 1 , 2 , 3 , 2 , 2 , 2 , 2 , 2 , 2 , 3 , 2 , 2 , 2 , 3 , 2 , 3 , 3 , 3 , 4 , 3 , 3 , 4 , 3 , 3 , 4 , 3 , 3 , 4 , 4 , 4 , 4 , 4 , 5 , 4 , 4 , 3 , 5 , 5 , 5 , 5 , 4 ]  , [    1 , 3 , 1 , 3 , 3 , 1 , 2 , 2 , 3 , 2 , 3 , 2 , 3 , 5 , 4 , 4 , 3 , 6 , 5 , 4 , 3 , 2 , 3 , 3 , 5 , 4 , 3 , 2 , 4 , 4 , 4 , 4 , 5 , 3 , 2 , 3 , 3 , 4 , 4 , 4 , 5 , 4 , 4 , 5 , 3 , 5 , 4 , 5 , 5 , 6 ]  , [    3 , 1 , 4 , 5 , 4 , 5 , 2 , 3 , 2 , 4 , 3 , 4 , 3 , 3 , 2 , 4 , 3 , 5 , 4 , 5 , 6 ]  , [    2 , 3 ]  ] ;
  maxR = 3; /* Maximal root of the b-function. */
  n = Length(sss);
  euler = 0;
  for (i=0; i<n; i++) {
    ttt = sss[i];
    ans = 0;
    for (j=0; j<Length(ttt); j++) {
      p = -ttt[j] + maxR + 3; /* degree */
      if (p-maxR >= 0) {
        ans = ans + CancelNumber(p*(p-1)*(p-2)/(3*2*1));
        /* Add the number of monomials */
      }
    }
    Print(ans); Print(", ");
    euler = euler+(-1)^i*ans;
  }
  Println(" ");
  Print("Euler number is : "); Println(euler);
}
def test21c() {
  local i,j,n,sss, maxR, ttt,ans,p, euler;
  Println("The dimensions of linear spaces -----");
  /* sss is the SgetShifts of the minimal resol. */
  sss= [    [    0 ]  , [    2 , 2 , 2 , 2 , 2 , 2 , 2 ]  , [    1 , 2 , 2 , 2 , 2 , 3 , 4 , 4 , 4 , 4 ]  , [    1 , 3 , 4 , 6 ]  ];
  maxR = 3; /* Maximal root of the b-function. */
  n = Length(sss);
  euler = 0;
  for (i=0; i<n; i++) {
    ttt = sss[i];
    ans = 0;
    for (j=0; j<Length(ttt); j++) {
      p = -ttt[j] + maxR + 3; /* degree */
      if (p-maxR >= 0) {
        ans = ans + CancelNumber(p*(p-1)*(p-2)/(3*2*1));
        /* Add the number of monomials */
      }
    }
    Print(ans); Print(", ");
    euler = euler+(-1)^i*ans;
  }
  Println(" ");
  Print("Euler number is : "); Println(euler);
}
def test22() {
   a=Sannfs3("x^3+y^3+z^3");
   b=a[0]; w = ["x",-1,"y",-2,"z",-3,"Dx",1,"Dy",2,"Dz",3];
   test_if_v_strict(b,w,"x,y,z");
   return(a);
}          

def FillFromLeft(mat,p,z) {
  local m,n,i,j,aa;
  m = Length(mat); n = Length(mat[0]);
  aa = NewMatrix(m,n+p);
  for (i=0; i<m; i++) {
    for (j=0; j<p; j++) {
      aa[i,j] = z; /* zero */
    }
    for (j=0; j<n; j++) {
      aa[i,j+p] = mat[i,j];
    }
  }
  return(aa);
}

def FillFromRight(mat,p,z) {
  local m,n,i,j,aa;
  m = Length(mat); n = Length(mat[0]);
  aa = NewMatrix(m,n+p);
  for (i=0; i<m; i++) {
    for (j=n; j<n+p; j++) {
      aa[i,j] = z; /* zero */
    }
    for (j=0; j<n; j++) {
      aa[i,j] = mat[i,j];
    }
  }
  return(aa);
}

def test23() {
  w = ["Dx1",1,"Dx2",1,"Dx3",1,"x1",-1,"x2",-1,"x3",-1];
  Sweyl("x1,x2,x3",[w]);
  d2 = [[Dx1^2-Dx2*h] , [-Dx1*Dx2+Dx3*h] , [Dx2^2-Dx1*Dx3] ];
  d1 = [[-Dx2, -Dx1, -h],[Dx3,Dx2,Dx1]];
  LL = x1*Dx1 + 2*x2*Dx2+3*x3*Dx3;
  /* It is exact for LL = Dx1 + 2*Dx2+3*Dx3;  */
  u1 = [[LL+4*h^2,Poly("0")],[Poly("0"),LL+5*h^2]];
  u2 = [[LL+2*h^2,Poly("0"),Poly("0")],
        [Poly("0"),LL+3*h^2,Poly("0")],
        [Poly("0"),Poly("0"),LL+4*h^2]];
  u3 = [[LL]];
  Println("Checking if it is a double complex. ");
  Println("u^2 d^2 - d^2 u^3");
  sm1_pmat(u2*d2 - d2*u3);
  Println("u^1 d^1 - d^1 u^2");
  sm1_pmat(u1*d1 - d1*u2);
  aa = [
         Join(u3,d2),
         Join(FillFromLeft(u2,1,Poly("0"))-FillFromRight(d2,3,Poly("0")),
              FillFromLeft(d1,1,Poly("0"))),
         FillFromLeft(u1,3,Poly("0"))-FillFromRight(d1,2,Poly("0"))
       ];
  Println([ aa[1]*aa[0], aa[2]*aa[1] ]);
  r= IsExact_h(aa,[x1,x2,x3]);
  Println(r);
  /* sm1_pmat(aa); */
  return(aa);
}


def test24() {
  local Res, Eqs, ww,a;
  ww = ["x",-1,"y",-1,"Dx",1,"Dy",1];
  Println("Example of V-minimal <> minimal ");
  Sweyl("x,y", [ww]);
  Eqs = [Dx-(x*Dx+y*Dy), 
         Dy-(x*Dx+y*Dy)]; 
  sm1(" Eqs dehomogenize /Eqs set");
  Res = Sminimal(Eqs);
  Sweyl("x,y", [ww]);
  a = Reparse(Res[0]);
  sm1_pmat(a);
  Println("Initial of the complex is ");
  sm1_pmat( Sinit_w(a,ww) );
  return(Res);
}

def test24b() {
  local Res, Eqs, ww ;
  ww = ["x",-1,"y",-1,"Dx",1,"Dy",1];
  Println("Construction of minimal ");
  Sweyl("x,y", [ww]);
  Eqs = [Dx-(x*Dx+y*Dy), 
         Dy-(x*Dx+y*Dy)]; 
  sm1(" Eqs dehomogenize /Eqs set");
  Res = Sminimal(Eqs,["Sordinary"]);
  sm1_pmat(Res[0]);
  return(Res);
}

def test25() {
  w = ["Dx1",1,"Dx2",1,"Dx3",1,"Dx4",1,"Dx5",1,"Dx6",1,
       "x1",-1,"x2",-1,"x3",-1,"x4",-1,"x5",-1,"x6",-1];
  ans2 = GKZ([[1,1,1,1,1,1],
              [0,0,0,1,1,1],
              [0,1,0,0,1,0],
              [0,0,1,0,0,1]],[0,0,0,0]);;
  Sweyl("x1,x2,x3,x4,x5,x6",[w]);
  ans2 = ReParse(ans2[0]);
  a = Sminimal(ans2);
}

def test25b() {
  w = ["Dx1",1,"Dx2",1,"Dx3",1,"Dx4",1,"Dx5",1,"Dx6",1,
       "x1",-1,"x2",-1,"x3",-1,"x4",-1,"x5",-1,"x6",-1];
  ans2 = GKZ([[1,1,1,1,1,1],
              [0,0,0,1,1,1],
              [0,1,0,0,1,0],
              [0,0,1,0,0,1]],[0,0,0,0]);
  Sweyl("x1,x2,x3,x4,x5,x6",[w]);
  ans2 = ans2[0];
  sm1(" ans2 rest rest rest rest /ans2 set ");
  Println(ans2);  /* Generators of the toric ideal */
  ans2 = ReParse(ans2);
  a = Sminimal(ans2);
}



