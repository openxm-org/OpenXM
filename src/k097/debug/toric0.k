
/* SSWork/yacc/ip-k/ex3.ccc,   1996, 8/12. This is original. ---> */
/*             debug/toric0.k */
/*  graver basis を求める関数.  
    toric の generator を求める関数.
    A-hypergeometric の Fourier 変換の indicial を求める関数.
*/

load("indexed.k");

def toric0_toMonom(aa,i,offset, ring)
{
  local j,ans,m;
  m = Length(aa);
  ans = PolyR("1",ring);
  for (j=0; j<m; j++) {
    ans = ans*(z[offset+j]^(aa[j,i]));
  }
  return(ans);
}

def testgraver() {
  local a,ans;
  a = [[1,1,1,1,1,1,1],
       [2,3,2,2,2,0,1],
       [2,2,3,2,2,0,1],
       [2,2,2,3,2,0,1],
       [1,1,1,1,2,0,1]];
  ans = Graver(a);
  return(ans);  /* degree 33. About 1 hour?  cf. this/Mirror/wp22211.ml */
}

def testgraver2() {
  local a,ans;
  a = [[1,1,1,1,1,1],
       [0,0,0,1,1,1],
       [0,1,0,0,1,0],
       [0,0,1,0,0,1]];
  ans = Graver(a);
  return(ans);
}


       
HelpAdd(["Graver",
 ["Graver(a) (matrix a ) returns a graver basis of the affine toric ideal",
  "defined by the matrix << a >>.",
  "It defines a ring of variables z[0], z[1], ..., z[n-1], ..... and ",
  "the output belongs to this ring.",
  "Example: Graver([[1,1,1,1],[0,1,2,3]]):",
  "[    -z[1]^2+z[2]*z[0] , -z[2]^2+z[3]*z[1] , -z[2]*z[1]+z[3]*z[0] ,",
  "     -z[1]^3+z[3]*z[0]^2 , -z[2]^3+z[3]^2*z[0] ] "
]]);

def Graver(a) {
  local aa,i,j,rz,n,d,ideal,ans,univ,rule,nn,weight,elim;
  d = Length(a);  n = Length(a[0]);
  aa = NewMatrix(d+n,2*n);
  for (i=0; i<d; i++) {
    for (j=0; j<n; j++) {
       aa[i,j] = a[i,j];
    }
  }
  for (i=0; i<n; i++) {
    aa[d+i,i] = 1;
    aa[d+i,n+i] = 1;
  }
  Println(aa);
 
  weight = [ ]; elim = [ ];
  for (i= 2*n; i< 2*n + n+d; i++) {
     weight = Join(weight,[Indexed("z",i), 1]);
     elim = Append(elim, Indexed("z",i));
  }
  Println(weight);
  Println(elim);

  rz = RingPonIndexedVariables("z",2*n+d+n, [weight]); 
  /* z[0], ..., z[2n-1], ... */
  ideal = [ ];
  for (i=0; i< 2*n; i++) {
     ideal = Append(ideal, z[i] - toric0_toMonom(aa,i,2*n,rz));
  }
  Println(" --------- input ideal -------------");
  Print(" z["); Print( 2*n ); Print( "] --- z["); Print( 2*n+n+d-1);
  Println("] should be eliminated."); 
  Println(ideal);
 
  ans = Groebner(ideal);
  Println(" -------------- gb is ----------------- "); Println(ans);
  ans = Eliminatev(ans,elim);
  Println(" ------------ eliminated -------------- "); Println(ans);

  rule = [[h, PolyR("1",rz) ] ];
  nn = Length(ans); univ = [ ];
  for (i=0; i<nn; i++) {
    univ = Append(univ,Replace(ans[i],rule));
  }
  ans = ReducedBase(univ);
  Println(" ----------- removed redundant elements ----------- ");
  Println(ans);

  rule = [ ];
  for (i= n; i<2*n; i++) {
    rule = Append(rule, [ z[i], PolyR("1",rz)]);
  }
  Println(" ------------ a rule for replacement ------------ ");
  Println(rule);

  univ = [ ];
  nn = Length(  ans );
  for (i=0; i<nn; i++) {
     univ = Append(univ,Replace(ans[i],rule));
  }
  Println(" ------------ graver basis is ------------ ");
  Println(univ);
  Println(" ");
  return(univ);
}

  
def toric(aa) {
  local i,j,rz,n,d,ideal,ans,univ,rule,nn,weight,elim;
  d = Length(aa);  n = Length(aa[0]);
  Println(aa);
 
  weight = [ ]; elim = [ ];
  for (i= n; i< n+d; i++) {
     weight = Join(weight,[Indexed("z",i), 1]);
     elim = Append(elim, Indexed("z",i));
  }
  Println(weight);
  Println(elim);

  rz = RingPonIndexedVariables("z",n+d, [weight]); 
  /* z[0], ..., z[n-1], ... , z[n+d]*/
  ideal = [ ];
  for (i=0; i< n; i++) {
     ideal = Append(ideal, z[i] - toric0_toMonom(aa,i,n,rz));
  }
  Println(" --------- input ideal -------------");
  Print(" z["); Print( n ); Print( "] --- z["); Print( n+d-1);
  Println("] should be eliminated."); 
  Println(ideal);
 
  ans = Groebner(ideal);
  Println(" -------------- gb is ----------------- "); Println(ans);
  ans = Eliminatev(ans,elim);
  Println(" ------------ eliminated -------------- "); Println(ans);

  rule = [[h, PolyR("1",rz) ] ];
  nn = Length(ans); univ = [ ];
  for (i=0; i<nn; i++) {
    univ = Append(univ,Replace(ans[i],rule));
  }
  ans = ReducedBase(univ);
  Println(" ----------- removed redundant elements ----------- ");
  Println(" ---------- generators of the toric ideal are ----- ");
  Println(ans);
  Println(" ");
  return(ans);
}

