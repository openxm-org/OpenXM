/* $OpenXM: OpenXM/src/k097/lib/minimal/minimal-test.k,v 1.7 2000/07/30 02:26:25 takayama Exp $ */
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
  return([r,ans2]);
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

          


