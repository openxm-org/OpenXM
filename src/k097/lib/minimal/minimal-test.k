/* $OpenXM: OpenXM/src/k097/lib/minimal/minimal-test.k,v 1.2 2000/06/08 08:37:53 takayama Exp $ */
load["minimal.k"];
def test5() {
  local a,b,c,cc,v;
  a = Sannfs3_laScala2("x^3-y^2*z^2");
  b = a[0];
  v = [x,y,z];
  c = Skernel(b[0],v);
  c = c[0];
  sm1_pmat([c,b[1],v]);
  Println("-----------------------------------");
  cc = sm1_res_div(c,b[1],v);
  sm1_pmat(sm1_gb(cc,v));
  c = Skernel(b[1],v);
  c = c[0];
  cc = sm1_res_div(c,b[2],v);
  sm1_pmat(sm1_gb(cc,v));
  return(a);
}
def test6() {
  local a,b,c,cc,v;
  a = Sannfs3("x^3-y^2*z^2");
  b = a[0];
  v = [x,y,z];
  c = Skernel(b[0],v);
  c = c[0];
  sm1_pmat([c,b[1],v]);
  Println("-------ker = im for minimal ?---------------------");
  cc = sm1_res_div(c,b[1],v);
  sm1_pmat(sm1_gb(cc,v));
  c = Skernel(b[1],v);
  c = c[0];
  cc = sm1_res_div(c,b[2],v);
  sm1_pmat(sm1_gb(cc,v));
  Println("------ ker=im for Schreyer ?------------------");
  b = a[3];
  c = Skernel(b[0],v);
  c = c[0];
  sm1_pmat([c,b[1],v]);
  cc = sm1_res_div(c,b[1],v);
  sm1_pmat(sm1_gb(cc,v));
  c = Skernel(b[1],v);
  c = c[0];
  cc = sm1_res_div(c,b[2],v);
  sm1_pmat(sm1_gb(cc,v));
  return(a);
}

/* May 23, Tue */
def test7() {
  local a,b,c,cc,v;
  a = Sannfs3_laScala2("x^3-y^2*z^2");
  b = a[0];
  v = [x,y,z];
  c = Skernel(b[0],v);
  c = c[0];
  sm1_pmat([c,b[1],v]);
  Println("-------ker = im for minimal ?---------------------");
  cc = sm1_res_div(c,b[1],v);
  sm1_pmat(sm1_gb(cc,v));
  c = Skernel(b[1],v);
  c = c[0];
  cc = sm1_res_div(c,b[2],v);
  sm1_pmat(sm1_gb(cc,v));
  Println("------ ker=im for Schreyer ?------------------");
  b = a[3];
  c = Skernel(b[0],v);
  c = c[0];
  sm1_pmat([c,b[1],v]);
  cc = sm1_res_div(c,b[1],v);
  sm1_pmat(sm1_gb(cc,v));
  c = Skernel(b[1],v);
  c = c[0];
  cc = sm1_res_div(c,b[2],v);
  sm1_pmat(sm1_gb(cc,v));
  return(a);
}

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

def test8a() {
  local p,pp,ans,b,c,cc,ww, ans_all;
  f = "x^3-y^2*z^2";
  p = Sannfs(f,"x,y,z");
  sm1(" p 0 get { [(x) (y) (z) (Dx) (Dy) (Dz)] laplace0 } map /p set ");
  ww = [["x",-1,"y",-1,"z",-1,"Dx",1,"Dy",1,"Dz",1]];
  /* Removed "x",1, ... ===> It causes an error. I do not know the reason.*/
  Sweyl("x,y,z",ww);
  pp = Map(p,"Spoly");
  /* return(pp); */
  /* pp =
     [y*Dy-z*Dz , -2*x*Dx-3*y*Dy+1 , 2*x*Dy*Dz^2-3*y*Dx^2 , 
      2*x*Dy^2*Dz-3*z*Dx^2 , 2*x*z*Dz^3-3*y^2*Dx^2+4*x*Dz^2 ] 
  */
  ans_all = Sschreyer(pp);
  ans = ans_all[0];
  /* ans = sm1_resol1([pp,"x,y,z",ww]); */
  /* Schreyer is in ans. */

  v = [x,y,z];
  b = ans;
  Println("------ ker=im for Schreyer ?----- wrong method!!!-----------");
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

/* Comparing two constructions */
def test9() {
  local p,pp,ans,b,c,cc,ww,ww2,ans_all,ans2;
  f = "x^3-y^2*z^2";
  p = Sannfs(f,"x,y,z");
  ww2 = [["x",-1,"y",-1,"z",-1,"Dx",1,"Dy",1,"Dz",1]];
  sm1(" p 0 get { [(x) (y) (z) (Dx) (Dy) (Dz)] laplace0 } map /p set ");
  Sweyl("x,y,z",ww2);
  pp = Map(p,"Spoly");
  ans = sm1_resol1([pp,"x,y,z",ww2]);

  f = "x^3-y^2*z^2";
  p = Sannfs(f,"x,y,z");
  sm1(" p 0 get { [(x) (y) (z) (Dx) (Dy) (Dz)] laplace0 } map /p set ");
  ww = [["x",-1,"y",-1,"z",-1,"Dx",1,"Dy",1,"Dz",1]];
  Sweyl("x,y,z",ww);
  pp = Map(p,"Spoly");
  ans_all = Sschreyer(pp);
  ans2 = ans_all[0];

  return([ans,ans2]);

}

/* Check if the complex by Sschreyer() is exact or not in our example? */
def test10() {
  local p,pp,ans,b,c,cc,ww,ww2,ans_all,ans2, r;
  f = "x^3-y^2*z^2";
  p = Sannfs(f,"x,y,z");
  ww2 = [["x",-1,"y",-1,"z",-1,"Dx",1,"Dy",1,"Dz",1]];
  sm1(" p 0 get { [(x) (y) (z) (Dx) (Dy) (Dz)] laplace0 } map /p set ");
  Sweyl("x,y,z",ww2);
  pp = Map(p,"Spoly");
  ans = sm1_resol1([pp,"x,y,z",ww2]);

  f = "x^3-y^2*z^2";
  p = Sannfs(f,"x,y,z");
  sm1(" p 0 get { [(x) (y) (z) (Dx) (Dy) (Dz)] laplace0 } map /p set ");
  ww = [["x",-1,"y",-1,"z",-1,"Dx",1,"Dy",1,"Dz",1]];
  Sweyl("x,y,z",ww);
  pp = Map(p,"Spoly");
  ans_all = Sschreyer(pp);  /* Schreyer by LaScala-Stillman */
  ans2 = ans_all[0];
 
  sm1(" /gb.verbose 1 def ");

  ww2 = [["x",-1,"y",-1,"z",-1,"Dx",1,"Dy",1,"Dz",1]];
  Sweyl("x,y,z",ww2);
  ans2 = ReParse(ans2);
  r= IsExact_h(ans2,[x,y,z]);
  Print(r);
  
  return([r,[ans,ans2]]);

}

def test11() {
  local  a;
  a = test_ann3("x^3-y^2*z^2");
  return(a);
}
/* f should be a string. */
def test_ann3(f) {
  local a,v,ww2,ans2;
  a = Sannfs3_laScala2(f);
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
  ans2 = ReParse(ans2);
  r= IsExact_h(ans2,[x,y,z]);
  Println(r);
  Println("It may stop by non-exact statement. The code of Sminimal_v (non-LaScala-Stillman contains bugs.");
  return([r,ans2]);
}