def zindicial0(input,n,m) {
  local rz,weight, ww,i,rule,zinverse,m,d,ans,elim;
  ww = [ ]; elim = [ ];
  /*  z[n+1], ..., z[n+m] がパラメータ変数 */
  /*  Dz[0] --- Dz[n-2], z[0] --- z[n-2] を消去する. */
  for (i=0; i<n-1; i++) {
    ww = Join(ww,[Indexed("Dz",i), 1]);
    ww = Join(ww,[Indexed("z",i), 1]);
    if (i != n-1) {
       elim = Append(elim,Indexed("Dz",i));
       elim = Append(elim,Indexed("z",i));
    }
  }
  weight = [[Indexed("z",n),1] , ww];
  Print("-------- weight ---------: "); Println(weight);
  rz = RingDonIndexedVariables("z",n+1+m, weight); 
  input = Mapto(input,rz);
  Println("------------ input ------------"); Println(input);

  /* F-homogenization. z[0], ..., z[n-1],  
     z[n] is the homogenization variable*/
  /* z[n]^(-1) とは書けないのはつらい. 1 を戻すので bug ともいえる. */
  zinverse = PolyR(AddString([Indexed("z",n),"^(-1)"]),rz);
  rule = [[Dz[n-1], Dz[n-1]*z[n]], [z[n-1],z[n-1]*zinverse]];
  input = Replace(input,rule);
  m = Length(input);
  for (i=0; i<m; i++) {
    d = -Degree(Replace(input[i],[[z[n],zinverse]]),z[n]);
    if (d < 0) {
         input[i] = z[n]^(-d)*input[i];
    }
  }
  Print("------ input : "); Println(input);
  ans = Groebner(input);
  m = Length(ans);
  for (i=0; i<m; i++)  {
    /* FW principal part をとる. */
    ans[i] = Init_w(ans[i],[z[n]],[1]);
  }
  Print("--------FW principal parts : ");Println(ans);
  ans = Eliminatev(ans,elim);
  m = Length(ans);
  /* h,  z[n] を 1 にする. */
  for (i=0; i<m; i++) {
    ans[i] = Replace(ans[i],[[h,PolyR("1",rz)],[z[n],PolyR("1",rz)]]);
  }
  Println(" "); Println(" ");
  return(ans);
}

def zrho(f,n) {
  local ans,i,top,w,rz;
  ans = 0;
  rz = Ringp(f);
  while(true) {
    if ( f == Poly("0")) sm1(" exit ");
    top = Init(f);
    f = f-top;
    w = Exponent(top,[Dz[n-1]]);
    top = Replace(top,[[Dz[n-1],PolyR("1",rz)]])*zipoch(z[n],w[0]);
    ans = ans + top;
  }
  return(ans);
}
 
def zipoch(f,w) {
  local ans,i;
  ans = 1;
  for  (i=0; i<w; i++) {
    ans = ans*(f-i);
  }
  return(ans);
}


fff = ["z[0]*Dz[0]+z[1]*Dz[1]+z[2]*Dz[2]+z[3]*Dz[3]-z[5]",
       "           z[1]*Dz[1]           +z[3]*Dz[3]-z[6]",
       "                      z[2]*Dz[2]+z[3]*Dz[3]+z[7]",
       "z[0]*z[3]-z[1]*z[2]"];
/*  z[4] はつかったらだめよ. */
/* Println(" type in zindicial0(fff,4,3); "); */

def foo2() {
  local ans,n,i,m,r,tmp;
  ans = zindicial0(fff,4,3); n = 4;
  Println(ans);
  m = Length(ans);
  r = [ ];
  for (i=0; i<m; i++) {
     tmp = ans[i];
     tmp = Replace(tmp,[[z[n-1],Poly("1")]]);
     tmp = zrho(tmp,n);
     Print(i); Print(" :  ");Println(tmp);
     r = Append(r,tmp);
  }
  return(r);
}

/* Println("Type in foo2()."); */

def zindicial(fff,n,mm) {
  local ans,n,i,m,r,tmp;
  ans = zindicial0(fff,n,mm);
  Println(ans);
  m = Length(ans);
  r = [ ];
  Println(AddString(["------ The indicial polynomial  along z[",
                     ToString(n-1),
                     "] = 0 is the minimal degree polynomial of the following",
                     "polynomials."]));
  Println(AddString(["z[",ToString(n),"] is equal to s."]));
  for (i=0; i<m; i++) {
     tmp = ans[i];
     tmp = Replace(tmp,[[z[n-1],Poly("1")]]);
     tmp = zrho(tmp,n);
     Print(i); Print(" :  ");Println(tmp);
     r = Append(r,tmp);
  }
  Println(" ");
  return(r);
}

